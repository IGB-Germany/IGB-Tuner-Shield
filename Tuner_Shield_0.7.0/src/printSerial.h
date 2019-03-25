//include guard
#ifndef PRINT_SERIAL_H
#define PRINT_SERIAL_H

/*Serial Monitor Print Functions*/

//include SI46xx.h
#include "SI468x.h"

//Print Functions
namespace serial
{

//Print DAB Menu
void dabPrintMenuMain();
//Print DAB Technical Menu
void dabPrintMenuTechnical();
//Print DAB Scan Menu
void dabPrintMenuScan();
//Print DAB Device Menu
void dabPrintMenuDevice();
//Print Overview
void dabPrintMenu();

//Print version information
void printVersionInformation();

//Device Print Functions
//Print device status information
void devicePrintStatus(deviceStatusInformation_t deviceStatusInformation);
//Print Device Part Number
void devicePrintPartNumber(devicePartNumber_t devicePartNumber);
//Print Device Image
void devicePrintImage(unsigned char deviceImage);
//Print Firmware Information
void devicePrintFirmwareInformation(deviceFirmwareInformation_t deviceFirmwareInformation);
/*Print power up parameters*/
void devicePrintPowerUpArguments(devicePowerUpArguments_t devicePowerUpArguments);
//Print RSSI Information In 8.8 Format
void devicePrintRssi(unsigned short rssi);
//Print Property Value
void devicePrintProperty(unsigned short devicePropertyId, unsigned short devicePropertyValue);
//Print device property value list
void devicePrintAllProperties(struct devicePropertyValueList_t devicePropertyValueList[], unsigned char num = numberDeviceProperties);


//DAB Print Functions
//Prints status information about the received signal quality
void dabPrintRsqStatus(dabRsqInformation_t dabRsqInformation);
//Print service list header
void dabPrintServiceListHeader(dabServiceListHeader_t dabServiceListHeader);
//Print status information of the digital service
void dabPrintDigitalServiceInformation(dabServiceInformation_t dabServiceInfo);
//Print status information of the digital ensemble
void dabPrintEnsembleInformation(dabEnsembleInformation_t dabEnsembleInfo);
//Print event information about the various events related to the DAB radio
void dabPrintEventInformation(dabEventInformation_t dabEventInfo);
//Print audio component information
void dabPrintComponentAudioInfo(dabAudioComponentInformation_t dabAudioComponentInformation);
/*Print local date and time or UTC*/
void dabPrintDateTimeInformation(unsigned short year, unsigned char month, unsigned char day, unsigned char hour, unsigned char minute, unsigned char second, unsigned char timeType = 1);
//Print technical information about the component
void dabPrintComponentTechnicalInformation(dabComponentTechnicalInformation_t dabComponentTechnicalInformation);
//Print information about the component
void dabPrintComponentInformation(dabComponentInformation_t dabComponentInformation);
//Print Service ID and Component ID
void dabPrintIds(unsigned long dabServiceId, unsigned long dabComponentId);
//Print frequency table
void dabPrintFrequencyTable(unsigned long* dabFreqTable, unsigned char dabNumFreq);
//Print valid index list
void dabPrintValidIndexList(unsigned char dabNumValidIndex, unsigned char dabValidIndexList[]);
//Print index
void dabPrintIndex(unsigned char index);
//Print DAB frequency
void dabPrintFrequency(unsigned long frequency);
//Print DAB Actual Service
void dabPrintActualService(dabEnsemble_t* dabEnsembleList, unsigned char dabActualService);
//Print Ensemble
void dabPrintEnsemble(dabEnsemble_t* dabEnsembleList, unsigned char dabNumServices);
//Print DAB property Value
void dabPrintProperty(unsigned short dabPropertyId, unsigned short dabPropertyValue);
//Print DAB property value list
void dabPrintAllProperties(struct dabPropertyValueList_t dabPropertyValueList[], unsigned char num = dabNumberProperties);
//Print service data
void dabPrintServiceData(struct dabServiceData_t dabServiceData);
//Print dls
void dabPrintDynamicLabelSegment(char dls[]);

}//namespace serial

#endif //PRINT_SERIAL_H


