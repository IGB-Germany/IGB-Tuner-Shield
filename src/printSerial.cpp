//Serial Monitor Print Functions
#include "printSerial.h"

//use snprintf, Serial
#include "Arduino.h"

//namespace to avoid naming conflicts
namespace serialPrintSi468x
{

void printVersion(const char version[])
{
  Serial.println(F("Version Info"));
  //Serial.println(F(__FILE__));
  Serial.println(F(__DATE__));
  //Serial.println(F(__TIME__));
  Serial.print(F("Version:\t"));
  Serial.println(version);
  Serial.println();
}

void printStatusRegister(statusRegister_t statusRegister)
{
  if      (statusRegister.state == 0)  Serial.println(F("Waiting for Power Up Cmd"));
  else if (statusRegister.state == 1)  Serial.println(F("Reserved"));
  else if (statusRegister.state == 2)  Serial.println(F("Bootloader running"));
  else if (statusRegister.state == 3)  Serial.println(F("Application running"));
  else                                             Serial.println(F("Error"));
  Serial.print(F("Clear to Send:\t\t"));
  Serial.println(statusRegister.cts);
  Serial.print(F("Command Error:\t\t"));
  Serial.println(statusRegister.cmdErr);
  Serial.print(F("Ensemble INT:\t\t"));
  Serial.println(statusRegister.dacqInt);
  Serial.print(F("Service INT:\t\t"));
  Serial.println(statusRegister.dsrvInt);
  Serial.print(F("Seek/Tune complete:\t"));
  Serial.println(statusRegister.stcInt);
  Serial.print(F("Event INT:\t\t"));
  Serial.println(statusRegister.eventInt);
  Serial.print(F("Status Device:\t\t"));
  Serial.println(statusRegister.state);
  Serial.print(F("FrontEnd Error:\t\t"));
  Serial.println(statusRegister.rfFeErr);
  Serial.print(F("DSP Error:\t\t"));
  Serial.println(statusRegister.dspErr);
  Serial.print(F("Reply Overflow Error:\t"));
  Serial.println(statusRegister.repOfErr);
  Serial.print(F("Command Overflow Error:\t"));
  Serial.println(statusRegister.cmdOfErr);
  Serial.print(F("Arbiter Error:\t\t"));
  Serial.println(statusRegister.arbErr);
  Serial.print(F("Non Recoverable Error:\t"));
  Serial.println(statusRegister.nonRecErr);
  Serial.print(F("Error Code:\t\t"));
  Serial.println(statusRegister.cmdErrCode);
  Serial.println();
}

void printPartInfo(partInfo_t partInfo)
{
  Serial.print(F("Chip Rev.:\t"));
  Serial.println(partInfo.chipRev);
  Serial.print(F("ROM Id:\t\t"));
  Serial.println(partInfo.romId);
  Serial.print(F("Part No.:\t"));
  Serial.println(partInfo.partNumber);
  Serial.println();
}

void printSystemState(unsigned char systemState)
{
  Serial.print(F("System State:\t\t"));
  Serial.println(systemState);

  if (systemState == 0) Serial.println(F("Bootloader"));
  else if (systemState == 1) Serial.println(F("FMHD"));
  else if (systemState == 2) Serial.println(F("DAB"));
  else if (systemState == 3) Serial.println(F("TDMB or data only DAB"));
  else if (systemState == 4) Serial.println(F("FMHD Demod"));
  else if (systemState == 5) Serial.println(F("AMHD"));
  else if (systemState == 6) Serial.println(F("AMHD Demod"));
  else if (systemState == 7) Serial.println(F("DAB Demod"));
  else if (systemState == 16) Serial.println(F("Reserved"));
  else    Serial.println(F("Device not ready"));
  Serial.println();
}

void printFirmwareInformation(firmwareInformation_t firmwareInformation)
{
  Serial.print(F("Firmware Revision:\t"));
  Serial.print(firmwareInformation.revisionNumberMajor);
  Serial.print(F("."));
  Serial.print(firmwareInformation.revisionNumberMinor);
  Serial.print(F("."));
  Serial.println(firmwareInformation.revisionNumberBuild);
  Serial.print(F("No. Subversion Flag:\t"));
  Serial.println(firmwareInformation.noSvnFlag);
  Serial.print(F("SVN Tag/Branch/Trunk:\t"));
  Serial.println(firmwareInformation.location);
  Serial.print(F("Mixed Rev. Flag:\t"));
  Serial.println(firmwareInformation.mixedRevFlag);
  Serial.print(F("Local Mod. Flag:\t"));
  Serial.println(firmwareInformation.localModFlag);
  Serial.print(F("Subversion Id:\t\t"));
  Serial.println(firmwareInformation.svnId);
  Serial.println();
}

/*Print power up parameters*/
void printPowerUpArguments(powerUpArguments_t powerUpArguments)
{
  Serial.println(F("Power Up Arguments"));
  Serial.print(F("Clock Mode (0,1,2,3):\t"));
  Serial.println(powerUpArguments.clockMode);
  Serial.print(F("XOSC TResistor Size:\t"));
  Serial.println(powerUpArguments.trSize);
  Serial.print(F("XOSC Ibias Start:\t"));
  Serial.println(powerUpArguments.iBiasStart);
  Serial.print(F("XTAL Frequency Hz:\t"));
  Serial.println(powerUpArguments.xtalFreq);
  Serial.print(F("XOSC Tuning Capacitor:\t"));
  Serial.println(powerUpArguments.cTune);
  Serial.print(F("XOSC Ibias Run:\t\t"));
  Serial.println(powerUpArguments.iBiasRun);
  Serial.println();
}

//Print Rssi Information In 8.8 Format
void printRssi(unsigned short rssi)
{
  Serial.print(F("Rssi:\t"));
  Serial.println(rssi / 256.00);
}

//Print property value and property id
void printPropertyValue(unsigned short id, unsigned short value)
{
  char string[25];
  snprintf(string, 25, "Id: 0x%04X Value: 0x%04X", id, value);
  Serial.println(string);
}

//Print 2 dimensional property value list
void printPropertyValueList(unsigned short propertyValueList[][2], unsigned char num)
{
  Serial.println(F("Properties:"));
  for (unsigned char i = 0; i < num; i++)
  {
    //http://www.c-howto.de/tutorial/arrays-felder/zeigerarithmetik/mehrdimensional/
    //https://stackoverflow.com/questions/33724205/pointer-arithmetic-with-2d-arrays-in-c
    // printProperty(propertyValueList + i * num, propertyValueList + i * num + 1);
    printPropertyValue(propertyValueList[i][0], propertyValueList[i][1]);
  }
  Serial.println();
}

void printResponseHex(unsigned char response[], unsigned long len)
{
  if (len == 0) return;

  char string[6];

  for (unsigned long i = 0; i < len; i++)
  {
    snprintf(string, 6, "0x%02x ", response[i]);
    Serial.print(string);
    delay(1);
  }
  Serial.println();//next line
}

//Print Mute
void printMute(uint8_t channel)
{
  if (channel != 0)
  {
    Serial.print(F("Channel Mute:\t"));
    Serial.println(channel);
  }
  else Serial.println(F("Unmute"));
  Serial.println();
}

//Print Volume
void printVolume(uint8_t volume)
{
  Serial.print(F("Volume:\t"));
  Serial.println(volume);
  Serial.println();
}

//Print Mono
void printMono(uint8_t mono)
{
  Serial.print(F("Mono:\t"));
  Serial.println(mono);
  Serial.println();
}

/*Print status information about the received signal quality*/
void dabPrintRsqStatus(rsqInformation_t rsqInformation)
{
  Serial.println(F("Receive Quality Info:"));
  Serial.print(F("Hardmute IRQ:\t"));
  Serial.println(rsqInformation.hardMuteInterrupt);
  Serial.print(F("Fic Error IRQ:\t"));
  Serial.println(rsqInformation.ficErrorInterrupt);
  Serial.print(F("Acquired IRQ:\t"));
  Serial.println(rsqInformation.acqInterrupt);
  Serial.print(F("Rssi High IRQ:\t"));
  Serial.println(rsqInformation.rssiHighInterrupt);
  Serial.print(F("Rssi LowQ IRQ:\t"));
  Serial.println(rsqInformation.rssiLowInterrupt);

  Serial.print(F("Hardmute:\t"));
  Serial.println(rsqInformation.hardmute);
  Serial.print(F("FIC Error:\t"));
  Serial.println(rsqInformation.ficError);
  Serial.print(F("Acquired:\t"));
  Serial.println(rsqInformation.acq);
  Serial.print(F("Valid:\t\t"));
  Serial.println(rsqInformation.valid);
  Serial.print(F("RSSI:\t\t"));
  Serial.print(rsqInformation.rssi, DEC);
  Serial.println(F(" dB"));
  Serial.print(F("SNR 0-20:\t"));
  Serial.print(rsqInformation.snr, DEC);
  Serial.println(F(" dB"));
  Serial.print(F("Fic Quality:\t"));
  Serial.println(rsqInformation.ficQuality);
  Serial.print(F("CNR 0-54:\t"));
  Serial.print(rsqInformation.cnr);
  Serial.println(F(" dB"));
  Serial.print(F("Block Error: \t"));
  Serial.println(rsqInformation.fibErrorCount);
  Serial.print(F("Frequency: \t"));
  Serial.print(rsqInformation.frequency);
  Serial.println(F(" kHz"));
  Serial.print(F("Index:\t\t"));
  Serial.println(rsqInformation.index);
  Serial.print(F("Fft Offset:\t"));
  Serial.println(rsqInformation.fftOffset);
  Serial.print(F("Varactor:\t"));
  Serial.println(rsqInformation.varactorCap);
  Serial.print(F("Capacity Units:\t"));
  Serial.println(rsqInformation.cuLevel);
  Serial.print(F("Fast Detect:\t"));
  Serial.println(rsqInformation.fastDect);
  Serial.println();
}

//Print status information of the digital service
void dabPrintEnsembleInformation(ensembleInformation_t ensembleInformation)
{
  Serial.println(F("Ensemble Info"));
  Serial.print(F("Label:\t\t\t"));
  Serial.println(ensembleInformation.ensembleLabel);
  Serial.print(F("Id:\t\t\t0x"));
  Serial.println(ensembleInformation.ensembleId, HEX);
  Serial.print(F("Extended Country Code:\t"));
  Serial.println(ensembleInformation.ecc);
  Serial.print(F("Character Set:\t\t0b"));
  Serial.println(ensembleInformation.charSet);
  Serial.print(F("Abbreviation Mask:\t0b"));
  Serial.println(ensembleInformation.abbreviationMask, BIN);
  Serial.println();
}

/*Print event information of the digital service*/
void dabPrintEventInformation(eventInformation_t eventInformation)
{
  Serial.println(F("Event Information:"));
  Serial.print(F("Reconfiguration:\t"));
  Serial.println(eventInformation.ensembleReconfigInterrupt);
  Serial.print(F("Reconfig. Warning:\t"));
  Serial.println(eventInformation.ensembleReconfigWarningInterrupt);
  Serial.print(F("Announcement:\t\t"));
  Serial.println(eventInformation.announcementInterrupt);
  Serial.print(F("Other Service:\t\t"));
  Serial.println(eventInformation.otherServiceInterrupt);
  Serial.print(F("Service Linking:\t"));
  Serial.println(eventInformation.serviceLinkingInterrupt);
  Serial.print(F("Frequency Interrupt:\t"));
  Serial.println(eventInformation.frequencyInterrupt);
  Serial.print(F("Service List IRQ:\t"));
  Serial.println(eventInformation.serviceListInterrupt);
  Serial.print(F("Announcement avail.:\t"));
  Serial.println(eventInformation.announcementAvailable);
  Serial.print(F("Other Service avail.:\t"));
  Serial.println(eventInformation.otherServiceAvailable);
  Serial.print(F("Service Linking avail.:\t"));
  Serial.println(eventInformation.serviceLinkingAvailable);
  Serial.print(F("Frequency List avail.:\t"));
  Serial.println(eventInformation.frequencyAvailable);
  Serial.print(F("Service List avail.:\t"));
  Serial.println(eventInformation.serviceListAvailable);
  Serial.print(F("Service List Version:\t"));
  Serial.println(eventInformation.currentServiceListVersion);
  Serial.println();
}

/*Print component audio information*/
void dabPrintComponentAudioInfo(audioInformation_t audioInformation)
{
  Serial.println(F("Audio Info:"));
  Serial.print(F("Bit Rate:\t"));
  Serial.print(audioInformation.audioBitRate);
  Serial.println(F(" kbps"));
  Serial.print(F("Sample Rate:\t"));
  Serial.print(audioInformation.audioSampleRate);
  Serial.println(F(" Hz"));
  Serial.print(F("PS flag:\t"));
  Serial.println(audioInformation.audioPsFlag);
  Serial.print(F("SBR flag:\t"));
  Serial.println(audioInformation.audioSbrFlag);
  Serial.print(F("Mode 0/1/2/3:\t"));
  Serial.println(audioInformation.audioMode);
  Serial.print(F("DRC Gain:\t"));
  Serial.print(audioInformation.audioDrcGain);
  Serial.println(F(" dB"));
  Serial.println();
}

/*Print local date and time or UTC*/
void dabPrintDateTimeInformation(timeDab_t dabTime)
{
  const unsigned char len = 28;
  char string[len];

  snprintf(string, len, "UTC:%d  %02d.%02d.%4d  %02d:%02d:%02d",
           dabTime.type,
           dabTime.day,
           dabTime.month,
           dabTime.year,
           dabTime.hour,
           dabTime.minute,
           dabTime.second);

  Serial.println(F("Date and Time Information"));
  Serial.println(string);
  Serial.println();
}

//Print component technical information
void dabPrintComponentTechnicalInformation(componentTechnicalInformation_t componentTechnicalInformation)
{
  Serial.println(F("Component Technical Info"));
  Serial.print(F("Service Mode (0-8):\t"));
  Serial.print(componentTechnicalInformation.serviceMode);
  //Serial.println(F("0:AUDIO, 1:DATA, 2:FIDC, 3:MSC, 4:DAB+, 5:DAB, 6:FIC, 7:XPAD, 8:NO MEDIA"));
  if (componentTechnicalInformation.serviceMode == 0)
    Serial.println(F("\tAudio"));
  else if (componentTechnicalInformation.serviceMode == 1)
    Serial.println(F("\tData"));
  else if (componentTechnicalInformation.serviceMode == 4)
    Serial.println(F("\tDAB+"));
  else Serial.println();

  Serial.print(F("UEP/EEP Protection:\t"));
  Serial.println(componentTechnicalInformation.protectionInfo);
  Serial.print(F("Bit Rate:\t\t"));
  Serial.print(componentTechnicalInformation.bitRate);
  Serial.println(F(" kbps"));
  Serial.print(F("Capacity Units:\t\t"));
  Serial.println(componentTechnicalInformation.numberCU);
  Serial.print(F("C.U. Start Adress:\t"));
  Serial.println(componentTechnicalInformation.addressCU );
  Serial.println();
}

/*Print information about the component*/
void dabPrintComponentInformation(componentInformation_t componentInformation)
{
  Serial.println(F("Component Information"));
  Serial.print(F("Global Id:\t"));
  Serial.println(componentInformation.globalId);
  Serial.print(F("Language:\t"));
  Serial.println(componentInformation.language);
  Serial.print(F("Character Set:\t"));
  Serial.println(componentInformation.characterSet);
  Serial.print(F("Label:\t"));
  Serial.println(componentInformation.label);
  Serial.print(F("Abbrev. Mask:\t"));
  Serial.println(componentInformation.abbreviationMask, BIN);
  Serial.print(F("Number of Apps:\t"));
  Serial.println(componentInformation.numberUserAppTypes);
  Serial.print(F("User App. Type:\t"));
  Serial.println(componentInformation.userAppType);
  Serial.print(F("Total Length:\t"));
  Serial.println(componentInformation.lenTotal);
  Serial.print(F("Field Length:\t"));
  Serial.println(componentInformation.lenField);
  Serial.println();
}

//Print Service ID and Component ID
void dabPrintIds(unsigned long serviceId, unsigned long componentId)
{
  Serial.print(F("Service Id: 0x"));
  Serial.print(serviceId, HEX);
  Serial.print(F(" Component Id: 0x"));
  Serial.println(componentId, HEX);
}

//Print DAB frequency
void dabPrintFrequency(unsigned long frequency)
{
  Serial.print(F("Frequency:\t"));
  Serial.print(frequency);
  Serial.print(F(" kHz\n"));
}

//Print frequency table
void dabPrintFrequencyTable(const unsigned long frequencyTable[], const unsigned char numFreq)
{
  char string[20];

  Serial.println(F("Frequency Table"));
  Serial.print(F("Number:  "));
  Serial.println(numFreq);

  for (unsigned char j = 0; j < numFreq; j++)
  {
    snprintf(string, 20, "%2u : %6lu kHz", j , frequencyTable[j]) ;
    Serial.println(string);
  }
  Serial.println();
}

//Print index
void dabPrintIndex(unsigned char index)
{
  Serial.print(F("Index:\t"));
  Serial.println(index);
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


/*Prints status information of the digital service*/
void dabPrintDigitalServiceInformation(serviceInformation_t dabServiceInfo)
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

//Print ensemble header
void dabPrintEnsembleHeader(ensembleHeader_t ensembleHeader)
{
  Serial.println(F("Ensemble Header"));
  Serial.print(F("Size [Bytes]:\t"));
  Serial.println(ensembleHeader.listSize);
  Serial.print(F("Version:\t"));
  Serial.println(ensembleHeader.version);
  Serial.print(F("Services:\t"));
  Serial.println(ensembleHeader.numServices);
  Serial.println();
}

//Print Ensemble
void dabPrintEnsemble(ensembleHeader_t ensembleHeader)
{
  dabPrintEnsembleHeader(ensembleHeader);

  for (unsigned char serviceNum = 0; serviceNum < ensembleHeader.numServices; serviceNum++)
  {
    //Serial.print(F("Service:\t"));
    //Serial.println(serviceNum);
    Serial.print(F("Service Id:\t0x"));
    Serial.print(ensembleHeader.serviceList[serviceNum].serviceId, HEX);
    Serial.print(F("\tData Flag:\t"));
    Serial.println(ensembleHeader.serviceList[serviceNum].dataFlag);
    Serial.print(F("Number of Components:\t"));
    Serial.println(ensembleHeader.serviceList[serviceNum].numComponents);
    for (unsigned char componentNum = 0; componentNum < ensembleHeader.serviceList[serviceNum].numComponents; componentNum++)
    {
      Serial.print(F("Component Id:\t0x"));
      Serial.print(ensembleHeader.serviceList[serviceNum].componentList[componentNum].componentId, HEX);
      Serial.print(F("\tService Type:\t"));
      Serial.println(ensembleHeader.serviceList[serviceNum].componentList[componentNum].serviceType);
    }
    Serial.println();
  }
  Serial.println();
}

/*Print Service Data*/
void dabPrintServiceData(serviceData_t dabServiceData)
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

//Print General Menu
//Reduce memory needs
void dabPrintMenu()
{
  Serial.println(F("?: Print this"));
  Serial.println(F("M: DAB Menu Main"));
  Serial.println(F("E: DAB Menu Scan Ensemble"));
  Serial.println(F("F: DAB Menu Scan Frequency"));
  Serial.println(F("T: DAB Menu Technical"));
  Serial.println(F("D: DAB Menu Device"));
  Serial.println();
}

//Print Menu Main dab
void dabPrintMenuMain()
{
  Serial.println(F("Main Menu"));
  //Print General Menu
  dabPrintMenu();
  Serial.println(F("q: Receive Quality"));
  Serial.println(F("e: Info Ensemble"));
  Serial.println(F("r: Info Service"));
  Serial.println(F("t: Show Running Service"));

  Serial.println(F("c: Check Service Data"));
  Serial.println(F("v: Get Service Data"));
  Serial.println();
  Serial.println(F("d: Next Service"));
  Serial.println(F("a: Previous Service"));
  Serial.println();
  Serial.println(F("1: Favorite 1"));
  Serial.println(F("2: Favorite 2"));
  Serial.println(F("3: Favorite 3"));
  Serial.println(F("4: Favorite 4"));
  Serial.println(F("5: Favorite 5"));
  Serial.println(F("x: Start Service"));
  Serial.println(F("y: Stop Service"));

  Serial.println(F("m: Mute/Unmute Audio"));
  Serial.println(F("+: Volume Up"));
  Serial.println(F("-: Volume Down"));
  Serial.println();
}

/*Print DAB Technical Menu*/
void dabPrintMenuTechnical()
{
  Serial.println(F("DAB Menu Technical"));
  /*Print General Menu*/
  dabPrintMenu();
  Serial.println(F("v: Event Info"));
  Serial.println(F("a: Audio Info"));
  Serial.println(F("x: Technical Info"));
  Serial.println(F("d: Date and Time"));
  Serial.println(F("c: Component Info"));
  Serial.println(F("i: Frequency List"));
  Serial.println(F("t: Test Varactor"));
  Serial.println(F("r: RSSI"));
  Serial.println(F("p: Properties DAB"));
  Serial.println();
}

//Print DAB Scan Menu
void dabPrintMenuScanFrequency()
{
  Serial.println(F("DAB Menu Frequency Scan"));
  //Print General Menu
  dabPrintMenu();
  Serial.println(F("f: Get Frequency Table"));
  Serial.println(F("1: Set Standard Table"));
  Serial.println(F("2: Set Table 2"));
  Serial.println(F("3: Set Table 3"));
  Serial.println(F("h: Service List Header"));
  Serial.println();
  Serial.println(F("u: Index Manual Up"));
  Serial.println(F("z: Index Manual Down"));
  Serial.println(F("d: Index Auto Up"));
  Serial.println(F("a: Index Auto Down"));
  Serial.println();
  Serial.println(F("s: Index Bandscan"));
  Serial.println(F("i: Index Valid List"));
  Serial.println(F("+: Index Valid Up:"));
  Serial.println(F("-: Index Valid Down"));
  Serial.println();
}

void printMenuScanEnsemble()
{
  Serial.println(F("Scan Ensemble"));
  Serial.println(F("e: Ensemble Information"));
  Serial.println(F("h: Header Ensemble"));
  Serial.println(F("p: Parse Ensemble"));
  Serial.println(F("0: Start 1st Service"));
  Serial.println();
  Serial.println(F("t: Check Service Data"));
  Serial.println(F("v: Read Service Data"));
  Serial.println();
  Serial.println(F("d: Next Service"));
  Serial.println(F("a: Previous Service"));
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

}//namespace
