#pragma once
#include "autobots.hpp"
#include "factoryentity.hpp"
#include <memory>
#include <vector>

enum class serviceType{
    CHARGING,
    MAINTENANCE
};

class serviceStation: public Factoryentity{
    protected:
        inline static size_t total_stations=0;
        size_t total_capacity;
        std::vector<autobots*> service_queue; // raw pointer to give access only to the unique ptr in the manager
        size_t required_service_tick=0;
        size_t progress_service_tick=0;
        bool has_power=true;
        bool is_servicing=false;
        serviceType servicetype;

    public: // in a constructor, passing by value and the real attribute stealing that value (std::move()) is modern way to make no copies.
        serviceStation(std::string name, size_t cap, serviceType t): Factoryentity(std::move(name)), total_capacity(cap), servicetype(t){
            ++total_stations;
        }
        virtual ~serviceStation(){
            --total_stations;
        }

        void increase_tick() override;

        void event(EntityEvent eventtype)override;
        
        virtual void add_bots_to_queue(autobots* bot);

        virtual void release_bot_from_queue(autobots* bot);

        //getters and setters 
        serviceType get_service_type() const{return servicetype;}
        size_t get_total_capacity() const{return total_capacity;}
        const std::vector<autobots*>& get_queue() const {return service_queue;}
        bool get_has_power()const {return has_power;}
        void set_has_power(bool newBool){has_power=newBool;}
        bool get_is_servicing()const {return is_servicing;}
        void set_is_servicing(bool newBool){is_servicing=newBool;}
        void set_servicetype(serviceType newtype){servicetype=newtype;}
        
};

class chargingStation: public serviceStation{
    private:
        bool is_charging=is_servicing;
        double incoming_powerAC_watts=100; //100 W AC
        double conversion_efficiency=0.92;

    
    public:
        chargingStation(std::string name, size_t cap): serviceStation(std::move(name),cap, serviceType::CHARGING){}

        double convert_to_DC()const;

        void increase_tick() override;

        void set_is_charging(bool newBool){is_charging=newBool;}

        bool get_is_charging()const{return is_charging;}
    
};

class maintenanceStation:public serviceStation{
    private:
        bool is_repairing=is_servicing;
        double repair_per_tick=15.40; //repair health restored per tick
        double diagnostic_precision;  //Multiplier/probability of identifying critical failures (e.g., 0.95 = 95%)
        bool auto_reboot_enabled;     //automatically reboot/reset bot software upon completion
        size_t parts_inventory;

    public:
        maintenanceStation(std::string name, size_t cap, size_t initial_parts = 100): serviceStation(std::move(name), cap, serviceType::MAINTENANCE),parts_inventory(initial_parts){}

        void increase_tick() override;

        void set_is_repairing(bool newBool){is_repairing=newBool;}

        bool get_is_repairing()const{return is_repairing;}

        int get_repair_rate() const { return repair_per_tick; }


};

