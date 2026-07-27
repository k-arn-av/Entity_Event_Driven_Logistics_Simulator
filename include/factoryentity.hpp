#pragma once

#include<iostream>
#include<string>
#include<print>

enum class entityState{
    IDLE=1,                //READY TO WORK, > 15% CHARGE, NO TASK STARTED
    RUNNING=2,             // RUNNING, CHARGE DECREASES PER TICK
    SEEKING_CHARGER=3,     //CHARGE LOW, <15% CHARGE, WILL STOP THE TASK AND STARTS CHARGING, WILL RUN TO THE CHARGING DOCK, WHICH WILL COST TICK AND CHARGE
    CHARGING=4,            //CURRENTLY CHARGING, CHARGE INCREASES BY 10% PER TICK
    UNDER_MAINTAINANCE=5,  // WAITING TO BE REPAIRED, 
    SEEKING_MAINTAINANCE=6,// SEEKS MAINTAINANCE DUE TO REPLACEMENT ISSUE
    FAULTED=7,             // SOMETHING BROKE, GOES TO SEEK MAINTAINANCE 
    SHUTDOWN=8,            // TASK DONE OR LOW CHARGE, SHUTTING DOWN
    PAUSED=9               // PAUSED, TASK NOT DONE
};

enum class EntityEvent {
    EMERGENCY_STOP,     // Operator hits E-Stop; forces entity into FAULTED
    SENSOR_OBSTRUCTION, // External environment/sensor reports a physical blockage
    CLEAR_FAULT,        // Operator manually resets an error state after resolving an issue
    PAUSE_WORK,         // Operator or central system temporarily pauses the current task
    RESUME_WORK,        // Operator commands the bot to unpause and continue task
    CANCEL_TASK         // Operator revokes the assigned job, forcing return to IDLE
};

class Factoryentity{

    protected:
        int id;
        std::string name;
        bool is_active;
        entityState state;
        inline static int total_count=0;
        
    public:
        Factoryentity(const std::string given_name): is_active(true), state(entityState::IDLE){
            ++total_count;
            id=total_count;
            name=given_name + "_" + std::to_string(id);
        }// name: bot_1 , status:IDLE, id: 1, total count=1, is_active=true.
        virtual ~Factoryentity(){
            --total_count;
            std::print("The floor device {} has been removed", name);
        }

        virtual void increase_tick()=0;  // virtual functions set to 0, meaning the other derived module MUST use this function or else wont compile 
        virtual void event(EntityEvent event_type)=0;

        //getters
        int get_id()const{
            return id;// or this->id
        }
        const std::string get_name() const{
            return name;
        }
        entityState get_state()const{
            return state;
        }
        bool get_is_active()const{
            return is_active;
        }
        //setters
        entityState set_state(entityState newstate){
            state=newstate;
        }
        bool set_is_active(bool activestate){
            is_active=activestate;
        }

};