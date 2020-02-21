#include "queue_system.h"
#include <stdio.h>

int order_array_length=0;

Order order_array[MAX_NUMBER_OF_ORDERS]=
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
 IGNORE,
 IGNORE,
 IGNORE,
 IGNORE,
 IGNORE,
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
    for(int i = 0;i < MAX_NUMBER_OF_ORDERS; i++){
        if(order->floor==order_ptr->floor){
            if(order->order_type==order_ptr->order_type){
                order_ptr->active=1;
                return;
            }
        }
        order_ptr++;
    }
}

void remove_order(int current_floor, Order *order_array){
    for(Order* o=order_array;o<&order_array[MAX_NUMBER_OF_ORDERS];o++){
        if(o->floor==current_floor){o->active=0;}
    }
}

void clear_order(int current_floor){
    for(Order* o=order_array;o<&order_array[MAX_NUMBER_OF_ORDERS];o++){
        if(o->floor==current_floor){o->active=0;}
    }
}

void print_queue(){
    printf("\033[H\033[J"); //Clears linux terminal
    Order *order_ptr=order_array;
    for(int i = 0; i<MAX_NUMBER_OF_ORDERS;i++)
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
    for(int i=0;i<MAX_NUMBER_OF_ACTIONS;i++){
        action_array[i]=IGNORE;
    }
    
    Order order_array_copy[MAX_NUMBER_OF_ORDERS];
    int floor_highest;
    int floor_lowest;
    int num_actions=0;
    for(int i=0;i<MAX_NUMBER_OF_ORDERS;i++){
        order_array_copy[i]=order_array[i];
    }
    for(int i=0;i<MAX_NUMBER_OF_ORDERS;i++){
        if(order_array_copy[i].active){

            if(order_array_copy[i].floor>floor_highest){
                floor_highest=order_array_copy[i].floor;
            }
            if(order_array_copy[i].floor<floor_lowest){
                floor_lowest=order_array_copy[i].floor;
            }
        }
    }
    //Hvis vi er på vei opp
    if(state!=DOWN && last_state!=DOWN){
        for(int f=current_floor;f<floor_highest;f++){
            int stopped=0;
            for(int i=3*f;i<3*f+3;i++){
                if(order_array_copy[i].active && order_array_copy[i].order_type!=HARDWARE_ORDER_DOWN){
                    action_array[num_actions]=IDLE;
                    num_actions+=1;
                    action_array[num_actions]=UP;
                    num_actions+=1;
                    remove_order(f,order_array_copy);
                    stopped=1;
                }
            }
            if(!stopped){
                action_array[num_actions]=UP;
                num_actions+=1;
                }
        }
        action_array[num_actions]=IDLE;
        num_actions+=1;
        action_array[num_actions]=DOWN;
        for(int f=floor_highest;f>floor_lowest;f--){
            int stopped=0;
            for(int i=3*f;i>3*f+3;i++){
                if(order_array_copy[i].active && order_array_copy[i].order_type!=HARDWARE_ORDER_UP){
                    action_array[num_actions]=IDLE;
                    num_actions+=1;
                    action_array[num_actions]=DOWN;
                    num_actions+=1;
                    remove_order(f,order_array_copy);
                    stopped=1;
                }
            }
            if(!stopped){
                action_array[num_actions]=DOWN;
                num_actions+=1;
                }
        }
        action_array[num_actions]=IDLE;
        num_actions+=1;
        action_array[num_actions]=UP;
        for(int f=floor_lowest;f<current_floor;f++){
            int stopped=0;
            for(int i=3*f;i>3*f+3;i++){
                if(order_array_copy[i].active){
                    action_array[num_actions]=IDLE;
                    num_actions+=1;
                    action_array[num_actions]=UP;
                    num_actions+=1;
                    remove_order(f,order_array_copy);
                    stopped=1;
                }
            }
            if(!stopped){
                action_array[num_actions]=UP;
                num_actions+=1;
                }
        }
       action_array[num_actions-1]=IGNORE;

    }

    //Ellers hvis vi er på vei ned eller venter
    else{

    }
    action_array[num_actions-1]=IGNORE;

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

void print_actions(){
    for(int i = 0; i < MAX_NUMBER_OF_ACTIONS; i++){
        switch (action_array[i])
        {
        case UP:
            printf("UP\t");
            break;
        case DOWN:
            printf("DOWN\t");
            break;
        case IDLE:
            printf("IDLE\t");
            break;

        default:
            break;
        }
    }
    printf("\n");
}