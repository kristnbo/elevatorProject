#include "order_handler.h"


int check_for_order(){
    for(int i=0;i<HARDWARE_NUMBER_OF_FLOORS;i++){
        if(hardware_read_order(i,HARDWARE_ORDER_UP)){
            order_activate(i,HARDWARE_ORDER_UP);
            hardware_command_order_light(i,HARDWARE_ORDER_UP,1);
            return 1;
        }
        if(hardware_read_order(i,HARDWARE_ORDER_DOWN)){
            order_activate(i,HARDWARE_ORDER_DOWN);
            hardware_command_order_light(i,HARDWARE_ORDER_DOWN,1);
            return 1;
            
        }
        if(hardware_read_order(i,HARDWARE_ORDER_INSIDE)){
            order_activate(i,HARDWARE_ORDER_INSIDE);
            hardware_command_order_light(i,HARDWARE_ORDER_INSIDE,1);
            return 1;
          
        }
    }
    return 0;
}