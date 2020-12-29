
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
    rsqInformation_t rsqInformation;
    readRsqInformation(rsqInformation);
    serialPrintSi468x::dabPrintRsqStatus(rsqInformation);
  }
  //Ensemble info
  else if (ch == 'e')
  {
    ensembleInformation_t ensembleInformation;
    readEnsembleInformation(ensembleInformation);
    serialPrintSi468x::dabPrintEnsembleInformation(ensembleInformation);
  }

  //Service info
  else if (ch == 'r')
  {
    serviceInformation_t serviceInformation;
    readServiceInformation(serviceInformation, serviceId);
    serialPrintSi468x::dabPrintDigitalServiceInformation(serviceInformation);
  }

  //Free Ram
  else if (ch == 'f')
  {
    Serial.print(F("Free RAM:\t"));
    Serial.println(getFreeRam());
    Serial.println();
  }

  //next service
  else if (ch == 'd')
  {
    nextService(serviceId, componentId);
    serviceInformation_t serviceInformation;
    readServiceInformation(serviceInformation, serviceId);
    Serial.println(serviceInformation.serviceLabel);
  }

  //previous service
  else if (ch == 'a')
  {
    previousService(serviceId, componentId);
    serviceInformation_t serviceInformation;
    readServiceInformation(serviceInformation, serviceId);
    Serial.println(serviceInformation.serviceLabel);
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
    serviceData_t serviceData;
    readServiceData(serviceData);
    serialPrintSi468x::dabPrintServiceData(serviceData);
  }

  //Get DAB service data
  else if (ch == 'v')
  {
    serviceData_t serviceData;
    readServiceData(serviceData, 0, 1);
    serialPrintSi468x::dabPrintServiceData(serviceData);
  }

  //Start dedicated service in ensemble
  else if (ch == '1')
  {
    //CHAN_5C=178352
    //DR Deutschland
    index = 2;

    //sunshine live
    serviceId = 0x15DC;
    componentId = 0x15;

    //Tunes DAB index
    tuneIndex(index);
    //Starts an audio or data service
    startService(serviceId, componentId);

    //parse ensemble
    getEnsemble(ensembleHeader);

  }

  //Start dedicated service in ensemble
  else if (ch == '2')
  {
    //ANTENNE DE
    index = 17;

    //TOGGO 0x15DD, 0x16
    serviceId = 0x1298;
    componentId = 0x05;

    //Tunes DAB index
    tuneIndex(index);
    //Starts an audio or data service
    startService(serviceId, componentId);

    //parse ensemble
    getEnsemble(ensembleHeader);
  }

  //Start dedicated service in ensemble
  else if (ch == '3')
  {
    index = 25; //CHAN_11A = 216928;//SWR RP
    //SWR3
    serviceId = 0xD3A3;
    componentId = 0x4;
    //Tunes DAB index
    tuneIndex(index);
    //Starts an audio or data service
    startService(serviceId, componentId);
    
    //parse ensemble
    getEnsemble(ensembleHeader);
  }

  //Start dedicated service in ensemble
  else if (ch == '4')
  {
    //Hessen Süd
    //227360 kHz
    index = 33;
    //Radio Teddy
    serviceId = 0x1B2E;
    componentId = 0x2;

    //Tunes DAB index
    tuneIndex(index);
    //Starts an audio or data service
    startService(serviceId, componentId);

    //parse ensemble
    getEnsemble(ensembleHeader);
  }

  //Start dedicated service in ensemble
  else if (ch == '5')
  {
    //190640 kHz
    index = 9;
    //hr-iNFO
    serviceId = 0xD367;
    componentId = 0x4;

    //Tunes DAB index
    tuneIndex(index);
    //Starts an audio or data service
    startService(serviceId, componentId);

    //parse ensemble
    getEnsemble(ensembleHeader);
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
    rsqInformation_t rsqInformation;
    readRsqInformation(rsqInformation);

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
    //ensemble list available ?
    eventInformation_t eventInformation;
    readEventInformation(eventInformation);

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
    eventInformation_t eventInformation;
    readEventInformation(eventInformation);

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



void menuScanFrequency(char ch)
{
  if (ch == '?' || ch == 'F' || displayMenu)
  {
    //Print DAB Menu Scan
    serialPrintSi468x::dabPrintMenuScanFrequency();
    //Hide menu
    displayMenu = false;
  }

  //Get Frequency Table
  else if (ch == '0')
  {
    readFrequencyTable(frequencyTableHeader);

    serialPrintSi468x::dabPrintFrequencyTable(frequencyTableHeader);
    serialPrintSi468x::printFreeRam(getFreeRam());
  }

  //Set Frequency Table
  else if (ch == '1')
  {
    //Set the frequency table
    writeFrequencyTable(FREQ_TABLE_DEFAULT, 41);
    readFrequencyTable(frequencyTableHeader);

    serialPrintSi468x::dabPrintFrequencyTable(frequencyTableHeader);
    serialPrintSi468x::printFreeRam(getFreeRam());
  }

  //Set Frequency Table
  else if (ch == '2')
  {
    //Set the frequency table
    writeFrequencyTable(FREQ_TABLE_DE_RP, 3);
    readFrequencyTable(frequencyTableHeader);

    serialPrintSi468x::dabPrintFrequencyTable(frequencyTableHeader);
    serialPrintSi468x::printFreeRam(getFreeRam());
  }

  //Set Frequency Table
  else if (ch == '3')
  {
    //Set the frequency table
    writeFrequencyTable(FREQ_TABLE_EMPTY, 1);
    readFrequencyTable(frequencyTableHeader);

    serialPrintSi468x::dabPrintFrequencyTable(frequencyTableHeader);
    serialPrintSi468x::printFreeRam(getFreeRam());
  }

  //index Up
  else if (ch == 'l')
  {
    tune(index, true);
    Serial.println(index);
  }

  //index down
  else if (ch == 'k')
  {
    tune(index, false);
    Serial.println(index);
  }

  //Scan Index Up
  else if (ch == '.')
  {
    //remember start index
    rsqInformation_t rsqInformation;
    readRsqInformation(rsqInformation);
    unsigned char indexStart = rsqInformation.index;

    do
    {
      //tune index up
      tune(index, true);

      //get receive quality
      readRsqInformation(rsqInformation);
      Serial.print(F("Index:\t"));
      Serial.println(rsqInformation.index);

      //DAB found and valid if threshold for dab detected and greater than 4
      if ((rsqInformation.fastDect > 4) && rsqInformation.valid == 1)
      {
        Serial.println(F("Valid index found"));
        index = rsqInformation.index;
        Serial.println(index);
        break;
      }
      //around
      else if (rsqInformation.index == indexStart)
      {
        Serial.println(F("Nothing found"));
        //nothing found
        index = rsqInformation.index;
        Serial.println(index);
        break;
      }
    }
    while (rsqInformation.index != indexStart);
  }

  //Scan Index Down
  else if (ch == ',')
  {
    //remember start index
    rsqInformation_t rsqInformation;
    readRsqInformation(rsqInformation);
    unsigned char indexStart = rsqInformation.index;

    do
    {
      //tune index down
      tune(index, false);

      //get receive quality
      readRsqInformation(rsqInformation);
      Serial.print(F("Index:\t"));
      Serial.println(rsqInformation.index);

      //DAB found and valid if threshold for dab detected and greater than 4
      if ((rsqInformation.fastDect > 4) && rsqInformation.valid == 1)
      {
        Serial.println(F("Valid index found"));
        index = rsqInformation.index;
        Serial.println(index);
        break;
      }
      //around
      else if (rsqInformation.index == indexStart)
      {
        Serial.println(F("Nothing found"));
        //nothing found
        index = rsqInformation.index;
        Serial.println(index);
        break;
      }
    }
    while (rsqInformation.index != indexStart);
  }

  //Bandscan
  else if (ch == 's')
  {
    scanIndices(indexListHeader);
    serialPrintSi468x::dabPrintIndexList(indexListHeader);

    if (indexListHeader.indexList != nullptr)
    {
      //Set index to first entry in scanned table
      index = indexListHeader.indexList[0].index;
      Serial.println(F("Index: "));
      Serial.println(index);
      tuneIndex(index);


      eventInformation_t eventInformation;
      //wait for ensemble list available
      for (uint8_t i = 0; i < 10; i++)
      {
        readEventInformation(eventInformation);
        delay(1000);
        Serial.println(F("Wait for Servicelist..."));
        if (eventInformation.serviceListAvailable == 1) break;
      }

      if (eventInformation.serviceListAvailable == 1)
      {
        //getEnsemble(ensembleHeader);
        serialPrintSi468x::dabPrintEnsemble(ensembleHeader);
        //startfirstService
        startFirstService(serviceId, componentId);
      }
      else
      {
        serialPrintSi468x::printError(2);
      }
    }
    serialPrintSi468x::printFreeRam(getFreeRam());
  }

  //Get Valid Index List
  else if (ch == 'i')
  {
    serialPrintSi468x::dabPrintIndexList(indexListHeader);
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
    eventInformation_t eventInformation;
    readEventInformation(eventInformation);
    serialPrintSi468x::dabPrintEventInformation(eventInformation);
  }

  //Get audio information
  else if (ch == 'A')
  {
    audioInformation_t audioInformation;
    readAudioInformation(audioInformation);
    serialPrintSi468x::dabPrintComponentAudioInfo(audioInformation);
  }

  //Get and print technical information
  else if (ch == 'x')
  {
    componentTechnicalInformation_t componentTechnicalInformation;
    readComponentTechnicalInformation(componentTechnicalInformation, serviceId, componentId);
    //Print technical information about the component
    serialPrintSi468x::dabPrintComponentTechnicalInformation(componentTechnicalInformation);
  }

  //Get and print date and time
  else if (ch == '#')
  {
    timeDab_t dabTime;
    readDateTime(dabTime);
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
    Serial.print(F("RAM: "));
    Serial.println(getFreeRam());
    Serial.println();
  }

  //Print DAB frequency information list
  else if (ch == 'i')
  {
    //check events
    eventInformation_t eventInformation;
    readEventInformation(eventInformation);

    //serviceLinkingInformation
    if (eventInformation.serviceLinkingInterrupt == 1)
    {
      serviceLinkingInformation_t serviceLinkingInformation;
      readServiceLinkingInfo(serviceLinkingInformation, serviceId);
      serialPrintSi468x::printServiceLinkingInformation(serviceLinkingInformation);
    }
    else
    {
      serialPrintSi468x::printError(3);
    }

    //readFrequencyInformationTable
    if (eventInformation.frequencyInterrupt == 1)
    {
      frequencyInformationTableHeader_t frequencyInformationTableHeader;
      readFrequencyInformationTable(frequencyInformationTableHeader);

      serialPrintSi468x::printFrequencyInformation(frequencyInformationTableHeader);
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
    testVaractorCap(index);
  }

  //Read And Print RSSI
  else if (ch == 'R')
  {
    while (ch != 'x')
    {
      //Get user input
      if (Serial.available()) ch = Serial.read();
      //Print rssi
      serialPrintSi468x::printRssi(readRssi());
    }
  }

  //Close or Open Front End Switch;
  else if (ch == 'W')
  {
    //0x1712 DAB_TUNE_FE_CFG Configure VHFSW switch from open to close
    //VHFSW sets the open or closed state for the front end switch.
    //0 : Switch Open
    //1 : Switch Closed Default
    uint8_t frontEndSwitch = 1;
    frontEndSwitch = readPropertyValue(DAB_TUNE_FE_CFG);
    if (frontEndSwitch == 1)
    {
      frontEndSwitch = 0;
      writePropertyValue(DAB_TUNE_FE_CFG, frontEndSwitch);
      Serial.println(F("Open"));
    }
    else
    {
      frontEndSwitch = 1;
      writePropertyValue(DAB_TUNE_FE_CFG, frontEndSwitch);
      Serial.println(F("Closed"));
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
    statusRegister_t statusRegister;
    readStatusRegister(statusRegister);
    serialPrintSi468x::printStatusRegister(statusRegister);
  }

  //Print Device Part Info
  else if (ch == 'n')
  {
    partInfo_t partInfo;
    readPartInfo(partInfo);
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
    firmwareInformation_t firmwareInformation;
    readFirmwareInformation(firmwareInformation);
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
  /*
    else if (ch == 'd')
    {
    powerDown(true);
    }
    //power up
    else if (ch == 'u')
    {
    powerDown(false);
    }
  */

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
    tuneIndex(index);
    //Starts inital audio service
    startService(serviceId, componentId);
  }

  else
  {
    //nothing
  };
}

unsigned short getFreeRam()
{
  extern unsigned int __heap_start;
  extern unsigned int *__brkval;
  //test variable created on stack
  unsigned int newVariable = 0;

  return (unsigned int) &newVariable - (__brkval == 0 ? (unsigned int) &__heap_start : (unsigned int) __brkval);
}

//Get status of mute
unsigned char readMute()
{
  unsigned char channelMuted = 0;
  channelMuted = readPropertyValue(AUDIO_MUTE);
  return channelMuted;
}

//Controls the audio mute on each audio output
void writeMute(unsigned char channelMuted)
{
  //  0 : Do not mute audio outputs
  //  1 : Mute Left Audio Out.
  //  2 : Mute Right Audio Out.
  //  3 : Mute both Left and Right Audio Out
  //Range ok ?
  if (channelMuted > 3) channelMuted = 0;

  writePropertyValue(AUDIO_MUTE, channelMuted);
}

unsigned char volumeUp()
{
  unsigned char volume = readPropertyValue(AUDIO_ANALOG_VOLUME);

  //volume 0...63;
  if (volume < 63) volume ++;
  else volume = 63;

  writePropertyValue(AUDIO_ANALOG_VOLUME, volume);
  return volume;
}

unsigned char volumeDown()
{
  unsigned char volume = readPropertyValue(AUDIO_ANALOG_VOLUME);

  //volume 0...63;
  if (volume > 0) volume --;
  else volume = 0;

  writePropertyValue(AUDIO_ANALOG_VOLUME, volume);
  return volume;
}

//Test varactor tuning capacitor
void testVaractorCap(unsigned char& index, unsigned char injection)
{
  //0x1710 FM/DAB_TUNE_FE_-VARM Slope of Varactor vs Frequency Curve
  //0x1711 FM/DAB_TUNE_FE_-VARB Intercept of Varactor vs Frequency Curve
  //0x1712 FM/DAB_TUNE_FE_CFG Configure VHFSW switch from open to close

  //measured rssi
  unsigned short rssi = 0;
  //max rssi
  unsigned short rssiMax = 0;

  unsigned char number = 129;
  //list of rssi measurments
  unsigned short varCapList[number] = {0};

  unsigned char varCapMax = 0;

  //count all values from 1...128
  for (unsigned char i = 1; i < number; i++)
  {
    //Serial.print(F("Varactor capacity: "));
    //Serial.println(varCap);
    //Serial.print(((varCap - 1) * 0.25));
    //Serial.print(F("4.2f pF \n",(cap-1)*0.25);
    //31.75 pF Max
    tuneIndex(index, i, injection);

    rssi = readRssi();

    //save in list
    varCapList[i] = rssi;

    //check if rssi better
    if (rssi > rssiMax)
    {
      //save new values
      rssiMax = rssi;
      varCapMax = i;
    }
    Serial.print(i);
    Serial.print(",");
    Serial.println(rssi / 256.00);
    //rssi = rssi >> 7;//shift 2^7
  }
  //Summary
  Serial.println();
  Serial.print(F("Index:\t"));
  Serial.println(index);
  Serial.print(F("Max Rssi:\t"));
  Serial.print(rssiMax / 256.0);
  Serial.println(F(" dBuV"));
  Serial.print(F("Varactor count:"));
  Serial.print(varCapMax);
  Serial.print(F(" pf:"));
  Serial.println(varCapMax / 4);
  Serial.println();
}
