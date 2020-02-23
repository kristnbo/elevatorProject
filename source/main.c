#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "order_handler.h"
#include "timer.h"

//Not part of skeleton code, our FSM 
//State defined in queue_system
/*
typedef enum {
    UP,
    DOWN,
    IDLE,
    WAITING,
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
        case IDLE:
            printf("Last state: IDLE");
            break;
        case WAITING:
            printf("Last state: WAITING");
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
        case IDLE:
            printf("\tCurrent state: IDLE");
            break;
        case WAITING:
            printf("\tCurrent state: WAITING");
            break;
        case EMERGENCY_STOP:
            printf("\tCurrent state: EMERGENCY_STOP");
            break;
        
        default:
            break;
        }
        printf("\t Current floor: %d.floor\n",current_floor+1);
        printf("Action array:\t");
        print_actions();
    }
}  


int main(){
    int error = hardware_init();    
    if(error != 0){
        fprintf(stderr, "Unable to initialize hardware\n");
        exit(1);
    }

    signal(SIGINT, sigint_handler);


    //Our code
    State state = DOWN;
    State last_state = DOWN;
    int state_repeated = 0;

    //Elevator init
    hardware_command_movement(HARDWARE_MOVEMENT_DOWN);
    int current_floor=hardware_get_floor();
    while(current_floor == -1){
        current_floor = hardware_get_floor();
    }
    last_state = state;
    state = WAITING;
    hardware_command_movement(HARDWARE_MOVEMENT_STOP);



    while(1){
        //Check stop signal
        if(hardware_read_stop_signal()){
            if(state!=EMERGENCY_STOP && state!=WAITING){
                last_state=state;
            }
            state = EMERGENCY_STOP;
        }
        //Check all order buttons and calculate
        if(check_for_order()){
            calculate_action_array(state,last_state,current_floor);
        }
        
        //FSM 
        switch (state)
        {
        case UP:
            hardware_command_movement(HARDWARE_MOVEMENT_UP);

            if(hardware_get_floor() != -1 &&hardware_get_floor() != current_floor){
                last_state = state;
                state  = request_action();
            }
            break;
        case DOWN:
            hardware_command_movement(HARDWARE_MOVEMENT_DOWN);

            if(hardware_get_floor() != -1&& hardware_get_floor() != current_floor){
                last_state = state;
                state = request_action();
            }
            break;
        case IDLE:
            hardware_command_movement(HARDWARE_MOVEMENT_STOP);

            clear_order(current_floor);
            clear_floor_order_lights(current_floor);
            if(!state_repeated){
                state_repeated = 1;
                timer_start(3);
                hardware_command_door_open(1);
            }
            if(check_timeout() == 1){
                hardware_command_door_open(0);
                state = request_action();
                if(state != WAITING){
                    last_state=IDLE;
                }
                state_repeated = 0;

            }
            else if(hardware_read_obstruction_signal() == 1){
                timer_start(3);
                hardware_command_door_open(1);
            }

            break;
        case EMERGENCY_STOP:
            hardware_command_movement(HARDWARE_MOVEMENT_STOP);

            if(hardware_get_floor() != -1){
                timer_start(3);
                hardware_command_door_open(1);
            }
            hardware_command_stop_light(1);
            
            clear_all_orders();
            clear_all_order_lights();
            clear_all_actions();
            if(hardware_read_stop_signal() == 0){
                state=WAITING;
            }
            break;
        case WAITING:
            hardware_command_movement(HARDWARE_MOVEMENT_STOP);

            hardware_command_order_light(current_floor,HARDWARE_ORDER_INSIDE,0);
            if(hardware_read_obstruction_signal()){
                timer_start(3);
            }
            if(check_timeout()){
                hardware_command_door_open(0);
            }
            hardware_command_stop_light(0);
            
            calculate_action_array(state,last_state,current_floor);
            if(check_timeout()){
                state = request_action();
            }
            
            //If the elevator has turned between floors
            if(state != WAITING&& state!=last_state){
                if(last_state==UP){
                    current_floor+=1;
                }
                if(last_state==DOWN){
                    current_floor-=1;
                }
            }
            break;
        default:
            break;
        }
        //Update floor and floor lights
        if(hardware_get_floor() != -1){
            current_floor=hardware_get_floor();
            hardware_command_floor_indicator_on(current_floor);
        }
        

        //Update all order lights
        for(int f = 0; f < HARDWARE_NUMBER_OF_FLOORS; f++){
            if(hardware_read_order(f, HARDWARE_ORDER_INSIDE)){
                hardware_command_order_light(f, HARDWARE_ORDER_INSIDE, 1);
            }

            if(hardware_read_order(f, HARDWARE_ORDER_UP)){
                hardware_command_order_light(f, HARDWARE_ORDER_UP, 1);
            }

            if(hardware_read_order(f, HARDWARE_ORDER_DOWN)){
                hardware_command_order_light(f, HARDWARE_ORDER_DOWN, 1);
            }
        }

        //Terminal sugar
        print_status(current_floor,last_state,state,1);
        
    }

    return 0;
}
