/*include guard*/
#ifndef COM_DRIVER_SPI_H
#define COM_DRIVER_SPI_H

/*SPI communication driver class*/

/*Include SPI.h*/
#include <SPI.h>

/*uint8_t*/
#include "stdint.h"

class ComDriverSpi
{
  public:

    ComDriverSpi();
    ComDriverSpi(uint8_t slaveSelectPin);
    ComDriverSpi(uint8_t slaveSelectPin, uint32_t frequency);
    ComDriverSpi(uint8_t slaveSelectPin, uint32_t frequency, uint8_t dataOrder, uint8_t dataMode);

    /*SPI transfer options*/
    enum transferOption_t {transferEnd, transferStart, transferStartEnd};

    /*Write SPI data*/
    bool writeSpi(uint8_t data[], uint32_t sizeToWrite, transferOption_t transferOption = transferStartEnd);
    /*Read SPI data*/
    bool readSpi(uint8_t data[], uint32_t sizeToRead, transferOption_t transferOption = transferStartEnd);
    /*Write and than read SPI data*/
    bool writeReadSpi(uint8_t dataWrite[], uint32_t sizeToWrite, uint8_t dataRead[], uint32_t sizeToRead, transferOption_t transferOption = transferStartEnd);

    bool setSlaveSelectPin(uint8_t slaveSelectPin);
    /*SPI_CLOCK_DIV2
      SPI_CLOCK_DIV4
      SPI_CLOCK_DIV8
      SPI_CLOCK_DIV16
      SPI_CLOCK_DIV32
      SPI_CLOCK_DIV64
      SPI_CLOCK_DIV128
    */
    bool setFrequency(uint32_t frequency);
    /*SPI_MODE0 SPI_MODE1 SPI_MODE2 SPI_MODE3*/
    bool setDataMode(uint8_t dataOrder);
    /*LSBFIRST, MSBFIRST*/
    bool setDataOrderMode(uint8_t dataMode);
    /*Start, End, StartEnd of SPI communication*/
    bool setTransferOption(transferOption_t transferOption);

  private:
    uint8_t           _slaveSelectPin;
    uint32_t          _frequency;
    uint8_t           _dataOrder;
    uint8_t           _dataMode;
    transferOption_t  _transferOption;
};
#endif /*COM_DRIVER_SPI_H*/
