#include "processinghub.hpp"
#include <iostream>

void processinghub::receive_package(std::unique_ptr<package>pkg){ 
    if (!pkg){
        return;
    }
    pkg->setStatus(package_status::PROCESSING);

    package_inventory[pkg->getID()]= std::move(pkg);

}

std::unique_ptr<package> processinghub::dispatch_package(const std::string& pkgID){

    if (!get_has_power() || get_state() != entityState::RUNNING) {
        return nullptr; // Cannot dispatch when hub is unpowered or halted
    }

    auto map_node= package_inventory.extract(pkgID); // extracts the package node (id, object) from the map

    if (map_node.empty()){
        return nullptr;
    }

    auto pkg=std::move(map_node.mapped());
    pkg->setStatus(package_status::PROCESSED);
    return pkg;
}

void processinghub::increase_tick(){
    if (!get_has_power() || !get_is_active()){return; }

    switch (get_state())
    {
    case entityState::RUNNING:

        if (package_inventory.empty()){return;}// no package to process

        ++progress_ticks;

        if (progress_ticks>=required_ticks){
            for (auto& [id,pkg]: package_inventory){// use auto& [key,value] to replace dict.items from py
                if (pkg != nullptr && pkg->getStatus()== package_status::PROCESSING){
                    technique(*pkg); // the function attribute for each processing hub that dictates its own function that modifies package object
                    pkg->setStatus(package_status::PROCESSED);
                }
            }  
            progress_ticks=0;
        }
        
        break;
    
    case entityState::PAUSED:
    case entityState::SHUTDOWN:
    case entityState::FAULTED:
        break;

    default:
        break;
    }
}

void processinghub::event(EntityEvent event_type){
    
    switch (event_type) {
        case EntityEvent::EMERGENCY_STOP:
        case EntityEvent::SENSOR_OBSTRUCTION:
            set_state(entityState::FAULTED);
            progress_ticks = 0; // Freeze processing cycle and reset cycle timer
            break;

        case EntityEvent::CLEAR_FAULT:
            if (get_state() == entityState::FAULTED) {
                set_state(entityState::RUNNING);
            }
            break;

        case EntityEvent::PAUSE_WORK:
            if (get_state() == entityState::RUNNING) {
                set_state(entityState::PAUSED);
            }
            break;

        case EntityEvent::RESUME_WORK:
            if (get_state() == entityState::PAUSED) {
                set_state(entityState::RUNNING);
            }
            break;

        case EntityEvent::CANCEL_TASK:
            progress_ticks = 0;
            break;

        default:
            break;
    }
}



