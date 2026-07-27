#pragma once

#include<iostream>
#include<string>
#include<print>

enum class entityState{
    IDLE=1, //READY TO WORK, > 15% CHARGE, NO TASK STARTED
    RUNNING=2,// RUNNING, CHARGE DECREASES PER TICK
    SEEKING_CHARGER=3,//CHARGE LOW, <15% CHARGE, WILL STOP THE TASK AND STARTS CHARGING, WILL RUN TO THE CHARGING DOCK, WHICH WILL COST TICK AND CHARGE
    CHARGING=4,//CURRENTLY CHARGING, CHARGE INCREASES BY 10% PER TICK
    WAITING_FOR_RESOURCE=5,// WAITING TO BE CHARGED OR REPAIRED, 
    SEEKING_MAINTAINANCE=6,// SEEKS MAINTAINANCE DUE TO REPLACEMENT ISSUE
    FAULTED=7,// SOMETHING BROKE, GOES TO SEEK MAINTAINANCE 
    SHUTDOWN=8// TASK DONE, SHUTTING DOWN
};

enum class EntityEvent {
    EMERGENCY_STOP,
    SENSOR_OBSTRUCTION,
    MAINTENANCE_REQUIRED,
    CLEAR_FAULT
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
        int id()const{
            return id;// or this->id
        }
        const std::string name() const{
            return name;
        }
        entityState state()const{
            return state;
        }
        bool is_active()const{
            return is_active;
        }
        //setters
        entityState state(entityState newstate){
            state=newstate;
        }
        bool is_active(bool activestate){
            is_active=activestate;
        }

};