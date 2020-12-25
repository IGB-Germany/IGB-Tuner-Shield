//Print detailed info about Ensemble
#define DEBUG_DAB_PARSE_ENSEMBLE


//Driver for tuner circuit SI468x by IGB
#include "SI468x.h"

//firmware
#include "firmware.h"

//SPI communication layer
#include "ComDriverSpi.h"
ComDriverSpi tuner(PIN_DEVICE_SLAVE_SELECT, SPI_FREQUENCY);


//flash functions
#include "FlashSst26.h"
//Serial Monitor Print Functions
#include "printSerial.h"

//Global variables of device

//Device power up arguments
powerUpArguments_t powerUpArguments =
{
  //CTS
  1,
  //CLK_MODE[5:4]
  //powerUpArguments.clockMode
  //0 //Oscillator and buffer are powered down.
  1, //Reference clock generator is in crystal mode.
  //2 //Oscillator is off and circuit acts as single ended buffer.
  //3 //Oscillator is off and circuit acts as differential buffer.

  //TR_SIZE[3:0]
  //powerUpArguments.trSize
  //Range 0-15
  //TR_SIZE(19,2MHz)=  7 see table
  //TR_SIZE(30MHz)  = 11
  //TR_SIZE = lowerTR_SIZE + (higherTR_SIZE ÃƒÂ¢Ã¢â€šÂ¬Ã¢â‚¬Å“ lowerTR_SIZE) x (f ÃƒÂ¢Ã¢â€šÂ¬Ã¢â‚¬Å“ fLower)/(fHigher ÃƒÂ¢Ã¢â€šÂ¬Ã¢â‚¬Å“ fLower)
  //TR_SIZE(30MHz)   =  0x0A + (0x0F - 0x0A) x (30 - 27)/(37,2 - 27) = 0x0A + 0x04 x (3/10,2) = 0x0B = 11
  11,//ok
  //15,//ok

  //IBIAS[6:0]
  //powerUpArguments.iBiasStart
  //Range 0-127, 10 uA steps, 0 to 1270 uA
  127,//ok
  //60//ok
  //30//ok
  //5//ok
  //1//ok

  //XTAL Frequency in Hz
  //powerUpArguments.xtalFreq
  //The supported crystal frequencies are: 6,12,18,24,30 MHz [5.4 MHz - 6.6 MHz]
  //[10.8 MHz - 13.2 MHz] [16.8 MHz - 19.8 MHz] [21.6 MHz - 26.4 MHz] [27 MHz - 46.2 MHz]
  //Range 5,4 - 46,2 MHz
  //19200000UL;
  //30000002UL//ok
  30000000UL,//ok
  //29999996UL//ok

  //powerUpArguments.cTune
  //CTUN programs a pair of internal tuning capacitances
  //Cload       = capacity of crystal from manufacturers datasheet
  //Cparasitic  = capacity from pcb etc.
  //Cx          = external discrete capacitance if Cload >14pF

  //63 steps for CTUN starting at 0x00...0x3F = 0...63 = 0-24pF
  //0,375 pF per step
  //Maximum CTUN = 24pF = 63
  //CTUN = 2*(Cload - Cparasitic)- Cx

  //Cload = 18pF, Cparasitic = 1pF, Cx=0, CTUN = 34pF = 91
  //Cload = 13pF, Cparasitic = 1pF, Cx=0, CTUN = 24pF = 63
  //Cload = 10pF, Cparasitic = 1pF, Cx=0, CTUN = 18pF = 48
  //Cload = 5,5pF, Cparasitic = 1pF, Cx=0, CTUN = 9pF = 24
  //Cload = 3,25pF, Cparasitic = 1pF, Cx=0, CTUN = 4,5pF = 12

  //Crystal with Cload <=13pF is necessary

  //powerUpArguments.cTune
  //63,//ok Max
  48,//ok
  //24,//ok
  //12,//nok

  //IBIAS_RUN[6:0]
  //powerUpArguments.iBiasRun
  //To reduce the impact of appearance of unwanted spurs in tuner chain for,
  //it is recommended to reduce the biasing current by 2x
  //If set to 0, will use the same value as iBiasStart
  //Range 0-127, 10 uA steps, 10 to 1270 uA

  //powerUpArguments.iBiasRun = 127;//ok
  60//ok
  //powerUpArguments.iBiasRun = 30;//ok
  //powerUpArguments.iBiasRun = 10;//ok
  //powerUpArguments.iBiasRun = 5;//nok
  //powerUpArguments.iBiasRun = 0;//ok
};

unsigned short propertyValueListDevice[NUM_PROPERTIES_DEVICE][2] =
{
  {INT_CTL_ENABLE,      1 << 7 | 1 << 6 | 1 << 5 | 1 << 4 | 1},//DEVNTIEN[13], CTSIEN[7],ERR_CMDIEN[6],DACQIEN[5],DSRVIEN[4],RSQIEN[3],ACFIEN[1], STCIEN[0]; default 0
  {INT_CTL_REPEAT,      0},//default 0

  //Digital Output I2S
  {DIGITAL_IO_OUTPUT_SELECT, (unsigned short)1 << 15},
  {DIGITAL_IO_OUTPUT_SAMPLE_RATE, 48000},
  {DIGITAL_IO_OUTPUT_FORMAT, 0x1800},
  {DIGITAL_IO_OUTPUT_FORMAT_OVERRIDES_1, 0},
  {DIGITAL_IO_OUTPUT_FORMAT_OVERRIDES_2, 0},
  {DIGITAL_IO_OUTPUT_FORMAT_OVERRIDES_3, 0},
  {DIGITAL_IO_OUTPUT_FORMAT_OVERRIDES_4, 0},

  //Volume
  {AUDIO_ANALOG_VOLUME, 63},//default 63 max
  {AUDIO_MUTE,          0x0000},//0,1,2,3; No, L, R, Both, default 0
  {AUDIO_OUTPUT_CONFIG, 0x0000},//0,1; Stereo, Mono, default 0

  //Interrupts, I2S Digital Out or DAC Analog Out
  {PIN_CONFIG_ENABLE,   (unsigned short) 1 << 15 | 0 << 1 | 1},//INTB enabled, I2S disabled, DAC enabled; default 0b1000000000000001

  //WAKE TONE
  {WAKE_TONE_ENABLE, 1},//default 0
  {WAKE_TONE_PERIOD, 2000},//default 250, 0 continous , 50...2000ms
  {WAKE_TONE_FREQ, 750},//default 750
  {WAKE_TONE_AMPLITUDE, 8}//default 8, 0...31
};


//Start
void deviceBegin()
{
  statusRegister_t statusRegister;

  //SW version
  serialPrintSi468x::printVersion(version);

  //check if Flash Memory is present
  serialPrintSi468x::printFlashMemoryInfo();

  initalize();

  reset();

  powerUp(powerUpArguments);

  loadFirmware(addrBootloaderPatchFull, sizeBootloaderPatchFull); //FullPatch

  readStatusRegister(statusRegister);
  serialPrintSi468x::printStatusRegister(statusRegister);

}

//Loads data from Flash Memory into host and than into device
void loadFirmware(unsigned long addressFirmware, unsigned long sizeFirmware)
{
  //to measure time
  //#define SPEED
  //time for dab firmware 2224ms @ package 0x100, 8MHz

#ifdef SPEED
  uint32_t start = millis();
  uint32_t zeit;
#endif

  //Max Package size of tuner circuit 0x1000 and modulo 4
  //Defines the size of packages load from flash to Arduino to device.Careful because of RAM effect
  //Tested with
  //0x100 ok
  //0x150 nok
  //0x200 nok
  //0x300 nok

  //Size of transferbuffer
  //open: works only with 0x100 because of flash
  const unsigned short lenData = 0x100;
  //transferbuffer
  uint8_t data[lenData];

  //If not initalized error in loading. Device reads commands ? every 2nd time after POR.
  for (uint16_t i = 0; i < lenData; i++) data[i] = 0xff;

  //Calculate modulo to get even number
  uint16_t modulo = sizeFirmware % lenData;

  //How many packages ?
  uint32_t packages = (sizeFirmware - modulo) / lenData;

  /*
    Serial.print(F("Size:"));
    Serial.print(sizeFirmware);
    Serial.print(F("\tPackages:"));
    Serial.print(packages);
    Serial.print(F("\tModulo:"));
    Serial.println(modulo);
  */

  //Create local object to use Flash Memory
  FlashSst26 flashSst26(PIN_FLASH_SLAVE_SELECT, SPI_FREQUENCY);

  //prepare circuit to load firmware
  loadInit();

  //cycle packages
  for (uint32_t i = 0 ; i < packages; i++)
  {
    //initalize dataBuffer[] with 0xff
    //for (uint16_t j = 0; j < lenData; j++) data[j] = 0xff;
    //Read data from flash
    flashSst26.readData(addressFirmware + i * lenData, data, lenData);
    //Write data to tuner
    hostLoad(data, lenData);
  }

  //last package is maybe not complete - fill up with 0xff
  if (modulo != 0)
  {
    //initalize dataBuffer[] with 0xff only for rest
    for (uint16_t j = 0; j < lenData; j++) data[j] = 0xff;
    //Read data from flash
    flashSst26.readData(addressFirmware + packages * lenData, data, modulo);
    //Write data to tuner
    hostLoad(data, lenData);
  }

#ifdef SPEED
  zeit = millis() - start;
  Serial.print(F("Zeit:\t"));
  Serial.println(zeit);
#endif
}

//Write command and argument
void writeCommandArgument(unsigned char cmd[], unsigned long lenCmd, unsigned char arg[], unsigned long lenArg)
{
  //no arguments
  if (lenArg == 0 && arg == nullptr)
  {
    tuner.writeSpi(cmd, lenCmd);
  }
  else if (lenArg != 0 && arg != nullptr)
  {
    tuner.writeSpi(cmd, lenCmd, ComDriverSpi::transferStart);
    tuner.writeSpi(arg, lenArg, ComDriverSpi::transferEnd);
  }
  else
    return;
}

//Write command
void writeCommand(unsigned char cmd[], unsigned long lenCmd)
{
  tuner.writeSpi(cmd, lenCmd);
}

//Initalize pins
void initalize()
{
  //Tuner interrupt pin ~INT
  pinMode(PIN_DEVICE_INTERRUPT, INPUT_PULLUP);
  //Tuner reset pin ~RST
  pinMode(PIN_DEVICE_RESET, OUTPUT);
  digitalWrite(PIN_DEVICE_RESET, HIGH);
  //slave select pin Si468x
  pinMode(PIN_DEVICE_SLAVE_SELECT, OUTPUT);
  digitalWrite(PIN_DEVICE_SLAVE_SELECT, HIGH);
  //slave select pin flash memory
  pinMode(PIN_FLASH_SLAVE_SELECT, OUTPUT);
  digitalWrite(PIN_FLASH_SLAVE_SELECT, HIGH);
}

//reset
void reset(unsigned char resetPin)
{
  digitalWrite(resetPin, HIGH);
  digitalWrite(resetPin, LOW);
  //Power supplies ramped up and stable to RSTB rise tPSUP:RSTB_HI 10 μs
  digitalWrite(resetPin, HIGH);
  delayMicroseconds(DURATION_RESET);
}

//Power Down
void powerDown(bool enable, unsigned char resetPin)
{
  if (enable)
    digitalWrite(resetPin, LOW);
  else
    digitalWrite(resetPin, HIGH);
  delayMicroseconds(DURATION_RESET);
}


