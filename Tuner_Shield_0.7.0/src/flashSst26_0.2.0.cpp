/*Functions for Flash Memory SST26*/
#include "flashSst26_0.2.0.h"

Sst26::Sst26()
{
  _address                  = 0;
  _id                       = {0xffffff, 0xff, 0xff, 0xff};
  _registerStatus           = {1, 1, 1, 1, 1, 1, 1, 1};
  _registerConfiguration    = {1, 1, 1, 1, 1, 1, 1, 1};
  _registerBlockProtection  = {0, 0, 0, 0, 0, 0};
  _uniqueId                 = {0xffffffff, 0xffffffff};

}

Sst26::Sst26(uint8_t slaveSelectPin)
  : _comDriverSpi(slaveSelectPin)
{
  _address                  = 0;
  _id                       = {0xffffff, 0xff, 0xff, 0xff};
  _registerStatus           = {1, 1, 1, 1, 1, 1, 1, 1};
  _registerConfiguration    = {1, 1, 1, 1, 1, 1, 1, 1};
  _registerBlockProtection  = {0, 0, 0, 0, 0, 0};
  _uniqueId                 = {0xffffffff, 0xffffffff};
}

Sst26::Sst26(uint8_t slaveSelectPin, uint32_t  frequency)
  : _comDriverSpi(slaveSelectPin, frequency)
{
  _address                  = 0;
  _id                       = {0xffffff, 0xff, 0xff, 0xff};
  _registerStatus           = {1, 1, 1, 1, 1, 1, 1, 1};
  _registerConfiguration    = {1, 1, 1, 1, 1, 1, 1, 1};
  _registerBlockProtection  = {0, 0, 0, 0, 0, 0};
  _uniqueId                 = {0xffffffff, 0xffffffff};
}

Sst26::Sst26(uint8_t slaveSelectPin, uint32_t  frequency, uint8_t dataOrder, uint8_t dataMode)
  : _comDriverSpi(slaveSelectPin, frequency, dataOrder, dataMode)
{
  _address                  = 0;
  _id                       = {0xffffff, 0xff, 0xff, 0xff};
  _registerStatus           = {1, 1, 1, 1, 1, 1, 1, 1};
  _registerConfiguration    = {1, 1, 1, 1, 1, 1, 1, 1};
  _registerBlockProtection  = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
  _uniqueId                 = {0xffffffff, 0xffffffff};
}

/*Functions*/

bool Sst26::enableWrite(void)
{
  unsigned char cmd[1];

  cmd[0] = ENABLE_WRITE;

  _comDriverSpi.writeSpi(cmd, sizeof(cmd));

  return true;
}

bool Sst26::disableWrite(void)
{
  unsigned char cmd[1];

  cmd[0] = DISABLE_WRITE;

  _comDriverSpi.writeSpi(cmd, sizeof(cmd));

  return true;
}

Sst26::id_t Sst26::readId()
{
  unsigned char cmd[1];
  unsigned char data[3] = { 0xff };

  cmd[0] = READ_JEDEC_ID;

  _comDriverSpi.writeSpi(cmd, sizeof(cmd), ComDriverSpi::transferStart);
  _comDriverSpi.readSpi(data, sizeof(data), ComDriverSpi::transferEnd);

  _id.manufacturerId = data[0];
  _id.deviceType = data[1];
  _id.deviceId = data[2];
  _id.jedecId = ((unsigned long)data[0] << 16) | (data[1] << 8) | data[2];

  return _id;
}

Sst26::registerStatus_t Sst26::readRegisterStatus()
{
  unsigned char cmd[1];
  unsigned char data[1] = { 0xff };

  cmd[0] = READ_STATUS_REGISTER;

  _comDriverSpi.writeSpi(cmd, sizeof(cmd), ComDriverSpi::transferStart);
  _comDriverSpi.readSpi(data, sizeof(data), ComDriverSpi::transferEnd);

  _registerStatus.busy1            = data[0] & 1;
  _registerStatus.writeEnableLatch = data[0] >> 1 & 1;
  _registerStatus.suspendErase     = data[0] >> 2 & 1;
  _registerStatus.suspendProgram   = data[0] >> 3 & 1;
  _registerStatus.writeProtection  = data[0] >> 4 & 1;
  _registerStatus.securityId       = data[0] >> 5 & 1;
  _registerStatus.reserved1        = data[0] >> 6 & 1;
  _registerStatus.busy2            = data[0] >> 7 & 1;

  return _registerStatus;
}

