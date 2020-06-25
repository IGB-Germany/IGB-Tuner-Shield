/*Serial DAB Menu functions*/
#include "dabMenuSerial.h"

/*Driver for SI46xx Tuner*/
#include "SI468x.h"

//Serial
#include "Arduino.h"

/*Serial Monitor Print Functions*/
#include "printSerial.h"

//Display menu if true
bool displayMenu = true;

//Statemachine
state myState = main;

//Serial Monitor
void callSerialMonitorApplication()
{
  //user input
  char ch = ' ';

  //Get user input
  if (Serial.available() > 0)
  {
    ch =  Serial.read();
  }
  //Received signal quality
  if (ch == 'q')
  {
    //Print status information about the received signal quality
    serialPrintSi468x::dabPrintRsqStatus(readRsqInformation());
  }
  //Ensemble info
  else if (ch == 'e')
  {
    serialPrintSi468x::dabPrintEnsembleInformation(readEnsembleInformation());
  }
  
  //Service info
  else if (ch == 'r')
  {
    //Digital service information
    serialPrintSi468x::dabPrintDigitalServiceInformation(readServiceInformation(serviceId));
  }
  
  //Free Ram
  else if (ch == 'f')
  {
    Serial.print(F("Free RAM:\t"));
    Serial.println(getFreeRam());
    Serial.println();
  }
  
  //Mute and Unmute
  else if (ch == 'm')
  {
    if (readMute() != 0)
      writeMute(0);
    else
      writeMute(3);
    serialPrintSi468x::printMute(readMute());
  }
  
  //Volume up
  else if (ch == '+')
  {
    serialPrintSi468x::printVolume(volumeUp());
  }

  //Volume down
  else if (ch == '-')
  {
    serialPrintSi468x::printVolume(volumeDown());
  }
  
  //change state
  else if (ch == 'M') myState = main;
  else if (ch == 'E')  myState = scanEnsemble;
  else if (ch == 'F')  myState = scanFrequency;
  else if (ch == 'T') myState = technical;
  else if (ch == 'D')  myState = device;
  else {};//nothing
  

  switch (myState)
  {
    case main:
      menuMain(ch);
      break;

    case scanEnsemble:
      menuScanEnsemble(ch);
      break;

    case scanFrequency:
      menuScanFrequency(ch);
      break;

    case technical:
      menuTechnical(ch);
      break;

    case device:
      menuDevice(ch);
      break;

    default:
      // Code
      break;
  }
}

void menuMain(char ch)
{
  if (ch == '?' || ch == 'M' || displayMenu)
  {
    serialPrintSi468x::dabPrintMenuMain();
    //Hide menu
    displayMenu = false;
  }

  //Check DAB service data
  else if (ch == 'c')
  {
    //Prints service data
    serialPrintSi468x::dabPrintServiceData(dabGetServiceData());
  }
  
  //Get DAB service data
  else if (ch == 'v')
  {
    //Prints service data
    serialPrintSi468x::dabPrintServiceData(dabGetServiceData(0, 1));
  }
  
  //next service
  else if (ch == 'd')
  {
    nextService(serviceId, componentId);
  }

  //previous service
  else if (ch == 'a')
  {
    previousService(serviceId, componentId);
  }

  //Start dedicated service in ensemble
  else if (ch == '1')
  {
    //sunshine live
    dabIndex = 2; //CHAN_5C=178352;//DR Deutschland
    serviceId = 0x15DC;
    componentId = 0x15;
    //Tunes DAB index
    tuneIndex(dabIndex);
    //Starts an audio or data service
    startService(serviceId, componentId);
  }

  //Start dedicated service in ensemble
  else if (ch == '2')
  {
    dabIndex = 2; //CHAN_5C=178352;//DR Deutschland
    //Energy 0x1A45, ??
    //Radio Bob 0x15DD, 0x16
    serviceId = 0x15DD;
    componentId = 0x16;
    //Absolut relax
    //serviceId = 0x17FA;
    //componentId = 0x03;
    //Tunes DAB index
    tuneIndex(dabIndex);
    //Starts an audio or data service
    startService(serviceId, componentId);
  }

  //Start dedicated service in ensemble
  else if (ch == '3')
  {
    dabIndex = 25; //CHAN_11A = 216928;//SWR RP
    //SWR3
    serviceId = 0xD3A3;
    componentId = 0x4;
    //Tunes DAB index
    tuneIndex(dabIndex);
    //Starts an audio or data service
    startService(serviceId, componentId);
  }


  //Start dedicated service in ensemble
  else if (ch == '4')
  {
    //CHAN_11D = 220352;//DR Hessen
    dabIndex = 28;
    //Radio TEDDY
    serviceId = 0x1B2E;
    componentId = 0xD;
    //Tunes DAB index
    tuneIndex(dabIndex);
    //Starts an audio or data service
    startService(serviceId, componentId);
  }

  //Start dedicated service in ensemble
  else if (ch == '5')
  {
    //CHAN_11D = 220352;//DR Hessen
    dabIndex = 28;
    //FFH
    serviceId = 0xD368;
    componentId = 0xF;
    //Tunes DAB index
    tuneIndex(dabIndex);
    //Starts an audio or data service
    startService(serviceId, componentId);
  }

  //Start service
  else if (ch == 'x')
  {
    startService(serviceId, componentId);
  }

  //Stop service
  else if (ch == 'y')
  {
    stopService(serviceId, componentId);
  }

  //Show Running Service
  else if (ch == 't')
  {
    rsqInformation_t rsqInformation;
    //Get status information about the received signal quality
    rsqInformation = readRsqInformation();
    serialPrintSi468x::dabPrintIndex(rsqInformation.index);
    serialPrintSi468x::dabPrintFrequency(rsqInformation.frequency);
    serialPrintSi468x::dabPrintIds(serviceId, componentId);
  }

  else
  {
    //nothing
  };


}//menuMainDab

