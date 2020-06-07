/*
  Tuner Shield DAB+
  by IGB

  Tuner circuit SI468x
  SPI MOSI:   Pin 11
  SPI MISO:   Pin 12
  SPI SCK:    Pin 13
  SPI SS:     Pin 8
  Interrupt:  Pin 6
  Reset:      Pin 3

  Flash memory circuit SST26W onboard
  SPI SS:     Pin 2
*/

/*Driver for SI468x Tuner*/
#include "SI468x.h"

/*Button application*/
#define BUTTON
#ifdef BUTTON
#include "buttonApplication.h"
#endif

/*To use serial monitor output*/
#define SERIAL_MONITOR
#ifdef SERIAL_MONITOR
#include "dabMenuSerial.h"
#endif/*SERIAL_MONITOR*/

/*Touch display application*/
//#define TOUCH_DISPLAY
#ifdef TOUCH_DISPLAY
#include "touchDisplayApplication.h"
#endif/*TOUCH_DISPLAY*/

void setup()
{
  Serial.begin(9600);
  while (!Serial); // Wait until Serial is ready

  /*Initalize device*/
  deviceInitalize();

  /*Run all setup functions*/
  deviceLoadFirmware();

  /*Start DAB*/
  dabStart();
}

void loop()
{
#ifdef BUTTON
  /*Call application*/
  callTactileSwitches();
#endif/*BUTTON*/

#ifdef TOUCH_DISPLAY
  /*Call application*/
  callTouchDisplayApplication();
#endif/*TOUCH_DISPLAY*/

#ifdef SERIAL_MONITOR
  /*Call application*/
  callSerialMonitorApplication();
#endif /*SERIAL_MONITOR*/

} //loop
