/*include guard*/
#ifndef PRINT_SERIAL_FLASH_SST26_H
#define PRINT_SERIAL_FLASH_SST26_H

/*Print serial functions for Flash Memory SST26*/
/*Version 0.3.0*/
/*05.12.2018*/
/*IGB*/

/*Functions for Flash Memory SST26*/
#include "FlashSst26.h"

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
    void printId(FlashSst26 flashSst26);
    /*Print register status*/
    void printRegisterStatus(FlashSst26 flashSst26);
     /*Print register configuration */
    void printRegisterConfiguration(FlashSst26 flashSst26);
    /*Print register block protection*/
    void printRegisterBlockProtection(FlashSst26 flashSst26);
    /*Print unique id*/
    void printUniqueId(FlashSst26 flashSst26);
    /*Print Error message*/
    void printError(int error = 0);
    
};

#endif /*PRINT_SERIAL_FLASH_SST26_H*/
