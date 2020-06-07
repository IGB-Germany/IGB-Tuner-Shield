/*Button application*/
#include "buttonApplication.h"

#include "TactileSwitch.h"
TactileSwitch myButton1(A0, 1);

/*Tuner shield DAB*/
#include "SI468x.h"

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
        if (mute == false)
        {
          mute = true;
          dabVolumeMute(mute);
        }
        else if (mute == true)
        {
          mute = false;
          dabVolumeMute(mute);
        }
      }

      break;

    //Cycle components
    case TactileSwitch::BTN_DBL_PUSH:
      {
        //Serial.println(F("Button double pushed"));

        //Check if list exists
        if (dabNumServices == 0)
        {
          if (dabParseEnsemble(dabEnsembleList, dabNumServices))
          {
            //Set to first entry
            dabServiceId = dabEnsembleList[0].serviceId;
            dabActualService = dabEnsembleList[0].serviceNumber;
            dabComponentId = dabEnsembleList[0].componentList[0];
            //Starts an audio or data service
            dabStartDigitalService(dabServiceId, dabComponentId);
          }
        }

        //Check if list exists
        else if (dabNumServices != 0)
        {
          //Increase Actual Service
          dabActualService++;
          //Wrap around
          if (dabActualService > dabNumServices - 1) dabActualService = 0;

          dabServiceId = dabEnsembleList[dabActualService].serviceId;
          dabComponentId = dabEnsembleList[dabActualService].componentList[0];

          dabStartDigitalService(dabServiceId, dabComponentId);
          //print actual service
          //serial::dabPrintActualService(dabEnsembleList, dabActualService);

          dabGetDigitalServiceInformation(dabServiceInformation, dabServiceId);
          //serial::dabPrintDigitalServiceInformation(dabServiceInformation);
        }
      }
      break;

    case TactileSwitch::BTN_HOLD:
      {
        Serial.println(F("Button hold"));

        //Check valid index
        if (dabNumValidIndex == 0)
        {
          Serial.println(F("Bandscan"));
          dabBandScan(dabNumValidIndex, dabValidIndexList);
          //Is there a valid index ?
          Serial.println(dabNumValidIndex);
          //tune index list 0
          dabTuneIndex(dabValidIndexList[0]);

          //Wait until list is available
          delay(500);
          //Parse ensemble an fill array with serviceId and component Id
          dabParseEnsemble(dabEnsembleList, dabNumServices);
          //Set to first entry
          dabServiceId = dabEnsembleList[0].serviceId;
          dabActualService = dabEnsembleList[0].serviceNumber;
          dabComponentId = dabEnsembleList[0].componentList[0];

          //Starts an audio or data service
          dabStartDigitalService(dabServiceId, dabComponentId);
        }

        else
        {
          dabIndex++;
          //Wrap around
          if (dabIndex == dabMaxNumFreq || dabIndex == dabNumValidIndex) dabIndex = 0;
          //tune index
          dabTuneIndex(dabValidIndexList[dabIndex]);
          
          //Wait until list is available
          delay(1000);
          //Parse ensemble an fill array with serviceId and component Id
          dabParseEnsemble(dabEnsembleList, dabNumServices);
          //Set to first entry
          dabServiceId = dabEnsembleList[0].serviceId;
          dabActualService = dabEnsembleList[0].serviceNumber;
          dabComponentId = dabEnsembleList[0].componentList[0];

          //Starts an audio or data service
          dabStartDigitalService(dabServiceId, dabComponentId);
        }
      }
      break;

    //Bandscan
    case TactileSwitch::BTN_HOLD_LONG:
      {
        //Serial.print(myButton.getNumber());
        //Serial.println(F(" : Button hold long"));
        //Bandscan
        if (dabBandScan(dabNumValidIndex, dabValidIndexList))
        {
          //Set dabIndex to first valid entry in table
          dabIndex = 0;
          //Tune index
          dabTuneIndex(dabValidIndexList[dabIndex]);

          //Wait until list is available
          delay(1000);
          //Parse ensemble an fill array with serviceId and component Id
          dabParseEnsemble(dabEnsembleList, dabNumServices);

          //Set to first entry
          dabServiceId = dabEnsembleList[0].serviceId;
          dabActualService = dabEnsembleList[0].serviceNumber;
          dabComponentId = dabEnsembleList[0].componentList[0];

          //Starts an audio or data service
          dabStartDigitalService(dabServiceId, dabComponentId);
        }
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
