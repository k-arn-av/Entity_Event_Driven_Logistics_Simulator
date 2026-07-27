#pragma once
#include "factoryentity.hpp"

class autobots: public Factoryentity{

    private:
        double charge;
        double max_charge=100;

        //task attributes with ticks
        bool has_active_task=false;
        int progress_ticks=0;
        int required_ticks=0;

        //movement ticks
        int travel_ticks=0;
        int required_charging_travel_ticks=5;
        int required_maintainance_travel_ticks=6;

        inline static int total_botscount=0;

        void decrease_charge(double amount);//decreases charge internally. No need for user or the bots to manipulate the drain. Only class actions can access it

    public:
        autobots(double initial_charge, const std::string& name): Factoryentity(name), charge(initial_charge){
            ++total_botscount;
        }
        ~autobots(){
            --total_botscount;//calling destructor decreases total count to match object destruction
        }
        //inherited polymorphic functions 
        void increase_tick()override;// will be used in .cpp to be able to do tasks under a universal clock(tick)
        void event(EntityEvent eventtype) override; //events called by user to affect its state

        // bot tasks
        void start_packaging_task(int total_duration_ticks);// sets the total duration for each packaging task 
        // and changes the internal attributes like state, is_active, has_active_task, required ticks

        //getters and setters
        double get_charge()const{
            return charge;
        }
};      