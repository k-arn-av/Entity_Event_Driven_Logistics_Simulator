#include <iostream>
#include "autobots.hpp"
#include "entity_factorymanager.hpp"
#include "package.hpp"
#include "warehouse.hpp"
#include "autobots.hpp"
#include "processinghub.hpp"
#include "service.hpp"

//each hub object has its own function
void package_barcode(package& pkg){
    std::print(" HUB Package Scanner scanning and labeling package: {}\n", pkg.getID());
}

int main(){

    //Factory Manager creation
    factorymanager manager;

    //Make a warehouse object with 10 package capacity,that generates 1 package every 3 ticks
    auto Warehouse=std::make_unique<warehouse>("Warehouse1", 10, 3);
    manager.set_warehouse(std::move(Warehouse));

    //Create a package
    auto pkg = std::make_unique<package>("Creative Tools", "ArnavK", "Karnav", 47, "Package Labeler");

    //create factory entities with the template 

    //Hub for packages scanning
    auto& hub= manager.create_entity<processinghub>("Package Scanner", package_barcode);
    
    //Service stations for bot servicing, with name, capacity and servicetype
    auto& charger= manager.create_entity<chargingStation>("BotCharger1", 5);
    auto& maintainer=manager.create_entity<maintenanceStation>("RepairMaster22", 3);

    //Autobots with initial charge, condition percent and name
    auto& bot1= manager.create_entity<autobots>(100, 98, "QT");
    auto& bot2= manager.create_entity<autobots>(89, 33, "TW");

    //SIMULATION LOOP
    std::cout<<"Starting Simulation Loop\n";

    for (size_t tick=0; tick<15; ++tick){

        //trigger a system event at tick 6 and 10 to test broadcasting mechanics
        if (tick==6){
            std::print("\n TRIGGERING SYSTEM EVENT: EMERGENCY STOP \n");
            manager.broadcast_event(EntityEvent::EMERGENCY_STOP);
        }
        if (tick==10){
            std::print("\n TRIGGERING SYSTEM EVENT: CLEAR FAULT & RESUME \n");
            manager.broadcast_event(EntityEvent::CLEAR_FAULT);
            manager.broadcast_event(EntityEvent::RESUME_WORK);
        }

        //route package created earlier to Package Scanner at tick 11
        if (tick=11){
            pkg->setDestinationID("Package Scanner");
            manager.route_package(std::move(pkg));
        }

        //call universal tick to increase tick over time
        manager.tick();
    }
    return 0;
}