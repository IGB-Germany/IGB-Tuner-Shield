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

//Driver for SI468x Tuner
#include "SI468x.h"

#include "TactileSwitch.h"
TactileSwitch mySwitch1(A0, 1, LOW);


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
  mySwitch1.readSwitch();//read mySwitch1
  processTactileSwitch(mySwitch1);//prosess event mySwitch1

} //loop

void processTactileSwitch(TactileSwitch& mySwitch)
{
  //get event
  switch (mySwitch.getEvent())
  {
    case TactileSwitch::NO_EVENT:
      break;

    case TactileSwitch::CLICK:
      {
        Serial.print(F("Switch "));
        Serial.print(mySwitch.getNumber());
        Serial.println(F(" was clicked once!"));

        //mute
        if (readMute() != 0)
          writeMute(0);
        else
          writeMute(3);
      }
      break;

    case TactileSwitch::MULTI_CLICK:
      {
        Serial.print(F("Switch "));
        Serial.print(mySwitch.getNumber());
        Serial.print(F(" was clicked "));
        Serial.print(mySwitch.getClicks());
        Serial.println(F(" times consecutively in quick succession!"));

        //Next service in ensemble
        nextService(serviceId, componentId);
      }
      break;

    case TactileSwitch::HOLD_LONG:
      {
        Serial.print(F("Switch "));
        Serial.print(mySwitch.getNumber());
        Serial.println(F(" is held down for a long press!"));

      }
      break;

    case TactileSwitch::RELEASED_HOLD_LONG:
      {
        Serial.print(F("Switch "));
        Serial.print(mySwitch.getNumber());
        Serial.println(F(" is released from a long press!"));

        //Scan frequencies auto up
        scan(dabIndex, true);
        startFirstService(serviceId, componentId);

      }
      break;

    case TactileSwitch::HOLD_VERY_LONG:
      {
        Serial.print(F("Switch "));
        Serial.print(mySwitch.getNumber());
        Serial.println(F(" is held down for a very long press!"));
      }
      break;

    case TactileSwitch::RELEASED_HOLD_VERY_LONG:
      {
        Serial.print(F("Switch "));
        Serial.print(mySwitch.getNumber());
        Serial.println(F(" is released from a very long press!"));


      }
      break;

    default:
      {
        //nothing
      }
  }

  //get message
  switch (mySwitch.getMessage())
  {
    case TactileSwitch::NO_MESSAGE:
      break;

    case TactileSwitch::BOUNCING_PRESSED:
      {
        Serial.print(mySwitch.getNumber());
        Serial.println(F(": Bouncing press"));
      }
      break;

    case TactileSwitch::BOUNCING_RELEASED:
      {
        Serial.print(mySwitch.getNumber());
        Serial.println(F(": Bouncing released"));
      }
      break;

    case TactileSwitch::POLLING_SLOW:
      {
        Serial.print(mySwitch.getNumber());
        Serial.print(F(": POLLING_SLOW"));
      }
      break;

    default:
      {
        //nothing
      }
  }

}
