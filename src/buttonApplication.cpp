/*Button application*/
#include "buttonApplication.h"

#include "TactileSwitch.h"
TactileSwitch myButton1(A0, 1);

/*Tuner shield DAB*/
#include "SI468x.h"

/*Serial Monitor Print Functions*/
#include "printSerial.h"

bool callTactileSwitches()
{
  processActionTactileSwitch(myButton1);
  return true;
}

bool processActionTactileSwitch(TactileSwitch &myButton)
{
  //Update
  myButton.checkUserAction();

  switch (myButton.getEvent())
  {
    case TactileSwitch::BTN_NO_ACTION:
      break;

    case TactileSwitch::BTN_PUSH:
      {
        //Serial.println(F("Button pushed"));

        //mute
        if (readMute() != 0)
          writeMute(0);
        else
          writeMute(3);
      }

      break;

    case TactileSwitch::BTN_DBL_PUSH:
      {
        //Serial.println(F("Button double pushed"));

      }

      break;

    case TactileSwitch::BTN_HOLD:
      {
        //Serial.println(F("Button hold"));
        
        //Next service in ensemble
        nextService(serviceId, componentId);

      }
      break;

    case TactileSwitch::BTN_HOLD_LONG:
      {
        //Serial.print(myButton.getNumber());
        //Serial.println(F("Button hold long"));
        
        //Scan frequencies auto up
        scan(dabIndex, true);

        startFirstService(serviceId, componentId);
        
      }
      break;

    case TactileSwitch::BTN_ERROR:
      {
        //Serial.print(myButton.getNumber());
        //Serial.println(F("Error"));
      }
      break;

    default:
      {

      }
  }
  return true;
}
