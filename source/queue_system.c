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
            printf(" with order type Up");
            break;
        case HARDWARE_ORDER_INSIDE:
            printf(" with order type Inside");
            break;
        
        default:
            break;
        }
        if(order_ptr->active){
            printf(" is ACTIVE");
        }
        else{
            printf(" is DECATIVATED");
        }
        printf("\n");
        order_ptr++;
    }
    
}

void calculate_action_array(){
    if(order_array_length==0){
        return;
    }


}