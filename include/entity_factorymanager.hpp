#pragma once
#include "autobots.hpp"
#include "processinghub.hpp"
#include <memory>
#include <utility>
#include <vector>

class factorymanager{
    private:
        std::vector<std::unique_ptr<Factoryentity>>entities;
        size_t current_tick=0; //univeral clock for the factory, increases without any 
    
    public:

        void tick();

        //automates object memory allocation to heap, and points each using unique ptrs
        template <typename T, typename... Args>
        T& create_entity(Args&&... args){

            auto entity=std::make_unique<T> (std::forward<Args>(args)...); //allocates any entity object created to heap using make_unique
            T& ref= *entity; //saving reference of the object to give access later
            entities.push_back(std::move(entity)); //appends the entity by giving complete ownership to the vector element(unique ptr) and making entity nullptr
            return ref;

        } // returns by reference to give access only, no ownership

        void broadcast_event(EntityEvent event_type);

        void route_package(std::unique_ptr<package> pkg);

        //getters and inspection

        size_t getentitySize()const {return entities.size();}

        size_t getCurrenttick() const {return current_tick;}

};


