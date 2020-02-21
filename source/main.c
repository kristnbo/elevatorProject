#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "order_handler.h"
#include "timer.h"


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

static void sigint_handler(int sig){
    (void)(sig);
    printf("Terminating elevator\n");
    hardware_command_movement(HARDWARE_MOVEMENT_STOP);
    exit(0);
}



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
State state=UP;
State last_state;
int current_floor=-1;



int main(){
    int error = hardware_init();
    
    

    
    if(error != 0){
        fprintf(stderr, "Unable to initialize hardware\n");
        exit(1);
    }

    signal(SIGINT, sigint_handler);

    printf("=== Example Program ===\n");
    printf("Press the stop button on the elevator panel to exit\n");

    //hardware_command_movement(HARDWARE_MOVEMENT_UP);


 
    
    while(1){
        
        if(hardware_get_floor()!=-1){
            if(current_floor!=hardware_get_floor()||state==IDLE||state==WAITING){
                if(check_timeout()==1){
                    hardware_command_door_open(0);
                    last_state=state;
                    state=request_action();
                    if(state==IDLE){
                        timer_start(3);
                        hardware_command_door_open(1);
                    }
                }
            }
            current_floor=hardware_get_floor();
        }
        switch (state)
        {
        case UP:
            hardware_command_movement(HARDWARE_MOVEMENT_UP);
            break;
        case DOWN:
            hardware_command_movement(HARDWARE_MOVEMENT_DOWN);
            break;
        case IDLE:
            hardware_command_movement(HARDWARE_MOVEMENT_STOP);
            break;
        case EMERGENCY_STOP:
            hardware_command_movement(HARDWARE_MOVEMENT_STOP);
            break;
        case WAITING:
            hardware_command_movement(HARDWARE_MOVEMENT_STOP);
            break;
        
        default:
            break;
        }
        check_for_order();
        //print_queue();



        /* All buttons must be polled, like this: */
        for(int f = 0; f < HARDWARE_NUMBER_OF_FLOORS; f++){
            if(hardware_read_order(f, HARDWARE_ORDER_INSIDE)){
                hardware_command_floor_indicator_on(f);
            }
        }

        /* Lights are set and cleared like this: */
        for(int f = 0; f < HARDWARE_NUMBER_OF_FLOORS; f++){
            /* Internal orders */
            if(hardware_read_order(f, HARDWARE_ORDER_INSIDE)){
                hardware_command_order_light(f, HARDWARE_ORDER_INSIDE, 1);
            }

            /* Orders going up */
            if(hardware_read_order(f, HARDWARE_ORDER_UP)){
                hardware_command_order_light(f, HARDWARE_ORDER_UP, 1);
            }

            /* Orders going down */
            if(hardware_read_order(f, HARDWARE_ORDER_DOWN)){
                hardware_command_order_light(f, HARDWARE_ORDER_DOWN, 1);
            }
        }

        if(hardware_read_obstruction_signal()){
            hardware_command_stop_light(1);
            clear_all_order_lights();
        }
        else{
            hardware_command_stop_light(0);
        }
    }

    return 0;
}
