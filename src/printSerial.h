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

void printStatusRegister(statusRegister_t statusRegister);
void printPartInfo(partInfo_t partInfo);
void printSystemState(unsigned char systemState);
void printFirmwareInformation(firmwareInformation_t firmwareInformation);
void printPowerUpArguments(powerUpArguments_t powerUpArguments);

void printPropertyValue(unsigned short id, unsigned short value);
void printPropertyValueList(unsigned short propertyValueList[][2], unsigned char numberProperties);

void printRssi(unsigned short rssi);
void printResponseHex(unsigned char response[], unsigned long len);
void printMute(unsigned char channel);
void printVolume(unsigned char volume);
void printMono(unsigned char mono);

//DAB Print Functions
//Print ensemble header
void dabPrintEnsembleHeader(ensembleHeader_t ensembleHeader);
//Print ensemble
void dabPrintEnsemble(ensembleHeader_t ensembleHeader);

//Prints status information about the received signal quality
void dabPrintRsqStatus(rsqInformation_t rsqInformation);
//Print status information of the digital service
void dabPrintDigitalServiceInformation(serviceInformation_t dabServiceInfo);
//Print status information of the digital ensemble
void dabPrintEnsembleInformation(ensembleInformation_t ensembleInformation);
//Print event information about the various events related to the DAB radio
void dabPrintEventInformation(eventInformation_t eventInformation);
//Print audio component information
void dabPrintComponentAudioInfo(audioInformation_t audioInformation);
/*Print local date and time or UTC*/
void dabPrintDateTimeInformation(dabTime_t dabTime);
//Print technical information about the component
void dabPrintComponentTechnicalInformation(dabComponentTechnicalInformation_t dabComponentTechnicalInformation);
//Print information about the component
void dabPrintComponentInformation(dabComponentInformation_t dabComponentInformation);
//Print Service ID and Component ID
void dabPrintIds(unsigned long serviceId, unsigned long componentId);

//Print frequency table
void dabPrintFrequencyTable(const unsigned long frequencyTable[], const unsigned char numFreq);

//Print valid index list
void dabPrintValidIndexList(unsigned char dabNumValidIndex, unsigned char dabValidIndexList[]);
//Print index
void dabPrintIndex(unsigned char index);
//Print DAB frequency
void dabPrintFrequency(unsigned long frequency);

//Print service data
void dabPrintServiceData(struct dabServiceData_t dabServiceData);
//Print dls
void dabPrintDynamicLabelSegment(char dls[]);
//Print error no reception
void dabPrintErrorNoReception();

}//namespace serial

#endif //PRINT_SERIAL_H
