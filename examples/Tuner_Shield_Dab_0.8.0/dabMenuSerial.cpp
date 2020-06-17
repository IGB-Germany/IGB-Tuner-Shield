/*Serial DAB Menu functions*/
#include "dabMenuSerial.h"

/*Driver for SI46xx Tuner*/
#include "SI468x.h"

//Serial
#include "Arduino.h"

/*Serial Monitor Print Functions*/
#include "printSerial.h"

//Display menu if true
bool displayMenuMain = true;
//Display menu if true
bool displayMenuTechnical = false;
//Display menu if true
bool displayMenuScan = false;
//Display menu if true
bool displayMenuDevice = false;

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

/*Serial Monitor*/
bool callSerialMonitorApplication()
{
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

  return true;
}


//Menu Main
bool dabMenuMainSerial(char ch)
{
  /*Print first time*/
  if (displayMenuMain)
  {
    //Print DAB Menu Main
    serial::dabPrintMenuMain();
    //Display menu if true
    displayMenuMain = false;
  }

  else if (ch == '?' || ch == 'M' )
  {
    //Print DAB Menu Main
    serial::dabPrintMenuMain();
  }

  //Get status information about the received signal quality
  else if (ch == 'q')
  {
    //Get status information about the received signal quality
    dabGetRsqStatus(dabRsqInformation);
    //Print status information about the received signal quality
    serial::dabPrintRsqStatus(dabRsqInformation);
  }

  //Ensemble info
  else if (ch == 'e')
  {
    dabGetEnsembleInfo();
    serial::dabPrintEnsembleInformation(dabEnsembleInformation);
  }

  //service info
  else if (ch == 'r')
  {
    dabGetDigitalServiceInformation(dabServiceInformation, dabServiceId);
    serial::dabPrintDigitalServiceInformation(dabServiceInformation);
  }

  //Check DAB service data
  else if (ch == 'c')
  {
    //Gets a block of data associated with one of the enabled data components of a digital services
    dabGetServiceData(dabServiceData);
    //Prints service data
    serial::dabPrintServiceData(dabServiceData);
  }

  //Get DAB service data
  else if (ch == 'v')
  {
    //Gets a block of data associated with one of the enabled data components of a digital services
    dabGetServiceData(dabServiceData, 0, 1);
    //Prints service data
    serial::dabPrintServiceData(dabServiceData);
  }

  //Parse ensemble
  else if (ch == 'p')
  {
    //Parse ensemble an fill array with serviceId and component Id
    //check if list available
    if (dabParseEnsemble(dabEnsembleList, dabNumServices))
    {
      //Print Ensemble
      serial::dabPrintEnsemble(dabEnsembleList, dabNumServices);
      //Set to first entry
      dabServiceId = dabEnsembleList[0].serviceId;
      dabActualService = dabEnsembleList[0].serviceNumber;
      dabComponentId = dabEnsembleList[0].componentList[0];
      //Starts an audio or data service
      dabStartDigitalService(dabServiceId, dabComponentId);
      //print actual service
      serial::dabPrintActualService(dabEnsembleList, dabActualService);
      /*Digital service information*/
      dabGetDigitalServiceInformation(dabServiceInformation, dabServiceId);
      serial::dabPrintDigitalServiceInformation(dabServiceInformation);
    }
    else serial::dabPrintErrorNoReception();
  }

  //next service
  else if (ch == 'd')
  {
    //Check if list exists
    if (dabNumServices != 0)
    {
      //Increase Actual Service
      dabActualService++;
      //Wrap around
      if (dabActualService > dabNumServices - 1) dabActualService = 0;

      dabServiceId = dabEnsembleList[dabActualService].serviceId;
      dabComponentId = dabEnsembleList[dabActualService].componentList[0];

      dabStartDigitalService(dabServiceId, dabComponentId);
      //print actual service
      serial::dabPrintActualService(dabEnsembleList, dabActualService);

      /*Digital service information*/
      dabGetDigitalServiceInformation(dabServiceInformation, dabServiceId);
      serial::dabPrintDigitalServiceInformation(dabServiceInformation);
    }
  }

  //previous service
  else if (ch == 'a')
  {
    //Check if list exists
    if (dabNumServices != 0)
    {
      //Actual Service
      dabActualService--;
      //Wrap around
      if (dabActualService < 0) dabActualService = dabNumServices - 1;

      dabServiceId = dabEnsembleList[dabActualService].serviceId;
      dabComponentId = dabEnsembleList[dabActualService].componentList[0];

      //Starts an audio or data service
      dabStartDigitalService(dabServiceId, dabComponentId);
      //print actual service
      serial::dabPrintActualService(dabEnsembleList, dabActualService);

      /*Digital service information*/
      dabGetDigitalServiceInformation(dabServiceInformation, dabServiceId);
      serial::dabPrintDigitalServiceInformation(dabServiceInformation);
    }
  }

  //Start dedicated service in ensemble
  else if (ch == '1')
  {
    //sunshine live
    dabIndex = 2; //CHAN_5C=178352;//DR Deutschland
    dabServiceId = 0x15DC;
    dabComponentId = 0x15;
    //Tunes DAB index
    dabTuneIndex(dabIndex);
    //Starts an audio or data service
    dabStartDigitalService(dabServiceId, dabComponentId);

    /*Digital service information*/
    dabGetDigitalServiceInformation(dabServiceInformation, dabServiceId);
    serial::dabPrintDigitalServiceInformation(dabServiceInformation);
  }

  //Start dedicated service in ensemble
  else if (ch == '2')
  {
    dabIndex = 2; //CHAN_5C=178352;//DR Deutschland
    //Energy 0x1A45, ??
    //Radio Bob 0x15DD, 0x16
    dabServiceId = 0x15DD;
    dabComponentId = 0x16;
    //Absolut relax
    //dabServiceId = 0x17FA;
    //dabComponentId = 0x03;
    //Tunes DAB index
    dabTuneIndex(dabIndex);
    //Starts an audio or data service
    dabStartDigitalService(dabServiceId, dabComponentId);

    /*Digital service information*/
    dabGetDigitalServiceInformation(dabServiceInformation, dabServiceId);
    serial::dabPrintDigitalServiceInformation(dabServiceInformation);
  }

  //Start dedicated service in ensemble
  else if (ch == '3')
  {
    dabIndex = 25; //CHAN_11A = 216928;//SWR RP
    //SWR3
    dabServiceId = 0xD3A3;
    dabComponentId = 0x4;
    //Tunes DAB index
    dabTuneIndex(dabIndex);
    //Starts an audio or data service
    dabStartDigitalService(dabServiceId, dabComponentId);

    /*Digital service information*/
    dabGetDigitalServiceInformation(dabServiceInformation, dabServiceId);
    serial::dabPrintDigitalServiceInformation(dabServiceInformation);
  }


  //Start dedicated service in ensemble
  else if (ch == '4')
  {
    //CHAN_11D = 220352;//DR Hessen
    dabIndex = 28;
    //Radio TEDDY
    dabServiceId = 0x1B2E;
    dabComponentId = 0xD;
    //Tunes DAB index
    dabTuneIndex(dabIndex);
    //Starts an audio or data service
    dabStartDigitalService(dabServiceId, dabComponentId);
    /*Digital service information*/
    dabGetDigitalServiceInformation(dabServiceInformation, dabServiceId);
    serial::dabPrintDigitalServiceInformation(dabServiceInformation);

  }

  //Start dedicated service in ensemble
  else if (ch == '5')
  {
    //CHAN_11D = 220352;//DR Hessen
    dabIndex = 28;
    //FFH
    dabServiceId = 0xD368;
    dabComponentId = 0xF;
    //Tunes DAB index
    dabTuneIndex(dabIndex);
    //Starts an audio or data service
    dabStartDigitalService(dabServiceId, dabComponentId);
    /*Digital service information*/
    dabGetDigitalServiceInformation(dabServiceInformation, dabServiceId);
    serial::dabPrintDigitalServiceInformation(dabServiceInformation);

  }

  //Start service
  else if (ch == 'x')
  {
    if (dabStartDigitalService(dabServiceId, dabComponentId))
    {
      dabServiceRunning = true;
    }
    else
      dabServiceRunning = false;
  }
  //Stop service
  else if (ch == 'y')
  {
    if (dabStopDigitalService(dabServiceId, dabComponentId))
    {
      dabServiceRunning = false;
    }
    else
      dabServiceRunning = true;
  }

  //Show Running Service
  else if (ch == 't')
  {
    //Get status information about the received signal quality
    dabGetRsqStatus(dabRsqInformation);
    serial::dabPrintIndex(dabRsqInformation.index);
    serial::dabPrintFrequency(dabRsqInformation.frequency);
    serial::dabPrintIds(dabServiceId, dabComponentId);
  }

  //Mute and Unmute
  else if (ch == 'm')
  {
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

  //Volume down
  else if (ch == 's')
  {
    dabVolumeDown(volume);
  }


  //Volume up
  else if (ch == 'w')
  {
    dabVolumeUp(volume);
  }


  else {
    /*nothing*/
  };

  return true;

}/*dabMenu*/


//Menu Technical
bool dabMenuTechnicalSerial(char ch)
{
  //DAB Menu Technical
  if (ch == '?' || ch == 'T' || displayMenuTechnical)
  {
    //Print DAB Technical Menu
    serial::dabPrintMenuTechnical();
    //Hide menu
    displayMenuTechnical = false;
  }

  //Gets event information about the various events related to the DAB radio
  else if (ch == 'e')
  {
    dabGetEventStatus(dabEventInformation);
    serial::dabPrintEventInformation(dabEventInformation);
  }

  //Get audio service info
  else if (ch == 'a')
  {
    dabGetComponentAudioInfo(dabAudioComponentInformation);
    serial::dabPrintComponentAudioInfo(dabAudioComponentInformation);
  }

  //Get and print technical information
  else if (ch == 'x')
  {
    //Get technical information about the component
    dabGetComponentTechnicalInformation(dabServiceId, dabComponentId, dabComponentTechnicalInformation);
    //Print technical information about the component
    serial::dabPrintComponentTechnicalInformation(dabComponentTechnicalInformation);
  }

  //Get and print date and time
  else if (ch == 'd')
  {
    //DAB time
    unsigned short year = 1;
    unsigned char month = 1;
    unsigned char day = 1;
    unsigned char hour = 0;
    unsigned char minute = 0;
    unsigned char second = 0;
    /*local*/
    //unsigned char timeType = 0;

    //Gets the ensemble time adjusted for the local time offset or the UTC
    dabGetDateTimeInformation(year, month, day, hour, minute, second);

    //Print local date and time or UTC
    serial::dabPrintDateTimeInformation(year, month, day, hour, minute, second);
  }

  //Get information about the component
  else if (ch == 'c')
  {
    //Get information about the component
    dabGetComponentInformation(dabServiceId, dabComponentId, dabComponentInformation);
    //Print information about the component
    serial::dabPrintComponentInformation(dabComponentInformation);
  }

  /*Print DAB frequency information list*/
  else if (ch == 'f')
  {
    dabGetEnsembleFrequencyInformationList();
  }

  //Get status information about the received signal quality
  else if (ch == 'q')
  {
    //Get status information about the received signal quality
    dabGetRsqStatus(dabRsqInformation);
    //Print status information about the received signal quality
    serial::dabPrintRsqStatus(dabRsqInformation);
  }

  //Test varactor tuning capacitor
  else if (ch == 't')
  {
    //Test varactor tuning capacitor
    dabTestVaractorCap(dabIndex);
  }

  //Read And Print RSSI
  else if (ch == 'r')
  {
    while (ch != 'x')
    {
      //Get user input
      if (Serial.available()) ch = Serial.read();
      /*Print average rssi*/
      serial::devicePrintRssi(deviceGetAverageRssi());
    }
  }

  //Read and print properties
  else if (ch == 'p')
  {
    //Get all device property values
    dabGetAllProperties(dabPropertyValueList);
    //Print device property value list
    serial::dabPrintAllProperties(dabPropertyValueList);
  }

  else {
    /*nothing*/
  };

  return true;

}//dabMenuTechnicalSerial()


//Menu Scan
bool dabMenuScanSerial(char ch)
{
  //DAB Menu Technical
  if (ch == '?' || ch == 'S' || displayMenuScan)
  {
    //Print DAB Menu Scan
    serial::dabPrintMenuScan();
    //Hide menu
    displayMenuScan = false;
  }

  //Get Frequency Table
  else if (ch == 'f')
  {
    //Get the DAB frequency table
    dabGetFrequencyTable(dabFreqTable, dabNumFreq);
    //Print frequency table
    serial::dabPrintFrequencyTable(dabFreqTable, dabNumFreq);
  }

  //Set Frequency Table
  else if (ch == '1')
  {
    //Get the DAB frequency table
    dabSetFrequencyTable(dabFreqTable, dabNumFreq, frequencyTableAll, 41);
    //Get the DAB frequency table
    dabGetFrequencyTable(dabFreqTable, dabNumFreq);
    //Print frequency table
    serial::dabPrintFrequencyTable(dabFreqTable, dabNumFreq);
  }

  //Set Frequency Table
  else if (ch == '2')
  {
    //Get the DAB frequency table
    dabSetFrequencyTable(dabFreqTable, dabNumFreq, frequencyTableRheinlandPfalz, 2);
    //Get the DAB frequency table
    dabGetFrequencyTable(dabFreqTable, dabNumFreq);
    //Print frequency table
    serial::dabPrintFrequencyTable(dabFreqTable, dabNumFreq);
  }

  //Set Frequency Table
  else if (ch == '3')
  {
    //Get the DAB frequency table
    dabSetFrequencyTable(dabFreqTable, dabNumFreq, frequencyTableItalienRas, 5);
    //Get the DAB frequency table
    dabGetFrequencyTable(dabFreqTable, dabNumFreq);
    //Print frequency table
    serial::dabPrintFrequencyTable(dabFreqTable, dabNumFreq);
  }

  //Scan Index Up
  else if (ch == 'u')
  {
    //scan
    dabScanUp(dabIndex);
  }

  //Scan Index Down
  else if (ch == 'd')
  {
    //scan
    dabScanDown(dabIndex);
  }

  //Bandscan
  else if (ch == 's')
  {
    //Bandscan
    if (dabBandScan(dabNumValidIndex, dabValidIndexList))
    {

      //if at least one valid frequency found
      //Print valid index list
      serial::dabPrintValidIndexList(dabNumValidIndex, dabValidIndexList);

      //Set dabIndex to first valid entry in table
      dabIndex = 0;
      //Tune index
      dabTuneIndex(dabValidIndexList[dabIndex]);
    }
  }

  //Get Valid Index List
  else if (ch == 'i')
  {
    //Is there a valid index ?
    if (dabNumValidIndex != 0)
    {
      //Print valid frequency table
      serial::dabPrintValidIndexList(dabNumValidIndex, dabValidIndexList);
    }
  }

  //Tune index (frequency) out of valid list up
  else if (ch == '+')
  {
    //Is there a valid index ?
    if (dabNumValidIndex != 0)
    {
      dabIndex++;
      //Wrap around
      if (dabIndex == dabMaxNumFreq || dabIndex == dabNumValidIndex) dabIndex = 0;
      //tune index
      dabTuneIndex(dabValidIndexList[dabIndex]);
      //Print DAB index
      serial::dabPrintIndex(dabIndex);
      //Prints status information of the digital ensemble
      serial::dabPrintEnsembleInformation(dabGetEnsembleInfo());
    }
  }

  //Tune index (frequency) out of valid list down
  else if (ch == '-')
  {
    //Is there a valid index ?
    if (dabNumValidIndex != 0)
    {
      dabIndex--;
      //Wrap around
      if (dabIndex == 255) dabIndex = (dabNumValidIndex - 1);
      //tune index
      dabTuneIndex(dabValidIndexList[dabIndex]);
      //Print DAB index
      serial::dabPrintIndex(dabIndex);

      //Prints status information of the digital ensemble
      serial::dabPrintEnsembleInformation(dabGetEnsembleInfo());
    }
  }

  //Get status information about the received signal quality
  else if (ch == 'q')
  {
    //Get status information about the received signal quality
    dabGetRsqStatus(dabRsqInformation);
    //Print status information about the received signal quality
    serial::dabPrintRsqStatus(dabRsqInformation);
  }

  //Ensemble information
  else if (ch == 'e')
  {
    //Prints status information of the digital ensemble
    serial::dabPrintEnsembleInformation(dabGetEnsembleInfo());
  }

  //Get service list header of the ensemble and print it
  else if (ch == 'h')
  {
    //Get service list header
    dabGetDigitalServiceListHeader(dabServiceListHeader);
    //Print service list header
    serial::dabPrintServiceListHeader(dabServiceListHeader);
  }

  else {
    /*nothing*/
  };

  return true;
}//dabMenuScanSerial


bool dabMenuDeviceSerial(char ch)
{
  //Menu device
  if (ch == '?' || ch == 'D' || displayMenuDevice)
  {
    //Print DAB Device Menu
    serial::dabPrintMenuDevice();
    displayMenuDevice = false;
  }

  //Print version information
  else if (ch == 'v')
  {
    //Print version information
    serial::printVersionInformation();
  }

  //Print device status information
  else if (ch == 's')
  {
    //Print device status information
    serial::devicePrintStatus(deviceGetStatus());
  }

  //Print Device Part Number
  else if (ch == 'n')
  {
    //Print Device Part Number
    serial::devicePrintPartNumber(deviceGetPartNumber());
  }

  //Print Device Mode
  else if (ch == 'm')
  {
    //Print Device Image
    serial::devicePrintImage(deviceGetImage());
  }

  //Print Firmware Information
  else if (ch == 'f')
  {
    //Print Firmware Information
    serial::devicePrintFirmwareInformation(deviceGetFirmwareInformation());
  }

  else if (ch == 'a')
  {
    //Print Parameters Used During Power Up
    serial::devicePrintPowerUpArguments(deviceGetPowerUpArguments());
  }

  else if (ch == 'p')
  {
    //Get all device property values
    deviceGetAllProperties(devicePropertyValueList);
    //Print device property value list
    serial::devicePrintAllProperties(devicePropertyValueList);
  }
  else {
    /*nothing*/
  };

  return true;
}