Sst26::registerConfiguration_t Sst26::readRegisterConfiguration()
{
  unsigned char cmd[1];
  unsigned char data[1] = { 0xff };

  cmd[0] = READ_CONFIGURATION_REGISTER;

  _comDriverSpi.writeSpi(cmd, sizeof(cmd), ComDriverSpi::transferStart);
  _comDriverSpi.readSpi(data, sizeof(data), ComDriverSpi::transferEnd);

  _registerConfiguration.reserved1       = data[0] & 1;
  _registerConfiguration.ioConfiguration = data[0] >> 1 & 1;
  _registerConfiguration.reserved2       = data[0] >> 2 & 1;
  _registerConfiguration.blockProtectionVolatility = data[0] >> 3 & 1;
  _registerConfiguration.reserved3       = data[0] >> 4 & 1;
  _registerConfiguration.reserved4       = data[0] >> 5 & 1;
  _registerConfiguration.reserved5       = data[0] >> 6 & 1;
  _registerConfiguration.writeProtectionPin  = data[0] >> 7 & 1;

  return _registerConfiguration;
}

bool Sst26::writeRegisterConfiguration(registerConfiguration_t registerConfiguration)
{
  _registerConfiguration  = registerConfiguration;

  unsigned char cmd[3] = {0xff, 0xff, 0xff};

  cmd[0] = WRITE_STATUS_REGISTER;

  cmd[1] = 0;
  /*
    (unsigned char)_registerStatus.busy1 |
    (unsigned char)_registerStatus.writeEnableLatch << 1 |
    (unsigned char)_registerStatus.suspendErase << 2 |
    (unsigned char)_registerStatus.suspendProgram << 3 |
    (unsigned char)_registerStatus.writeProtection << 4 |
    (unsigned char)_registerStatus.securityId << 5 |
    (unsigned char)_registerStatus.reserved1 << 6 |
    (unsigned char)_registerStatus.busy2 << 7;
  */
  cmd[2] =
    (unsigned char)_registerConfiguration.reserved1 |
    (unsigned char)_registerConfiguration.ioConfiguration << 1 |
    (unsigned char)_registerConfiguration.reserved2 << 2 |
    (unsigned char)_registerConfiguration.blockProtectionVolatility << 3 |
    (unsigned char)_registerConfiguration.reserved3 << 4 |
    (unsigned char)_registerConfiguration.reserved4 << 5 |
    (unsigned char)_registerConfiguration.reserved5 << 6 |
    (unsigned char)_registerConfiguration.writeProtectionPin << 7;

  enableWrite();

  _comDriverSpi.writeSpi(cmd, sizeof(cmd));

  return true;
}

Sst26::registerBlockProtection_t Sst26::readRegisterBlockProtection()
{
  unsigned char cmd[1];
  unsigned char data[6] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

  cmd[0] = READ_BP_REGISTER;

  _comDriverSpi.writeSpi(cmd, sizeof(cmd), ComDriverSpi::transferStart);
  _comDriverSpi.readSpi(data, sizeof(data), ComDriverSpi::transferEnd);

  _registerBlockProtection.adressRange0 = data[0];
  _registerBlockProtection.adressRange1 = data[1];
  _registerBlockProtection.adressRange2 = data[2];
  _registerBlockProtection.adressRange3 = data[3];
  _registerBlockProtection.adressRange4 = data[4];
  _registerBlockProtection.adressRange5 = data[5];

  return _registerBlockProtection;
}

bool Sst26::writeRegisterBlockProtection(registerBlockProtection_t registerBlockProtection)
{
  _registerBlockProtection = registerBlockProtection;

  unsigned char data[7];

  data[0] = WRITE_BP_REGISTER;
  data[1] = _registerBlockProtection.adressRange0;
  data[2] = _registerBlockProtection.adressRange1;
  data[3] = _registerBlockProtection.adressRange2;
  data[4] = _registerBlockProtection.adressRange3;
  data[5] = _registerBlockProtection.adressRange4;
  data[6] = _registerBlockProtection.adressRange5;

  enableWrite();

  _comDriverSpi.writeSpi(data, sizeof(data));

  /*retry until device not busy anymore*/
  for (unsigned char retry = 0; retry < maxRetry; retry++)
  {
    if (!isBusy()) break;
    if (retry == maxRetry)return false;
  }

  return true;
}

