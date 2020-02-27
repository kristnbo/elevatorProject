#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "order_handler.h"
#include "timer.h"



#define DOOR_OPEN_TIME 1
//Not part of skeleton code, our FSM 
//State defined in queue_system
/*
typedef enum {
    UP,
    DOWN,
    DOOR_OPEN,
    IDLE,
    EMERGENCY_STOP   
} State;

*/

static void clear_all_order_lights(){
    HardwareOrder order_types[3] = {
        HARDWARE_ORDER_UP,
        HARDWARE_ORDER_INSIDE,
        HARDWARE_ORDER_DOWN
    };

    for(int f = 0; f < HARDWARE_NUMBER_OF_FLOORS; f++){
        for(int i = 0; i < 3; i++){
            HardwareOrder type = order_types[i];
            hardware_command_order_light(f, type, 0);
        }
    }
}
//Our function
static void clear_floor_order_lights(int floor){
    HardwareOrder order_types[3] = {
        HARDWARE_ORDER_UP,
        HARDWARE_ORDER_INSIDE,
        HARDWARE_ORDER_DOWN
    };

    
    for(int i = 0; i < 3; i++){
        HardwareOrder type = order_types[i];
        hardware_command_order_light(floor, type, 0);
    }  
}

static void sigint_handler(int sig){
    (void)(sig);
    printf("Terminating elevator\n");
    hardware_command_movement(HARDWARE_MOVEMENT_STOP);
    exit(0);
}

static void print_status(int current_floor,State last_state, State state, int show){
    if(show){
        print_orders();
        switch (last_state)
        {
        case UP:
            printf("Last state: UP");
            break;
        case DOWN:
            printf("Last state: DOWN");
            break;
        case DOOR_OPEN:
            printf("Last state: DOOR_OPEN");
            break;
        case IDLE:
            printf("Last state: IDLE");
            break;
        case EMERGENCY_STOP:
            printf("Last state: EMERGENCY_STOP");
            break;
        
        default:
            break;
        }
        switch (state)
        {
        case UP:
            printf("\tCurrent state: UP");
            break;
        case DOWN:
            printf("\tCurrent state: DOWN");
            break;
        case DOOR_OPEN:
            printf("\tCurrent state: DOOR_OPEN");
            break;
        case IDLE:
            printf("\tCurrent state: IDLE");
            break;
        case EMERGENCY_STOP:
            printf("\tCurrent state: EMERGENCY_STOP");
            break;
        
        default:
            break;
        }
        printf("\t Current floor: %d.floor\n",current_floor+1);
    }
}  


int main(){
    signal(SIGINT, sigint_handler);

//Init_routine

    int error = hardware_init();    
    if(error != 0){
        fprintf(stderr, "Unable to initialize hardware\n");
        exit(1);
    }

    State state = DOWN;
    State last_state = DOWN;
    int state_repeated = 0;
    int above = 1;
   
    hardware_command_movement(HARDWARE_MOVEMENT_DOWN);
    int current_floor = hardware_command_get_floor();
    while(current_floor == -1){
        current_floor = hardware_command_get_floor();
    }
    last_state = state;
    state = IDLE;
    hardware_command_movement(HARDWARE_MOVEMENT_STOP);

//end init_routine

    while(1){
        //Check stop signal
        if(hardware_read_stop_signal()){
            if(state == UP || state == DOWN){
                last_state=state;
                
            }
            state = EMERGENCY_STOP;
            state_repeated=0; 
        }

        //Sets floor and floor lights
        int temp = hardware_command_get_floor();
        if(temp != -1){
            current_floor = temp;
        }


        //Check all order buttons
        order_update();
        
        //FSM 
        switch (state)
        {
        case UP:
            hardware_command_movement(HARDWARE_MOVEMENT_UP);
            if(hardware_command_get_floor() != -1){
                above = 1; 
            }
            
            if(hardware_command_get_floor() != -1){    
                last_state = state;
                state  = IDLE;
            }
            break;

        case DOWN:
            hardware_command_movement(HARDWARE_MOVEMENT_DOWN);
            if(hardware_command_get_floor() != -1){
                above = 0; 
            }

            if(hardware_command_get_floor() != -1){
                last_state = state;
                state = IDLE;
            }
            break;

        case DOOR_OPEN:
            hardware_command_movement(HARDWARE_MOVEMENT_STOP);
            order_deactivate(current_floor);
            clear_floor_order_lights(current_floor);

            if(!state_repeated){
                state_repeated = 1;
                timer_start(DOOR_OPEN_TIME);
                hardware_command_door_open(1);
            }
            if(check_timeout()){
                hardware_command_door_open(0);
                state = IDLE;
                state_repeated = 0;

            }
            else if(hardware_read_obstruction_signal()){
                timer_start(DOOR_OPEN_TIME);
                hardware_command_door_open(1);
            }
            break;

        case EMERGENCY_STOP:
            hardware_command_movement(HARDWARE_MOVEMENT_STOP);
            hardware_command_stop_light(hardware_read_stop_signal());

            if(hardware_command_get_floor() != -1 && !state_repeated){
                timer_start(DOOR_OPEN_TIME);
                hardware_command_door_open(1);
                state_repeated = 1;
            }

            if(hardware_read_obstruction_signal() && !check_timeout()){
                timer_start(DOOR_OPEN_TIME);
            }

            order_deactivate_all();
            clear_all_order_lights();
            if(!hardware_read_stop_signal() && check_timeout()){
                state = IDLE;
                state_repeated = 0;
                hardware_command_stop_light(0);
                hardware_command_door_open(0);
            }
            break;

        case IDLE:
            hardware_command_floor_indicator_on(current_floor);
            if(state_repeated){
                last_state = IDLE;
                hardware_command_movement(HARDWARE_MOVEMENT_STOP);
            }
            
            state_repeated = 1;
            state = request_state(state, last_state, current_floor, above);
            if (state != IDLE){
                state_repeated = 0;
            }
            break;

        default:
            break;
        }
 

        //Terminal sugar
        print_status(current_floor,last_state,state,1);
        
    }

    return 0;
}
