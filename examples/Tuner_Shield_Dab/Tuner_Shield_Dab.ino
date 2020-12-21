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

//TACTILE_SWITCH application
#define TACTILE_SWITCH
#ifdef TACTILE_SWITCH
#include "buttonApplication.h"
#endif

//To use serial monitor output application
#define SERIAL_MONITOR
#ifdef SERIAL_MONITOR
#include "dabMenuSerial.h"
#endif//SERIAL_MONITOR


void setup()
{
  Serial.begin(9600);
  while (!Serial); // Wait until Serial is ready


  //Run all setup functions
  deviceBegin();
  //Start DAB
  dabBegin();
}

void loop()
{
#ifdef TACTILE_SWITCH
  //Call application
  callTactileSwitches();
#endif//TACTILE_SWITCH

#ifdef SERIAL_MONITOR
  //Call application
  callSerialMonitorApplication();
#endif //SERIAL_MONITOR

} //loop
