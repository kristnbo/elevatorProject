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
    for(int i=0;i<MAX_NUMBER_OF_ORDERS;i++){
        if(order_array[i].active){
            floor_lowest=order_array[i].floor;
            floor_highest=order_array[i].floor;
            break;
            }
    }
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
    printf("Highest: %d\n",floor_highest+1);
    printf("Current: %d\n",current_floor+1);
    //Hvis vi må oppover først, da blir lengste mulige opp-ned-opp
    if(state!=DOWN && last_state!=DOWN && floor_highest>current_floor){

        if(floor_highest-current_floor>1){
        //Siden det er etasjer mellom
        //der du er og dit du skal
        //må du sjekke om du må stoppe noe sted på veien
            for(int f=current_floor+1;f<floor_highest;f++){
                printf("Checking floor: %d\n",f+1);
                int stopped = 0;
                for(int i=0;i<3;i++){
                    if(order_array_copy[3*f+i].active&&order_array_copy[3*f+i].order_type!=HARDWARE_ORDER_DOWN){
                        action_array[num_actions]=IGNORE;
                        num_actions+=1;
                        action_array[num_actions]=IDLE;
                        num_actions+=1;
                        action_array[num_actions]=UP;
                        num_actions+=1;
                        remove_order(f,order_array_copy);
                        stopped =1;
                        printf("Should stop at floor: %d\n",f+1);
                    }
                }
                if(!stopped){
                    action_array[num_actions]=IGNORE;
                    num_actions+=1;
                    action_array[num_actions]=UP;
                    num_actions+=1;
                }
            }
            printf("Should stop at: %d\n", floor_highest+1);
            action_array[num_actions]=IDLE;
            num_actions+=1;
            action_array[num_actions]=DOWN;
            num_actions+=1;
            remove_order(floor_highest,order_array_copy);

        }
        else{
            //Skjønner ikke hvorfor denne ignoren trengs, men funker
            action_array[num_actions]=IGNORE;
            num_actions+=1;
            action_array[num_actions]=IDLE;
            num_actions+=1;
            action_array[num_actions]=DOWN;
            num_actions+=1;
        }
        //Nå har du vært så høyt du skal, nå må du så lavt du skal,
        //Men må igjen sjekke om du skal stoppe på veien
        for(int f=floor_highest-1;f>floor_lowest;f--){
            int stopped = 0;
            for(int i=0;i<3;i++){
                if(order_array_copy[3*f+i].active&&order_array_copy[3*f+i].order_type!=HARDWARE_ORDER_UP){
                    action_array[num_actions]=IGNORE;
                    num_actions+=1;
                    action_array[num_actions]=IDLE;
                    num_actions+=1;
                    action_array[num_actions]=DOWN;
                    num_actions+=1;
                    remove_order(f,order_array_copy);
                    stopped =1;
                    printf("Should stop at floor: %d\n",f+1);
                    printf("Lowest: %d",floor_lowest);
                }
            }
            if(!stopped){
                action_array[num_actions]=IGNORE;
                num_actions+=1;
                action_array[num_actions]=DOWN;
                num_actions+=1;
            }
        }
        printf("Should stop at: %d\n", floor_highest+1);
        for(int i=0;i<MAX_NUMBER_OF_ORDERS;i++){
            if(order_array_copy[i].active){
                action_array[num_actions]=IDLE;
                num_actions+=1;
                action_array[num_actions]=UP;
                num_actions+=1;
                break;
            }
        }

        //Nå må starten av handlingsruten settes rikitg basert på state
        if(state==IDLE||state==WAITING){
            if(action_array[0]==IDLE){action_array[0]=IGNORE;}
            else{action_array[0]=UP;}
        }
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