//Menu Scan ensemble
void menuScanEnsemble(char ch)
{
  if (ch == '?' || ch == 'E' || displayMenu)
  {
    serialPrintSi468x::printMenuScanEnsemble();
    //Hide menu
    displayMenu = false;
  }

  //Get ensemble header
  else if (ch == 'h')
  {
    getEnsembleHeader(ensembleHeader);
    serialPrintSi468x::dabPrintEnsembleHeader(ensembleHeader);
  }

  //Parse ensemble
  else if (ch == 'p')
  {
    getEnsemble(ensembleHeader);
    serialPrintSi468x::dabPrintEnsemble(ensembleHeader);
  }

  //Start 1st Service
  else if (ch == '0')
  {
    startFirstService(serviceId, componentId);
  }

  //next service
  else if (ch == 'd')
  {
    nextService(serviceId, componentId);
  }

  //previous service
  else if (ch == 'a')
  {
    previousService(serviceId, componentId);
  }

  //Check DAB service data
  else if (ch == 't')
  {
    //Status Only = 1 , Acknowledge = 0
    //serialPrintSi468x::dabPrintServiceData(readServiceDataHeader());
  }

  //Get DAB service data
  else if (ch == 'v')
  {
    //Status Only = 0 , Acknowledge = 1
    //serialPrintSi468x::dabPrintServiceData(readServiceData());
  }
}


//Menu Technical
void menuTechnical(char ch)
{
  //DAB Menu Technical
  if (ch == '?' || ch == 'T' || displayMenu)
  {
    //Print DAB Technical Menu
    serialPrintSi468x::dabPrintMenuTechnical();
    //Hide menu
    displayMenu = false;
  }
  
  //Gets event information about the various events related to the DAB radio
  else if (ch == 'v')
  {
    serialPrintSi468x::dabPrintEventInformation(readEventInformation());
  }

  //Get audio information
  else if (ch == 'a')
  {
    serialPrintSi468x::dabPrintComponentAudioInfo(readAudioInformation());
  }

  //Get and print technical information
  else if (ch == 'x')
  {
    //Print technical information about the component
    serialPrintSi468x::dabPrintComponentTechnicalInformation(dabGetComponentTechnicalInformation(serviceId, componentId));
  }

  //Get and print date and time
  else if (ch == 'd')
  {
    //Print local date and time or UTC
    serialPrintSi468x::dabPrintDateTimeInformation(dabGetDateTimeInformation());
  }

  //Get information about the component
  else if (ch == 'c')
  {
    //Print information about the component
    serialPrintSi468x::dabPrintComponentInformation(dabGetComponentInformation(serviceId, componentId));
  }

  //Print DAB frequency information list
  else if (ch == 'f')
  {
    dabGetEnsembleFrequencyInformationList();
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
      //Print rssi
      serialPrintSi468x::printRssi(readRssi());
    }
  }

  //Read and print properties
  else if (ch == 'p')
  {
    //Print property value list DAB
    serialPrintSi468x::printPropertyValueList(readPropertyValueList(propertyValueListDab, NUM_PROPERTIES_DAB), NUM_PROPERTIES_DAB);
  }

  else
  {
    //nothing
  };

}//dabMenuTechnicalSerial()