//Read ststus register
void readStatusRegister(statusRegister_t& statusRegister)
{
  unsigned char buf[4] = {0xff, 0xff, 0xff, 0xff};

  readReply(buf, sizeof(buf));

  statusRegister.cts       = buf[0] >> 7 & 1;
  statusRegister.cmdErr    = buf[0] >> 6 & 1;
  statusRegister.dacqInt   = buf[0] >> 5 & 1;
  statusRegister.dsrvInt   = buf[0] >> 4 & 1;
  statusRegister.stcInt    = buf[0] & 1;
  statusRegister.eventInt  = buf[1] >> 5 & 1;
  statusRegister.state     = buf[3] >> 6 & 3;
  statusRegister.rfFeErr   = buf[3] >> 5 & 1;
  statusRegister.dspErr    = buf[3] >> 4 & 1;
  statusRegister.repOfErr  = buf[3] >> 3 & 1;
  statusRegister.cmdOfErr  = buf[3] >> 2 & 1;
  statusRegister.arbErr    = buf[3] >> 1 & 1;
  statusRegister.nonRecErr = buf[3] & 1;
}


//Read properties and return a pointer to a 2dim list of id and value
unsigned short (*readPropertyValueList(unsigned short propertyValueList[][2], unsigned char numberProperties))[2]
{
  //read from device
  for (uint8_t i = 0; i < numberProperties; i++)
  {
    //readPropertyValue() returns the value if given the right id
    propertyValueList[i][1] = readPropertyValue(propertyValueList[i][0]);
  }
  return propertyValueList;
}

//Writes 2 dimensional property value list with numberProperties to device
void writePropertyValueList(unsigned short propertyValueList[][2], unsigned char numberProperties)
{
  for (uint8_t i = 0; i < numberProperties; i++)
  {
    writePropertyValue(propertyValueList[i][0], propertyValueList[i][1]);
  }
}

//0x00 RD_REPLY Read answer of device
bool readReply(unsigned char reply[], unsigned long len)
{
  //result of reading
  bool readResult = false;

  //CMD
  unsigned char cmd[1] = {READ_REPLY};

  //retry until maxRetry
  for (unsigned char retry = 0; retry < MAX_RETRY; retry++)
  {
    //init reply to 0xff
    for (unsigned char i = 0; i < len; i++) reply[i] = 0xff;

    ///wait for device - to be improved
    delayMicroseconds(DURATION_REPLY);
    delayMicroseconds(DURATION_REPLY);

    writeCommandArgument(cmd, sizeof(cmd), reply, len);

    //Clear to send and no error then break loop
    if ((((reply[0] >> 7) & 1) == 1) && (((reply[0] >> 6) & 1) == 0))
    {
      readResult = true;
      break;
    }
    //error
    else if ((reply[0] >> 6 & 1) == 1)
    {
      //if cmdErr read byte 5 of reply
      unsigned char errBuf[5] = {0xff, 0xff, 0xff, 0xff, 0xff};
      writeCommandArgument(cmd, sizeof(cmd), errBuf, sizeof(errBuf));
      serialPrintSi468x::printResponseHex(errBuf, sizeof(errBuf));
      //statusRegister.cmdErrCode = errBuf[4];
      readResult = false;
      break;
    }

    //too many reads
    else if (retry == MAX_RETRY - 1)
    {
      readResult = false;
      break;
    }

    else
    {
      readResult = false;
    }
  }

  return readResult;
}


//0x01 POWER_UP Power-up the device and set system settings
void powerUp(powerUpArguments_t powerUpArguments)
{
  uint8_t cmd[16];
  uint8_t buf[4] = {0xff, 0xff, 0xff, 0xff};

  cmd[0] = POWER_UP;
  cmd[1] = (powerUpArguments.cts & 1) << 7;  //Arg 1   CTSIEN 0 0 0 0 0 0 0  Clear to Sent Interrupt Enabled
  cmd[2] = (powerUpArguments.clockMode & 0x03) << 4 | (powerUpArguments.trSize & 0xF); //Arg 2 0 0 CLK_MODE[1:0] TR_SIZE[3:0]
  cmd[3] = powerUpArguments.iBiasStart & 0x7F;     //ARG3 0 IBIAS[6:0]
  cmd[4] = powerUpArguments.xtalFreq & 0xFF;       //Arg4 XTAL_FREQ[7:0]
  cmd[5] = powerUpArguments.xtalFreq >> 8 & 0xFF;  //Arg5 XTAL_FREQ[15:8]
  cmd[6] = powerUpArguments.xtalFreq >> 16 & 0xFF; //Agr6 XTAL_FREQ[23:16]
  cmd[7] = powerUpArguments.xtalFreq >> 24 & 0xFF; //Agr7 XTAL_FREQ[31:24]
  cmd[8] = powerUpArguments.cTune & 0x3F;                 //Arg8 0 0 CTUN[5:0]
  cmd[9] = 1 << 4;                                  //ARG9 0 0 0 1 0 0 0 0
  cmd[10] = 0;                                       //Arg10 0
  cmd[11] = 0;                                      //Arg11 0
  cmd[12] = 0;                                      //Arg12 0
  cmd[13] = powerUpArguments.iBiasRun & 0x3F;      //Arg13 IBIAS_RUN[6:0]
  cmd[14] = 0;                                      //Arg14 0
  cmd[15] = 0;                                      //Arg15 0

  writeCommand(cmd, sizeof(cmd));
  delayMicroseconds(DURATION_POWER_UP);
  readReply(buf, sizeof(buf));
}

//0x04 HOST_LOAD Loads an image from HOST over command interface
void hostLoad(unsigned char package[], unsigned short len)
{
  //Validity ? Maximal 4096 bytes of application image
  if (len > 0x1000) len = 0x1000;

  unsigned char cmd[4];

  cmd[0] = HOST_LOAD;
  cmd[1] = 0x00; //ARG1
  cmd[2] = 0x00; //ARG2
  cmd[3] = 0x00; //ARG3

  writeCommandArgument(cmd, sizeof(cmd), package, len);
}

//0x05 FLASH_LOAD Loads an image from external FLASH over secondary SPI bus
void flashLoad(unsigned long address, unsigned char subCommand)
{
  unsigned char cmd[12];

  cmd[0] = FLASH_LOAD; //CMD
  cmd[1] = subCommand; //SUBCMD1
  cmd[2] = 0x00;       //SUBCMD2
  cmd[3] = 0x00;       //SUBCMD3
  cmd[4] = address & 0xFF;      //ARG4 FLASH_ADDR [7:0]
  cmd[5] = address >> 8 & 0xFF; //ARG5 FLASH_ADDR [15:8]
  cmd[6] = address >> 16 & 0xFF;//ARG6 FLASH_ADDR [23:16]
  cmd[7] = address >> 24 & 0xFF;//ARG7 FLASH_ADDR [31:24]
  cmd[8] = 0x00;   //ARG8 0x00
  cmd[9] = 0x00;   //ARG9 0x00
  cmd[10] = 0x00;   //ARG10 0x00
  cmd[11] = 0x00;  //ARG11 0x00

  writeCommand(cmd, sizeof(cmd));
}

//0x06 LOAD_INIT Prepares the bootloader to receive a new image
void loadInit()
{
  unsigned char cmd[2];

  cmd[0] = LOAD_INIT;
  cmd[1] = 0x00;

  ///wait for device - to be improved
  delayMicroseconds(DURATION_LOAD_INIT);
  writeCommand(cmd, sizeof(cmd));
  delayMicroseconds(DURATION_LOAD_INIT);
}

//0x07 BOOT Boots the image currently loaded in RAM
void boot()
{
  unsigned char cmd[2];

  cmd[0] = BOOT;
  cmd[1] = 0;

  writeCommand(cmd, sizeof(cmd));

  //wait for device - to be improved
  for (uint8_t i = 0; i < 30; i++)
  {
    delayMicroseconds (DURATION_BOOT);
  }
}

//0x08 GET_PART_INFO Get Device Part Info
void readPartInfo(partInfo_t& partInfo)
{
  unsigned char cmd[2];
  cmd[0] = GET_PART_INFO;
  cmd[1] = 0;

  unsigned char buf[23] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

  writeCommand(cmd, sizeof(cmd));
  readReply(buf, sizeof(buf));

  partInfo.chipRev    = buf[4];
  partInfo.romId      = buf[5];
  partInfo.partNumber = buf[9] << 8 | buf[8];
}

