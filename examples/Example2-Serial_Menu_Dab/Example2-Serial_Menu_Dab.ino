/*Driver for SI468x Tuner*/
#include "SI468x.h"

//To use serial monitor output application
#include "dabMenuSerial.h"


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
  //Call application
  callSerialMonitorApplication();
} //loop
