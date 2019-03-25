/*Touch display application*/
#include "touchDisplayApplication.h"

/*Timer functions*/
#include "isTime.h"

unsigned long timer1 = 0;
//unsigned long timerInterval1 = 1000;

#include "printTft.h"

bool callTouchDisplayApplication()
{
  /*touch ccordinates*/
  int x = 0;
  int y = 0;

  /*Check Tft touch*/
  checkTouchTft(x, y);

  /*Compare touch with coordinates*/
  checkTouchButtons(x, y);

  /*If touched redraw*/
  redrawTouchButtons();

  /*Process Action Buttons*/
  processActionButtons();

  /*Timer functions*/
  if (isTime(timer1, timerInterval1))
  {
    /*print time at coordinates and with graphic*/
    //printTime(timeCoordinates1, timeGraphic1);
    //printDate(dateCoordinates1, dateGraphic1);
  }

  return true;
}
