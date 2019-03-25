/*
  Tuner Shield DAB+
  by Marcus Bockting

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

/*firmware*/
#include "firmware.h"

/*To use the onboard button*/
#define BUTTON
#ifdef BUTTON
#include "TactileSwitch_0.1.0.h"
TactileSwitch myButton1(A1, 1);
#endif

/*To use serial monitor output*/
#define SERIAL_MONITOR
#ifdef SERIAL_MONITOR
#include "dabMenuSerial.h"
#endif/*SERIAL_MONITOR*/

/*To use Adafruit capacitive TFT display*/
//#define TOUCH_MONITOR
#ifdef TOUCH_MONITOR
#include "printTft.h"
#endif/*TOUCH_MONITOR*/

/*Calculate available free RAM*/
#include "ramTesting.h"

/*Timer functions*/
#include "isTime.h"

/*To use SD_CARD */
//#define SD_CARD
#ifdef SD_CARD
#include <SD.h>
#endif/*SD_CARD*/

void setup()
{

  Serial.begin(9600);
  while (!Serial); // Wait until Serial is ready

  /*Initalize device*/
  deviceInitalize();

  //Run all setup functions
  deviceSetUpPreFirmware();
  //Minipatch
  //deviceLoadInit();
  //deviceHostLoad(minipatch, 940);
  //hostLoadDataTuner(addrBootloaderPatchMini, sizeBootloaderPatchMini);

  //FullPatch
  hostLoadDataTuner(addrBootloaderPatchFull, sizeBootloaderPatchFull);

  //DAB Firmware
  hostLoadDataTuner(addrFirmwareDab, sizeFirmwareDab);

  //Run setup functions after loading firmware
  deviceSetUpAfterFirmware();

  //Set DAB properties
  dabSetAllProperties(dabPropertyValueList, dabNumberProperties);

  //Tunes DAB inital index
  dabTuneIndex(dabIndex);

  //Starts inital audio service
  if (dabStartDigitalService(dabServiceId, dabComponentId)) dabServiceRunning = true;

  //Get information of the digital ensemble
  dabGetEnsembleInfo();
  //dabGetDigitalServiceInformation(dabServiceInformation, dabServiceId);
  //writeTextField(dabServiceInformation.serviceLabel);

  //Parse ensemble an fill array with serviceId and component Id
  //dabParseEnsemble(dabEnsembleList, dabNumServices);
  //dabGetDateTimeInformation();
}


#ifdef SERIAL_MONITOR
/* Statemachine of system*/
enum state
{
  //start,
  //showService,
  //sleeping,
  //error,
  dabMain,
  dabScan,
  dabTechnical,
  device

} myState = dabMain;

/*user input*/
char ch = '0';

#endif

void loop()
{


#ifdef SERIAL_MONITOR

  
  /*Get user input*/
  if (Serial.available()) ch = Serial.read();

  switch (myState)
  {
    case dabMain:
      /*process action*/
      dabMenuMainSerial(ch);

      /*change state*/
      if (ch == 'M' || ch == '?') myState = dabMain;
      else if (ch == 'T') myState = dabTechnical;
      else if (ch == 'S') myState = dabScan;
      else if (ch == 'D') myState = device;
      else {
        /*nothing*/
      };

      break;

    case dabTechnical:
      /*process action*/
      dabMenuTechnicalSerial(ch);

      /*change state*/
      if (ch == 'M') myState = dabMain;
      else if (ch == 'T' || ch == '?') myState = dabTechnical;
      else if (ch == 'S') myState = dabScan;
      else if (ch == 'D') myState = device;
      else {
        /*nothing*/
      };
      break;

    case dabScan:
      /*process action*/
      dabMenuScanSerial(ch);

      /*change state*/
      if (ch == 'M') myState = dabMain;
      else if (ch == 'T') myState = dabTechnical;
      else if (ch == 'S' || ch == '?') myState = dabScan;
      else if (ch == 'D') myState = device;
      else {
        /*nothing*/
      };
      break;

    case device:
      /*process action*/
      dabMenuDeviceSerial(ch);

      /*change state*/
      if (ch == 'M') myState = dabMain;
      else if (ch == 'T') myState = dabTechnical;
      else if (ch == 'S') myState = dabScan;
      else if (ch == 'D' || ch == '?') myState = device;
      else {
        /*nothing*/
      };
      
      ch = '0';
      break;

    default:
      // Code
      break;

  }
#endif /*SERIAL_MONITOR*/


#ifdef TOUCH_MONITOR
  /*touch ccordinates*/
  int16_t x, y;

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


#endif/*TOUCH_MONITOR*/


#ifdef BUTTON
  /*Check user action*/
  if (myButton1.checkUserAction() != TactileSwitch::BTN_NO_ACTION)
    processActionTactileSwitch(myButton1);
#endif/*BUTTON*/

} //loop

/*To use the onboard button*/
#ifdef BUTTON
bool processActionTactileSwitch(TactileSwitch &myButton)
{
  switch (myButton.getEvent())
  {
    case TactileSwitch::BTN_NO_ACTION:
      break;

    case TactileSwitch::BTN_PUSH:
      {
        Serial.print(myButton.getNumber());
        Serial.println(F(" : Button pushed"));
        if (dabServiceRunning)
        {
          //Stops a service
          dabStopDigitalService(dabServiceId, dabComponentId);
          //Actual Service is running
          dabServiceRunning = false;
        }
        else if (!dabServiceRunning)
        {
          //Start a service
          dabStartDigitalService(dabServiceId, dabComponentId);
          //Actual Service is running
          dabServiceRunning = true;
        }
      }
      break;

    //Cycle Services
    case TactileSwitch::BTN_DBL_PUSH:
      {
        //Serial.print(myButton.getNumber());
        //Serial.println(F(" : Button double pushed"));

        //No Service list
        if (dabNumServices == 0)
          //Parse ensemble an fill array with serviceId and component Id
          dabParseEnsemble(dabEnsembleList, dabNumServices);

        //Service List available
        else if (dabNumServices != 0)
        {
          //Increase Actual Service
          dabActualService++;
          //Wrap around
          if (dabActualService > dabNumServices - 1) dabActualService = 0;

          dabServiceId = dabEnsembleList[dabActualService].serviceId;
          dabComponentId = dabEnsembleList[dabActualService].componentList[0];
          dabStartDigitalService(dabServiceId, dabComponentId);
        }
      }
      break;

    case TactileSwitch::BTN_HOLD:
      {
        //Serial.print(myButton.getNumber());
        //Serial.println(F(" : Button hold"));
        //Is there a valid index ?
        if (dabNumValidIndex == 0) dabBandScan(dabNumValidIndex, dabValidIndexList);

        //Is there a valid index ?
        if (dabNumValidIndex != 0)
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
#endif/*BUTTON*/











