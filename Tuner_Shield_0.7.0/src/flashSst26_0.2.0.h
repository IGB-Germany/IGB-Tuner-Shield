#ifndef FLASH_SST26_H
#define FLASH_SST26_H

/*Functions for Flash Memory SST26*/

/*SPI communication layer*/
#include "comDriverSpi_0.1.0.h"

class Sst26
{
    /*
      2018
      Marcus Bockting
      0.2.0

      to do:
      New: return int errorCode for all functions
      New: printErrorCode
      Changed: Getters, Setter for class and separate methods for HW command read, write or one method ?
      Changed: writeData, readData accepts big array and chrashes 

      Flash: 2832 Bytes
      RAM:   332 Bytes
    */

    /*
      SST26WF016B/
      SST26WF016BA
    */

    /*
      Pinning

      PIN 1 CE#   Chip Enable
      PIN 2 SO    Serial Data Output To transfer data serially out of the device.
      PIN 3 WP#   Write Protect The Write Protect (WP#) pin is used to enable/disable BPL bit in the status register.
      PIN 4 VSS   Ground
      PIN 5 SI    Serial Data Input  To transfer commands, addresses, or data serially into the device.
      PIN 6 SCK   Serial Clock    To provide the timing of the serial interface.
      PIN 7 HOLD# Hold To temporarily stop serial communication with SPI flash memory without resetting the device.
      PIN 8 VDD   Power Supply

      SPI:
      Mode 0 (0,0)
      Mode 3 (1,1)

      Speed:
      50MHz

      Supply Voltage:
      1.65V - 1.95V

      Address:
      0x000000 - 0x1FFFFF
      4 KByte erasable sectors

    */
  public:

    /*type id_t*/
    struct id_t
    {
      unsigned long jedecId;      /*0xBF2651*/
      unsigned char manufacturerId;/*0xBF*/
      unsigned char deviceType;   /*0x26*/
      unsigned char deviceId;     /*0x51*/
    };

    /*type uniqueId_t*/
    struct uniqueId_t
    {
      unsigned long uniqueIdLower;//lower 64 bit
      unsigned long uniqueIdUpper;//upper 64 bit
    };

    /*type registerStatus_t*/
    struct registerStatus_t
    {
      unsigned char busy1           : 1; /*read only*/
      unsigned char writeEnableLatch: 1; /*read only*/
      unsigned char suspendErase    : 1; /*read only*/
      unsigned char suspendProgram  : 1; /*read only*/
      unsigned char writeProtection : 1; /*read only*/
      unsigned char securityId      : 1; /*read only*/
      unsigned char reserved1       : 1; /*read only*/
      unsigned char busy2           : 1; /*read only*/
    };

    /*type registerConfiguration_t*/
    struct registerConfiguration_t
    {
      unsigned char reserved1                 : 1;/*read only*/
      unsigned char ioConfiguration           : 1;/*R/W*/
      unsigned char reserved2                 : 1;/*read only*/
      unsigned char blockProtectionVolatility : 1;/*read only*/ /*Default at power up 1*/
      unsigned char reserved3                 : 1;/*read only*/
      unsigned char reserved4                 : 1;/*read only*/
      unsigned char reserved5                 : 1;/*read only*/
      unsigned char writeProtectionPin        : 1;/*R/W non-volatile*/
    };

    /*The default state after a power-on reset is write-protected BPR[47:0] = 5555 FFFF FFFF*/

    struct registerBlockProtection_t
    {
      unsigned char adressRange0;/*8 bit 0x010000 - 0x08FFFF*/
      unsigned char adressRange1;/*8 bit 0x090000 -*/
      unsigned char adressRange2;/*8 bit*/
      unsigned char adressRange3;/*8 bit*/
      unsigned char adressRange4;/*8 bit*/
      unsigned char adressRange5;/*8 bit*/
    };

    Sst26();
    Sst26(uint8_t slaveSelectPin);
    Sst26(uint8_t slaveSelectPin, uint32_t  frequency);
    Sst26(uint8_t slaveSelectPin, uint32_t  frequency, uint8_t dataOrder, uint8_t dataMode);

    /*Functions*/
    /*Enable write*/
    bool enableWrite(void);
    /*Disable write*/
    bool disableWrite(void);

    /*Read id*/
    id_t readId();
    /*Read register status*/
    registerStatus_t readRegisterStatus();
    /*Read register configuration*/
    registerConfiguration_t readRegisterConfiguration();
    /*Read register block protection */
    registerBlockProtection_t readRegisterBlockProtection();

    /*Write register configuration*/
    bool writeRegisterConfiguration(registerConfiguration_t registerConfiguration);
    /*Write register block protection*/
    bool writeRegisterBlockProtection(registerBlockProtection_t registerBlockProtection);

    /*Disable Global Block Protection*/
    bool disableGlobalBlockProtection(void);
    
    /*Read unique id*/
    uniqueId_t readUniqueId();
    /*Read security data*/
    unsigned char readSecurityData(unsigned long addressStart);
    /*Write security data*/
    bool writeSecurityData();

    /*Erase 4kBytes = 0x1000*/
    bool erase4kByte(unsigned long addressStart);

    /*
        Sector 4kByte
        Address bits
        [AMS:A12] determine the sector address (SAX)
        (AMS = Most Significant Address)

        Blocks are 64 KByte, 32 KByte, or 8KByte, depending on location
        AMS - A13 for 8 KByte
        AMS - A15 for 32 KByte
        AMS - A16 for 64 KByte

        Remaining addresses are dont care, but must be set to VIL or VIH
    */

