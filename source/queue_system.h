#include "hardware.h"

#include <stdio.h> //for clearing terminal
/**
 * @file
 * @brief A system for keeping track of all current orders. 
 * The system also calculates the elevators movements based on this.
 */
#ifndef QUEUE_SYSTEM_H
#define QUEUE_SYSTEM_H
#define NUMBER_OF_ORDERS 10
#define MAX_NUMBER_OF_ACTIONS 20

/**
 * @brief A type for orders. Specifying floor, type of order and whether the order is active or not.
 */

typedef struct Order {
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
    IDLE,
    WAITING,
    EMERGENCY_STOP,
    IGNORE
} State;


/**
 * @brief Adds an order to the order array by setting active to 1 for the specified order.
 */
void add_order(Order *order);

/**
 * @brief Removes all orders for a floor, by setting active to zero. 
 * @param current_floor The floor orders should be deactivated for.
 */
void remove_order(int current_floor,Order *order_array);

void clear_order(int current_floor);

/**
 * @brief Prints all orders with floor and type, and shows whether they are active or not.
 */
void print_orders();

/**
 * @brief Calculates the elevators actions and refreshes the action_list. Also
 * updates order_array for current floor.
 */

void calculate_action_array(State state, State last_state, int current_floor);

/**
 * @brief Fetches next action, and updates the order array by deleting all completed orders.
 * @param state FSM current state.
 */
State request_action();

/**
 * @brief Prints all actions planned.
 */
void print_actions();

/**
 * @brief Sets active to 0 for all orders in order_array.
 */
void clear_all_orders();

/**
 * @brief Sets all elements in action_arry to IGNORE.
 */
void clear_all_actions();


#endif

