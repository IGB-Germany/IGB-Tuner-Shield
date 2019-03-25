/*SPI communication layer*/
#include "comDriverSpi_0.1.0.h"

ComDriverSpi::ComDriverSpi()
{
  //Serial.println("Default constructor ComDriverSpi");

  _slaveSelectPin = 10;
  /*SPI settings*/
  _frequency = 8000000;
  _dataOrder = MSBFIRST;
  _dataMode = SPI_MODE0;

  _transferOption = transferStartEnd;

  /*initalize the chip select pin*/
  pinMode(_slaveSelectPin, OUTPUT);
  digitalWrite(_slaveSelectPin, HIGH);

  /*start the SPI library*/
  SPI.begin();
}

ComDriverSpi::ComDriverSpi(uint8_t slaveSelectPin): _slaveSelectPin(slaveSelectPin)
{
  //Serial.println("Special constructor ComDriverSpi");

  /*SPI settings*/
  _frequency = 8000000;
  _dataOrder = MSBFIRST;
  _dataMode = SPI_MODE0;

  _transferOption = transferStartEnd;

  /*initalize the chip select pin*/
  pinMode(_slaveSelectPin, OUTPUT);
  digitalWrite(_slaveSelectPin, HIGH);

  /*start the SPI library*/
  SPI.begin();
}

ComDriverSpi::ComDriverSpi(uint8_t slaveSelectPin, uint32_t frequency): _slaveSelectPin(slaveSelectPin), _frequency(frequency)
{
  //Serial.println("Special constructor ComDriverSpi");

  /*SPI settings*/
  _dataOrder = MSBFIRST;
  _dataMode = SPI_MODE0;

  _transferOption = transferStartEnd;

  /*initalize the chip select pin*/
  pinMode(_slaveSelectPin, OUTPUT);
  digitalWrite(_slaveSelectPin, HIGH);

  /*start the SPI library*/
  SPI.begin();
}

ComDriverSpi::ComDriverSpi(uint8_t slaveSelectPin, uint32_t  frequency, uint8_t dataOrder, uint8_t dataMode)
{
  _slaveSelectPin = slaveSelectPin;
  /*SPI settings*/
  _frequency = frequency;
  _dataOrder = dataOrder;
  _dataMode = dataMode;
  _transferOption = transferStartEnd;

  /*initalize the chip select pin*/
  pinMode(_slaveSelectPin, OUTPUT);
  digitalWrite(_slaveSelectPin, HIGH);

  /*start the SPI library*/
  SPI.begin();

}

bool ComDriverSpi::setSlaveSelectPin(uint8_t slaveSelectPin)
{
  _slaveSelectPin = slaveSelectPin;
  return true;
}

bool ComDriverSpi::setFrequency(uint32_t frequency)
{
  _frequency = frequency;
  return true;
}

/*SPI_MODE0 SPI_MODE1 SPI_MODE2 SPI_MODE3*/
bool ComDriverSpi::setDataMode(uint8_t dataOrder)
{
  _dataOrder = dataOrder;
  return true;
}
/*LSBFIRST, MSBFIRST*/
bool ComDriverSpi::setDataOrderMode(uint8_t dataMode)
{
  _dataMode = dataMode;
  return true;
}

/*Start, End, StartEnd*/
bool ComDriverSpi::setTransferOption(transferOption_t transferOption)
{
  _transferOption = transferOption;
  return true;
}

bool ComDriverSpi::writeSpi(uint8_t data[], uint32_t sizeToWrite, transferOption_t transferOption)
{

  _transferOption = transferOption;

  /*Control the SPI bus*/
  SPI.beginTransaction(SPISettings(_frequency, _dataOrder, _dataMode));

  /*Slave select pin low*/
  digitalWrite(_slaveSelectPin, LOW);
  /*send data and datasize*/
  SPI.transfer(data, sizeToWrite);

  /*Option transferEnd*/
  if (_transferOption == transferEnd || _transferOption == transferStartEnd)
  {
    /*Slave select pin high*/
    digitalWrite(_slaveSelectPin, HIGH);
    /*Release the SPI bus*/
  }

  /*Release the SPI bus*/
  SPI.endTransaction();

  return true;
}

bool ComDriverSpi::readSpi(uint8_t data[], uint32_t sizeToRead, transferOption_t transferOption)
{

  _transferOption = transferOption;

  /*Control the SPI bus*/
  SPI.beginTransaction(SPISettings(_frequency, _dataOrder, _dataMode));

  /*Slave select pin low*/
  digitalWrite(_slaveSelectPin, LOW);
  /*send data and datasize*/
  SPI.transfer(data, sizeToRead);

  /*Option transferEnd*/
  if (_transferOption == transferEnd || _transferOption == transferStartEnd)
  {
    /*Slave select pin high*/
    digitalWrite(_slaveSelectPin, HIGH);
  }
  /*Release the SPI bus*/
  SPI.endTransaction();

  return true;
}

bool ComDriverSpi::writeReadSpi(uint8_t dataWrite[], uint32_t sizeToWrite, uint8_t dataRead[], unsigned long sizeToRead, transferOption_t transferOption)
{

  _transferOption = transferOption;

  /*Control the SPI bus*/
  SPI.beginTransaction(SPISettings(_frequency, _dataOrder, _dataMode));

  /*Slave select pin low*/
  digitalWrite(_slaveSelectPin, LOW);
  /*send data and datasize*/
  SPI.transfer(dataWrite, sizeToWrite);
  SPI.transfer(dataRead, sizeToRead);

  /*Slave select pin high*/
  if (_transferOption == transferEnd || _transferOption == transferStartEnd)
  {
    digitalWrite(_slaveSelectPin, HIGH);
  }

  /*Release the SPI bus*/
  SPI.endTransaction();

  return true;
}


