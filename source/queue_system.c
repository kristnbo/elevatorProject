#include "queue_system.h"
#include <stdio.h> 

Order order_array[NUMBER_OF_ORDERS]=
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
 IGNORE,
 IGNORE,
 IGNORE,
 IGNORE,
 IGNORE,
 IGNORE
 };

void order_activate(Order *order){
    for(int i = 0;i < NUMBER_OF_ORDERS; i++){
        if(order->floor==order_array[i].floor){
            if(order->order_type==order_array[i].order_type){
                order_array[i].active=1;
                return;
            }
        }
    }
}

void order_deactivate(int current_floor, Order *order_array){
    for(Order* o=order_array;o<&order_array[NUMBER_OF_ORDERS];o++){
        if(o->floor==current_floor){o->active=0;}
    }
}

void clear_order(int current_floor){
    for(Order* o=order_array;o<&order_array[NUMBER_OF_ORDERS];o++){
        if(o->floor==current_floor){o->active=0;}
    }
}

void print_orders(){
    printf("\033[H\033[J"); //Clears linux terminal
    Order *order_ptr=order_array;
    for(int i = 0; i<NUMBER_OF_ORDERS;i++)
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
    //Tømmer action_array
    for(int i=0;i<MAX_NUMBER_OF_ACTIONS;i++){
        action_array[i]=IGNORE;
    }
    int num_actions=0;

    //Lager en kopi av ordre og sjekker at det er aktive ordre
    int active_orders=0;
    Order order_array_copy[NUMBER_OF_ORDERS];
    for(int i=0;i<NUMBER_OF_ORDERS;i++){
        order_array_copy[i]=order_array[i];
        if(order_array[i].active){active_orders=1;}
    }
    if(!active_orders){return;}

    //Finner øverste og laveste ønskede etasje
    int floor_highest;
    int floor_lowest;
    for(int i=0;i<NUMBER_OF_ORDERS;i++){
        if(order_array[i].active){
            floor_lowest=order_array[i].floor;
            floor_highest=order_array[i].floor;
            break;
            }
    }
    for(int i=0;i<NUMBER_OF_ORDERS;i++){
        if(order_array_copy[i].active){

            if(order_array_copy[i].floor>floor_highest){
                floor_highest=order_array_copy[i].floor;
            }
            if(order_array_copy[i].floor<floor_lowest){
                floor_lowest=order_array_copy[i].floor;
            }
        }
    }

    //If doors already open in a requested floor or if waiting at a floor with doors closed
    if((state==IDLE && last_state != WAITING) || (state == WAITING && hardware_get_floor() != -1)){
        for(int i =0;i<NUMBER_OF_ORDERS;i++){
            if(order_array_copy[i].floor==current_floor&&order_array_copy[i].active){
                if(order_array_copy[i].order_type==HARDWARE_ORDER_INSIDE){
                    order_array_copy[i].active=0;
                    order_deactivate(current_floor,order_array_copy);
                    //should be done another place
                    order_deactivate(current_floor,order_array);
                }
                else
                {
                    order_deactivate(current_floor,order_array_copy);
                    action_array[num_actions]=IDLE;
                    num_actions+=1;
                }
                
            }
        }   
    }

    //If waiting between floors 
    if((state == WAITING && hardware_get_floor() == -1)){
        for(int i =0;i<NUMBER_OF_ORDERS;i++){
            if(order_array_copy[i].floor==current_floor&&order_array_copy[i].active){
                if(last_state == UP){
                    action_array[num_actions]=DOWN;
                    num_actions+=1;
                    action_array[num_actions]=IDLE;
                    num_actions+=1;
                    order_deactivate(current_floor,order_array_copy);
                }
                if(last_state == DOWN){
                    action_array[num_actions]=UP;
                    num_actions+=1;
                    action_array[num_actions]=IDLE;
                    num_actions+=1;
                    order_deactivate(current_floor,order_array_copy);
                }
                
            }
        }   
    }
    
    

   
  
    //Sjekker om vi skal oppover først
    if(((state!=DOWN && last_state!=DOWN) || state == WAITING) && floor_highest>current_floor){
        //Er det etasjer mellom der vi er og topp etasjen
        if(floor_highest-current_floor>1){
            //Siden det er etasjer mellom
            //der du er og dit du skal
            //må du sjekke om du må stoppe noe sted på veien
            for(int f=current_floor+1;f<floor_highest;f++){
                int stopped = 0;
                for(int i=0;i<3;i++){
                    if(order_array_copy[3*f+i].active&&order_array_copy[3*f+i].order_type!=HARDWARE_ORDER_DOWN){
                        action_array[num_actions]=IGNORE;
                        num_actions+=1;
                        action_array[num_actions]=IDLE;
                        num_actions+=1;
                        action_array[num_actions]=UP;
                        num_actions+=1;
                        order_deactivate(f,order_array_copy);
                        stopped =1;

                    }
                }
                //hvis du ikke stopper i en etasje skal du kjøre forbi
                if(!stopped){
                    action_array[num_actions]=IGNORE;
                    num_actions+=1;
                    action_array[num_actions]=UP;
                    num_actions+=1;
                }
            }
            //PÅ øverste etasje skal du stoppe og snu

            action_array[num_actions]=IDLE;
            num_actions+=1;
            order_deactivate(floor_highest,order_array_copy);

        }
        //Hvis du bare skal til etasjen over 
        else{
            //Skjønner ikke hvorfor denne ignoren trengs(var egentlig up), men funker
            action_array[num_actions]=IGNORE;
            num_actions+=1;
            action_array[num_actions]=IDLE;
            num_actions+=1;
            order_deactivate(floor_highest,order_array_copy);
        }

        //Nå må starten av handlingsruten settes rikitg basert på state
        if(state==IDLE||state==WAITING){
            action_array[0]=UP;
        }
    }
    //Sjekker om vi må nedover først
    else if(((state!=UP && last_state!=UP)||state == WAITING )&& floor_lowest<current_floor){
        //Er det etasjer mellom der vi er og topp etasjen
        if(current_floor-floor_lowest>1){
            //Siden det er etasjer mellom
            //der du er og dit du skal
            //må du sjekke om du må stoppe noe sted på veien
            for(int f=current_floor-1;f>floor_lowest;f--){
                int stopped = 0;
                for(int i=0;i<3;i++){
                    if(order_array_copy[3*f+i].active&&order_array_copy[3*f+i].order_type!=HARDWARE_ORDER_UP){
                        action_array[num_actions]=IGNORE;
                        num_actions+=1;
                        action_array[num_actions]=IDLE;
                        num_actions+=1;
                        action_array[num_actions]=DOWN;
                        num_actions+=1;
                        order_deactivate(f,order_array_copy);
                        stopped =1;

                    }
                }
                //hvis du ikke stopper i en etasje skal du kjøre forbi
                if(!stopped){
                    action_array[num_actions]=IGNORE;
                    num_actions+=1;
                    action_array[num_actions]=DOWN;
                    num_actions+=1;
                }
            }
            //PÅ laveste etasje skal du stoppe og snu

            action_array[num_actions]=IDLE;
            num_actions+=1;
            order_deactivate(floor_highest,order_array_copy);

        }
        //Hvis du bare skal til etasjen under
        else{
            //Skjønner ikke hvorfor denne ignoren trengs(var egentlig up), men funker
            action_array[num_actions]=IGNORE;
            num_actions+=1;
            action_array[num_actions]=IDLE;
            num_actions+=1;
            order_deactivate(floor_highest,order_array_copy);
        }

        //Nå må starten av handlingsruten settes rikitg basert på state
        if(state==IDLE||state==WAITING){
            action_array[0]=DOWN;
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

void clear_all_orders(){
    for(int i = 0;i<NUMBER_OF_ORDERS;i++){
        order_array[i].active=0;
    }
}

void clear_all_actions(){
    for(int i = 0;i<MAX_NUMBER_OF_ACTIONS;i++){
        action_array[i]=IGNORE;
    }
}