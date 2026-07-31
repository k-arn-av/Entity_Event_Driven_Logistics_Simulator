#include <iostream>
#include "service.hpp"

void serviceStation::add_bots_to_queue(autobots* bot){

    if (!bot){return;}

    if (service_queue.size()>=total_capacity){
        std::cout << "No space available for this new bot";
        return; 
    }
    service_queue.push_back(bot);

}

void serviceStation::release_bot_from_queue(autobots* bot){
    if (!bot){return;}
    //find the bot in the vector and remove it
    auto it= std::find(service_queue.begin(),service_queue.end(),bot);
    service_queue.erase(it);
}

void serviceStation::event(EntityEvent eventtype) {
    switch (eventtype) {
        case EntityEvent::EMERGENCY_STOP:
            set_has_power(false);
            set_is_servicing(false);
            break;

        case EntityEvent::CLEAR_FAULT:
            set_has_power(true);
            break;

        case EntityEvent::PAUSE_WORK:
            set_is_servicing(false);
            break;

        case EntityEvent::RESUME_WORK:
            if (get_has_power() && !service_queue.empty()) {
                set_is_servicing(true);
            }
            break;

        case EntityEvent::SENSOR_OBSTRUCTION:
            break;

        case EntityEvent::CANCEL_TASK:
            // Optional: release_all_bots() or halt current operation
            set_is_servicing(false);
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
    }
}



void maintenanceStation::increase_tick() {
    // 1. Hardware and queue availability checks
    if (!get_has_power() || service_queue.empty()) {
        set_is_servicing(false);
        set_is_repairing(false);
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
            std::cout << "[" << get_name() << "] Bot " << bot->get_name() << " repair complete!\n";
            
            if (auto_reboot_enabled) {
                std::cout << "[" << get_name() << "] System reboot initiated for: " << bot->get_name() << "\n";
            }

            release_bot_from_queue(bot);
        }
    }

    // 3. Update status flags if queue emptied out
    if (service_queue.empty()) {
        set_is_servicing(false);
        set_is_repairing(false);
    }
}