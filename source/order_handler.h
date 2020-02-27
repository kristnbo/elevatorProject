#include "hardware.h"

#include <stdio.h> //for clearing terminal
/**
 * @file
 * @brief A system for keeping track of all active orders. 
 * The system also calculates the elevators movements based on this.
 */

#ifndef QUEUE_SYSTEM_H
#define QUEUE_SYSTEM_H
#define NUMBER_OF_ORDERS 10


/**
 * @brief A type for orders. Specifying floor, type of order and whether the order is active or not.
 */
typedef struct {
    int floor;
    HardwareOrder order_type;
    int active;
}Order; 


/**
 * @brief The state type found in main. 
 */
typedef enum {
    UP,
    DOWN,
    DOOR_OPEN,
    IDLE,
    EMERGENCY_STOP

} State;


/**
 * @brief Activates an order in the order_array.
 * @param floor the floor related to the order
 * @param order_type the type of order 
 */
void order_activate(int floor,HardwareOrder order_type);

/**
 * @brief Deactivates all orders for a floor.
 * @param current_floor The floor the orders should be deactivated for.
 */
void order_deactivate(int current_floor);

/**
 * @brief Deactivates all orders in order_array.
 */
void order_deactivate_all();

/**
 * @brief Polls the hardware for pressed order-buttons, activates orders based on this. 
 */
void order_update();


/**
 * @brief Prints all orders with floor and type, and shows whether they are active or not.
 */
void print_orders();




/**
 * @brief Fetches next action, and updates the order array by deleting all completed orders.
 * @return FSM new state.
 */
State request_state(State state, State last_state, int current_floor, int above);







#endif