bool Sst26::readByte(unsigned long adressStart, unsigned char data[])
{
  unsigned char cmd[4];

  data[0] = {0xff};

  cmd[0] = READ_DATA;
  cmd[1] = adressStart >> 16 & 0xFF;
  cmd[2] = adressStart >> 8 & 0xFF;
  cmd[3] = adressStart & 0xFF;

  _comDriverSpi.writeSpi(cmd, sizeof(cmd), ComDriverSpi::transferStart);
  _comDriverSpi.readSpi(data, sizeof(data), ComDriverSpi::transferEnd);

  /*retry until device not busy anymore*/
  for (unsigned char retry = 0; retry < maxRetry; retry++)
  {
    if (!isBusy()) break;
    if (retry == maxRetry)return false;
  }

  return data[0];
}

bool Sst26::writeByte(unsigned long adressStart, unsigned char data[])
{
  unsigned char cmd[5];

  cmd[0] = WRITE_DATA;
  cmd[1] = adressStart >> 16 & 0xFF;
  cmd[2] = adressStart >> 8 & 0xFF;
  cmd[3] = adressStart & 0xFF;
  cmd[4] = data[0];

  enableWrite();

  _comDriverSpi.writeSpi(cmd, sizeof(cmd));

  /*retry until device not busy anymore*/
  for (unsigned char retry = 0; retry < maxRetry; retry++)
  {
    if (!isBusy()) break;
    if (retry == maxRetry)return false;
  }
  return true;
}

bool Sst26::readPage(unsigned long addressStart, unsigned char page[])
{
  /*Adress has to start at begin of page*/
  //if (adressStart % _sizePage != 0) return false;

  /*Adress has to start at begin of page*/
  addressStart = addressStart & 0xFFFFFF00;

  unsigned char cmd[4];

  cmd[0] = READ_DATA;
  cmd[1] = addressStart >> 16 & 0xFF;
  cmd[2] = addressStart >> 8 & 0xFF;
  cmd[3] = addressStart & 0xFF;

  _comDriverSpi.writeSpi(cmd, sizeof(cmd), ComDriverSpi::transferStart);
  _comDriverSpi.readSpi(page, _sizePage, ComDriverSpi::transferEnd);

  /*retry until device not busy anymore*/
  for (unsigned char retry = 0; retry < maxRetry; retry++)
  {
    if (!isBusy()) break;
    if (retry == maxRetry)return false;
  }
  return true;
}

bool Sst26::writePage(unsigned long addressStart, unsigned char page[])
{
  /*Adress has to start at begin of page*/
  //if (addressStart % _sizePage != 0) return false;

  /*Adress has to start at begin of page*/
  addressStart = addressStart & 0xFFFFFF00;

  /*
    Serial.print(F("Address: "));
    Serial.println(adressStart, HEX);
    unsigned short i = 0;
     while(i < 0x100)
    {
      Serial.print(page[i], HEX);
      Serial.print(F(" "));
      i++;
    }
  */

  unsigned char cmd[4];

  cmd[0] = WRITE_DATA;
  cmd[1] = addressStart >> 16 & 0xFF;
  cmd[2] = addressStart >> 8 & 0xFF;
  cmd[3] = addressStart & 0xFF;

  enableWrite();

  _comDriverSpi.writeSpi(cmd, sizeof(cmd), ComDriverSpi::transferStart);
  _comDriverSpi.writeSpi(page, _sizePage, ComDriverSpi::transferEnd);

  /*retry until device not busy anymore*/
  for (unsigned char retry = 0; retry < maxRetry; retry++)
  {
    delay(durationWritePage);
    if (!isBusy()) break;
    if (retry == maxRetry)return false;
  }
  return true;
}


/*Read data max 40 MHz*/
bool Sst26::readData(unsigned long adressStart, unsigned char data[], unsigned long lenData)
{
  unsigned char cmd[4];

  cmd[0] = READ_DATA;
  cmd[1] = adressStart >> 16 & 0xFF;
  cmd[2] = adressStart >> 8 & 0xFF;
  cmd[3] = adressStart & 0xFF;

  _comDriverSpi.writeSpi(cmd, sizeof(cmd), ComDriverSpi::transferStart);
  _comDriverSpi.readSpi(data, lenData, ComDriverSpi::transferEnd);

  /*retry until device not busy anymore*/
  for (unsigned char retry = 0; retry < maxRetry; retry++)
  {
    if (!isBusy()) break;
    if (retry == maxRetry)return false;
  }
  return true;
}

