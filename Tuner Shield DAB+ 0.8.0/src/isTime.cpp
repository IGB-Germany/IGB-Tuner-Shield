/*Timer functions*/
#include "isTime.h"

/*millis()*/
#include "arduino.h"

unsigned long timeStamp1 = 0;
unsigned long timeStamp2 = 0;

unsigned long timerInterval1 = 1000;
unsigned long timerInterval2 = 2000;

bool isTime(unsigned long &timeStamp, unsigned long timeInterval)
{
  unsigned long timeElapsed = 0;
  unsigned long timeCurrent = millis();

  timeElapsed = timeCurrent - timeStamp;

  if (timeElapsed >= timeInterval)
  {
    /*set timeStamp to new value*/
    timeStamp = timeCurrent;
    return true;
  }

  else return false;
}
