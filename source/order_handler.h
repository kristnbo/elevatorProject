#include "hardware.h"
#include "queue_system.h"

#ifndef ORDER_HANDLER_H
#define ORDER_HANDLER_H


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
int check_for_order();

#endif