bool Sst26::writeData(unsigned long addressStart, unsigned char data[], unsigned long lenData)
{
  /*Adress has to start at begin of page*/
  if (addressStart % _sizePage != 0) return false;

  //position in data[]
  unsigned long position = 0;
  //number of buffers written
  unsigned short numBuffer = 0;

  Serial.println();
  Serial.println("Address:");
  Serial.print("Length:\t");
  Serial.print(F("0x"));
  Serial.println(lenData, HEX);
  Serial.print("Start:\t");
  Serial.print(F("0x"));
  Serial.println(addressStart, HEX);
  Serial.println();

  //cycle trough array data[]
  while (position < lenData)
  {

    //create local buffer
    unsigned char buffer[_sizePage];
    //initalize local buffer with 0xff
    for (unsigned short i = 0; i < _sizePage; i++)buffer[i] = 0xff;

    //fill buffer
    for (unsigned short j = 0; j < _sizePage; j++)
    {
      //copy data in buffer
      buffer[j] = data[position];
      //next position in data[]
      position++;

      //end of data[] reached before buffer[] full
      if (position >= lenData)break;
    }
    //buffer full ..write page at right address...if not full write 0xff
    writePage(addressStart + numBuffer * _sizePage, buffer);

    //count buffers written for next page address
    numBuffer++;
  }
  return true;
}

/*Erase 4kBytes = 0x1000*/
bool Sst26::erase4kByte(unsigned long addressStart)
{
  /*Adress has to start at begin of sector*/
  Serial.print(F("Address to Erase:\t0x"));
  Serial.println(addressStart, HEX);
  addressStart = addressStart & 0xFFFFF000;
  Serial.print(F("Sector to Erase:\t0x"));
  Serial.println(addressStart, HEX);

  unsigned char cmd[4];

  cmd[0] = ERASE_4KB;
  cmd[1] = addressStart >> 16 & 0xFF;
  cmd[2] = addressStart >> 8 & 0xFF;
  cmd[3] = addressStart & 0xFF;

  enableWrite();

  _comDriverSpi.writeSpi(cmd, sizeof(cmd));

  /*retry until device not busy anymore*/
  for (unsigned char retry = 0; retry < maxRetry; retry++)
  {
    delay(durationEraseSector);
    if (!isBusy()) break;
    if (retry == maxRetry)return false;
  }
  return true;
}

/*Erase Block 8kByte*/
bool Sst26::erase8kByte(unsigned long addressStart)
{
  /*Adress has to start at begin of Block*/
  if (addressStart % 0x2000 != 0) return false;

  /*Adress has to start at begin of sector*/
  Serial.print(F("Address to Erase:\t0x"));
  Serial.println(addressStart, HEX);
  //addressStart = addressStart & 0xFFFFF000;
  Serial.print(F("Sector to Erase:\t0x"));
  Serial.println(addressStart, HEX);

  unsigned char cmd[4];

  cmd[0] = ERASE_8KB;
  cmd[1] = addressStart >> 16 & 0xFF;
  cmd[2] = addressStart >> 8 & 0xFF;
  cmd[3] = addressStart & 0xFF;

  enableWrite();

  _comDriverSpi.writeSpi(cmd, sizeof(cmd));

  /*retry until device not busy anymore*/
  for (unsigned char retry = 0; retry < maxRetry; retry++)
  {
    delay(durationEraseSector);
    if (!isBusy()) break;
    if (retry == maxRetry)return false;
  }

  return true;
}

/*Erase all*/
bool Sst26::eraseAll()
{
  unsigned char cmd[1];

  cmd[0] = ERASE_ALL;

  enableWrite();

  _comDriverSpi.writeSpi(cmd, sizeof(cmd));

  /*retry until device not busy anymore*/
  for (unsigned char retry = 0; retry < maxRetry; retry++)
  {
    delay(durationEraseAll);
    if (!isBusy()) break;
    if (retry == maxRetry)return false;
  }

  return true;
}

