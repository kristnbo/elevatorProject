#include "hardware.h"

#include <stdio.h> //for clearing terminal
/**
 * @file
 * @brief A system for keeping track of all current orders. 
 * The system also calculates the elevators movements based on this.
 */
#ifndef QUEUE_SYSTEM_H
#define QUEUE_SYSTEM_H
#define MAX_NUMER_OF_ORDERS 10
#define MAX_NUMBER_OF_ACTIONS 15

/**
 * @brief Common type for orders.
 */

typedef struct Order {
    int floor;
    HardwareOrder order_type;
}Order; 


/**
 * @brief An array for keeping track of all active orders.
 */


/**
 * @brief An array for keeping track of all planned future actions.
 */
HardwareMovement action_array[MAX_NUMBER_OF_ACTIONS];

/**
 * @brief Allocates all memory needed for the order and action arrays.
 */
void queue_init();


/**
 * @brief Adds an order to the order array. First checks if the order 
 * is already in the array.
 */
void add_order(Order *order);

/**
 * @brief Prints all orders with floor and type.
 */
void print_queue();

/**
 * @brief Calculates the elevators actions and refreshes the action_list.
 */

void calculate_action_array();

/**
 * @brief Updates the order array, deletes all completed orders.
 * @param state FSM current state.
 */

void update_order_array(int state);


#endif

