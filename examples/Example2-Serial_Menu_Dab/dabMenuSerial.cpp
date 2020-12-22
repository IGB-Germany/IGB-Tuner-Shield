/*Serial DAB Menu functions*/
#include "dabMenuSerial.h"

/*Driver for SI46xx Tuner*/
#include "SI468x.h"

//Serial
#include "Arduino.h"

//firmware
#include "firmware.h"

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
    rsqInformation_t rsqInformation = readRsqInformation();
    //Print status information about the received signal quality
    serialPrintSi468x::dabPrintRsqStatus(rsqInformation);
  }
  //Ensemble info
  else if (ch == 'e')
  {
    ensembleInformation_t ensembleInformation = readEnsembleInformation();
    serialPrintSi468x::dabPrintEnsembleInformation(ensembleInformation);
  }

  //Service info
  else if (ch == 'r')
  {
    serviceInformation_t dabServiceInfo = readServiceInformation(serviceId);

    //Digital service information
    serialPrintSi468x::dabPrintDigitalServiceInformation(dabServiceInfo);
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
    serviceData_t dabServiceData = readServiceData();
    //Prints service data
    serialPrintSi468x::dabPrintServiceData(dabServiceData);
  }

  //Get DAB service data
  else if (ch == 'v')
  {
    serviceData_t dabServiceData = readServiceData(0, 1);
    //Prints service data
    serialPrintSi468x::dabPrintServiceData(dabServiceData);
  }

  //next service
  else if (ch == 'd')
  {
    nextService(serviceId, componentId);
    serviceInformation_t serviceInformation = readServiceInformation(serviceId);
    Serial.println(serviceInformation.serviceLabel);
  }

  //previous service
  else if (ch == 'a')
  {
    previousService(serviceId, componentId);
    serviceInformation_t serviceInformation = readServiceInformation(serviceId);
    Serial.println(serviceInformation.serviceLabel);
  }

  //Start dedicated service in ensemble
  else if (ch == '1')
  {
    //CHAN_5C=178352
    //DR Deutschland
    dabIndex = 2;

    //sunshine live
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
    //CHAN_5C=178352
    //DR Deutschland
    dabIndex = 2;

    //Radio Bob 0x15DD, 0x16
    serviceId = 0x15DD;
    componentId = 0x16;

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
    //227360 kHz
    dabIndex = 33;
    //Joke FM
    serviceId = 0x1464;
    componentId = 0x18;

    //Tunes DAB index
    tuneIndex(dabIndex);
    //Starts an audio or data service
    startService(serviceId, componentId);
  }

  //Start dedicated service in ensemble
  else if (ch == '5')
  {
    //190640 kHz
    dabIndex = 9;
    //hr-iNFO
    serviceId = 0xD367;
    componentId = 0x4;

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
    //Get status information about the received signal quality
    rsqInformation_t rsqInformation = readRsqInformation();

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
    //ensemble list availabel ?
    eventInformation_t eventInformation = readEventInformation();
    if (eventInformation.serviceListAvailable == 1)
    {
      getEnsembleHeader(ensembleHeader);
      serialPrintSi468x::dabPrintEnsembleHeader(ensembleHeader);
    }
    else
    {
      serialPrintSi468x::printError(2);
    }
  }

  //Parse ensemble
  else if (ch == 'p')
  {
    //ensemble list available ?
    eventInformation_t eventInformation = readEventInformation();

    if (eventInformation.serviceListAvailable == 1)
    {
      getEnsemble(ensembleHeader);
      serialPrintSi468x::dabPrintEnsemble(ensembleHeader);
    }
    else
    {
      serialPrintSi468x::printError(2);
    }
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
    eventInformation_t eventInformation = readEventInformation();
    serialPrintSi468x::dabPrintEventInformation(eventInformation);
  }

  //Get audio information
  else if (ch == 'a')
  {
    audioInformation_t audioInformation = readAudioInformation();
    serialPrintSi468x::dabPrintComponentAudioInfo(audioInformation);
  }

  //Get and print technical information
  else if (ch == 'x')
  {
    componentTechnicalInformation_t componentTechnicalInformation = readComponentTechnicalInformation(serviceId, componentId);
    //Print technical information about the component
    serialPrintSi468x::dabPrintComponentTechnicalInformation(componentTechnicalInformation);
  }

  //Get and print date and time
  else if (ch == 'd')
  {
    timeDab_t dabTime = readDateTime();
    //Print local date and time or UTC
    serialPrintSi468x::dabPrintDateTimeInformation(dabTime);
  }

  //Get information about the component
  else if (ch == 'o')
  {
    componentInformation_t componentInformation;

    readComponentInformation(componentInformation, serviceId, componentId);
    //Print information about the component
    serialPrintSi468x::printComponentInformation(componentInformation);

  }

  //Print DAB frequency information list
  else if (ch == 'i')
  {
    //check events
    eventInformation_t eventInformation = readEventInformation();

    if (eventInformation.serviceLinkingInterrupt == 1)
    {
      serviceLinkingInformation_t serviceLinkingInformation = readServiceLinkingInfo(serviceId);
      serialPrintSi468x::printServiceLinkingInformation(serviceLinkingInformation);
    }
    else
    {
      serialPrintSi468x::printError(3);
    }

    if (eventInformation.frequencyInterrupt == 1)
    {
      readFrequencyInformationList();
    }
    else
    {
      serialPrintSi468x::printError(3);
    }
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
    tune(dabIndex, true);
    Serial.println(dabIndex);
  }

  //Tune index down
  else if (ch == 'z')
  {
    tune(dabIndex, false);
    Serial.println(dabIndex);
  }

  //Scan Index Up
  else if (ch == 'd')
  {
    //remember start index
    rsqInformation_t rsqInformation = readRsqInformation();
    unsigned char indexStart = rsqInformation.index;

    do
    {
      //tune index up
      tune(dabIndex, true);

      //get receive quality
      rsqInformation = readRsqInformation();
      Serial.print(F("Index:\t"));
      Serial.println(rsqInformation.index);

      //DAB found and valid if threshold for dab detected and greater than 4
      if ((rsqInformation.fastDect > 4) && rsqInformation.valid == 1)
      {
        Serial.println(F("Valid index found"));
        dabIndex = rsqInformation.index;
        Serial.println(dabIndex);
        break;
      }
      //around
      else if (rsqInformation.index == indexStart)
      {
        Serial.println(F("Nothing found"));
        //nothing found
        dabIndex = rsqInformation.index;
        Serial.println(dabIndex);
        break;
      }
    }
    while (rsqInformation.index != indexStart);
  }

  //Scan Index Down
  else if (ch == 'a')
  {
    //remember start index
    rsqInformation_t rsqInformation = readRsqInformation();
    unsigned char indexStart = rsqInformation.index;

    do
    {
      //tune index down
      tune(dabIndex, false);

      //get receive quality
      rsqInformation = readRsqInformation();
      Serial.print(F("Index:\t"));
      Serial.println(rsqInformation.index);

      //DAB found and valid if threshold for dab detected and greater than 4
      if ((rsqInformation.fastDect > 4) && rsqInformation.valid == 1)
      {
        Serial.println(F("Valid index found"));
        dabIndex = rsqInformation.index;
        Serial.println(dabIndex);
        break;
      }
      //around
      else if (rsqInformation.index == indexStart)
      {
        Serial.println(F("Nothing found"));
        //nothing found
        dabIndex = rsqInformation.index;
        Serial.println(dabIndex);
        break;
      }
    }
    while (rsqInformation.index != indexStart);
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
    statusRegister_t statusRegister = readStatusRegister();
    serialPrintSi468x::printStatusRegister(statusRegister);
  }

  //Print Device Part Info
  else if (ch == 'n')
  {
    partInfo_t partInfo = readPartInfo();
    serialPrintSi468x::printPartInfo(partInfo);
  }

  //Print system state
  else if (ch == 'y')
  {
    serialPrintSi468x::printSystemState(readSystemState());
  }

  //Print Firmware Information
  else if (ch == 'i')
  {
    firmwareInformation_t firmwareInformation = readFirmwareInformation();
    serialPrintSi468x::printFirmwareInformation(firmwareInformation);
  }

  //Print Parameters Used During Power Up
  else if (ch == 'a')
  {
    readPowerUpArguments(powerUpArguments);
    serialPrintSi468x::printPowerUpArguments(powerUpArguments);
  }

  //Print device property value list
  else if (ch == 'p')
  {
    serialPrintSi468x::printPropertyValueList(readPropertyValueList(propertyValueListDevice, NUM_PROPERTIES_DEVICE), NUM_PROPERTIES_DEVICE);
  }

  //power down
  else if (ch == 'd')
  {
    powerDown(true);
  }
  //power up
  else if (ch == 'u')
  {
    powerDown(false);
  }

  //boot
  else if (ch == '6')
  {
    initalize();
    reset();
    powerUp(powerUpArguments);
    loadFirmware(addrBootloaderPatchFull, sizeBootloaderPatchFull); //FullPatch
    loadFirmware(addrFirmwareDab, sizeFirmwareDab);//DAB Firmware
    boot();
    serialPrintSi468x::printSystemState(readSystemState());
    //Set device properties
    writePropertyValueList(propertyValueListDevice, NUM_PROPERTIES_DEVICE);
    //Set DAB properties
    writePropertyValueList(propertyValueListDab, NUM_PROPERTIES_DAB);
    //Tunes DAB inital index
    tuneIndex(dabIndex);
    //Starts inital audio service
    startService(serviceId, componentId);
  }

  else
  {
    //nothing
  };
}