/*Read unique id*/
Sst26::uniqueId_t Sst26::readUniqueId()
{
  unsigned char cmd[4];
  unsigned char data[8] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

  cmd[0] = READ_SECURITY_DATA;
  cmd[1] = 0x00;/*address*/
  cmd[2] = 0x00;/*address*/
  cmd[3] = 0xff;/*dummy*/

  _comDriverSpi.writeSpi(cmd, sizeof(cmd), ComDriverSpi::transferStart);
  _comDriverSpi.readSpi(data, sizeof(data), ComDriverSpi::transferEnd);

  _uniqueId.uniqueIdLower =  (unsigned long long) data[4] << 24
                             | (unsigned long long) data[5] << 16
                             | (unsigned long long) data[6] << 8
                             | (unsigned long long) data[7];

  _uniqueId.uniqueIdUpper =  (unsigned long long) data[0] << 24
                             | (unsigned long long) data[1] << 16
                             | (unsigned long long) data[2] << 8
                             | (unsigned long long) data[3];

  return _uniqueId;
}

/*Read security data*/
unsigned char Sst26::readSecurityData(unsigned long addressStart)
{
  //User Programmable 0008H â€“ 07FFH
  if (addressStart < 0x08 || addressStart > 0x07FF) addressStart = 0x08;

  unsigned char cmd[3];
  /*buffer security data*/
  unsigned char securityData[1] = {0xff};

  cmd[0] = READ_SECURITY_DATA;
  cmd[1] = (addressStart) >> 8 & 0xff;
  cmd[2] = (addressStart) & 0xff;

  _comDriverSpi.writeSpi(cmd, sizeof(cmd), ComDriverSpi::transferStart);
  _comDriverSpi.readSpi(securityData, sizeof(securityData), ComDriverSpi::transferEnd);

  return securityData[0];
}

/*Write security data*/
bool Sst26::writeSecurityData()
{
  unsigned long address = 8;
  unsigned char cmd[3];
  /*2040 bytes of security data available*/
  unsigned short const lenSecureData = 100;
  /*buffer security data*/
  unsigned char secureData[lenSecureData] = { 0 };
  /*initalize*/
  for (unsigned short i = 0; i < lenSecureData; i++) secureData[i] = 0xef;

  cmd[0] = WRITE_SECURITY_DATA;
  cmd[1] = address >> 8 & 0xff;
  cmd[2] = address & 0xff;

  enableWrite();

  _comDriverSpi.writeSpi(cmd, sizeof(cmd), ComDriverSpi::transferStart);
  _comDriverSpi.readSpi(secureData, sizeof(secureData), ComDriverSpi::transferEnd);

  return true;
}

/*Disable Global Block Protection*/
bool Sst26::disableGlobalBlockProtection(void)
{
  unsigned char cmd[1];

  cmd[0] = DISABLE_GLOBAL_BP_REGISTER;

  enableWrite();

  _comDriverSpi.writeSpi(cmd, sizeof(cmd));

  return true;
}

/*Enable deep power down*/
bool Sst26::enableDeepPowerDown(void)
{
  unsigned char cmd[1];

  cmd[0] = ENABLE_DEEP_POWER_DOWN;

  _comDriverSpi.writeSpi(cmd, sizeof(cmd));

  return true;
}

/*Release deep power down*/
bool Sst26::releaseDeepPowerDown(void)
{
  unsigned char cmd[4];

  cmd[0] = RELEASE_DEEP_POWER_DOWN;
  cmd[1] = 0xff;/*dummy*/
  cmd[2] = 0xff;/*dummy*/
  cmd[3] = 0xff;/*dummy*/

  _comDriverSpi.writeSpi(cmd, sizeof(cmd));

  return true;
}

/*Reset device*/
bool Sst26::reset(void)
{
  unsigned char cmd[1];

  cmd[0] = ENABLE_RESET;

  _comDriverSpi.writeSpi(cmd, sizeof(cmd));

  cmd[0] = RESET;

  _comDriverSpi.writeSpi(cmd, sizeof(cmd));

  return true;
}

/*Test if device is busy*/
bool Sst26::isBusy(void)
{
  /*Read register status*/

  Sst26::readRegisterStatus();

  if (_registerStatus.busy1 == 1 ) return true;

  return false;
}



