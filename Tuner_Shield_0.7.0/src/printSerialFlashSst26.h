/*Print serial functions for Flash Memory SST26*/

/*include guard*/
#ifndef PRINT_SERIAL_FLASH_SST26_H
#define PRINT_SERIAL_FLASH_SST26_H

/*Functions for Flash Memory SST26*/
#include "flashSst26_0.2.0.h"

class PrintSerialFlashSst26
{
  public:
   
    /*Print hex string*/
    void printHexStr(unsigned char str[], unsigned long len);

    /*Print address*/
    void printAddress(unsigned long address);

    /*Print lenData*/
    void printLenData(unsigned long lenData);
    
    /*Print byte*/
    void printByte(unsigned char data);
    
    /*Print Data*/
    void printData(unsigned char data[], unsigned long lenData, unsigned char bytesPerLine = 16);

    /*Flash print functions*/

    /*Print id*/
    void printId(Sst26::id_t id);
    /*Print register status*/
    void printRegisterStatus(Sst26::registerStatus_t registerStatus);
     /*Print register configuration */
    void printRegisterConfiguration(Sst26::registerConfiguration_t registerConfiguration);
    /*Print register block protection*/
    void printRegisterBlockProtection(Sst26::registerBlockProtection_t registerBlockProtection);
    /*Print unique id*/
    void printUniqueId(Sst26::uniqueId_t uniqueId);
    /*Print Error message*/
    void printError(int error = 0);
    
};

#endif /*PRINT_SERIAL_FLASH_SST26_H*/



