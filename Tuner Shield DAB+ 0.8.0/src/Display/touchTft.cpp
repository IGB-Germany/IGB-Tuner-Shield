/*TFT capacitive touch functions*/
#include "touchTft.h"

/*
 * X: 1...319
 * Y: 5...235
*/
Adafruit_FT6206 ts = Adafruit_FT6206();

/*Init touchscreen controller FT6206*/
bool initTouchFt6206(uint8_t threshold)
{
  /*Threshold*/
  /*
      255 = min
     128 = default
     0 = max
  */
  if (ts.begin(threshold)) return true;

  else 
  {

    return false;
  }
}

/*Check Tft touch*/
bool checkTouchTft(int16_t &x, int16_t &y)
{
  /*Coordinates*/
  TS_Point point(-1, -1, -1);

  /*Check if touch data available*/
  if (ts.touched())
  {
    /*get point*/
    point = ts.getPoint();

/*
    Serial.print(F("Px: "));
    Serial.print(point.x);
    Serial.print(F(" Py: "));
    Serial.println(point.y);
*/
    
    /*rotation touch*/
    /*90°*/
    x = point.y;
    y = 240 - point.x;
   
  }
  else
  {
    /*touch released*/
    x = y = -1;
  }

  return true;
}

