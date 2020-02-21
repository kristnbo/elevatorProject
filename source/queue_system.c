#include "queue_system.h"
#include <stdio.h>

int order_array_length=0;

Order order_array[MAX_NUMER_OF_ORDERS]=
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

State action_array[MAX_NUMBER_OF_ACTIONS]=
{
 IDLE,
 UP,
 IDLE,
 DOWN,
 IDLE,
 IGNORE,
 IGNORE,
 IGNORE,
 IGNORE,
 IGNORE,
 IGNORE,
 IGNORE,
 IGNORE,
 IGNORE,
 IGNORE
 };



//Add order function before active variable was added
/*
void add_order(Order *order){
    Order *order_ptr=order_array;
    for(int i = 0;i < order_array_length; i++){
        if(order->floor==order_ptr->floor){
            if(order->order_type==order_ptr->order_type){
                return;
            }
        }
        order_ptr++;
    }
    order_array[order_array_length].floor=order->floor;
    order_array[order_array_length].order_type=order->order_type;
    order_array_length+=1;
    
}
*/
void add_order(Order *order){
    Order *order_ptr=order_array;
    for(int i = 0;i < MAX_NUMER_OF_ORDERS; i++){
        if(order->floor==order_ptr->floor){
            if(order->order_type==order_ptr->order_type){
                order_ptr->active=1;
                return;
            }
        }
        order_ptr++;
    }
}

void print_queue(){
    printf("\033[H\033[J"); //Clears linux terminal
    Order *order_ptr=order_array;
    for(int i = 0; i<MAX_NUMER_OF_ORDERS;i++)
    {
        printf("Floor %d",order_ptr->floor+1);
        switch (order_ptr->order_type)
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
        if(order_ptr->active){
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
        order_ptr++;
    }
    
}

void calculate_action_array(State state, State last_state, int current_floor){
    Order order_array_copy[MAX_NUMER_OF_ORDERS];
    int floor_highest;
    int floor_lowest;
    for(int i=0;i<MAX_NUMER_OF_ORDERS;i++){
        order_array_copy[i]=order_array[i];
    }
    for(Order* o=order_array_copy;o<&order_array_copy[MAX_NUMER_OF_ORDERS];o++){
        if(o->active){

            if(o->floor>floor_highest){
                floor_highest=o->floor;
            }
            if(o->floor<floor_lowest){
                floor_lowest=o->floor;
            }
        }
    }
}

State request_action(){
  for(int i =0;i<MAX_NUMBER_OF_ACTIONS;i++){
      if(action_array[i]!=IGNORE){
          State return_value=action_array[i];
          action_array[i]=IGNORE;
          return return_value;
      }
  }  
  return WAITING;


}