//0x09 GET_SYS_STATE reports basic system state information such as which mode is active; FM, DAB, etc.
unsigned char readSystemState()
{
  unsigned char systemState = 0xff;

  unsigned char cmd[2];

  unsigned char buf[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

  cmd[0] = GET_SYS_STATE;
  cmd[1] = 0;

  writeCommand(cmd, sizeof(cmd));

  readReply(buf, sizeof(buf));

  systemState = buf[4];

  return systemState;
}

//0x0A GET_POWER_UP_ARGS Reports basic information about the device such as arguments used during POWER_UP
void readPowerUpArguments(powerUpArguments_t &powerUpArguments)
{

  unsigned char buf[18] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

  unsigned char cmd[2];
  cmd[0] = GET_POWER_UP_ARGS;
  cmd[1] = 0;

  writeCommand(cmd, sizeof(cmd));

  readReply(buf, sizeof(buf));

  powerUpArguments.clockMode  = buf[6] >> 4 & 3;
  powerUpArguments.trSize     = buf[6] & 0xF;
  powerUpArguments.iBiasStart = buf[7] & 0x7F;
  powerUpArguments.xtalFreq   = (unsigned long) buf[11] << 24 | (unsigned long)buf[10] << 16 | (unsigned long) buf[9] << 8 | buf[8];
  powerUpArguments.cTune      = buf[12] & 0x3F;
  powerUpArguments.iBiasRun   = buf[17] & 0x7F;

}

//0x10 READ_OFFSET Reads a portion of the response buffer (not the status)from an offset
bool readReplyOffset(uint8_t reply[], uint16_t len, uint16_t offset)
{
  /*
    0x10 READ_OFFSET is used for applications that cannot read the entire response buffer. This type of application can
    use this command to read the response buffer in segments. The host must pass in an offset from the beginning of
    the response buffer to indicate the starting point from which to read. This offset must be modulo 4. The response
    buffer remains intact as in the READ_REPLY command so that the response can be read again if needed. This
    function is available for both I2C and SPI mode. This is a software command, therefore it is best to read as much
    data in each calling as possible. This will reduce the overhead associated with using this command. It is
    recommended that the minimum reply size be on the order of 512 bytes. This means that for APIs that return less
    the 512 bytes the standard READ_REPLY should be used. The command is complete when the CTS bit (and
    optional interrupt) is set. The ERR bit (and optional interrupt) is set if an invalid argument is sent. Note that only a
    single interrupt occurs if both the CTS and ERR bits are set. The command may only be sent in powerup mode.
  */

  bool readResult = false;

  uint8_t cmd[4] =
  {
    READ_OFFSET,
    0,
    (uint8_t)(offset & 0xff),
    (uint8_t) (offset >> 8)
  };

  uint8_t cmd2[1] = {READ_REPLY};

  writeCommand(cmd, sizeof(cmd));

  //retry until maxRetry
  for (uint8_t retry = 0; retry < MAX_RETRY; retry++)
  {
    ///wait for device - to be improved
    delayMicroseconds(DURATION_REPLY_OFFSET);
    delayMicroseconds(DURATION_REPLY_OFFSET);

    //initalize reply to 0xff
    for (uint8_t i = 0; i < len; i++) reply[i] = 0xff;

    writeCommandArgument(cmd2, sizeof(cmd2), reply, len);

    //Clear to send and no error then break loop
    if ((((reply[0] >> 7) & 1) == 1) && (((reply[0] >> 6) & 1) == 0))
    {
      readResult = true;
      //break loop
      break;
    }

    //error
    else if (((reply[0] >> 6) & 1) == 1 )
    {
      //if cmdErr read byte 5 of reply
      unsigned char errBuf[5] = {0xff, 0xff, 0xff, 0xff, 0xff};
      writeCommandArgument(cmd, sizeof(cmd), errBuf, sizeof(errBuf));
      serialPrintSi468x::printResponseHex(errBuf, sizeof(errBuf));

      readResult = false;
      //break loop
      break;
    }

    //too many reads
    else if (retry == MAX_RETRY - 1)
    {
      readResult = false;
      //break loop
      break;
    }

    else
    {
      readResult = false;
    }
  }

  return readResult;
}

//0x12 GET_FUNC_INFO Get Firmware Information
void readFirmwareInformation(firmwareInformation_t& firmwareInformation)
{
  unsigned char cmd[2];
  cmd[0] = GET_FUNC_INFO;
  cmd[1] = 0;

  unsigned char buf[12] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

  writeCommand(cmd, sizeof(cmd));

  readReply(buf, sizeof(buf));

  firmwareInformation.revisionNumberMajor = buf[4];
  firmwareInformation.revisionNumberMinor = buf[5];
  firmwareInformation.revisionNumberBuild = buf[6];
  firmwareInformation.noSvnFlag = buf[7] >> 7 & 1;
  firmwareInformation.location = buf[7] >> 4 & 2;
  firmwareInformation.mixedRevFlag = buf[7] >> 1 & 1;
  firmwareInformation.localModFlag = buf[7] & 1;
  firmwareInformation.svnId = buf[8] | (unsigned long)buf[9] << 8 | (unsigned long)buf[10] << 16 | (unsigned long)buf [11] << 24;
}

//0x13 SET_PROPERTY Sets the value of a property
void writePropertyValue(unsigned short id, unsigned short value)
{
  unsigned char buf[4] = {0xff, 0xff, 0xff, 0xff};

  unsigned char cmd[6];
  cmd[0] = SET_PROPERTY;
  cmd[1] = 0x00;
  cmd[2] = id & 0xff;
  cmd[3] = id >> 8;
  cmd[4] = value & 0xff;
  cmd[5] = value >> 8;

  writeCommand(cmd, sizeof(cmd));

  readReply(buf, sizeof(buf));
}

//0x14 GET_PROPERTY Retrieve the value of a property
unsigned short readPropertyValue(unsigned short id)
{
  unsigned char buf[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

  unsigned short propertyValue = 0xffff;

  unsigned char cmd[4];
  cmd[0] = GET_PROPERTY;
  cmd[1] = 1;//count = 1
  cmd[2] = id & 0xff;
  cmd[3] = id >> 8 & 0xff;

  writeCommand(cmd, sizeof(cmd));
  delayMicroseconds(DURATION_PROPERTY);
  delayMicroseconds(DURATION_PROPERTY);
  delayMicroseconds(DURATION_PROPERTY);
  readReply(buf, sizeof(buf));

  propertyValue = (unsigned short)buf[5] << 8 | buf[4];

  return propertyValue;
}

//0x15 WRITE_STORAGE Writes data to the on board storage area at a specified offset
void writeStorage(unsigned char data[], unsigned char len, unsigned short offset)
{
  /*
    WRITE_STORAGE writes data to the no board storage area at the specified offset. the largest block of data that
    can be written at one time is 256 bytes. The command is complete when the CTS bit (and optional interrupt) is set.
    The ERR bit (and optional interrupt) is set if an invalid argument is sent. Note that only a single interrupt occurs if
    both the CTS and ERR bits are set. The command may only be sent in powerup mode.
  */

  unsigned char buf[4] = {0xff, 0xff, 0xff, 0xff};

  unsigned char cmd[8];
  cmd[0] = WRITE_STORAGE; //CMD
  cmd[1] = 0;             //0x00
  cmd[2] = offset & 0xff; //ARG2 OFFSET[7:0]
  cmd[3] = offset >> 8;   //ARG3 OFFSET[15:8]
  cmd[4] = len & 0xff;    //ARG4 LENGTH[7:0]
  cmd[5] = len >> 8;      //ARG5 LENGTH[15:8]
  cmd[6] = 0;             //ARG6 0x00
  cmd[7] = 0;             //ARG7 0x00

  writeCommandArgument(cmd, sizeof(cmd), data, len);

  readReply(buf, sizeof(buf));
}

//0x16 READ_STORAGE Reads data from the on board storage area from a specified offset
void readStorage(unsigned char data[], unsigned char len, unsigned short offset)
{
  unsigned char cmd[4];
  cmd[0] = READ_STORAGE;
  cmd[1] = 0;
  cmd[2] = offset & 0xff;//ARG2 OFFSET[7:0]
  cmd[3] = offset >> 8;  //ARG3 OFFSET[15:8]

  writeCommand(cmd, sizeof(cmd));
  //open: readStorage() handle first 4 bytes of answer
  readReply(data, len);
}

//0xE5 TEST_GET_RSSI returns the reported RSSI in 8.8 format
unsigned short readRssi()
{
  unsigned short rssi = 0;

  unsigned char buf[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

  unsigned char cmd[2];
  cmd[0] = TEST_GET_RSSI;
  cmd[1] = 0;

  writeCommand(cmd, sizeof(cmd));
  delayMicroseconds(10000);
  readReply(buf, sizeof(buf));

  rssi = buf[5] << 8 | buf[4];

  return rssi;
}





//Global variables DAB definitions

//Ensemble header with pointer to service list - dynamic memory allocation
ensembleHeader_t ensembleHeader = {0, 0, 0, 0, 0, nullptr};

//Frequency table - dynamic memory allocation
long unsigned* frequencyTable = nullptr;

frequencyTableHeader_t frequencyTableHeader = {0, nullptr};

//Table of valid frequencies
unsigned char* dabValidIndexList = nullptr;
//Number of valid indices
unsigned char dabNumValidIndex = 0;

//Actual index
//unsigned char dabIndex = 2;//CHAN_5C = 178352;//DR Deutschland
unsigned char dabIndex = 25; //CHAN_11A = 216928;//SWR RP
//unsigned char dabIndex = 28; //CHAN_11D = 220352;//DR Hessen

//Actual Digital Service
//sunshine live
//unsigned long serviceId = 0x15DC;
//unsigned long componentId = 0x0015;
//Radio TEDDY
//serviceId = 0x1B2E;
//componentId = 0xD;
//SWR3
unsigned long serviceId = 0xD3A3;
unsigned long componentId = 0x4;

//property value list DAB
unsigned short propertyValueListDab[NUM_PROPERTIES_DAB][2] =
{
  //General Properties
  //0x1710 DAB_TUNE_FE_VARM DAB/DMB Front End Varactor configuration slope m 0
  {DAB_TUNE_FE_VARM, 0},//default //0xF8A9//-2217 // 0xF7C8;//-1922
  //0x1711 DAB_TUNE_FE_VARB DAB/DMB Front End Varactor configuration intercept b 0
  {DAB_TUNE_FE_VARB, 0},//default //0x01C6);//454 //0x01D2);//466
  //0x1712 DAB_TUNE_FE_CFG Additional configuration options for the front end. 0x0001
  //front end switch closed = 1 front end switch open = 0
  //{DAB_TUNE_FE_CFG, 1},//default switch closed
  {DAB_TUNE_FE_CFG, 0},//switch open

  //0x8100 DIGITAL_SERVICE_INT_SOURCE Configures the interrupt sources for digital services 0x0000
  //DSRVOVFLINT, DSRVPCKTINT
  //{DIGITAL_SERVICE_INT_SOURCE, 0},//default
  {DIGITAL_SERVICE_INT_SOURCE, (1 << 1) | 1 },

  //0x8101 DIGITAL_SERVICE_RESTART_DELAY sets the delay time (in milliseconds) to restart digital service when recovering from acquisition loss 200
  {DIGITAL_SERVICE_RESTART_DELAY, 100},//default
  //{DIGITAL_SERVICE_RESTART_DELAY, 200},//default

  //0xB000 DAB_DIGRAD_INTERRUPT_SOURCE Configures interrupts related to digital receiver. 0
  //HARDMUTEIEN FICERRIEN ACQIEN RSSIHIEN RSSILIEN
  //{DAB_DIGRAD_INTERRUPT_SOURCE, 0},//default
  {DAB_DIGRAD_INTERRUPT_SOURCE, (1 << 4) | (1 << 3) | (1 << 2) | (1 << 1) | 1},

  //0xB001 DAB_DIGRAD_RSSI_HIGH_THRESHOLD  sets the high threshold,which triggers the DIGRAD interrupt if the RSSI is above this threshold. 127
  {DAB_DIGRAD_RSSI_HIGH_THRESHOLD, 127},//default

  //0xB002 DAB_DIGRAD_RSSI_LOW_THRESHOLD  sets the low threshold, which triggers the DIGRAD interrupt if the RSSI is below this threshold. -128
  //(unsigned short)warning: narrowing conversion of '-128' from 'int' to 'short unsigned int' inside { } [-Wnarrowing]
  //0xff80 = -128
  {DAB_DIGRAD_RSSI_LOW_THRESHOLD, 0xFF80},//default

  //0xB200 DAB_VALID_RSSI_TIME Sets the time to allow the RSSI metric to settle before evaluating. 30
  //validation time in ms. If set to 0, then RSSI is not considered when determining a channel's validity. Range: 0-63
  //{DAB_VALID_RSSI_TIME, 30},//default
  {DAB_VALID_RSSI_TIME, 63},

  //0xB201 DAB_VALID_RSSI_THRESHOLD Sets the RSSI threshold for a valid DAB Seek/Tune 12
  //If the desired channel RSSI is above this threshold, then it is considered valid. The CTS bit (and optional interrupt) is set when it is safe to send
  //the next command. This property may only be set or read in powerup mode. Default: 12 Units: dBuV. Range: -128 to 127
  {DAB_VALID_RSSI_THRESHOLD, 12},//default

  //0xB202 DAB_VALID_ACQ_TIME Set the time to wait for acquisition before evaluating 2000
  //validation time in ms. If 0, then acquisition is not considered when determining a channel's validity. Range: 0-2047
  //{DAB_VALID_ACQ_TIME, 500},
  //{DAB_VALID_ACQ_TIME, 1000},
  {DAB_VALID_ACQ_TIME, 1500},
  //{DAB_VALID_ACQ_TIME, 2000},//default

  //0xB203 DAB_VALID_SYNC_TIME Sets the time to wait for ensemble synchronization. 1200
  //validation time in ms. If set to 0, then synchronization is not considered when determining a channel's validity. Range: 0-2047
  //{DAB_VALID_SYNC_TIME, 0},//not used
  //{DAB_VALID_SYNC_TIME, 500},
  {DAB_VALID_SYNC_TIME, 1200},//default
  //{DAB_VALID_SYNC_TIME, 2047},//max

  //0xB204 DAB_VALID_DETECT_TIME Sets the time to wait for ensemble fast signal detection. 35 ms
  //validation time in ms. If set to 0, then fast detect is not considered when determining a channel's validity. Range: 0-2047
  //{DAB_VALID_DETECT_TIME, 35},//default
  {DAB_VALID_DETECT_TIME, 300},
  //{DAB_VALID_DETECT_TIME, 2047},//max

  //0xB300 DAB_EVENT_INTERRUPT_SOURCE Configures which dab events will set the DEVENTINT status bit. 0
  {DAB_EVENT_INTERRUPT_SOURCE, 0},//default
  //0xB301 DAB_EVENT_MIN_SVRLIST_PERIOD Configures how often service list notifications can occur. 10
  {DAB_EVENT_MIN_SVRLIST_PERIOD, 10},//default
  //0xB302 DAB_EVENT_MIN_SVRLIST_PERIOD_RECONFIG Configures how often service list notifications can occur during reconfiguration events. 10
  {DAB_EVENT_MIN_SVRLIST_PERIOD_RECONFIG, 10},//default
  //0xB303 DAB_EVENT_MIN_FREQINFO_PERIOD Configures how often frequency information notifications can occur. 5
  {DAB_EVENT_MIN_FREQINFO_PERIOD, 5},//default

  //0xB400 DAB_XPAD_ENABLE Selects which XPAD data will forwarded to the host. 1
  //15:3 Reserved Always write to 0x0000.
  //2 TDC_ENABLE Enables XPAD delivered TDC data.
  //1 MOT_ENABLE Enables XPAD delivered MOT objects.
  //0 DLS_ENABLE Enables PAD delivered DLS packets.
  {DAB_XPAD_ENABLE, 0 << 2 | 0 << 1 | 1},//default
  //{DAB_XPAD_ENABLE, 1 << 2 | 1 << 1 | 1},

  //0xB401 DAB_DRC_OPTION Select DRC (dynamic range control) option.
  //  0 : DRC is off. Do not apply additional gain to the audio.
  //  1 : Apply 1/2 gain value specified by the PAD data to the audio.
  //  2 : Apply full gain value specified by the PAD data to the audio.
  //  3 : RESERVED. Do not use.
  {DAB_DRC_OPTION, 0},//default

  //0xB500 DAB_CTRL_DAB_MUTE_ENABLE enables the feature of hard muting audio when signal level is low 1
  {DAB_CTRL_DAB_MUTE_ENABLE, 1},//default

  //0xB501 DAB_CTRL_DAB_MUTE_SIGNAL_LEVEL_THRESHOLD Set the signal quality threshold, which is evaluated by fic_quality. 98
  {DAB_CTRL_DAB_MUTE_SIGNAL_LEVEL_THRESHOLD, 98},//default

  //0xB502 DAB_CTRL_DAB_MUTE_WIN_THRESHOLD Set the time window threshold (in milliseconds)to mute audio 1000
  //{DAB_CTRL_DAB_MUTE_WIN_THRESHOLD, 1000},//default
  {DAB_CTRL_DAB_MUTE_WIN_THRESHOLD, 2000},

  //0xB503 DAB_CTRL_DAB_UNMUTE_WIN_THRESHOLD Set the time window threshold (in milliseconds) to unmute audio 1500
  {DAB_CTRL_DAB_UNMUTE_WIN_THRESHOLD, 500},
  //{DAB_CTRL_DAB_UNMUTE_WIN_THRESHOLD, 1500},//default

  //0xB504 DAB_CTRL_DAB_MUTE_SIGLOSS_THRESHOLD Set the signal RSSI threshold to mute audio. RSSI below this threshold indicates that signal is lost. In this case, audio will be muted.6
  {DAB_CTRL_DAB_MUTE_SIGLOSS_THRESHOLD, 6},//default

  //0xB505 DAB_CTRL_DAB_MUTE_SIGLOW_THRESHOLD Set the signal SNR threshold. The fic_quality based audio mute operation only engages when signal SNR is below this threshold. 9
  {DAB_CTRL_DAB_MUTE_SIGLOW_THRESHOLD, 3},
  //{DAB_CTRL_DAB_MUTE_SIGLOW_THRESHOLD, 9},//default

  //0xB700 DAB_ANNOUNCEMENT_ENABLE enables announcement types.Default: 0x07FF
  //RESERVED5 RESERVED4 RESERVED3 RESERVED2 RESERVED1 FINANCIAL SPORT PROGRAM SPECIAL EVENT WEATHER NEWS WARNING TRANSPORT TRAFFIC ALARM
  {DAB_ANNOUNCEMENT_ENABLE, 0x07FF}//default

};


void dabBegin()
{
  loadFirmware(addrFirmwareDab, sizeFirmwareDab);//DAB Firmware

  //Print device status information
  //serialPrintSi468x::devicePrintStatus(deviceGetStatus());
  //Boot device
  boot();

  statusRegister_t statusRegister;
  readStatusRegister(statusRegister);

  //Print device status information
  serialPrintSi468x::printStatusRegister(statusRegister);

  //Set device properties
  writePropertyValueList(propertyValueListDevice, NUM_PROPERTIES_DEVICE);

  //Print system state
  serialPrintSi468x::printSystemState(readSystemState());


  //Set DAB properties
  writePropertyValueList(propertyValueListDab, NUM_PROPERTIES_DAB);

  //Tunes DAB inital index
  tuneIndex(dabIndex);
  //Starts inital audio service
  startService(serviceId, componentId);

  //readServiceInformation(dabServiceInformation, serviceId);
  //writeTextField(dabServiceInformation.serviceLabel);
}

//0x81
void startService(const unsigned long &serviceId, const unsigned long &componentId, const unsigned char serviceType)
{
  unsigned char buf[4] = {0xff, 0xff, 0xff, 0xff};

  unsigned char cmd[1];
  cmd[0] = START_DIGITAL_SERVICE;

  unsigned char arg[11];
  arg[0] = serviceType & 1;
  arg[1] = 0x00;
  arg[2] = 0x00;
  arg[3] = serviceId & 0xFF;
  arg[4] = serviceId >> 8 & 0xFF;
  arg[5] = serviceId >> 16 & 0xFF;
  arg[6] = serviceId >> 24 & 0xFF;
  arg[7] = componentId & 0xFF;
  arg[8] = componentId >> 8 & 0xFF;
  arg[9] = componentId >> 16 & 0xFF;
  arg[10] = componentId >> 24 & 0xFF;

  writeCommandArgument(cmd, sizeof(cmd), arg, sizeof(arg));
  for (uint8_t j = 0; j < 10; j++) delayMicroseconds(DURATION_STOP_START_SERVICE);
  readReply(buf, sizeof(buf));
}

void stopService(const unsigned long &serviceId, const unsigned long &componentId, const unsigned char serviceType)
{
  unsigned char buf[4] = {0xff, 0xff, 0xff, 0xff};

  unsigned char cmd[1];
  cmd[0] = STOP_DIGITAL_SERVICE;

  unsigned char arg[11];
  arg[0] = serviceType & 1;
  arg[1] = 0x00;
  arg[2] = 0x00;
  arg[3] = serviceId & 0xFF;
  arg[4] = serviceId >> 8 & 0xFF;
  arg[5] = serviceId >> 16 & 0xFF;
  arg[6] = serviceId >> 24 & 0xFF;
  arg[7] = componentId & 0xFF;
  arg[8] = componentId >> 8 & 0xFF;
  arg[9] = componentId >> 16 & 0xFF;
  arg[10] = componentId >> 24 & 0xFF;

  writeCommandArgument(cmd, sizeof(cmd), arg, sizeof(arg));
  delayMicroseconds(DURATION_STOP_START_SERVICE);
  readReply(buf, sizeof(buf));
}

//0x84 GET_DIGITAL_SERVICE_DATA Gets a block of data associated with one of the enabled data components of a digital services*/
void readServiceData(serviceData_t& serviceData, unsigned char statusOnly, unsigned char ack)
{
  /*
    GET_DIGITAL_SERVICE_DATA gets a block of data associated with one of the enabled data components of a
    digital service. Information about this block of data is found in the data header that is returned at the beginning of
    the data block. In order to determine the ideal number of PAYLOAD bytes to read, the header information can be
    read first followed by a second read of the full (header + PAYLOAD) length - it is unnecessary to call
    GET_DIGITAL_SERVICE_DATA twice to use this method. The data associated with this transaction will be
    discarded at the receipt of a next GET_DIGITAL_SERVICE_DATA command if STATUS_ONLY = 0. Reading past
    the end of the buffer will result in zeros for the invalid bytes. Please refer to iBiquity document: SY_IDD_1019s Rev
    F (sections 5 and 6) for the format of the HD Radio data service data. The command is complete when the CTS bit
    (and optional interrupt) is set. The ERR bit (and optional interrupt) is set if an invalid argument is sent. Note that
    only a single interrupt occurs if both the CTS and ERR bits are set. The command may only be sent in powerup
    mode.
    Maximum DSRV queue depth on the Si468x is 8
  */

  //read statusRegister to analyze dsrvINT
  //if DSRVINT == 0 return else go on
  statusRegister_t statusRegister;
  readStatusRegister(statusRegister);

  if (statusRegister.dsrvInt == 0)
  {
    return;
  }

  unsigned char buf[24];
  //initalize
  for (unsigned short i = 0; i < 24; i++) buf[i] = 0xff;

  unsigned char cmd[2];
  cmd[0]  = GET_DIGITAL_SERVICE_DATA;
  cmd[1] = ((statusOnly  & 1 ) << 4) | (ack & 1);

  writeCommand(cmd, sizeof(cmd));

  delayMicroseconds(10000);
  readReply(buf, sizeof(buf));

  serviceData.errorInterrupt     =  buf[4] >> 2 & 1;
  serviceData.overflowInterrupt  =  buf[4] >> 1 & 1;
  serviceData.packetInterrupt    =  buf[4] & 1;
  serviceData.bufferCount        =  buf[5];
  serviceData.statusService      =  buf[6];//3 : Indicates the this data packet represents the beginning of a new data object.
  serviceData.dataSource         =  buf[7] >> 6 & 3;//2 : Indicates that the payload is DLS PAD and DATA_TYPE is 0.
  serviceData.dataType           =  buf[7] & 0x3F;
  serviceData.serviceId          =  (unsigned long) buf[8] | (unsigned long) buf[9] << 8 | (unsigned long) buf[10] << 16 | (unsigned long) buf[11] << 24;
  serviceData.componentId        =  (unsigned long) buf[12] | (unsigned long) buf[13] << 8 | (unsigned long) buf[14] << 16 | (unsigned long) buf[15] << 24;
  //serviceData.rfu              =  buf[16] | buf[17] << 8;
  serviceData.dataLength         =  buf[18] | buf[19] << 8;
  serviceData.segmentNumber      =  buf[20] | buf[21] << 8;
  serviceData.numberSegments     =  buf[22] | buf[23] << 8;

  //nothing in buffer, length = 0, No DLS or DLS+
  if (serviceData.bufferCount == 0 || serviceData.dataLength == 0)
  {
    return;
  }
  //No DLS or DLS+
  //if (serviceData.dataSource != 2) return serviceData;

  //Create buffer with 24 + payload
  unsigned char payloadBuffer[24 + serviceData.dataLength];

  //initalize buffer
  for (unsigned short i = 0; i < (24 + serviceData.dataLength); i++) payloadBuffer[i] = 0xff;

  readReply(payloadBuffer, sizeof(payloadBuffer));

  //For DLS and DLS+ a two byte prefix is added to the payload.

  //Toggle[7] RFU[6:5] C[4] (Field 1) C=1, Command[3:0]/ C=0, 0
  //(Field 2) C=1, Link[4]/C=0 Charset[7:4] RFU[3:0]

  struct prefixDls_t
  {
    unsigned char toggle:  1;
    //unsigned char rfu:   2;
    unsigned char c:       1;//DL = 0, DL Plus = 1
    unsigned char command: 4;//b0001 remove label from display, b0010 DL Plus

    unsigned char charSet: 4;
    //unsigned char rfu2:  4;
  } prefixDls;

  prefixDls.toggle  = payloadBuffer[24] >> 7 & 1;
  //prefixDls.rfu   = payloadBuffer[24] >> 5 & 3;
  prefixDls.c       = payloadBuffer[24] >> 3 & 1;
  prefixDls.command = payloadBuffer[24] & 0xF;

  prefixDls.charSet = payloadBuffer[25] >> 4 & 0xF;
  //prefixDls.rfu2    = payloadBuffer[25] & 0xF;

  //Serial.println(prefixDls.toggle, BIN);
  //Serial.println(prefixDls.c, BIN);
  //Serial.println(prefixDls.command, BIN);
  //Serial.println(prefixDls.charSet, BIN);
  //Serial.println();
  //Serial.println(payloadBuffer[24], BIN);
  //Serial.println(payloadBuffer[25], BIN);

  //Dynamic Label
  if (prefixDls.c == 0 && prefixDls.command != 0b0010)
  {
    //Create dls string
    char dls[serviceData.dataLength];
    memcpy(dls, &payloadBuffer[26], serviceData.dataLength); // Bytes 26...dataLength
    /*label terminated by '/0'*/
    //dabEnsembleInfo.label[serviceData.dataLength+1] = '\0';

    serialPrintSi468x::dabPrintDynamicLabelSegment(dls);
  }

  //Dynamic Label+
  else if (prefixDls.c == 1 && prefixDls.command == 0b0010)
  {
    //Create dls string
    char tags[serviceData.dataLength];
    Serial.println(F("DLS+:"));
    Serial.println(payloadBuffer[26]);

    memcpy(tags, &payloadBuffer[27], serviceData.dataLength); // Bytes 27...dataLength
    serialPrintSi468x::dabPrintDynamicLabelSegment(tags);
  }

  /*
     Application type Description
    0 End marker
    1 Data group length indicator
    2 Dynamic label segment, start of X-PAD data group
    3 Dynamic label segment, continuation of X-PAD data group
    4 to 11 User defined
    12 MOT, start of X-PAD data group, see ETSI EN 301 234 [6]
    13 MOT, continuation of X-PAD data group, see ETSI EN 301 234 [6]
    14 MOT, start of CA messages, see ETSI EN 301 234 [6]
    15 MOT, continuation of CA messages, see ETSI EN 301 234 [6]
    16 to 30 User defined
    31 Not used
  */

}

//Get ensemble header
void getEnsembleHeader(ensembleHeader_t &ensembleHeader, unsigned char serviceType)
{
  uint8_t cmd[2];
  cmd[0] = GET_DIGITAL_SERVICE_LIST;
  cmd[1] = serviceType & 1;

  //12 bytes = 4 status bytes 0...3, 2 list size, 2 version, 1 number of services, 3 align pads
  //+24 bytes first service
  //+4 bytes first component
  //uint16_t len = 12 + 24 + 4 + 24 + 4;
  uint16_t len = 12 + 24 + 4;

  uint8_t buf[len];
  for (uint8_t i = 0; i < len; i++) buf[i] = 0xff;

  writeCommand(cmd, sizeof(cmd));
  delayMicroseconds(10000);
  readReply(buf, sizeof(buf));

  //Indicates the number of bytes in the digital service list (Max = 2047 bytes, not including list size of 2 bytes)
  ensembleHeader.listSize = (uint16_t) buf[5] << 8 | buf[4];
  ensembleHeader.version = (uint16_t)buf[7] << 8 | buf[6];
  ensembleHeader.numServices = buf[8];

  //Align Pad 3 Bytes reserved for future use
  //ensembleHeader.reserved1 = buf[9];
  //ensembleHeader.reserved2 = buf[10];
  //ensembleHeader.reserved3 = buf[11];

  //Valid? Check if list exists or too big
  if (ensembleHeader.listSize == 0 || ensembleHeader.listSize > 2694 || ensembleHeader.numServices > 32)
  {
    //ensembleHeader.actualService = 0;
    //ensembleHeader.actualComponent = 0;
    ensembleHeader.listSize = 0;
    ensembleHeader.version = 0;
    ensembleHeader.numServices = 0;
    //ensembleHeader.serviceList = nullptr;
  }

}

//Get ensemble an fill serviceList and componentList
void getEnsemble(ensembleHeader_t& ensembleHeader, uint8_t serviceType)
{
  //free memory from previous Ensemble List Data Structure
  freeMemoryFromEnsembleList(ensembleHeader);

  //Read Header
  getEnsembleHeader(ensembleHeader, serviceType);

  //Check services
  if (ensembleHeader.numServices == 0 )
  {
    Serial.print(F("No services"));
    return;
  }

  //Create dynamic array of right size for serviceList
  //ensembleHeader.serviceList = (serviceList_t*) calloc (ensembleHeader.numServices, sizeof(serviceList_t));
  ensembleHeader.serviceList = new serviceList_t[ensembleHeader.numServices];

  //No memory left
  if (ensembleHeader.serviceList == nullptr)
  {
    freeMemoryFromEnsembleList(ensembleHeader);
    Serial.print(F("No memory left"));
    return;
  }

  //Read service list
  //Read again response with readReplyOffset
  uint8_t cmd[2];
  cmd[0] = GET_DIGITAL_SERVICE_LIST;
  cmd[1] = serviceType & 1;

  writeCommand(cmd, sizeof(cmd));
  delayMicroseconds(10000);
  delayMicroseconds(10000);

  //OFFSET parameter must be modulo four
  //Listheader 8 Byte
  uint16_t offset = 8;

  //buffer for next service 24 Bytes per service + 4 byte status
  uint8_t service[24 + 4];
  for (uint8_t i = 0; i < 24 + 4; i++) service[i] = 0xff;

  //Cycle services
  for (uint8_t i = 0; i < ensembleHeader.numServices && i < MAX_NUMBER_SERVICES; i++)
  {
    readReplyOffset(service, sizeof(service), offset);

    //Byte [8]
    //Service Info 1
    //ensembleHeader.serviceList[i].reservedServiceInfo1   = service[8] >> 7 & 1;  //RFU
    //ensembleHeader.serviceList[i].serviceLinkingFlag    = service[8] >> 6 & 1;
    //ensembleHeader.serviceList[i].programType           = service[8] >> 1 & 0x1F;
    ensembleHeader.serviceList[i].dataFlag              = service[8] & 1;

    ensembleHeader.serviceList[i].serviceId = (uint32_t)service[7] << 24 | (uint32_t)service[6] << 16 | (uint32_t)service[5] << 8 | service[4];

    /*
        if (ensembleHeader.serviceList[i].dataFlag == 0) // Dataflag = 0 Audioprogram
        {
          //Bytes [4:7]
          ensembleHeader.serviceList[i].serviceReference = (service[5] & 0b1111) | service[4];
          ensembleHeader.serviceList[i].countryId = (service[5] >> 4 ) & 0b1111;
        }

        if (ensembleHeader.serviceList[i].dataFlag == 1) // Dataflag = 1
        {
          //Bytes [4:7]
          ensembleHeader.serviceList[i].serviceReference = (service[6] & 0b11) << 16 | service[5] << 8 | service[4];
          ensembleHeader.serviceList[i].countryId = (service[6] >> 4 ) & 0b1111;
        }
    */

    //Byte[9]
    //Service Info 2
    //ensembleHeader.serviceList[i].localFlag             = service[9] >> 7 & 1;
    //ensembleHeader.serviceList[i].conditionalAccess     = service[9] >> 4 & 0x7;
    ensembleHeader.serviceList[i].numComponents         = service[9] & 0x0F;

    //Byte [10]
    //Service Info 3
    // ensembleHeader.serviceList[i].reservedServiceInfo3 = service[10] >> 4 & 0x0F;
    //ensembleHeader.serviceList[i].characterSet          = service[10] & 0x0F;

    //Byte [11]
    //Align Pad 1

    //Byte[12:28]
    //Service Label
    //memcpy(ensembleHeader.serviceList[i].serviceLabel, &service[12], 16);
    //Terminate string
    //ensembleHeader.serviceList[i].serviceLabel[16] = '\0';

    //Create dynamic array of right size for componentList
    //ensembleHeader.serviceList[i].componentList = (componentList_t*) realloc(ensembleHeader.serviceList[i].componentList, ensembleHeader.serviceList[i].numComponents * sizeof(componentList_t));
    ensembleHeader.serviceList[i].componentList = new componentList_t[ensembleHeader.serviceList[i].numComponents];

    //No memory left
    if (ensembleHeader.serviceList[i].componentList == nullptr)
    {
      //free memory
      //free(ensembleHeader.serviceList[i].componentList);
      delete[] ensembleHeader.serviceList[i].componentList;
      //Set pointer to nullptr
      ensembleHeader.serviceList[i].componentList = nullptr;
      Serial.print(F("No memory left"));
      return;
    }

    //Components
    //Very memory intensive for Uno !
    //Dont exceed MAX_NUMBER_COMPONENTS

    //intial offset component
    uint16_t offsetComponent = offset + 24;//header 8 + service 24

    for (uint8_t j = 0; (j < ensembleHeader.serviceList[i].numComponents) && (j < MAX_NUMBER_COMPONENTS); j++)
    {
      //Buffer status 4 + component 4
      uint8_t component[4 + 4] = {0xff, 0xff, 0xff, 0xff, 0, 0, 0, 0};

      //24 bytes for one service + n Components * 4 Bytes
      offsetComponent = offsetComponent + j * (ensembleHeader.serviceList[i].numComponents * 4); //4 bytes per component

      readReplyOffset(component, sizeof(component), offsetComponent);

      //4 bytes per component
      ensembleHeader.serviceList[i].componentList[j].componentId = (uint16_t)component[5] << 8 | component[4];
      ensembleHeader.serviceList[i].componentList[j].conditionalAccessFlag = component[6] & 1;
      ensembleHeader.serviceList[i].componentList[j].secondaryFlag = component[6] >> 1 & 1;
      ensembleHeader.serviceList[i].componentList[j].serviceType = component[6] >> 2 & 0x7F;
      ensembleHeader.serviceList[i].componentList[j].validFlag   = component[7] & 1;

      /*
        //Debugging
            Serial.print(F("Service: "));
            Serial.print(i);
            Serial.print(F("\tId:\t"));
            Serial.print(ensembleHeader.serviceList[i].serviceId, HEX);
            Serial.print(F("\tOffset:\t"));
            Serial.println(offset);
            Serial.print(F("Num Components:\t"));
            Serial.println(ensembleHeader.serviceList[i].numComponents);

            Serial.print(F("Component:\t"));
            Serial.print(j);
            Serial.print(F("\tId:\t"));
            Serial.print(ensembleHeader.serviceList[i].componentList[j].componentId, HEX);
            Serial.print(F("\toffset:\t"));
            Serial.println(offsetComponent);
            //Serial.println(ensembleHeader.serviceList[i].serviceLabel);

            Serial.print(F("Free RAM: "));
            Serial.println(freeRam());
            Serial.println();
      */

    }//component cycle
    //New offset
    //24 bytes for one service + n Components * 4 Bytes
    offset = offset + (24 + ensembleHeader.serviceList[i].numComponents * 4); //4 bytes per component
  }//service cycle

}

//Free memory from ensembleList data structures
void freeMemoryFromEnsembleList(ensembleHeader_t &ensembleHeader)
{
  //ensemble empty
  if (ensembleHeader.numServices == 0 || ensembleHeader.serviceList == nullptr)
  {
    Serial.println(F("numServices = 0"));
    return;
  }
  else
  {
    Serial.print(F("numServices = "));
    Serial.println(ensembleHeader.numServices);
  }

  //cycle numServices
  for (uint8_t i = 0; i < ensembleHeader.numServices; i++)
  {
    //pointer to componentList is available
    if (ensembleHeader.serviceList[i].componentList != nullptr)
    {
      delete[] ensembleHeader.serviceList[i].componentList;
      Serial.print(F("Delete componentList of service:\t"));
      Serial.println(i);
    }
    //componentList deleted
    //Set pointer to component list to nullptr
    ensembleHeader.serviceList[i].componentList = nullptr;
  }
  //pointer to serviceList is available
  if (ensembleHeader.serviceList != nullptr)
  {
    Serial.print(F("Delete serviceList\t"));
    delete[] ensembleHeader.serviceList;
  }
  //Set pointer to service list to nullptr
  ensembleHeader.serviceList = nullptr;

  Serial.println(F("serviceList deleted"));
  Serial.println();
}

//Search service and component in servicelist
bool searchService(unsigned long &serviceId, unsigned long &componentId)
{
  bool found = false;

  //no services
  if (ensembleHeader.numServices == 0)
  {
    //parse ensemble
    getEnsemble(ensembleHeader);
    //nothing found return
    if (ensembleHeader.numServices == 0)  return false;
  }

  //search all services in servicelist
  for (uint8_t i = 0; i < ensembleHeader.listSize; i++)
  {
    //found ?
    if (ensembleHeader.serviceList[i].serviceId == serviceId)
    {
      //remember
      ensembleHeader.actualService = i;

#ifdef DEBUG_PARSE_ENSEMBLE
      Serial.println(i);
      Serial.println(serviceId, HEX);
      Serial.println(ensembleHeader.actualService);
      Serial.println();
#endif //DEBUG_PARSE_ENSEMBLE

      found = true;
      break;
    }
  }

  //not found stop and return
  if (found == false) return found;

  //search all components in componentList
  for (uint8_t j = 0; j < ensembleHeader.serviceList[ensembleHeader.actualService].numComponents; j++)
  {
    //found ?
    if (ensembleHeader.serviceList[ensembleHeader.actualService].componentList[j].componentId == componentId)
    {
      //remember
      ensembleHeader.actualComponent = j;
#ifdef DEBUG_PARSE_ENSEMBLE
      Serial.println(j);
      Serial.println(componentId, HEX);
      Serial.println(ensembleHeader.actualComponent);
      Serial.println();
#endif //DEBUG_PARSE_ENSEMBLE
      found = true;
      break;
    }
  }

  return found;
}

//Start next service in ensemble
void nextService(unsigned long &serviceId, unsigned long &componentId)
{
  //no ensemble
  if (ensembleHeader.numServices == 0)
  {
    //parse ensemble
    getEnsemble(ensembleHeader);
    //no reception, no service list ready return
    if (ensembleHeader.numServices == 0)
    {
      Serial.println(F("No service"));
      return;
    }
  }

  //lookup in ensemble because of changed frequency
  bool found = searchService(serviceId, componentId);

  //Found in ensemble
  if (found)
  {
    //Wrap around
    if (ensembleHeader.actualService == ensembleHeader.numServices - 1)
      ensembleHeader.actualService = 0;
    else  ensembleHeader.actualService++;

    //global var
    componentId = ensembleHeader.serviceList[ensembleHeader.actualService].componentList[0].componentId;
    serviceId = ensembleHeader.serviceList[ensembleHeader.actualService].serviceId;

    startService(serviceId, componentId);
  }
}

//Start previous service in ensemble
void previousService(unsigned long &serviceId, unsigned long &componentId)
{
  //no ensemble
  if (ensembleHeader.numServices == 0)
  {
    //parse ensemble
    getEnsemble(ensembleHeader);
    //no reception, no service list ready return
    if (ensembleHeader.numServices == 0)
    {
      Serial.println(F("No service"));
      return;
    }
  }

  //lookup in ensemble because of changed frequency
  bool found = searchService(serviceId, componentId);

  //Found in ensemble
  if (found)
  {
    //Wrap around
    if (ensembleHeader.actualService == 0)
      ensembleHeader.actualService = ensembleHeader.numServices;
    else ensembleHeader.actualService--;

    //global var
    componentId = ensembleHeader.serviceList[ensembleHeader.actualService].componentList[0].componentId;
    serviceId = ensembleHeader.serviceList[ensembleHeader.actualService].serviceId;

    startService(serviceId, componentId);
  }
}

//Start first service (0 = audio) in ensemble
void startFirstService(unsigned long &serviceId, unsigned long &componentId, uint8_t serviceType)
{

  eventInformation_t eventInformation;

  //wait for serviceList
  for (unsigned char retry = 0; retry < MAX_RETRY; retry++)
  {
    //longer delay
    for (unsigned char i = 0; i < MAX_RETRY; i++)
    {
      delayMicroseconds(10000);
      delayMicroseconds(10000);
      delayMicroseconds(10000);
    }

    readEventInformation(eventInformation);

    if (eventInformation.serviceListAvailable == 1)
    {
      break;
    }
    //too many tries
    if (retry == MAX_RETRY - 1)
    {
      Serial.println(F("Error ServList"));
      return;
    }
  }

  //Parse servicelist
  /*
    if (ensembleHeader.numServices == 0 || ensembleHeader.serviceList == nullptr)
    {
    Serial.println(F("Parse ServList"));
    //parse ensemble
    getEnsemble(ensembleHeader);
    //nothing found return
    if (ensembleHeader.numServices == 0)  return;
    }
  */  Serial.println(F("Parse ServList"));
  //parse ensemble every time

  getEnsemble(ensembleHeader);

  //there is an ensemble with serviceList and numServices
  if (ensembleHeader.numServices != 0 && ensembleHeader.serviceList != nullptr)
  {
    //cycle trough numServices
    for (uint8_t i = 0; i < ensembleHeader.numServices; i++)
    {
      //Serial.println(ensembleHeader.serviceList[i].serviceId, HEX);
      //Serial.println(ensembleHeader.serviceList[i].componentList[i].componentId, HEX);
      //Serial.println(ensembleHeader.serviceList[i].dataFlag);

      //check if an audio service, serviceType = 0
      if (ensembleHeader.serviceList[i].dataFlag == serviceType)
      {
        //set members
        serviceId = ensembleHeader.serviceList[i].serviceId;
        componentId = ensembleHeader.serviceList[i].componentList[0].componentId;
        //start
        startService(serviceId, componentId);
        break;
      }
    }
  }

  else
  {
    //not parsed
    return;
  }
}

//0xB0 Tunes to frequency index
void tuneIndex(unsigned char index, unsigned short varCap, unsigned char injection)
{
  /*
    Tunes the DAB Receiver to a frequency between 168.16 and 239.20 MHz defined by the frequency table
    through DAB_SET_FREQ_LIST.
    INJECTION[1:0] Injection selection  0 : Automatic injection selection 1 : Low-side injection 2 : High-side injection
    FREQ_INDEX[7:0] Frequency index for the tuned frequency, see the DAB_SET_FREQ_LIST command that sets the frequency table
    ANTCAP[15:0] Antenna tuning capacitor value in 250 fF units (31.75 pF Max) Range: 0-128
    0 : Automatically determines the cap setting
  */

  //Validity index 0...47
  if (index >= MAX_INDEX) index = MAX_INDEX - 1;

  unsigned char buf[4] = {0xff, 0xff, 0xff, 0xff};

  unsigned char cmd[6];
  cmd[0] = DAB_TUNE_FREQ;
  cmd[1] = injection & 3;
  cmd[2] = index;
  cmd[3] = 0;
  cmd[4] = varCap & 0xFF;
  cmd[5] = varCap >> 8;

  writeCommand(cmd, sizeof(cmd));

  //STC ? 600ms = 60 * 10000us
  for (uint8_t i = 0; i < 10; i++)
  {
    //wait 30*DURATION_TUNE
    for (uint8_t j = 0; j < 30; j++) delayMicroseconds(DURATION_TUNE);

    readReply(buf, sizeof(buf));
    Serial.print('.');
    if ((buf[0] & 1) == 1)
    {
      for (uint8_t j = 0; j < 30; j++) delayMicroseconds(DURATION_TUNE);
      break;
    }
  }
  Serial.println();

}

//0xB2 DAB_DIGRAD_STATUS Get status information about the received signal quality
void readRsqInformation(rsqInformation_t& rsqInformation, unsigned char clearDigradInterrupt, unsigned char rssiAtTune, unsigned char clearStcInterrupt)
{
  unsigned char cmd[2];
  cmd[0] = DAB_DIGRAD_STATUS;
  cmd[1] = ((clearDigradInterrupt & 1) << 3) | ((rssiAtTune & 1) << 2) | (clearStcInterrupt & 1);

  unsigned char buf[23];
  //initalize buffer
  for (unsigned char i = 0; i < 23; i++) buf[i] = 0xff;

  writeCommand(cmd, sizeof(cmd));
  delayMicroseconds(10000);
  readReply(buf, sizeof(buf));

  rsqInformation.hardMuteInterrupt =  buf[4] >> 4 & 1;
  rsqInformation.ficErrorInterrupt =  buf[4] >> 3 & 1;
  rsqInformation.acqInterrupt =       buf[4] >> 2 & 1;
  rsqInformation.rssiHighInterrupt =  buf[4] >> 1 & 1;
  rsqInformation.rssiLowInterrupt =   buf[4] & 1;

  rsqInformation.hardmute = buf[5] >> 4 & 1;
  rsqInformation.ficError = buf[5] >> 3 & 1;
  rsqInformation.acq =      buf[5] >> 2 & 1;
  rsqInformation.valid =    buf[5] & 1;

  rsqInformation.rssi        = buf[6]; //Received signal strength indicator. Range: -128-63
  rsqInformation.snr         = buf[7];
  rsqInformation.ficQuality  = buf[8];
  rsqInformation.cnr         = buf[9];
  rsqInformation.fibErrorCount = (unsigned short) buf[11] << 8 | buf[10];

  rsqInformation.frequency   = (unsigned long)((buf[12] | (unsigned long)buf[13] << 8 | (unsigned long)buf[14] << 16 | (unsigned long)buf[15] << 24));
  rsqInformation.index       = buf[16];
  rsqInformation.fftOffset   = buf[17];
  rsqInformation.varactorCap = (unsigned short) buf[19] << 8 | buf[18];
  rsqInformation.cuLevel     = (int) buf[21] << 8 | buf[20]; // 0-470
  rsqInformation.fastDect    = buf[22];
}

//0xB3 DAB_GET_EVENT_STATUS Gets information about the various events related to the DAB radio
void readEventInformation(eventInformation_t& eventInformation, unsigned char eventAck)
{
  unsigned char cmd[2];
  cmd[0] = DAB_GET_EVENT_STATUS;
  cmd[1] = eventAck & 1;

  unsigned char buf[8] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

  writeCommand(cmd, sizeof(cmd));
  delayMicroseconds(10000);
  readReply(buf, sizeof(buf));

  eventInformation.ensembleReconfigInterrupt        = buf[4] >> 7 & 1;
  eventInformation.ensembleReconfigWarningInterrupt = buf[4] >> 6 & 1;

  eventInformation.announcementInterrupt            = buf[4] >> 4 & 1;
  eventInformation.otherServiceInterrupt            = buf[4] >> 3 & 1;
  eventInformation.serviceLinkingInterrupt          = buf[4] >> 2 & 1;
  eventInformation.frequencyInterrupt               = buf[4] >> 1 & 1;
  eventInformation.serviceListInterrupt             = buf[4] & 1;

  eventInformation.announcementAvailable            = buf[5] >> 4 & 1;
  eventInformation.otherServiceAvailable            = buf[5] >> 3 & 1;
  eventInformation.serviceLinkingAvailable          = buf[5] >> 2 & 1;
  eventInformation.frequencyAvailable               = buf[5] >> 1 & 1;
  eventInformation.serviceListAvailable             = buf[5] & 1;

  eventInformation.currentServiceListVersion         = (unsigned short)buf[7] << 8 | buf[6];
}

//0xB4 DAB_GET_ENSEMBLE_INFO Gets information about the current ensemble
void readEnsembleInformation(ensembleInformation_t& ensembleInformation)
{
  unsigned char cmd[2] = {DAB_GET_ENSEMBLE_INFO, 0};

  unsigned char buf[26];
  for (unsigned short i = 0; i < 26; i++) buf[i] = 0xff;

  writeCommand(cmd, sizeof(cmd));
  delayMicroseconds(10000);
  readReply(buf, sizeof(buf));

  ensembleInformation.ensembleId        = (unsigned short)buf[5] << 8 | buf[4];
  //16 characters for the ensemble label terminated by '/0'
  memcpy(ensembleInformation.label, &buf[6], 16); //buf[6]....buf[21]
  ensembleInformation.label[16] = '\0';
  ensembleInformation.ecc               = buf[22];
  ensembleInformation.charSet           = buf[23];
  ensembleInformation.abbreviationMask  = (unsigned short)buf[25] << 8 | buf[24];
}

//0xB7 DAB_GET_SERVICE_LINKING_INFO Provides service linking info for the passed in service ID
void readServiceLinkingInfo(serviceLinkingInformation_t& serviceLinkingInformation, unsigned long &serviceId)
{
  unsigned char cmd[8];

  cmd[0] = DAB_GET_SERVICE_LINKING_INFO;
  cmd[1] = 0;//arg
  cmd[2] = 0;//arg
  cmd[3] = 0;//0x00
  cmd[4] = serviceId & 0xFF;//serviceId
  cmd[5] = serviceId >> 8 & 0xFF;//serviceId
  cmd[6] = serviceId >> 16 & 0xFF;//serviceId
  cmd[7] = serviceId >> 24 & 0xFF;//serviceId

  unsigned char bufferSize = 11;
  unsigned char buf[bufferSize];
  for (unsigned short i = 0; i < bufferSize; i++) buf[i] = 0xff;

  writeCommand(cmd, sizeof(cmd));
  delayMicroseconds(10000);
  readReply(buf, sizeof(buf));

  serviceLinkingInformation.size                = (unsigned short) buf[5] << 8 | buf[4];
  serviceLinkingInformation.numLinkSets         = buf[6];
  serviceLinkingInformation.lsn                 = (unsigned short) buf[9] << 8 | buf[8];
  serviceLinkingInformation.activeFlag          = buf[10] >> 7;
  serviceLinkingInformation.shortHandFlag       = (buf[10] >> 6) & 1;
  serviceLinkingInformation.linkType            = (buf[10] >> 4) & 0x3;
  serviceLinkingInformation.hardLinkFlag        = (buf[10] >> 1) & 1;
  serviceLinkingInformation.internationalFlag   = buf[10] & 1;
}

//0xB8 DAB_SET_FREQ_LIST Set Frequency table
void writeFrequencyTable(const unsigned long frequencyTable[], const unsigned char numFreq)
{
  /*
    DAB_SET_FREQ_LIST command sets the DAB frequency table. The frequencies are in units of 1 kHz. The table
    can be populated with a single entry or a regional list (for example 5 or 6 entries). It is recommended to make the
    list regional to increase scanning speed. The command is complete when the CTS bit (and optional interrupt) is
    set. The ERR bit (and optional interrupt) is set if an invalid argument is sent. Note that only a single interrupt occurs
    if both the CTS and ERR bits are set. The command may only be sent in powerup mode.
  */

  uint8_t cmd[1] = {DAB_SET_FREQ_LIST};

  uint8_t arg[3 + 4 * numFreq]; //4 arg per frequency
  arg[0] = numFreq;
  arg[1] = 0;
  arg[2] = 0;
  //fill arg array with frequencies
  for (uint8_t i = 0; i < numFreq && i < MAX_INDEX; i++)
  {
    arg[3 + 4 * i] = frequencyTable[i] & 0xFF;
    arg[4 + 4 * i] = frequencyTable[i] >> 8 & 0xFF;
    arg[5 + 4 * i] = frequencyTable[i] >> 16 & 0xFF;
    arg[6 + 4 * i] = frequencyTable[i] >> 24 & 0xFF;
  }

  uint8_t buf[4] = {0xff, 0xff, 0xff, 0xff};

  writeCommandArgument(cmd, sizeof(cmd), arg, sizeof(arg));
  delayMicroseconds(10000);
  readReply(buf, sizeof(buf));
}

//0xB9 DAB_GET_FREQ_LIST Get frequency table
unsigned long* readFrequencyTable()
{
  //free memory from previous table
  if (frequencyTable != nullptr)
  {
    delete[]frequencyTable;
    frequencyTable = nullptr;
  }

  //read number of frequencies
  unsigned char numberFrequencies;
  readNumberFrequencies(numberFrequencies);

  //Dynamically allocate memory
  frequencyTable = new uint32_t[numberFrequencies];
  //Error
  if (frequencyTable == nullptr) return nullptr;

  uint8_t cmd[2]  = {DAB_GET_FREQ_LIST, 0};
  //4 Statusbytes + 4 Bytes from command DAB_GET_FREQ_LIST
  uint8_t freq[8] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
  writeCommand(cmd, sizeof(cmd));
  delayMicroseconds(10000);

  //Fill index table
  //Start reading 4 Bytes per frequency
  //OFFSET parameter must be modulo four
  uint16_t offset = 0;
  readReplyOffset(freq, sizeof(freq), offset);

  for (uint8_t i = 0; i < numberFrequencies; i++)
  {
    offset = offset + 4 ; //increase offset to next frequency
    readReplyOffset(freq, sizeof(freq), offset);

    frequencyTable[i] = (uint32_t) freq[7] << 24 | (uint32_t) freq[6] << 16 | (uint32_t) freq[5] << 8 | freq[4];

    //initalize for next frequency
    for (uint8_t j = 0;  j < 8; j++) freq[j] = 0xff;
  }

  return  frequencyTable;
}

//0xB9 DAB_GET_FREQ_LIST Read number of indices
void readNumberFrequencies(unsigned char& numberIndices)
{
  unsigned char cmd[2]  = {DAB_GET_FREQ_LIST, 0};
  unsigned char buf[12] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

  writeCommand(cmd, sizeof(cmd));
  delayMicroseconds(DURATION_10000_MIKROS);
  readReply(buf, sizeof(buf));

  //save number of frequencies in table
  numberIndices = buf[4];

  //Validity ?
  if (numberIndices > MAX_INDEX) numberIndices = 0;
}


//0xB9 DAB_GET_FREQ_LIST Get frequency table
void readFrequencyTable(frequencyTableHeader_t& frequencyTableHeader)
{
  //free memory from previous table
  if (frequencyTableHeader.table != nullptr)
  {
    delete[]frequencyTableHeader.table;
    frequencyTableHeader.table = nullptr;
    frequencyTableHeader.number = 0;
  }

  uint8_t cmd[2]  = {DAB_GET_FREQ_LIST, 0};
  //4 Statusbytes + 4 Bytes from command DAB_GET_FREQ_LIST
  uint8_t buf[8] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

  //read number of frequencies
  readNumberFrequencies(frequencyTableHeader.number);

  //Dynamically allocate memory
  frequencyTableHeader.table = new uint32_t[frequencyTableHeader.number];

  //Error
  if (frequencyTableHeader.table == nullptr)
  {
    frequencyTableHeader.number = 0;
    return nullptr;
  }


  writeCommand(cmd, sizeof(cmd));
  delayMicroseconds(DURATION_10000_MIKROS);

  //Fill index table
  //Start reading 4 Bytes per frequency
  //OFFSET parameter must be modulo four
  uint16_t offset = 0;
  //readReplyOffset(buf, sizeof(buf), offset);

  for (uint8_t i = 0; i < frequencyTableHeader.number; i++)
  {
    offset = offset + 4 ; //increase offset to next frequency
    readReplyOffset(buf, sizeof(buf), offset);
    delayMicroseconds(DURATION_10000_MIKROS);
    frequencyTableHeader.table[i] = (uint32_t) buf[7] << 24 | (uint32_t) buf[6] << 16 | (uint32_t) buf[5] << 8 | buf[4];

    //initalize for next frequency
    for (uint8_t j = 0;  j < 8; j++) buf[j] = 0xff;
  }

}

//0xBB DAB_GET_COMPONENT_INFO Get information about the component application data
void readComponentInformation(componentInformation_t &componentInformation, unsigned long &serviceId, unsigned long &componentId)
{
  //User application information provides signalling to allow data applications to be associated with the correct user
  //application decoder by the receiver.ETSI EN 300 401 V1.4.1 clause 8.1.20, Figure 68.

  delete[] componentInformation.userAppData;  // When done, free memory

  componentInformation.globalId             = 0;
  componentInformation.language             = 0;
  componentInformation.characterSet         = 0;
  memcpy(componentInformation.label, "No information ", 16); //[8]....[23]
  componentInformation.label[16] = '\0';
  componentInformation.abbreviationMask     = 0;
  componentInformation.numberUserAppTypes   = 0;
  componentInformation.lenTotal             = 0;
  componentInformation.userAppType          = 0;
  componentInformation.lenField             = 0;

  unsigned char buf[31];
  //initalize buffer
  for (unsigned short i = 0; i < 31; i++) buf[i] = 0xff;

  unsigned char cmd[12];
  cmd[0] = DAB_GET_COMPONENT_INFO;
  cmd[1] = 0x00;
  cmd[2] = 0x00;
  cmd[3] = 0x00;
  cmd[4] = serviceId & 0xFF;
  cmd[5] = serviceId >> 8 & 0xFF;
  cmd[6] = serviceId >> 16 & 0xFF;
  cmd[7] = serviceId >> 24 & 0xFF;
  cmd[8] = componentId & 0xFF;
  cmd[9] = componentId >> 8 & 0xFF;
  cmd[10] = componentId >> 16 & 0xFF;
  cmd[11] = componentId >> 24 & 0xFF;

  writeCommand(cmd, sizeof(cmd));
  delayMicroseconds(10000);
  delayMicroseconds(10000);
  readReply(buf, sizeof(buf));

  componentInformation.globalId            = buf[4];
  componentInformation.language            = buf[6] & 0x3F;
  componentInformation.characterSet        = buf[7] & 0x3F;
  memcpy(componentInformation.label, &buf[8], 16); //[8]....[23]
  componentInformation.label[16] = '\0';
  componentInformation.abbreviationMask    = (unsigned short) buf[25] << 8 | buf[24];
  componentInformation.numberUserAppTypes  = buf[26];

  componentInformation.lenTotal            = buf[27];
  componentInformation.userAppType         = buf[29] << 8 | buf[28];
  componentInformation.lenField            = buf[30];

  /*
    Table 1 - X-PAD application types [1, Table 27]
    Application type  Description
    0   End marker
    1   Data group length indicator
    2   Dynamic label segment, start of X-PAD data group
    3   Dynamic label segment, continuation of X-PAD data group
    4 to 11   User defined
    12  MOT, start of X-PAD data group, see EN 301 234 [14]
    13  MOT, continuation of X-PAD data group, see EN 301 234 [14]
    14  MOT, start of CA messages, see EN 301 234 [14]
    15  MOT, continuation of CA messages, see EN 301 234 [14]
    16 to 30  User defined
    31  Not used
  */

  // Allocate n unsigned chars and save ptr in a
  componentInformation.userAppData = new unsigned char[componentInformation.lenField];

  //check if success
  if (componentInformation.userAppData == nullptr)
  {
    //error
    Serial.println(F("Error allocate memory"));
    return;
  }


  //create buffer size componentInformation.lenUserAppData + 4
  unsigned char len =  4 + componentInformation.lenField;
  unsigned char appDataBuffer[len];

  //initalize buffer to 0xff
  for (uint8_t i = 0; i < len; i++) appDataBuffer[i] = 0xff;

  //offset
  unsigned long offset = 28;
  //read in appDataBuffer from offset with len
  readReplyOffset(appDataBuffer, len, offset);

  //print for development purpose
  serialPrintSi468x::printResponseHex(appDataBuffer, len);

  //copy answer in componentInformation.userAppData
  for (uint16_t j = 0; j < componentInformation.lenField; j++)
  {
    componentInformation.userAppData[j] = appDataBuffer[j + 4];
  }

}

//0xBC DAB_GET_TIME Gets the ensemble time adjusted for the local time offset or the UTC
void readDateTime(timeDab_t& timeDab, unsigned char timeType)
{
  unsigned char cmd[2];
  cmd[0] = DAB_GET_TIME;
  cmd[1] = timeType & 1;

  unsigned char buf[11];
  //initalize buffer
  for (unsigned short i = 0; i < 11; i++) buf[i] = 0xff;

  writeCommand(cmd, sizeof(cmd));
  delayMicroseconds(10000);
  readReply(buf, sizeof(buf));

  timeDab.year    = (unsigned short)buf[5] << 8 | buf[4];
  timeDab.month   = buf[6];
  timeDab.day     = buf[7];
  timeDab.hour    = buf[8];
  timeDab.minute  = buf[9];
  timeDab.second  = buf[10];
  timeDab.type    = timeType;
}

//0xBD DAB_GET_AUDIO_INFO Gets audio information
void readAudioInformation(audioInformation_t& audioInformation)
{
  unsigned char cmd[2];
  cmd[0] = DAB_GET_AUDIO_INFO;
  cmd[1] = 0;

  unsigned char buf[10];
  //initalize buffer
  for (unsigned char i = 0; i < 10; i++) buf[i] = 0xff;

  writeCommand(cmd, sizeof(cmd));
  delayMicroseconds(10000);
  delayMicroseconds(10000);
  readReply(buf, sizeof(buf));

  audioInformation.audioBitRate =     (unsigned short) buf[5] << 8 | buf[4];
  audioInformation.audioSampleRate =  (unsigned short) buf[7] << 8 | buf[6];
  audioInformation.audioPsFlag =  buf[8] >> 2 & 1;
  audioInformation.audioSbrFlag = buf[8] >> 1 & 1;
  audioInformation.audioMode =    buf[8] & 1;
  audioInformation.audioDrcGain = buf[9] & 1;
}

//0xBE DAB_GET_SUBCHAN_INFO Get technical information about the component
void readComponentTechnicalInformation(componentTechnicalInformation_t& componentTechnicalInformation, unsigned long &serviceId, unsigned long &componentId)
{
  unsigned char cmd[12];

  cmd[0] = DAB_GET_SUBCHAN_INFO;
  cmd[1] = 0x00;
  cmd[2] = 0x00;
  cmd[3] = 0x00;
  cmd[4] = serviceId & 0xFF;
  cmd[5] = serviceId >> 8 & 0xFF;
  cmd[6] = serviceId >> 16 & 0xFF;
  cmd[7] = serviceId >> 24 & 0xFF;
  cmd[8] = componentId & 0xFF;
  cmd[9] = componentId >> 8 & 0xFF;
  cmd[10] = componentId >> 16 & 0xFF;
  cmd[11] = componentId >> 24 & 0xFF;

  unsigned char buf[12];
  //initalize buffer
  for (unsigned short i = 0; i < 12; i++) buf[i] = 0xff;

  writeCommand(cmd, sizeof(cmd));
  delayMicroseconds(10000);
  delayMicroseconds(10000);
  readReply(buf, sizeof(buf));

  componentTechnicalInformation.serviceMode    = buf[4];
  componentTechnicalInformation.protectionInfo = buf[5];
  componentTechnicalInformation.bitRate        = (unsigned short) buf[7] << 8 | buf[6];
  componentTechnicalInformation.numberCU       = (unsigned short) buf[9] << 8 | buf[8];
  componentTechnicalInformation.addressCU      = (unsigned short) buf[11] << 8 | buf[10];

}

//0xBF DAB_GET_FREQ_INFO Gets ensemble frequency information table
void readFrequencyInformationTable(frequencyInformationTableHeader_t& frequencyInformationTableHeader)
{
  unsigned char cmd[2] = {DAB_GET_FREQ_INFO, 0};

  unsigned char buf[20];
  //initalize buffer
  for (unsigned short i = 0; i < 20; i++) buf[i] = 0xff;

  writeCommand(cmd, sizeof(cmd));
  delayMicroseconds(10000);
  delayMicroseconds(10000);
  readReply(buf, sizeof(buf));

  serialPrintSi468x::printResponseHex(buf, sizeof(buf));

  //free memory
  delete[] frequencyInformationTableHeader.frequencyInformationTable;

  //read size
  frequencyInformationTableHeader.len = (unsigned long)buf[7] << 24 | (unsigned long)buf[5] << 16 | (unsigned long)buf[5] << 8 | buf[4];

  //allocate memory
  frequencyInformationTableHeader.frequencyInformationTable = new frequencyInformationTable_t[frequencyInformationTableHeader.len];

  //check if success
  if (frequencyInformationTableHeader.frequencyInformationTable == nullptr)
  {
    return;
    //errorCode = 10//error
  }

}

//0xC0 DAB_GET_SERVICE_INFO Get digital service information
void readServiceInformation(serviceInformation_t& serviceInformation, unsigned long &serviceId)
{
  unsigned char cmd[1] = {DAB_GET_SERVICE_INFO};

  unsigned char buf[26];
  //initalize buffer
  for (unsigned short i = 0; i < 26; i++) buf[i] = 0xff;

  unsigned char arg[7];
  arg[0] = 0;
  arg[1] = 0;
  arg[2] = 0;
  arg[3] = serviceId & 0xFF;
  arg[4] = serviceId >> 8 & 0xFF;
  arg[5] = serviceId >> 16 & 0xFF;
  arg[6] = serviceId >> 24 & 0xFF;

  writeCommandArgument(cmd, sizeof(cmd), arg, sizeof(arg));
  delayMicroseconds(DURATION_10000_ms);
  delayMicroseconds(DURATION_10000_ms);
  readReply(buf, sizeof(buf));

  //serviceInfo1
  serviceInformation.serviceLinkingInfoFlag = buf[4] >> 6 & 1;
  serviceInformation.pType                  = buf[4] >> 1 & 0x1F;
  serviceInformation.pdFlag                 = buf[4] & 1;

  //serviceInfo2
  serviceInformation.localFlag              = buf[5] >> 7 & 1;
  serviceInformation.caId                   = buf[5] >> 4 & 0x7;
  serviceInformation.numComponents          = buf[5] & 0xF;

  //serviceInfo3
  serviceInformation.characterSet           = buf[6] & 0xF;
  serviceInformation.ecc                    = buf[7];

  //Service Label
  memcpy(serviceInformation.serviceLabel, &buf[8], 16); // Bytes 16...36
  serviceInformation.serviceLabel[16] = '\0';

  serviceInformation.abbreviationMask       = (unsigned short) buf[25] << 8 | buf[24];
}


//Scan all indices of frequency table
bool dabBandScan(unsigned char &dabValidNumFreq, unsigned char* &dabValidFreqTable)
{
  /*free memory from previous table*/
  free(dabValidFreqTable);
  /*Set pointer to nullptr*/
  dabValidFreqTable = nullptr;
  /*Set to valid frequencies = 0*/
  dabValidNumFreq = 0;

  //To get receive signal quality
  rsqInformation_t rsqInformation;

  //Get actual frequency table
  //unsigned long* frequencyTable readFrequencyTable();

  unsigned char numberIndices;
  readNumberFrequencies(numberIndices);

  //Scan trough index table - start with index 0
  for (unsigned char index = 0; index < numberIndices; index++)
  {
    tuneIndex(index);
    //Check receive signal quality
    readRsqInformation(rsqInformation);

    //DAB found and valid save The threshold for dab detected is greater than 4.
    if ((rsqInformation.fastDect > 4) && rsqInformation.valid)
    {
      //serialPrintSi468x::dabPrintIndex(index);
      //increase memory allocation, in C use type cast for realloc to supress warning of void*. Start with dabValidNumFreq + 1
      dabValidFreqTable = (unsigned char*) realloc(dabValidFreqTable, (dabValidNumFreq + 1) * sizeof(unsigned char));
      //write valid index in list
      dabValidFreqTable[dabValidNumFreq] = index;
      //increase valid frequencies
      dabValidNumFreq++;
    }
  }
  //If no valid frequency found return
  if (dabValidNumFreq == 0)
  {
    //free memory from previous malloc
    free(dabValidFreqTable);
    //Set pointer to nullptr
    dabValidFreqTable = nullptr;
    //Set to valid frequencies = 0
    dabValidNumFreq = 0;

    return false;
  }

  return true;
}

//Tune index up/down
void tune(unsigned char &dabIndex, bool up)
{
  //read actual index
  rsqInformation_t rsqInformation;
  readRsqInformation(rsqInformation);

  //remember actual index
  uint8_t indexStart = rsqInformation.index;
  //get actual number of indices
  uint8_t numberIndices;
  readNumberFrequencies(numberIndices);

  if (up)
  {
    //wrap around at _numberIndices
    if (indexStart == numberIndices - 1) indexStart = 0;
    //only increase if smaller 48 or _numberIndices
    else if (indexStart < numberIndices - 1) indexStart++;
  }
  else
  {
    //wrap around at 0 to numberIndices -1
    if (indexStart == 0)  indexStart = numberIndices - 1;
    //only decrease if min 1
    else if (indexStart >= 1)  indexStart--;
  }

  //tune index
  tuneIndex(indexStart);

  //read result
  readRsqInformation(rsqInformation);

  dabIndex = rsqInformation.index;
}
