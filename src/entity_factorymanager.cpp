#include <iostream>
#include "entity_factorymanager.hpp"
#include <print>

void factorymanager::tick(){

    ++current_tick;
    std::print("Tick {} started.\n", current_tick);

    for (auto& entity:entities){
        entity->increase_tick();
    }
}

// Activates every entity's event type internal condition mechanics 
void factorymanager::broadcast_event(EntityEvent event_type){

    for (auto& entity:entities){
        entity->event(event_type);
    }
    std::print("Broadcasted event to all entities.\n");
}

void factorymanager::route_package(std::unique_ptr<package> pkg){

    if (!pkg){
        std::print("Manager: null package.\n");
        return;
    }
    std::string destinationID= pkg->getDestinationID();

    if (destinationID.empty()) {
        std::print("[MANAGER] Error: Package '{}' has no destination assigned\n", pkg->getID());
        return;
    }

    for (auto& entity:entities){

        if (entity && entity->get_name() == destinationID) {

            //creating a raw ptr that holds the address of the .get() raw ptr of entity,
            //only if the .get() raw ptr matches the dynamic casts configured datatype(raw ptr of procesinghub obj)
            processinghub* hub = dynamic_cast<processinghub*>(entity.get()); 
            //hub holds the accessing address (rawptr) of object of the unique ptr

            if (hub) {
                hub->receive_package(std::move(pkg)); // transfers ownership to the receiving hub
                std::print("Package routed to {}.\n", destinationID);
                return;
            }
        }
    }
    std::print("[MANAGER] Error: Destination '{}' not found\n", destinationID);
}
