#include <iostream>
#include "entity_factorymanager.hpp"
#include <print>

void factorymanager::tick(){

    ++current_tick;

    for (auto& entity:entities){
        entity->increase_tick();
    }
}

template <typename T, typename... Args>
T& factorymanager::create_entity(Args&&... args){

    auto entity=std::make_unique<T> (std::forward<Args>(args)...); //allocates any entity object created to heap using make_unique
    T& ref= *entity; //saving reference of the object to give access later
    entities.push_back(std::move(entity)); //appends the entity by giving complete ownership to the vector element(unique ptr) and making entity nullptr
    return ref;

}

// Activates every entity's event type internal condition mechanics 
void factorymanager::broadcast_event(EntityEvent event_type){

    for (auto& entity:entities){
        entity->event(event_type);
    }
}

void factorymanager::route_package(std::unique_ptr<package> pkg){

    if (!pkg){
        std::cout << "[MANAGER] Warning: Attempted to route a null package.\n";
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
                return;
            }
        }
    }
    std::print("[MANAGER] Error: Destination '{}' not found\n", destinationID);
}
