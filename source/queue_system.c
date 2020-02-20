#include "queue_system.h"
#include <stdio.h>

int order_array_length=0;

Order order_array[MAX_NUMER_OF_ORDERS];


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

void print_queue(){
    printf("\033[H\033[J"); //Clears linux terminal
    Order *order_ptr=order_array;
    for(int i = 0; i<order_array_length;i++)
    {
        printf("Floor %d",order_ptr->floor+1);
        switch (order_ptr->order_type)
        {
        case HARDWARE_ORDER_DOWN:
            printf(" with order type: Down");
            break;
        case HARDWARE_ORDER_UP:
            printf(" with order type: Up");
            break;
        case HARDWARE_ORDER_INSIDE:
            printf(" with order type: Inside");
            break;
        
        default:
            break;
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