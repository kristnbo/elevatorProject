/**
 * @file
 * @brief A timer module for starting a timer, and checking if it has timed out.
 */

#ifndef TIMER_H
#define TIMER_H


/**
 * @brief Starts the timer with a given duration
 * @param duration The duration of the timer, given in seconds.
 */
void timer_start(int duration);

/**
 * @brief Checks if timer is timed out.
 * @return 1 if timed out, else 0.
 */
int check_timeout();

#endif 