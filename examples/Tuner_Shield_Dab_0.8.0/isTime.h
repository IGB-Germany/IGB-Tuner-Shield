/*Timer functions*/
#ifndef IS_TIME_H
#define IS_TIME_H

/*define timer and interval for different tasks*/
extern unsigned long timeStamp1, timeStamp2;
extern unsigned long timerInterval1, timerInterval2;
/*
 * timeInterval: How many milliseconds for the  cycle
 * timeStamp:    When was the timer called last time 
*/
bool isTime(unsigned long &timeStamp, unsigned long timeInterval);

#endif /*IS_TIME_H*/
