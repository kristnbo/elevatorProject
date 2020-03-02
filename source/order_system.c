#include "order_system.h"
#include <stdio.h> 

Order order_array[NUMBER_OF_ORDERS]=
{
    {0,HARDWARE_ORDER_UP,0},
    {0,HARDWARE_ORDER_INSIDE,0},
    {1,HARDWARE_ORDER_UP,0},
    {1,HARDWARE_ORDER_DOWN,0},
    {1,HARDWARE_ORDER_INSIDE,0},
    {2,HARDWARE_ORDER_UP,0},
    {2,HARDWARE_ORDER_DOWN,0},
    {2,HARDWARE_ORDER_INSIDE,0},
    {3,HARDWARE_ORDER_DOWN,0},
    {3,HARDWARE_ORDER_INSIDE,0},
    
};

void order_activate(int floor, HardwareOrder order_type){
    for(int i = 0; i < NUMBER_OF_ORDERS; i++){
        if(order_array[i].floor == floor && order_array[i].order_type == order_type){
            order_array[i].active = 1;
            hardware_command_order_light(floor, order_type, 1);
            return;
        }
    }
}

void order_deactivate(int floor){
    for(int i = 0; i < NUMBER_OF_ORDERS; i++){
        if(order_array[i].floor == floor){
            order_array[i].active = 0;
            hardware_command_order_light(floor, order_array[i].order_type, 0);
        }
    }
}

void order_deactivate_all(){
    for(int i = 0; i < NUMBER_OF_ORDERS; i++){
        order_array[i].active = 0;
        hardware_command_order_light(order_array[i].floor, order_array[i].order_type, 0);
    }
}

void order_update(){
    for(int i=0;i<HARDWARE_NUMBER_OF_FLOORS;i++){
        if(hardware_read_order(i, HARDWARE_ORDER_UP)){
            order_activate(i, HARDWARE_ORDER_UP);
        }
        if(hardware_read_order(i, HARDWARE_ORDER_DOWN)){
            order_activate(i, HARDWARE_ORDER_DOWN);
        }
        if(hardware_read_order(i,HARDWARE_ORDER_INSIDE)){
            order_activate(i, HARDWARE_ORDER_INSIDE);  
        }
    }

}

State order_request_state(State last_state, int current_floor, int above){
    
    int active_order_exist = 0;
    for(int i =0;i<NUMBER_OF_ORDERS;i++){
        if(order_array[i].active){
            active_order_exist = 1;
        }
    }
    if(!active_order_exist){return STATE_IDLE;}


    if(hardware_read_all_floor_sensors() == -1 && last_state == STATE_IDLE){   
        for(int i = 0; i < NUMBER_OF_ORDERS; i++){
            if(order_array[i].active){
                if(order_array[i].floor > current_floor){
                    return STATE_UP;
                }
                if(order_array[i].floor < current_floor){
                    return STATE_DOWN;
                }
                if (above){
                    return STATE_DOWN;
                }
                return STATE_UP;
            }
        }   
    }


    //If the elevator just left the floor sensor range, so floor == -1, state could be STATE_IDLE, but should be a moving state.
    if(hardware_read_all_floor_sensors() == -1){
        return last_state;
    }   
    if(hardware_read_all_floor_sensors() != -1){
        if(last_state == STATE_IDLE){
            for(int i = 0; i < NUMBER_OF_ORDERS; i++){
                if(order_array[i].active){
                    if(order_array[i].floor > current_floor){
                        return STATE_UP;
                    }
                    if(order_array[i].floor < current_floor){
                        return STATE_DOWN;
                    }
                    return STATE_DOOR_OPEN;
                }
            }
        }
        //Stopping at floor to continue in same direction
        for(int i = 0; i < NUMBER_OF_ORDERS; i++){
            if(order_array[i].floor == current_floor && order_array[i].active){
                if(last_state == STATE_UP && order_array[i].order_type != HARDWARE_ORDER_DOWN){
                    return STATE_DOOR_OPEN;
                }
                if(last_state == STATE_DOWN && order_array[i].order_type != HARDWARE_ORDER_UP){
                    return STATE_DOOR_OPEN;
                }
            }
        }
        //Continues in same direction without stopping
        for(int i = 0; i < NUMBER_OF_ORDERS; i++){
            if(order_array[i].active){
                if(last_state == STATE_DOWN && order_array[i].floor < current_floor){
                    return STATE_DOWN;
                }
                if(last_state == STATE_UP && order_array[i].floor > current_floor){
                    return STATE_UP;
                }
            }
        }
        //No further orders in the current direction
        for(int i = 0; i < NUMBER_OF_ORDERS; i++){
            if(order_array[i].floor==current_floor && order_array[i].active){
                return STATE_DOOR_OPEN;
            }
        }
    }
    //The elevator has to repeat STATE_IDLE in order to turn
    return STATE_IDLE;
}