    /*Erase 8kBytes = 0x2000*/
    bool erase8kByte(unsigned long addressStart);
    /*Erase 32kBytes = 0x8000*/
    bool erase32kByte(unsigned long addressStart);
    /*Erase 64kBytes = 0xFFFF*/
    bool erase64kByte(unsigned long addressStart);

    /*Erase all*/
    bool eraseAll(void);

    /*Write byte*/
    bool writeByte(unsigned long adressStart, unsigned char data[]);
    /*Read byte*/
    bool readByte(unsigned long adressStart, unsigned char data[]);

    /*Write page*/
    bool writePage(unsigned long addressStart, unsigned char page[]);
    /*Read page*/
    bool readPage(unsigned long adressStart, unsigned char page[]);

    /*Write data pagewise*/
    bool writeData(unsigned long addressStart, unsigned char data[], unsigned long lenData);
    /*Read data*/
    bool readData(unsigned long adressStart, unsigned char data[], unsigned long lenData);

    /*Enable deep power down*/
    bool enableDeepPowerDown(void);
    /*Release deep power down*/
    bool releaseDeepPowerDown(void);
    /*Reset device*/
    bool reset(void);
    /*Test if device is busy*/
    bool isBusy(void);

  private:

    /*SPI communication driver class*/
    ComDriverSpi _comDriverSpi;

    /*address we are working on*/
    unsigned long _address;
    /*Size of buffer we are working on*/
    static const unsigned short _sizePage = 0x100;
   
    /*Max numbers of retry when chip is busy*/
    const unsigned char maxRetry = 10;

    /*Id*/
    id_t _id;
    /*Register status*/
    registerStatus_t _registerStatus;
    /*Register configuration*/
    registerConfiguration_t _registerConfiguration;
    /*Register block protection*/
    registerBlockProtection_t _registerBlockProtection;

    /*unique Id*/
    uniqueId_t _uniqueId;
   
    /*security data buffer*/
    unsigned char _securityDataBuffer;
    // unsigned char _sfdpBuffer[1];

    /*Durations in ms*/
    const unsigned char durationWritePage             = 2;
    const unsigned char durationWriteStatusRegister   = 10;

    const unsigned char durationEraseSector           = 18;
    const unsigned char durationEraseAll              = 35;

    const unsigned char durationWriteSecurityId       = 2;
    const unsigned char durationWriteSuspend          = 1;
    const unsigned char durationEnableWriteProtection = 25;

    /*Commands*/
    /*Configuration*/
    const unsigned char NO_OPERATION          = 0x00;
    const unsigned char DISABLE_WRITE         = 0x04;
    const unsigned char ENABLE_WRITE          = 0x06;

    const unsigned char READ_STATUS_REGISTER  = 0x05;
    const unsigned char WRITE_STATUS_REGISTER = 0x01;
    const unsigned char READ_CONFIGURATION_REGISTER   = 0x35;
    const unsigned char WRITE_CONFIGURATION_REGISTER  = 0x01;
    /*Identification*/
    const unsigned char READ_JEDEC_ID         = 0x9F;
    const unsigned char READ_JEDEC_ID_QUAD    = 0xAF;
    const unsigned char READ_SFDP             = 0x5A;
    /*Read*/
    const unsigned char READ_DATA             = 0x03;
    const unsigned char READ_DATA_FAST        = 0x0B;
    const unsigned char READ_DATA_QUAD        = 0x6B;
    const unsigned char READ_I_O_QUAD         = 0xEB;
    const unsigned char READ_DATA_DUAL        = 0x3B;
    const unsigned char READ_I_O_DUAL         = 0xBB;
    const unsigned char SET_BURST_LENGTH      = 0xC0;
    const unsigned char READ_BURST_QUAD       = 0x0C;
    const unsigned char READ_BURST            = 0xEC;
    /*Write*/
    const unsigned char WRITE_DATA            = 0x02;
    const unsigned char WRITE_DATA_PAGE_QUAD  = 0x32;
    /*Interrupt and resume erase or program operation*/
    const unsigned char WRITE_SUSPEND         = 0xB0;
    const unsigned char WRITE_RESUME          = 0x30;
    /*Erase*/
    const unsigned char ERASE_4KB             = 0x20;
    const unsigned char ERASE_8KB             = 0xD8;
    const unsigned char ERASE_32KB            = 0xD8;
    const unsigned char ERASE_64KB            = 0xD8;
    const unsigned char ERASE_ALL             = 0xC7;
    /*Protection*/
    const unsigned char READ_BP_REGISTER      = 0x72;
    const unsigned char WRITE_BP_REGISTER     = 0x42;
    const unsigned char LOCK_BP_REGISTER      = 0x8D;
    const unsigned char ENABLE_NON_VOLATILE_BP_REGISTER = 0xE8;
    const unsigned char DISABLE_GLOBAL_BP_REGISTER = 0x98;

    const unsigned char READ_SECURITY_DATA    = 0x88;
    const unsigned char WRITE_SECURITY_DATA   = 0xA5;
    const unsigned char LOCK_SECURITY_DATA    = 0x85;
    /*Power Saving*/
    const unsigned char ENABLE_DEEP_POWER_DOWN  = 0xB9;
    const unsigned char RELEASE_DEEP_POWER_DOWN = 0xAB;
    /*QPI*/
    const unsigned char ENTER_QPI             = 0x38;
    const unsigned char EXIT_QPI              = 0xFF;
    /*Reset*/
    const unsigned char ENABLE_RESET          = 0x66;
    const unsigned char RESET                 = 0x99;

};

#endif /*FLASH_SST26_H*/


