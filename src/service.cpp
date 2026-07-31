#include <iostream>
#include "service.hpp"

void serviceStation::add_bots_to_queue(autobots* bot){

    if (!bot){return;}

    if (service_queue.size()>=total_capacity){
        std::print("Queue full. Bot not added.\n");
        return; 
    }
    service_queue.push_back(bot);
    std::print("{} queued for service.\n", bot->get_name());

}

void serviceStation::release_bot_from_queue(autobots* bot){
    if (!bot){
        std::print("Service: invalid bot release.\n");
        return;
    }
    //find the bot in the vector and remove it
    auto it= std::find(service_queue.begin(),service_queue.end(),bot);
    service_queue.erase(it);
    std::print("{} released from service queue.\n", bot->get_name());
}

void serviceStation::event(EntityEvent eventtype) {
    switch (eventtype) {
        case EntityEvent::EMERGENCY_STOP:
            set_has_power(false);
            set_is_servicing(false);
            std::print("{} stopped for emergency.\n", get_name());
            break;

        case EntityEvent::CLEAR_FAULT:
            set_has_power(true);
            std::print("{} power restored.\n", get_name());
            break;

        case EntityEvent::PAUSE_WORK:
            set_is_servicing(false);
            std::print("{} paused.\n", get_name());
            break;

        case EntityEvent::RESUME_WORK:
            if (get_has_power() && !service_queue.empty()) {
                set_is_servicing(true);
                std::print("{} resumed.\n", get_name());
            }
            break;

        case EntityEvent::SENSOR_OBSTRUCTION:
            break;

        case EntityEvent::CANCEL_TASK:
            // Optional: release_all_bots() or halt current operation
            set_is_servicing(false);
            std::print("{} cancelled current task.\n", get_name());
            break;
    }
}

double chargingStation::convert_to_DC() const{
    if (!get_has_power()) {
        return 0.0;
    }
    return incoming_powerAC_watts * conversion_efficiency;
}

void chargingStation::increase_tick(){
    // hardware and queue check
    if (!get_has_power() || service_queue.empty()) {
        set_is_charging(false);
        set_is_servicing(false);
        std::print("{} idle: no charging work.\n", get_name());
        return;
    }
    set_is_charging(true);
    set_is_servicing(true);
    double dc_output=convert_to_DC();

    // Iterate backwards so erasing finished bots doesn't invalidate iteration
    for (int i = static_cast<int>(service_queue.size()) - 1; i >= 0; --i) { //static cast can convert datatypes to ensure error free code
        // since .size() is size_t and i needs to be negative, static cast converts size_t to int
        autobots* bot = service_queue[i];
        if (!bot) continue;

        // Apply charge
        bot->set_charge(dc_output);

        // Check completion condition
        if (bot->get_charge() == bot->get_maxCharge()){
            release_bot_from_queue(bot);
        }
    }
    // update status boolean if emptied
    if (service_queue.empty()) {
        set_is_servicing(false);
        set_is_charging(false);
        std::print("{} charging queue cleared.\n", get_name());
    }
}



void maintenanceStation::increase_tick() {
    // 1. Hardware and queue availability checks
    if (!get_has_power() || service_queue.empty()) {
        set_is_servicing(false);
        set_is_repairing(false);
        std::print("{} idle: no repair work.\n", get_name());
        return;
    }

    set_is_servicing(true);
    set_is_repairing(true);

    // 2. Iterate backwards to safely perform repairs and manage queue removals
    for (int i = static_cast<int>(service_queue.size()) - 1; i >= 0; --i) {
        autobots* bot = service_queue[i];
        if (!bot) continue;

        // Perform repair using restore rate
        bot->repair(repair_per_tick);

        // Check if bot health is fully restored
        if (bot->is_fully_repaired()) {
            std::print("{}: Bot {} repaired.\n", get_name(), bot->get_name());
            
            if (auto_reboot_enabled) {
                std::print("{}: Rebooting {}.\n", get_name(), bot->get_name());
            }

            release_bot_from_queue(bot);
        }
    }

    // 3. Update status flags if queue emptied out
    if (service_queue.empty()) {
        set_is_servicing(false);
        set_is_repairing(false);
        std::print("{} repair queue cleared.\n", get_name());
    }
}