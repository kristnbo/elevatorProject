#include "hardware.h"
#include "queue_system.h"


/**
 * @file
 * @brief Handler checking for elevator orders and adding them to the queue-system. 
 */



/**
 * @brief Polls the hardware for pressed order-buttons, generates orders based on this. 
 * These orders are added to the order_array
 *
 * @return 1 if the stop signal is high; 0 if it is low.
 */
void check_for_order();