void menuScanFrequency(char ch)
{
  if (ch == '?' || ch == 'F' || displayMenu)
  {
    //Print DAB Menu Scan
    serialPrintSi468x::dabPrintMenuScanFrequency();
    //Hide menu
    displayMenu = false;
  }

  //Print ensemble header
  else if (ch == 'h')
  {
    getEnsembleHeader(ensembleHeader);
    serialPrintSi468x::dabPrintEnsembleHeader(ensembleHeader);
  }

  //Print ensemble
  else if (ch == 'p')
  {
    getEnsemble(ensembleHeader);
    serialPrintSi468x::dabPrintEnsemble(ensembleHeader);
  }

  //Get Frequency Table
  else if (ch == 'f')
  {
    //Print frequency table
    serialPrintSi468x::dabPrintFrequencyTable(readFrequencyTable(), readNumberFrequencies());
  }

  //Set Frequency Table
  else if (ch == '1')
  {
    //Set the frequency table
    writeFrequencyTable(frequencyTableDefault, 41);

    //Print frequency table
    serialPrintSi468x::dabPrintFrequencyTable(readFrequencyTable(), readNumberFrequencies());
  }

  //Set Frequency Table
  else if (ch == '2')
  {
    //Set the frequency table
    writeFrequencyTable(frequencyTableRheinlandPfalz, 2);

    //Print frequency table
    serialPrintSi468x::dabPrintFrequencyTable(readFrequencyTable(), readNumberFrequencies());
  }

  //Set Frequency Table
  else if (ch == '3')
  {
    //Set the frequency table
    writeFrequencyTable(frequencyTableItalienRas, 5);

    //Print frequency table
    serialPrintSi468x::dabPrintFrequencyTable(readFrequencyTable(), readNumberFrequencies());
  }

  //Tune index Up
  else if (ch == 'u')
  {
    Serial.println(tune(true));
  }

  //Tune index down
  else if (ch == 'z')
  {
    Serial.println(tune(false));
  }

  //Scan Index Up
  else if (ch == 'd')
  {
    Serial.println(scan(true));
  }

  //Scan Index Down
  else if (ch == 'a')
  {
    Serial.println(scan(false));
  }

  //Bandscan
  else if (ch == 's')
  {
    //Bandscan
    if (dabBandScan(dabNumValidIndex, dabValidIndexList))
    {

      //if at least one valid frequency found
      //Print valid index list
      serialPrintSi468x::dabPrintValidIndexList(dabNumValidIndex, dabValidIndexList);

      //Set dabIndex to first valid entry in table
      dabIndex = 0;
      //Tune index
      tuneIndex(dabValidIndexList[dabIndex]);
    }
  }

  //Get Valid Index List
  else if (ch == 'i')
  {
    //Is there a valid index ?
    if (dabNumValidIndex != 0)
    {
      //Print valid frequency table
      serialPrintSi468x::dabPrintValidIndexList(dabNumValidIndex, dabValidIndexList);
    }
  }

  //Tune valid index (frequency) up
  else if (ch == '+')
  {

  }

  //Tune valid index (frequency) down
  else if (ch == '-')
  {


  }

  else
  {
    //nothing
  };
}


void menuDevice(char ch)
{
  //Menu device
  if (ch == '?' || ch == 'D' || displayMenu)
  {
    //Print DAB Device Menu
    serialPrintSi468x::dabPrintMenuDevice();
    displayMenu = false;
  }

  //Print version information
  else if (ch == 'v')
  {
    serialPrintSi468x::printVersion(version);
  }

  //Print device status information
  else if (ch == 's')
  {
    serialPrintSi468x::printStatusRegister(readStatusRegister());
  }

  //Print Device Part Info
  else if (ch == 'n')
  {
    serialPrintSi468x::printPartInfo(readPartInfo());
  }

  //Print system state
  else if (ch == 'm')
  {
    serialPrintSi468x::printSystemState(readSystemState());
  }

  //Print Firmware Information
  else if (ch == 'f')
  {
    serialPrintSi468x::printFirmwareInformation(readFirmwareInformation());
  }

  else if (ch == 'a')
  {
    //Print Parameters Used During Power Up
    serialPrintSi468x::printPowerUpArguments(readPowerUpArguments());
  }

  else if (ch == 'p')
  {
    //Print device property value list
    serialPrintSi468x::printPropertyValueList(readPropertyValueList(propertyValueListDevice, NUM_PROPERTIES_DEVICE), NUM_PROPERTIES_DEVICE);
  }
  else
  {
    //nothing
  };
}
