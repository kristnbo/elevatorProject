#include "hardware.h"

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
 * @brief The states of the elevator. 
 */
typedef enum {
    STATE_UP,
    STATE_DOWN,
    STATE_DOOR_OPEN,
    STATE_IDLE,
    STATE_EMERGENCY_STOP

} State;


/**
 * @brief Activates an order from floor @p floor of order type @p order_type in the order_array.
 * 
 * @param floor The floor of the order.
 * @param order_type The type of order.  
 */
void order_activate(int floor,HardwareOrder order_type);

/**
 * @brief Deactivates all orders for floor @p floor in the order_array.
 * @param floor The floor the orders should be deactivated for.
 */
void order_deactivate(int floor);

/**
 * @brief Deactivates all orders in the order_array.
 */
void order_deactivate_all();

/**
 * @brief Polls the hardware for pressed order-buttons and activates corresponding orders in the order_array. 
 */
void order_update();

/**
 * @brief Calculates next state, and updates the order_array by deleting all handled orders.
 * @param last_state The last state of the FSM.
 * @param current_floor The current floor of the elevator.
 * @param above A variable indicating whether the elevator is above or below @p current_floor.
 * @return FSM new state.
 */
State order_request_state(State last_state, int current_floor, int above);







#endif

