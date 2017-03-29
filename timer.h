#ifndef _timer_h_
#define _timer_h_

const unsigned timerPeriod = 55; //ms

//*Threadsafe*

void initialize_timer();
void finalize_timer();

#endif