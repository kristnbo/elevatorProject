#include "order_system.h"
#include <stdio.h> 

//The structure of this array must be preserved 
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
            return;
        }
    }
}

void order_deactivate(int floor){
    for(int i = 0; i < NUMBER_OF_ORDERS; i++){
        if(order_array[i].floor == floor){
            order_array[i].active = 0;
        }
    }
}

void order_deactivate_all(){
    for(int i = 0;i<NUMBER_OF_ORDERS;i++){
        order_array[i].active=0;
    }
}

void order_update(){
    for(int i=0;i<HARDWARE_NUMBER_OF_FLOORS;i++){
        if(hardware_read_order(i,HARDWARE_ORDER_UP)){
            order_activate(i,HARDWARE_ORDER_UP);
            hardware_command_order_light(i,HARDWARE_ORDER_UP,1);
        }
        if(hardware_read_order(i,HARDWARE_ORDER_DOWN)){
            order_activate(i,HARDWARE_ORDER_DOWN);
            hardware_command_order_light(i,HARDWARE_ORDER_DOWN,1); 
        }
        if(hardware_read_order(i,HARDWARE_ORDER_INSIDE)){
            order_activate(i,HARDWARE_ORDER_INSIDE);
            hardware_command_order_light(i,HARDWARE_ORDER_INSIDE,1);  
        }
    }

}

void print_orders(){
    printf("\033[H\033[J"); //Clears linux terminal
    for(int i = NUMBER_OF_ORDERS-1; i >-1;i--)
    {
        printf("Floor %d",order_array[i].floor+1);
        switch (order_array[i].order_type)
        {
        case HARDWARE_ORDER_DOWN:
            printf(" with order type Down");
            break;
        case HARDWARE_ORDER_UP:
            printf(" with order type Up   ");
            break;
        case HARDWARE_ORDER_INSIDE:
            printf(" with order type Inside");
            break;
        
        default:
            break;
        }
        if(order_array[i].active){
            printf(" is \t ");
            printf("\033[0;32m ACTIVE\n");
            printf("\033[0m");
        }
        else{
            printf(" is \t ");
            printf("\033[0;31m DECTIVATED\n");
            printf("\033[0m");
        }
        printf("\n");
    }
    
}

State order_request_state(State state, State last_state, int current_floor, int above){
    
    int active_order_exist = 0;
    for(int i =0;i<NUMBER_OF_ORDERS;i++){
        if(order_array[i].active){
            active_order_exist = 1;
        }
    }
    if(!active_order_exist){return STATE_IDLE;}

    //If STATE_IDLE between floors
    if(hardware_read_all_floor_sensors() == -1 && last_state==STATE_IDLE){   
        for(int i =0;i<NUMBER_OF_ORDERS;i++){
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
    //in order to not stop if elevator just left floor so floor == -1, but state == STATE_IDLE
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
        //stopping at floor to continue in same direction
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
        //continues in same direction
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
        //no further orders in same direction
        for(int i = 0; i < NUMBER_OF_ORDERS; i++){
            if(order_array[i].floor==current_floor && order_array[i].active){
                return STATE_DOOR_OPEN;
            }
        }
        //finished moving in current direction
        return STATE_IDLE;
    }
    //@Simen
    //Should default if between floors be the last_state? So that it wont turn abdruptly after leaving a floor..
    return last_state;
}







