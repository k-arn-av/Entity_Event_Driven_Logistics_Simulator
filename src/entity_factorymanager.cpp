#include "entity_factorymanager.hpp"
#include <iostream>
#include <print>

void factorymanager::tick() {
    ++current_tick;
    std::print("Tick {} started.\n", current_tick);

    //Tick the standalone warehouse if configured
    if (factory_warehouse) {
        //Option to pass 'this' and a default destination ID for auto-generation/routing
        factory_warehouse->increase_tick(this, "");
    }

    //Tick all registered Factoryentity instances (stations, hub, autobots)
    for (auto& entity : entities) {
        if (entity) {
            entity->increase_tick();
        }
    }
}

//Activates every entity's event type internal condition mechanics 
void factorymanager::broadcast_event(EntityEvent event_type) {
    //Forward event to standalone warehouse
    if (factory_warehouse) {
        factory_warehouse->event(event_type);
    }

    //Broadcast event to all polymorphic factory entities
    for (auto& entity : entities) {
        if (entity) {
            entity->event(event_type);
        }
    }
    std::print("Broadcasted event to all entities and warehouse.\n");
}

void factorymanager::route_package(std::unique_ptr<package> pkg) {
    if (!pkg) {
        std::print("Manager: null package.\n");
        return;
    }

    std::string destinationID = pkg->getDestinationID();

    if (destinationID.empty()) {
        std::print("[MANAGER] Error: Package '{}' has no destination assigned\n", pkg->getID());
        return;
    }

    //Check 1: Route to Standalone Warehouse
    if (factory_warehouse && factory_warehouse->get_name() == destinationID) {
        if (factory_warehouse->receive_package(std::move(pkg))) {
            std::print("Package routed to Warehouse ({}).\n", destinationID);
        } else {
            std::print("[MANAGER] Error: Warehouse '{}' rejected package.\n", destinationID);
        }
        return;
    }

    //Check 2: Route to Polymorphic Entities (Processing Hubs)
    for (auto& entity : entities) {
        if (entity && entity->get_name() == destinationID) {
            //Check if entity is a processinghub using dynamic_cast
            //creating a raw ptr that holds the address of the .get() raw ptr of entity,
            //only if the .get() raw ptr matches the dynamic casts configured datatype(raw ptr of procesinghub obj)
            //hub holds the accessing address (rawptr) of object of the unique ptr
            processinghub* hub = dynamic_cast<processinghub*>(entity.get());

            if (hub) {
                hub->receive_package(std::move(pkg)); // transfers ownership to receiving hub
                std::print("Package routed to {}.\n", destinationID);
                return;
            }
        }
    }

    std::print("[MANAGER] Error: Destination '{}' not found\n", destinationID);
}

 