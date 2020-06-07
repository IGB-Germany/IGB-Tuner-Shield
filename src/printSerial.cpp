/*Serial Monitor Print Functions*/
#include "printSerial.h"

/*snprintf, Serial*/
#include "Arduino.h"

namespace serial
{

/*Print version information*/
void printVersionInformation()
{
  Serial.println(F("Version Info"));
  //Serial.println(F(__FILE__));
  Serial.println(F(__DATE__));
  //Serial.println(F(__TIME__));
  Serial.println(swVersion);
  Serial.println();
}

/*Print device status information*/
void devicePrintStatus(deviceStatusInformation_t deviceStatusInformation)
{
  if      (deviceStatusInformation.pUpState == 0)  Serial.println(F("Waiting for Power Up Cmd"));
  else if (deviceStatusInformation.pUpState == 1)  Serial.println(F("Reserved"));
  else if (deviceStatusInformation.pUpState == 2)  Serial.println(F("Bootloader running"));
  else if (deviceStatusInformation.pUpState == 3)  Serial.println(F("Application running"));
  else                                             Serial.println(F("Error"));
  
  Serial.print(F("Status Device:\t\t"));
  Serial.println(deviceStatusInformation.pUpState);
  Serial.print(F("Ensemble IRQ:\t\t"));
  Serial.println(deviceStatusInformation.dacqInt);
  Serial.print(F("Data IRQ:\t\t"));
  Serial.println(deviceStatusInformation.dsrvInt);
  Serial.print(F("Digital Event IRQ:\t"));
  Serial.println(deviceStatusInformation.devNtInt);
  Serial.print(F("Seek/Tune complete:\t"));
  Serial.println(deviceStatusInformation.stcInt);
  Serial.print(F("Clear to Send:\t\t"));
  Serial.println(deviceStatusInformation.cts);
  Serial.print(F("Command Error:\t\t"));
  Serial.println(deviceStatusInformation.cmdErr);
  Serial.print(F("DSP Error:\t\t"));
  Serial.println(deviceStatusInformation.dspErr);
  Serial.print(F("Reply Overflow Error:\t"));
  Serial.println(deviceStatusInformation.repOfErr);
  Serial.print(F("Command Overflow Error:\t"));
  Serial.println(deviceStatusInformation.cmdOfErr);
  Serial.print(F("Arbiter Error:\t\t"));
  Serial.println(deviceStatusInformation.arbErr);
  Serial.print(F("Non Recoverable Error:\t"));
  Serial.println(deviceStatusInformation.nonRecErr);
  Serial.print(F("Error Code:\t\t"));
  Serial.println(deviceStatusInformation.cmdErrCode);
  Serial.println();
}

/*Print Device Part Number*/
void devicePrintPartNumber(devicePartNumber_t devicePartNumber)
{
  Serial.print(F("Chip Rev.:\t"));
  Serial.println(devicePartNumber.chipRev);
  Serial.print(F("ROM Id:\t\t"));
  Serial.println(devicePartNumber.romId);
  Serial.print(F("Part No.:\t"));
  Serial.println(devicePartNumber.partNumber);
  Serial.println();
}

/*Print Device Image*/
void devicePrintImage(unsigned char deviceImage)
{
  Serial.print(F("Firmware Image Mode:\t"));
  Serial.println(deviceImage);
  if (deviceImage == 0) Serial.println(F("Bootloader"));
  else if (deviceImage == 1) Serial.println(F("FMHD"));
  else if (deviceImage == 2) Serial.println(F("DAB"));
  else if (deviceImage == 3) Serial.println(F("TDMB or data only DAB"));
  else if (deviceImage == 4) Serial.println(F("FMHD Demod"));
  else if (deviceImage == 5) Serial.println(F("AMHD"));
  else if (deviceImage == 6) Serial.println(F("AMHD Demod"));
  else if (deviceImage == 7) Serial.println(F("DAB Demod"));
  else if (deviceImage == 16) Serial.println(F("Reserved"));
  else    Serial.println(F("Device not ready"));
  Serial.println();
}

/*Print Firmware Information*/
void devicePrintFirmwareInformation(deviceFirmwareInformation_t deviceFirmwareInformation)
{
  Serial.print(F("Firmware Revision:\t"));
  Serial.print(deviceFirmwareInformation.revisionNumberMajor);
  Serial.print(F("."));
  Serial.print(deviceFirmwareInformation.revisionNumberMinor);
  Serial.print(F("."));
  Serial.println(deviceFirmwareInformation.revisionNumberBuild);
  Serial.print(F("No. Subversion Flag:\t"));
  Serial.println(deviceFirmwareInformation.noSvnFlag);
  Serial.print(F("SVN Tag/Branch/Trunk:\t"));
  Serial.println(deviceFirmwareInformation.location);
  Serial.print(F("Mixed Rev. Flag:\t"));
  Serial.println(deviceFirmwareInformation.mixedRevFlag);
  Serial.print(F("Local Mod. Flag:\t"));
  Serial.println(deviceFirmwareInformation.localModFlag);
  Serial.print(F("Subversion Id:\t\t"));
  Serial.println(deviceFirmwareInformation.svnId);
  Serial.println();
}

/*Print power up parameters*/
void devicePrintPowerUpArguments(devicePowerUpArguments_t devicePowerUpArguments)
{
  Serial.println(F("Power Up Arguments"));
  Serial.print(F("Clock Mode (0,1,2,3):\t"));
  Serial.println(devicePowerUpArguments.clockMode);
  Serial.print(F("XOSC TResistor Size:\t"));
  Serial.println(devicePowerUpArguments.trSize);
  Serial.print(F("XOSC Ibias Start:\t"));
  Serial.println(devicePowerUpArguments.iBiasStart);
  Serial.print(F("XTAL Frequency Hz:\t"));
  Serial.println(devicePowerUpArguments.xtalFreq);
  Serial.print(F("XOSC Tuning Capacitor:\t"));
  Serial.println(devicePowerUpArguments.cTune);
  Serial.print(F("XOSC Ibias Run:\t\t"));
  Serial.println(devicePowerUpArguments.iBiasRun);
  Serial.println();
}

//Print Rssi Information In 8.8 Format
void devicePrintRssi(unsigned short rssi)
{
  Serial.print(F("Rssi:\t"));
  Serial.println(rssi / 256.00);
}

/*Print property value*/
void devicePrintProperty(unsigned short devicePropertyId, unsigned short devicePropertyValue)
{
  char string[30];
  snprintf(string, 30, "Id: 0x%04X  Value:  0x%04X", devicePropertyId, devicePropertyValue);
  Serial.println(string);
}

/*Print property value list*/
void devicePrintAllProperties(struct devicePropertyValueList_t devicePropertyValueList[], unsigned char num)
{
  Serial.println(F("Device Properties"));
  for (unsigned char i = 0; i < num; i++)
  {
    devicePrintProperty(devicePropertyValueList[i].devicePropertyId, devicePropertyValueList[i].devicePropertyValue);
  }
  Serial.println();
}

/*Print status information about the received signal quality*/
void dabPrintRsqStatus(dabRsqInformation_t dabRsqInformation)
{
  Serial.println(F("Receive Quality Info:"));
  Serial.print(F("Hardmute IRQ:\t"));
  Serial.println(dabRsqInformation.hardMuteInterrupt);
  Serial.print(F("Fic Error IRQ:\t"));
  Serial.println(dabRsqInformation.ficErrorInterrupt);
  Serial.print(F("Acquired IRQ:\t"));
  Serial.println(dabRsqInformation.acqInterrupt);
  Serial.print(F("Rssi High IRQ:\t"));
  Serial.println(dabRsqInformation.rssiHighInterrupt);
  Serial.print(F("Rssi LowQ IRQ:\t"));
  Serial.println(dabRsqInformation.rssiLowInterrupt);
  
  Serial.print(F("Hardmute:\t"));
  Serial.println(dabRsqInformation.hardmute);
  Serial.print(F("FIC Error:\t"));
  Serial.println(dabRsqInformation.ficError);
  Serial.print(F("Acquired:\t"));
  Serial.println(dabRsqInformation.acq);
  Serial.print(F("Valid:\t\t"));
  Serial.println(dabRsqInformation.valid);
  Serial.print(F("RSSI:\t\t"));
  Serial.print(dabRsqInformation.rssi, DEC);
  Serial.println(F(" dB"));
  Serial.print(F("SNR 0-20:\t"));
  Serial.print(dabRsqInformation.snr, DEC);
  Serial.println(F(" dB"));
  Serial.print(F("Fic Quality:\t"));
  Serial.println(dabRsqInformation.ficQuality);
  Serial.print(F("CNR 0-54:\t"));
  Serial.print(dabRsqInformation.cnr);
  Serial.println(F(" dB"));
  Serial.print(F("Block Error: \t"));
  Serial.println(dabRsqInformation.fibErrorCount);
  Serial.print(F("Frequency: \t"));
  Serial.print(dabRsqInformation.frequency);
  Serial.println(F(" kHz"));
  Serial.print(F("Index:\t\t"));
  Serial.println(dabRsqInformation.index);
  Serial.print(F("Fft Offset:\t"));
  Serial.println(dabRsqInformation.fftOffset);
  Serial.print(F("Varactor:\t"));
  Serial.println(dabRsqInformation.varactorCap);
  Serial.print(F("Capacity Units:\t"));
  Serial.println(dabRsqInformation.cuLevel);
  Serial.print(F("Fast Detect:\t"));
  Serial.println(dabRsqInformation.fastDect);
  Serial.println();
}

/*Print status information of the digital service*/
void dabPrintEnsembleInformation(struct dabEnsembleInformation_t dabEnsembleInfo)
{
  Serial.println(F("Ensemble Info"));
  Serial.print(F("Ensemble Id:\t\t0x"));
  Serial.println(dabEnsembleInfo.ensembleId, HEX);
  Serial.print(F("Ensemble Label:\t\t"));
  Serial.println(dabEnsembleInfo.ensembleLabel);
  Serial.print(F("Extended Country Code:\t"));
  Serial.println(dabEnsembleInfo.ecc);
  Serial.print(F("Character Set:\t\t0b"));
  Serial.println(dabEnsembleInfo.charSet);
  Serial.print(F("Abbreviation Mask:\t0b"));
  Serial.println(dabEnsembleInfo.abbreviationMask, BIN);
  Serial.println();
}

/*Print event information of the digital service*/
void dabPrintEventInformation(struct dabEventInformation_t dabEventInfo)
{
  Serial.println(F("Event Information:"));
  Serial.print(F("Reconfiguration:\t"));
  Serial.println(dabEventInfo.ensembleReconfigInterrupt);
  Serial.print(F("Reconfig. Warning:\t"));
  Serial.println(dabEventInfo.ensembleReconfigWarningInterrupt);
  Serial.print(F("Announcement:\t\t"));
  Serial.println(dabEventInfo.announcementInterrupt);
  Serial.print(F("Other Service:\t\t"));
  Serial.println(dabEventInfo.otherServiceInterrupt);
  Serial.print(F("Service Linking:\t"));
  Serial.println(dabEventInfo.serviceLinkingInterrupt);
  Serial.print(F("Frequency Interrupt:\t"));
  Serial.println(dabEventInfo.frequencyInterrupt);
  Serial.print(F("Service List IRQ:\t"));
  Serial.println(dabEventInfo.serviceListInterrupt);
  Serial.print(F("Announcement avail.:\t"));
  Serial.println(dabEventInfo.announcementAvailable);
  Serial.print(F("Other Service avail.:\t"));
  Serial.println(dabEventInfo.otherServiceAvailable);
  Serial.print(F("Service Linking avail.:\t"));
  Serial.println(dabEventInfo.serviceLinkingAvailable);
  Serial.print(F("Frequency List avail.:\t"));
  Serial.println(dabEventInfo.frequencyAvailable);
  Serial.print(F("Service List avail.:\t"));
  Serial.println(dabEventInfo.serviceListAvailable);
  Serial.print(F("Service List Version:\t"));
  Serial.println(dabEventInfo.currentServiceListVersion);
  Serial.println();
}

/*Print component audio information*/
void dabPrintComponentAudioInfo(dabAudioComponentInformation_t dabAudioComponentInformation)
{
  Serial.println(F("Audio Info:"));
  Serial.print(F("Bit Rate:\t"));
  Serial.print(dabAudioComponentInformation.audioBitRate);
  Serial.println(F(" kbps"));
  Serial.print(F("Sample Rate:\t"));
  Serial.print(dabAudioComponentInformation.audioSampleRate);
  Serial.println(F(" Hz"));
  Serial.print(F("PS flag:\t"));
  Serial.println(dabAudioComponentInformation.audioPsFlag);
  Serial.print(F("SBR flag:\t"));
  Serial.println(dabAudioComponentInformation.audioSbrFlag);
  Serial.print(F("Mode 0/1/2/3:\t"));
  Serial.println(dabAudioComponentInformation.audioMode);
  Serial.print(F("DRC Gain:\t"));
  Serial.print(dabAudioComponentInformation.audioDrcGain);
  Serial.println(F(" dB"));
  Serial.println();
}

/*Print local date and time or UTC*/
void dabPrintDateTimeInformation(unsigned short year, unsigned char month, unsigned char day, unsigned char hour, unsigned char minute, unsigned char second, unsigned char timeType)
{
  const unsigned char len = 28;
  char string[len];
  snprintf(string, len, "UTC:%d  %02d.%02d.%4d  %02d:%02d:%02d", timeType, day, month, year, hour, minute, second);
  Serial.println(F("Date and Time Information"));
  Serial.println(string);
  Serial.println();
}

/*Print component technical information*/
void dabPrintComponentTechnicalInformation(dabComponentTechnicalInformation_t dabComponentTechnicalInformation)
{
  Serial.println(F("Component Technical Info"));
  Serial.print(F("Service Mode (0-8):\t"));
  Serial.print(dabComponentTechnicalInformation.serviceMode);
  //Serial.println(F("0:AUDIO, 1:DATA, 2:FIDC, 3:MSC, 4:DAB+, 5:DAB, 6:FIC, 7:XPAD, 8:NO MEDIA")); 
  if (dabComponentTechnicalInformation.serviceMode == 4)
    Serial.println(F(" DAB+"));
  else if (dabComponentTechnicalInformation.serviceMode == 1)
    Serial.println(F(" Data"));
  else Serial.println();
  Serial.print(F("UEP/EEP Protection:\t")); 
  Serial.println(dabComponentTechnicalInformation.protectionInfo);
  Serial.print(F("Bit Rate:\t\t"));
  Serial.print(dabComponentTechnicalInformation.bitRate);
  Serial.println(F(" kbps"));
  Serial.print(F("Capacity Units:\t\t"));
  Serial.println(dabComponentTechnicalInformation.numberCU);
  Serial.print(F("C.U. Start Adress:\t"));
  Serial.println(dabComponentTechnicalInformation.addressCU );
  Serial.println();
}

/*Print information about the component*/
void dabPrintComponentInformation(dabComponentInformation_t dabComponentInformation)
{
  Serial.println(F("Component Information"));
  Serial.print(F("Global Id:\t"));
  Serial.println(dabComponentInformation.globalId);
  Serial.print(F("Language:\t"));
  Serial.println(dabComponentInformation.language);
  Serial.print(F("Character Set:\t"));
  Serial.println(dabComponentInformation.characterSet);
  Serial.print(F("Label:\t"));
  Serial.println(dabComponentInformation.label);
  Serial.print(F("Abbrev. Mask:\t"));
  Serial.println(dabComponentInformation.abbreviationMask, BIN);
  Serial.print(F("Number of Apps:\t"));
  Serial.println(dabComponentInformation.numberUserAppTypes);
  Serial.print(F("User App. Type:\t"));
  Serial.println(dabComponentInformation.userAppType);
  Serial.print(F("Data Length:\t"));
  Serial.println(dabComponentInformation.lenUserApp);
  Serial.print(F("Field Length:\t"));
  Serial.println(dabComponentInformation.lenUserAppData);
  Serial.println();
}

/*Print Service ID and Component ID*/
void dabPrintIds(unsigned long dabServiceId, unsigned long dabComponentId)
{
  Serial.print(F("Service Id: 0x"));
  Serial.print(dabServiceId, HEX);
  Serial.print(F(" Component Id: 0x"));
  Serial.println(dabComponentId, HEX);
}

/*Print DAB frequency*/
void dabPrintFrequency(unsigned long frequency)
{
  Serial.print(F("Frequency:\t"));
  Serial.print(frequency);
  Serial.println(F(" kHz"));
  Serial.println();
}
/*Print frequency table*/
void dabPrintFrequencyTable(unsigned long* dabFreqTable, unsigned char dabNumFreq)
{
  char string[20];

  Serial.println(F("Frequency Table"));
  Serial.print(F("Number:  "));
  Serial.println(dabNumFreq);

  for (unsigned char j = 0; j < dabNumFreq; j++)
  {
    snprintf(string, 20, "%2u : %6lu kHz", j , dabFreqTable[j]) ;
    Serial.println(string);
  }
  Serial.println();
}

/*Print index*/
void dabPrintIndex(unsigned char index)
{
  Serial.print(F("Index:\t"));
  Serial.println(index);
  Serial.println();
}

/*Print valid index list*/
void dabPrintValidIndexList(unsigned char dabNumValidIndex, unsigned char dabValidIndexList[])
{
  Serial.println(F("Valid Index List:"));
  Serial.println();
  
  for (unsigned char j = 0; j < dabNumValidIndex; j++)
  {
    Serial.print(F("Valid Index "));
    Serial.print(j);
    Serial.print(F(" :"));
    Serial.println(dabValidIndexList[j]);
  }
  Serial.println();
}

/*Print property value*/
void dabPrintProperty(unsigned short dabPropertyId, unsigned short dabPropertyValue)
{
  char string[30];
  snprintf(string, 30, "Id: 0x%04X  Value:  0x%04X", dabPropertyId, dabPropertyValue);
  Serial.println(string);
}

/*Print property value list*/
void dabPrintAllProperties(struct dabPropertyValueList_t dabPropertyValueList[], unsigned char num)
{
  Serial.println(F("DAB Properties:"));
  for (unsigned char i = 0; i < num; i++)
  {
    dabPrintProperty(dabPropertyValueList[i].dabPropertyId, dabPropertyValueList[i].dabPropertyValue);
  }
  Serial.println();
}

/*Print DAB Actual Service*/
void dabPrintActualService(dabEnsemble_t* dabEnsembleList, unsigned char dabActualService)
{
  Serial.print(F("Service Number: "));
  Serial.print(dabEnsembleList[dabActualService].serviceNumber);
  Serial.print(F(" Service Id: 0x"));
  Serial.print(dabEnsembleList[dabActualService].serviceId, HEX);
  Serial.print(F(" Component Id: 0x"));
  Serial.print(dabEnsembleList[dabActualService].componentList[0], HEX);
  Serial.print(F(" Type: "));
  Serial.println(dabEnsembleList[dabActualService].pdFlag);
  Serial.println();
}

/*Prints status information of the digital service*/
void dabPrintDigitalServiceInformation(struct dabServiceInformation_t dabServiceInfo)
{
  Serial.println(F("Service Info"));
  Serial.print(F("Service Label:\t\t"));
  Serial.println(dabServiceInfo.serviceLabel);
  Serial.print(F("Data Flag:\t\t"));
  Serial.println(dabServiceInfo.pdFlag);
  Serial.print(F("Program Type PTY:\t"));
  Serial.println(dabServiceInfo.pType);
  Serial.print(F("Service Linking Info:\t"));
  Serial.println(dabServiceInfo.serviceLinkingInfoFlag);
  Serial.print(F("Only Local Service:\t"));
  Serial.println(dabServiceInfo.localFlag);
  Serial.print(F("Scrambled Access:\t"));
  Serial.println(dabServiceInfo.caId);
  Serial.print(F("Number of Components:\t"));
  Serial.println(dabServiceInfo.numComponents);
  Serial.print(F("Character Set:\t\t0b"));
  Serial.println(dabServiceInfo.characterSet, BIN);
  Serial.print(F("Extended Country Code:\t"));
  Serial.println(dabServiceInfo.ecc);
  Serial.print(F("Abbreviation Mask:\t0b"));
  Serial.println(dabServiceInfo.abbreviationMask, BIN);
  Serial.println();
}

/*Print service list header*/
void dabPrintServiceListHeader(struct dabServiceListHeader_t dabServiceListHeader)
{
  Serial.println(F("Header"));
  Serial.print(F("Listsize:\t"));
  Serial.println(dabServiceListHeader.listSize);
  Serial.print(F("List Version:\t"));
  Serial.println(dabServiceListHeader.version);
  Serial.print(F("Services:\t"));
  Serial.println(dabServiceListHeader.numServices);
  /*
    Serial.print(F("Reserved 1:         "));
    Serial.println(dabServiceListHeader.reserved1);
    Serial.print(F("Reserved 2:         "));
    Serial.println(dabServiceListHeader.reserved2);
    Serial.print(F("Reserved 3:         "));
    Serial.println(dabServiceListHeader.reserved3);
  */
  Serial.println();
}

//Print Ensemble
void dabPrintEnsemble(dabEnsemble_t* dabEnsembleList, unsigned char dabNumServices)
{
  for (unsigned char serviceNum = 0; serviceNum < dabNumServices; serviceNum++)
  {
    Serial.print(F("Service Number: "));
    Serial.print(dabEnsembleList[serviceNum].serviceNumber);
    Serial.print(F(" Service Id: "));
    Serial.print(dabEnsembleList[serviceNum].serviceId, HEX);
    //Serial.print(F(" Component Id: "));
    //Serial.print(dabEnsembleList[serviceNum].componentId, HEX);
    Serial.print(F(" Number of Components: "));
    Serial.print(dabEnsembleList[serviceNum].numComponents);
    Serial.print(F(" Data Flag: "));
    Serial.println(dabEnsembleList[serviceNum].pdFlag);

    for (unsigned char componentNum = 0; componentNum < dabEnsembleList[serviceNum].numComponents; componentNum++)
    {
      Serial.print(F("Component Id: "));
      Serial.println(dabEnsembleList[serviceNum].componentList[componentNum], HEX);
    }
  }
  Serial.println();
}

/*Print Service Data*/
void dabPrintServiceData(struct dabServiceData_t dabServiceData)
{
  Serial.println(F("Service Data"));
  Serial.print(F("Error: "));
  Serial.print(dabServiceData.errorInterrupt);
  Serial.print(F(" Overflow: "));
  Serial.print(dabServiceData.overflowInterrupt);
  Serial.print(F(" Packet: "));
  Serial.println(dabServiceData.packetInterrupt);
  Serial.print(F("Buffer Count:\t"));
  Serial.println(dabServiceData.bufferCount);
  Serial.print(F("Status Service:\t"));
  Serial.println(dabServiceData.statusService);
  Serial.print(F("Source 0,1,2,3:\t"));
  Serial.println(dabServiceData.dataSource);
  Serial.print(F("Data Type:\t"));
  Serial.println(dabServiceData.dataType);
  Serial.print(F("Service Id:\t0x"));
  Serial.println(dabServiceData.serviceId, HEX);
  Serial.print(F("Component Id:\t0x"));
  Serial.println(dabServiceData.componentId, HEX);
  Serial.print(F("Length of Data:\t"));
  Serial.println(dabServiceData.dataLength);
  Serial.print(F("Segment Number:\t"));
  Serial.println(dabServiceData.segmentNumber);
  Serial.print(F("Number Segments:"));
  Serial.println(dabServiceData.numberSegments);
  Serial.println();
}

/*Print dls*/
void dabPrintDynamicLabelSegment(char dls[])
{
  Serial.println(dls);
  Serial.println();
}

//Print error no reception
void dabPrintErrorNoReception()
{
  Serial.println(F("Error - No Reception"));
  Serial.println();
}

/*Print General Menu*/
/*Reduce memory needs*/
void dabPrintMenu()
{
  Serial.println(F("?: Print this"));
  Serial.println(F("M: DAB Menu Main"));
  Serial.println(F("T: DAB Menu Technical"));
  Serial.println(F("S: DAB Menu Scan"));
  Serial.println(F("D: DAB Menu Device"));
  Serial.println();
}

/*Print DAB Menu Main*/
void dabPrintMenuMain()
{
  Serial.println(F("DAB Menu Main"));
  /*Print General Menu*/
  dabPrintMenu();
  Serial.println(F("q: Receive Quality"));
  Serial.println(F("e: Info Ensemble"));
  Serial.println(F("r: Info Service"));
  Serial.println(F("t: Show Running Service"));

  Serial.println(F("c: Check Service Data"));
  Serial.println(F("v: Get Service Data"));
  Serial.println();
  Serial.println(F("p: Parse Ensemble"));//Audio, Data flag ?
  Serial.println(F("d: Next Service"));
  Serial.println(F("a: Previous Service"));
  Serial.println();
  Serial.println(F("1: Favorite 1 (sunshine)"));
  Serial.println(F("2: Favorite 2 (Bob)"));
  Serial.println(F("3: Favorite 3 (SWR3)"));
  Serial.println(F("4: Favorite 4 (TEDDY)"));
  Serial.println(F("5: Favorite 5 (FFH)"));
  Serial.println(F("x: Start Service"));
  Serial.println(F("y: Stop Service"));

  Serial.println(F("m: Mute/Unmute Audio"));
  Serial.println(F("w: Volume Up"));
  Serial.println(F("s: Volume Down"));
  Serial.println();
}

/*Print DAB Technical Menu*/
void dabPrintMenuTechnical()
{
  Serial.println(F("DAB Menu Technical"));
  /*Print General Menu*/
  dabPrintMenu();
  Serial.println(F("e: Event Info"));
  Serial.println(F("a: Audio Info"));
  Serial.println(F("x: Technical Info"));
  Serial.println(F("d: Date and Time"));
  Serial.println(F("c: Component Info"));
  Serial.println(F("f: Frequency List"));
  Serial.println(F("q: Receive Quality"));
  Serial.println(F("t: Test Varactor"));
  Serial.println(F("r: RSSI"));
  Serial.println(F("p: Properties DAB"));
  Serial.println();
}

/*Print DAB Scan Menu*/
void dabPrintMenuScan()
{
  Serial.println(F("DAB Menu Frequency Scan"));
  /*Print General Menu*/
  dabPrintMenu();
  Serial.println(F("f: Get Frequency Table"));
  Serial.println(F("1: Set Standard Table"));
  Serial.println(F("2: Set Table 2"));
  Serial.println(F("3: Set Table 3"));
  Serial.println(F("q: Receive Quality"));
  Serial.println(F("e: Ensemble Information"));
  Serial.println(F("h: Service List Header"));
  Serial.println(F("u: Index Manual Up"));
  Serial.println(F("d: Index Manual Down"));
  Serial.println(F("s: Index Bandscan"));
  Serial.println(F("i: Index Valid List"));
  Serial.println(F("+: Index Valid Up:"));
  Serial.println(F("-: Index Valid Down"));
  Serial.println();
}

/*Print Device Menu*/
void dabPrintMenuDevice()
{
  Serial.println(F("DAB Menu Device"));
  /*Print General Menu*/
  dabPrintMenu();
  Serial.println(F("v: Version Info"));
  Serial.println(F("a: Power Up Arguments"));
  Serial.println(F("n: Part Number"));
  Serial.println(F("m: Firmware Image Mode"));
  Serial.println(F("f: Firmware Info"));
  Serial.println(F("s: Status of Device"));
  Serial.println(F("p: Properties Device"));
  Serial.println();
}

}/*namespace serial*/
