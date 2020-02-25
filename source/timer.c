#include <time.h>
#include "timer.h"
#include <stdio.h>



time_t timeout_timestamp=0;


void timer_start(int duration){
    timeout_timestamp = time(NULL)+duration;

}
int check_timeout(){
    if(time(NULL) > timeout_timestamp){
        return 1;
    }
    return 0;
}
