#pragma once
#include "factoryentity.hpp"
#include "package.hpp"
#include <functional>
#include <unordered_map>
#include <memory>
class processinghub:public Factoryentity{

    private:
        //to give each processing hub object its own function as an attribute, use functional library.
        using processTechnique=std::function<void(package&)>; //used alias processTechnique to avoid defining the function repeatedly

        processTechnique technique;
        size_t required_ticks=0;
        size_t progress_ticks=0;
        bool has_power=true;
        std::unordered_map<std::string, std::unique_ptr<package>> package_inventory;//assigning an unordered map to store packages by their id
        
        public:
            processinghub(const std::string& n, processTechnique tech): Factoryentity(n), technique(tech){}

            void increase_tick()override;

            void receive_package(const package& pkg);

            std::unique_ptr<package> dispatch_package(const std::string& pkgID);// removes package from inventory and transfers the ownership of the object

            //getters
            size_t getInventorycount(){return package_inventory.size();}
            size_t getHubId(){return id;}

};