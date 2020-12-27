//include guard
#ifndef PRINT_SERIAL_H
#define PRINT_SERIAL_H

//Serial Monitor Print Functions

//include SI46xx.h
#include "SI468x.h"

//namespace to avoid naming conflicts
namespace serialPrintSi468x
{

//Print DAB Menu
void dabPrintMenuMain();
//Print DAB Technical Menu
void dabPrintMenuTechnical();
//Print Menu DAB Scan Frequency 
void dabPrintMenuScanFrequency();

void printMenuScanEnsemble();

//Print DAB Device Menu
void dabPrintMenuDevice();
//Print Overview
void dabPrintMenu();

void printVersion(const char version[]);
void printFreeRam(const unsigned short freeRam);

void printFlashMemoryInfo();

void printStatusRegister(statusRegister_t& statusRegister);
void printPartInfo(partInfo_t& partInfo);
void printSystemState(unsigned char systemState);
void printFirmwareInformation(firmwareInformation_t& firmwareInformation);
void printPowerUpArguments(powerUpArguments_t& powerUpArguments);

void printPropertyValue(unsigned short id, unsigned short value);
void printPropertyValueList(unsigned short propertyValueList[][2], unsigned char numberProperties);

void printRssi(unsigned short rssi);
void printResponseHex(unsigned char response[], unsigned long len);
void printMute(unsigned char channel);
void printVolume(unsigned char volume);
void printMono(unsigned char mono);

//DAB Print Functions
//Prints status information about the received signal quality
void dabPrintRsqStatus(rsqInformation_t& rsqInformation);
//Print status information of the digital service
void dabPrintDigitalServiceInformation(serviceInformation_t& dabServiceInfo);
//Print ensemble header
void dabPrintEnsembleHeader(ensembleHeader_t& ensembleHeader);
//Print ensemble
void dabPrintEnsemble(ensembleHeader_t& ensembleHeader);
//Print status information of the digital ensemble
void dabPrintEnsembleInformation(ensembleInformation_t& ensembleInformation);
//Print event information about the various events related to the DAB radio
void dabPrintEventInformation(eventInformation_t& eventInformation);
//Print audio component information
void dabPrintComponentAudioInfo(audioInformation_t& audioInformation);

//Print local date and time or UTC
void dabPrintDateTimeInformation(timeDab_t& dabTime);
//Print technical information about the component
void dabPrintComponentTechnicalInformation(componentTechnicalInformation_t& componentTechnicalInformation);
//Print information about the component
void printComponentInformation(componentInformation_t& componentInformation);
//Print Service ID and Component ID
void dabPrintIds(unsigned long serviceId, unsigned long componentId);
//Print frequency list
void dabPrintFrequencyTable(frequencyTableHeader_t frequencyTableHeader);

//Print index list
void dabPrintIndexList(const indexListHeader_t& indexListHeader);

//Print index
void dabPrintIndex(unsigned char index);
//Print DAB frequency
void dabPrintFrequency(unsigned long frequency);
//Print Service Linking Information
void printServiceLinkingInformation(serviceLinkingInformation_t& serviceLinkingInformation);
//Print ensemble frequency information table
void printFrequencyInformation(frequencyInformationTableHeader_t& frequencyInformationTableHeader);

//Print service data
void dabPrintServiceData(serviceData_t& dabServiceData);
//Print dls
void dabPrintDynamicLabelSegment(char dls[]);
//Print error
void printError(unsigned char errorCode);

}//namespace serialPrintSi468x

#endif //PRINT_SERIAL_H
