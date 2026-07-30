#include "autobots.hpp"

//method to decrease and update state
void autobots::decrease_charge(double amount){

    charge-=amount;

    if (charge<=0.0)
    {
        charge=0.0;
        set_state(entityState::SHUTDOWN);
        set_is_active(false);
    }
    else if (charge<15.0 && get_state()!= entityState::SEEKING_CHARGER)
    {
        set_state(entityState::SEEKING_CHARGER);
    }
}

void autobots::start_packaging_task(int total_duration_ticks){//can only start task if state is IDLE

    if (get_state()!=entityState::IDLE)
    {

        std::print("{} is busy and cannot start a new task\n", get_name());
        return;
    }
    progress_ticks=0;
    required_ticks=total_duration_ticks;
    has_active_task=true;
    set_state(entityState::RUNNING);
    
}//bot has started a task, and is running

void autobots::increase_tick(){ // used by manager class to act as a universal clock engine, where increasing ticks will change internal attributes
    if (!is_active){return;} //if bot is inactive, ticks doesn't affect its nature

    switch (get_state())//main usecase is to change the internal attribute of object like charge, task bool, ticks, based on real life scenarios and reset on completing it's task
    {
        case entityState::RUNNING:
            //change internal value
            has_active_task=true;
            progress_ticks+=1;
            decrease_charge(1.5); 

            if (get_state() != entityState::RUNNING) {break;}

            //check if task completed
            if (progress_ticks>=required_ticks){

                std::print("The task has been completed at {} ticks\n",progress_ticks);
                has_active_task=false;
                progress_ticks=0;
                required_ticks=0;
                set_state(entityState::IDLE);

            }

            break;

        case entityState::CHARGING:
            has_active_task=false;
            charge+=5.0;

            if (charge>=max_charge){
                charge=max_charge;
                set_state(entityState::IDLE);
            }
        
            break;
        
        case entityState::SEEKING_CHARGER:

            has_active_task=false;
            decrease_charge(0.5);

            if (get_state()==entityState::SHUTDOWN){break;}

            travel_ticks+=1;

            if (travel_ticks>=required_charging_travel_ticks){
                travel_ticks=0;
                set_state(entityState::CHARGING);
            }

            break;

        case entityState::SHUTDOWN:

            has_active_task=false;
            set_is_active(false);
            break;
        
        case entityState::SEEKING_MAINTAINANCE:

            has_active_task=false;
            travel_ticks+=1;
            decrease_charge(0.5);

            if (get_state()==entityState::SHUTDOWN){break;}

            if (travel_ticks>=required_maintainance_travel_ticks){
                travel_ticks=0;
                set_state(entityState::UNDER_MAINTAINANCE);
            }
            break;
        
        case entityState::UNDER_MAINTAINANCE:
        case entityState::IDLE:
        case entityState::FAULTED:
        break;

        default:
            has_active_task=false;
            break;
    }
}
// change entity internal attribute under different events
void autobots::event(EntityEvent event_type){
    switch (event_type){

        case EntityEvent::EMERGENCY_STOP:
            has_active_task=false;
            set_state(entityState::FAULTED);
            break;
    
        case EntityEvent::CANCEL_TASK:
            has_active_task=false;
            progress_ticks=0;
            required_ticks=0;
            set_state(entityState::IDLE);
            break;
        
        case EntityEvent::CLEAR_FAULT:
            if (get_state()==entityState::FAULTED){
                set_state(entityState::IDLE);
            }
            break;
        
        case EntityEvent::SENSOR_OBSTRUCTION:
            set_state(entityState::FAULTED);
            break;
        
        case EntityEvent::PAUSE_WORK:
            if (get_state()==entityState::RUNNING){
                set_state(entityState::PAUSED);
            }
            break;  
            
        case EntityEvent::RESUME_WORK:
            if (get_state()==entityState::PAUSED){
            set_state(entityState::RUNNING);
            }
            break;

        default:
            break;

    }
}



