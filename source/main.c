#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "order_system.h"
#include "timer.h"



#define DOOR_OPEN_TIME 3


static void sigint_handler(int sig){
    (void)(sig);
    printf("Terminating elevator\n");
    hardware_command_movement(HARDWARE_MOVEMENT_STOP);
    exit(0);
}


int main(){
    signal(SIGINT, sigint_handler);

//Init_routine

    int error = hardware_init();    
    if(error != 0){
        fprintf(stderr, "Unable to initialize hardware\n");
        exit(1);
    }
    
    State state = STATE_DOWN;
    State last_state = STATE_DOWN;
    int state_repeated = 0;
    int above = 1;
   
    hardware_command_movement(HARDWARE_MOVEMENT_DOWN);
    int current_floor = hardware_read_all_floor_sensors();
    while(current_floor == -1){
        current_floor = hardware_read_all_floor_sensors();
    }

    last_state = state;
    state = STATE_IDLE;
    hardware_command_movement(HARDWARE_MOVEMENT_STOP);

//end init_routine

    while(1){
        if(hardware_read_stop_signal()){
            if(state == STATE_UP || state == STATE_DOWN){
                last_state=state;
                
            }
            state = STATE_EMERGENCY_STOP;
            state_repeated = 0; 
        }

        int temp = hardware_read_all_floor_sensors();
        if(temp != -1){
            current_floor = temp;
        }

        order_update();
    
        switch (state)
        {
        case STATE_UP:
            hardware_command_movement(HARDWARE_MOVEMENT_UP);
            if(hardware_read_all_floor_sensors() != -1){
                above = 1; 
            }
            
            if(hardware_read_all_floor_sensors() != -1){    
                last_state = state;
                state  = STATE_IDLE;
            }
            break;

        case STATE_DOWN:
            hardware_command_movement(HARDWARE_MOVEMENT_DOWN);
            if(hardware_read_all_floor_sensors() != -1){
                above = 0; 
            }

            if(hardware_read_all_floor_sensors() != -1){
                last_state = state;
                state = STATE_IDLE;
            }
            break;

        case STATE_DOOR_OPEN:
            hardware_command_movement(HARDWARE_MOVEMENT_STOP);
            order_deactivate(current_floor);

            if(!state_repeated){
                state_repeated = 1;
                timer_start(DOOR_OPEN_TIME);
                hardware_command_door_open(1);
            }
            if(timer_check_timeout()){
                hardware_command_door_open(0);
                state = STATE_IDLE;
                state_repeated = 0;

            }
            else if(hardware_read_obstruction_signal()){
                timer_start(DOOR_OPEN_TIME);
                hardware_command_door_open(1);
            }
            break;

        case STATE_EMERGENCY_STOP:
            hardware_command_movement(HARDWARE_MOVEMENT_STOP);
            hardware_command_stop_light(hardware_read_stop_signal());

            if(hardware_read_all_floor_sensors() != -1 && !state_repeated){
                timer_start(DOOR_OPEN_TIME);
                hardware_command_door_open(1);
                state_repeated = 1;
            }

            if(hardware_read_obstruction_signal() && !timer_check_timeout()){
                timer_start(DOOR_OPEN_TIME);
            }

            order_deactivate_all();
            if(!hardware_read_stop_signal() && timer_check_timeout()){
                state = STATE_IDLE;
                last_state = STATE_IDLE;
                state_repeated = 0;
                hardware_command_stop_light(0);
                hardware_command_door_open(0);
            }
            break;

        case STATE_IDLE:
            hardware_command_floor_indicator_on(current_floor);
            if(state_repeated){
                last_state = STATE_IDLE;
                hardware_command_movement(HARDWARE_MOVEMENT_STOP);
            }
            
            state_repeated = 1;
            state = order_request_state(last_state, current_floor, above);
            if (state != STATE_IDLE){
                state_repeated = 0;
            }
            break;

        default:
            break;
        }
    }
    return 0;
}
