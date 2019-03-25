/*TFT capacitive touch functions*/
#ifndef TOUCH_TFT_H
#define TOUCH_TFT_H

/*FT6206 driver functions*/
#include "Adafruit_FT6206.h"

/*The FT6206 uses hardware I2C (SCL/SDA)*/
extern Adafruit_FT6206 ts;

/*Init touchscreen controller FT6206*/
bool initTouchFt6206(uint8_t threshold = 60);

/*Check Tft touch*/
bool checkTouchTft(int16_t &x, int16_t &y);

#endif /*TOUCH_TFT_H*/
