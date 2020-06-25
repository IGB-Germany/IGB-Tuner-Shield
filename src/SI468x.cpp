//Testing
//Print detailed info about frequency table
#define DEBUG_DAB_FREQUENCY_TABLE
//Print detailed info about Ensemble
#define DEBUG_DAB_PARSE_ENSEMBLE

//Calculate available free RAM
#include "RamTesting.h"
#define DEBUG_FREE_RAM

/*to test for error handling*/
#include "errno.h"

//Driver for tuner circuit SI468x by IGB
#include "SI468x.h"

/*firmware*/
#include "firmware.h"

//SPI communication layer
#include "ComDriverSpi.h"
ComDriverSpi tuner(PIN_DEVICE_SLAVE_SELECT, SPI_FREQUENCY);

//properties of tuner circuit
#include "properties.h"
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
  serialPrintSi468x::printVersion(version);

  initalize();
  reset();
  powerUp(powerUpArguments);
  loadFirmware(addrBootloaderPatchFull, sizeBootloaderPatchFull); //FullPatch

  serialPrintSi468x::printStatusRegister(readStatusRegister());

  loadFirmware(addrFirmwareDab, sizeFirmwareDab);//DAB Firmware

  //Print device status information
  //serialPrintSi468x::devicePrintStatus(deviceGetStatus());
  //Boot device
  boot();
  //Print device status information
  serialPrintSi468x::printStatusRegister(readStatusRegister());

  //Set device properties
  writePropertyValueList(propertyValueListDevice, NUM_PROPERTIES_DEVICE);

  //Print system state
  serialPrintSi468x::printSystemState(readSystemState());
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

  //open: generic message handler to avoid PrintSerialFlashSst26 object
  //flashSst26.readId();
  //flashSst26.readUniqueId();

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

//Read ststus register
statusRegister_t readStatusRegister()
{
  statusRegister_t statusRegister;

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

  return statusRegister;
}

unsigned short getFreeRam()
{
  return freeRam();
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
statusRegister_t readReply(unsigned char reply[], unsigned long len)
{
  statusRegister_t statusRegister;
  //CMD
  unsigned char cmd[1] = {READ_REPLY};

  //retry until maxRetry
  for (unsigned char retry = 0; retry < MAX_RETRY; retry++)
  {
    //init reply to 0xff
    for (unsigned char i = 0; i < len; i++) reply[i] = 0xff;

    ///wait for device - to be improved
    delayMicroseconds(DURATION_REPLY);

    writeCommandArgument(cmd, sizeof(cmd), reply, len);

    statusRegister.cts       = reply[0] >> 7 & 1;
    statusRegister.cmdErr    = reply[0] >> 6 & 1;
    statusRegister.dacqInt   = reply[0] >> 5 & 1;
    statusRegister.dsrvInt   = reply[0] >> 4 & 1;
    statusRegister.stcInt    = reply[0] & 1;
    statusRegister.eventInt  = reply[1] >> 5 & 1;
    statusRegister.state     = reply[3] >> 6 & 3;
    statusRegister.rfFeErr   = reply[3] >> 5 & 1;
    statusRegister.dspErr    = reply[3] >> 4 & 1;
    statusRegister.repOfErr  = reply[3] >> 3 & 1;
    statusRegister.cmdOfErr  = reply[3] >> 2 & 1;
    statusRegister.arbErr    = reply[3] >> 1 & 1;
    statusRegister.nonRecErr = reply[3] & 1;

    //Clear to send and no error then break loop
    if ((statusRegister.cts == 1) && ((statusRegister.cmdErr & 1) == 0))
    {
      statusRegister.cmdErrCode = 0;
      return statusRegister;
    }
    //error or too many reads
    else if ((statusRegister.cmdErr == 1) || (retry == MAX_RETRY - 1))
    {
      //if cmdErr read byte 5 of reply
      unsigned char errBuf[5] = {0xff, 0xff, 0xff, 0xff, 0xff};
      writeCommandArgument(cmd, sizeof(cmd), errBuf, 5);
      serialPrintSi468x::printResponseHex(errBuf, sizeof(errBuf));
      statusRegister.cmdErrCode = errBuf[5];
      return statusRegister;
    }
  }
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

  writeCommandArgument(cmd, sizeof(cmd));
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

  writeCommandArgument(cmd, sizeof(cmd));
}

//0x06 LOAD_INIT Prepares the bootloader to receive a new image
void loadInit()
{
  unsigned char cmd[2];

  cmd[0] = LOAD_INIT;
  cmd[1] = 0x00;

  ///wait for device - to be improved
  delayMicroseconds(DURATION_LOAD_INIT);
  writeCommandArgument(cmd, sizeof(cmd));
  delayMicroseconds(DURATION_LOAD_INIT);
}

//0x07 BOOT Boots the image currently loaded in RAM
void boot()
{
  unsigned char cmd[2];

  cmd[0] = BOOT;
  cmd[1] = 0;

  writeCommandArgument(cmd, sizeof(cmd));

  //wait for device - to be improved
  for (uint8_t i = 0; i < 30; i++)
  {
    delayMicroseconds (DURATION_BOOT);
  }
}

//0x08 GET_PART_INFO Get Device Part Info
partInfo_t readPartInfo()
{
  partInfo_t partInfo;
  unsigned char cmd[2];
  unsigned char buf[23] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

  cmd[0] = GET_PART_INFO;
  cmd[1] = 0;

  writeCommandArgument(cmd, sizeof(cmd));

  readReply(buf, sizeof(buf));

  partInfo.chipRev    = buf[4];
  partInfo.romId      = buf[5];
  partInfo.partNumber = buf[9] << 8 | buf[8];

  return partInfo;
}

//0x09 GET_SYS_STATE reports basic system state information such as which mode is active; FM, DAB, etc.
unsigned char readSystemState()
{
  unsigned char systemState;
  unsigned char cmd[2];
  unsigned char buf[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

  cmd[0] = GET_SYS_STATE;
  cmd[1] = 0;

  writeCommandArgument(cmd, sizeof(cmd));

  readReply(buf, sizeof(buf));

  systemState = buf[4];

  return systemState;
}

//0x0A GET_POWER_UP_ARGS Reports basic information about the device such as arguments used during POWER_UP
powerUpArguments_t readPowerUpArguments()
{
  powerUpArguments_t powerUpArguments;
  unsigned char cmd[2];
  unsigned char buf[18] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

  cmd[0] = GET_POWER_UP_ARGS;
  cmd[1] = 0;

  writeCommandArgument(cmd, sizeof(cmd));

  readReply(buf, sizeof(buf));

  powerUpArguments.clockMode  = buf[6] >> 4 & 3;
  powerUpArguments.trSize     = buf[6] & 0xF;
  powerUpArguments.iBiasStart = buf[7] & 0x7F;
  powerUpArguments.xtalFreq   = (unsigned long) buf[11] << 24 | (unsigned long)buf[10] << 16 | (unsigned long) buf[9] << 8 | buf[8];
  powerUpArguments.cTune      = buf[12] & 0x3F;
  powerUpArguments.iBiasRun   = buf[17] & 0x7F;

  return powerUpArguments;
}

//0x10 READ_OFFSET Reads a portion of response buffer from an offset
statusRegister_t readReplyOffset(unsigned char reply[], unsigned short len, unsigned short offset)
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

  statusRegister_t statusRegister;

  //retry until maxRetry
  for (uint8_t retry = 0; retry < MAX_RETRY; retry++)
  {
    uint8_t cmd[1] = {READ_OFFSET};
    uint8_t arg[3] = {0, (uint8_t)(offset & 0xff), (uint8_t) (offset >> 8)};

    delayMicroseconds(DURATION_REPLY);
    writeCommandArgument(cmd, sizeof(cmd), arg, sizeof(arg));
    ///wait for device - to be improved
    delayMicroseconds(DURATION_REPLY);

    uint8_t cmd2[1] = {READ_REPLY};
    //initalize reply to 0xff
    for (uint8_t i = 0; i < len; i++) reply[i] = 0xff;
    writeCommandArgument(cmd2, sizeof(cmd2), reply, len);

    statusRegister.cts      = (reply[0] >> 7) & 1;
    statusRegister.cmdErr   = (reply[0] >> 6) & 1;
    statusRegister.dacqInt  = reply[0] >> 5 & 1;
    statusRegister.dsrvInt  = reply[0] >> 4 & 1;
    statusRegister.stcInt   = reply[0] & 1;
    statusRegister.eventInt = reply[1] >> 5 & 1;
    statusRegister.state    = reply[3] >> 6 & 3;
    statusRegister.rfFeErr  = reply[3] >> 5 & 1;
    statusRegister.dspErr   = reply[3] >> 4 & 1;
    statusRegister.repOfErr = reply[3] >> 3 & 1;
    statusRegister.cmdOfErr = reply[3] >> 2 & 1;
    statusRegister.arbErr   = reply[3] >> 1 & 1;
    statusRegister.nonRecErr = reply[3] & 1;

    //Clear to send and no error then break loop
    if ((statusRegister.cts == 1) && (statusRegister.cmdErr == 0))
    {
      statusRegister.cmdErrCode = 0;
      return statusRegister;
    }
    //error or too many reads
    else if ((statusRegister.cmdErr) == 1 || (retry == MAX_RETRY - 1))
    {
      //if cmdErr read byte 5 of reply
      unsigned char errBuf[5] = {0xff, 0xff, 0xff, 0xff, 0xff};
      writeCommandArgument(cmd, sizeof(cmd), errBuf, 5);
      serialPrintSi468x::printResponseHex(errBuf, sizeof(errBuf));
      statusRegister.cmdErrCode = errBuf[5];
      return statusRegister;
    }
  }
}

//0x12 GET_FUNC_INFO Get Firmware Information
firmwareInformation_t readFirmwareInformation()
{
  firmwareInformation_t deviceFirmwareInfo = {0, 0, 0, 0, 0, 0, 0, 0};
  unsigned char cmd[2];
  unsigned char buf[12] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

  cmd[0] = GET_FUNC_INFO;
  cmd[1] = 0;

  writeCommandArgument(cmd, sizeof(cmd));

  readReply(buf, sizeof(buf));

  deviceFirmwareInfo.revisionNumberMajor = buf[4];
  deviceFirmwareInfo.revisionNumberMinor = buf[5];
  deviceFirmwareInfo.revisionNumberBuild = buf[6];
  deviceFirmwareInfo.noSvnFlag = buf[7] >> 7 & 1;
  deviceFirmwareInfo.location = buf[7] >> 4 & 2;
  deviceFirmwareInfo.mixedRevFlag = buf[7] >> 1 & 1;
  deviceFirmwareInfo.localModFlag = buf[7] & 1;
  deviceFirmwareInfo.svnId = buf[8] | (unsigned long)buf[9] << 8 | (unsigned long)buf[10] << 16 | (unsigned long)buf [11] << 24;

  return deviceFirmwareInfo;
}

//0x13 SET_PROPERTY Sets the value of a property
void writePropertyValue(unsigned short id, unsigned short value)
{
  unsigned char cmd[1];
  unsigned char arg[5];
  unsigned char buf[4] = {0xff, 0xff, 0xff, 0xff};

  cmd[0] = SET_PROPERTY;

  arg[0] = 0x00;
  arg[1] = id & 0xff;
  arg[2] = id >> 8;
  arg[3] = value & 0xff;
  arg[4] = value >> 8;

  writeCommandArgument(cmd, sizeof(cmd), arg, sizeof(arg));

  readReply(buf, sizeof(buf));
}

//0x14 GET_PROPERTY Retrieve the value of a property
unsigned short readPropertyValue(unsigned short id)
{
  unsigned char cmd[1];
  unsigned char arg[3];
  unsigned char buf[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
  unsigned short propertyValue = 0xffff;

  cmd[0] = GET_PROPERTY;

  arg[0] = 1;//count = 1
  arg[1] = id & 0xff;
  arg[2] = id >> 8 & 0xff;

  writeCommandArgument(cmd, sizeof(cmd), arg, sizeof(arg));
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
  unsigned char cmd[8];
  unsigned char buf[4] = {0xff, 0xff, 0xff, 0xff};

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

  writeCommandArgument(cmd, sizeof(cmd));
  //open: readStorage() handle first 4 bytes of answer
  readReply(data, len);
}

//0xE5 TEST_GET_RSSI returns the reported RSSI in 8.8 format
unsigned short readRssi()
{
  unsigned short rssi = 0;
  unsigned char cmd[1];
  unsigned char arg[1];
  unsigned char buf[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

  cmd[0] = TEST_GET_RSSI;
  arg[0] = 0;

  writeCommandArgument(cmd, sizeof(cmd), arg, sizeof(arg));
  delayMicroseconds(10000);
  readReply(buf, sizeof(buf));

  rssi = buf[5] << 8 | buf[4];

  return rssi;
}

//Controls the audio mute on each audio output
void writeMute(unsigned char channelMuted)
{
  //  0 : Do not mute audio outputs
  //  1 : Mute Left Audio Out.
  //  2 : Mute Right Audio Out.
  //  3 : Mute both Left and Right Audio Out
  //Range ok ?
  if (channelMuted > 3) channelMuted = 0;

  writePropertyValue(AUDIO_MUTE, channelMuted);
}

//Get status of mute
unsigned char readMute()
{
  unsigned char channelMuted = 0;
  channelMuted = readPropertyValue(AUDIO_MUTE);
  return channelMuted;
}

unsigned char volumeUp()
{
  unsigned char volume = readPropertyValue(AUDIO_ANALOG_VOLUME);

  //volume 0...63;
  if (volume < 63) volume ++;
  else volume = 63;

  writePropertyValue(AUDIO_ANALOG_VOLUME, volume);
  return volume;
}

unsigned char volumeDown()
{
  unsigned char volume = readPropertyValue(AUDIO_ANALOG_VOLUME);

  //volume 0...63;
  if (volume > 0) volume --;
  else volume = 0;

  writePropertyValue(AUDIO_ANALOG_VOLUME, volume);
  return volume;
}

//Global variables DAB definitions

//Ensemble header with pointer to service list - dynamic memory allocation
ensembleHeader_t ensembleHeader = {0, 0, 0, 0, 0, nullptr};

//Frequency table - dynamic memory allocation
long unsigned* frequencyTable = nullptr;

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
  //Set DAB properties
  writePropertyValueList(propertyValueListDab, NUM_PROPERTIES_DAB);

  //Tunes DAB inital index
  tuneIndex(dabIndex);
  //Starts inital audio service
  startService(serviceId, componentId);
  //Get information of the digital ensemble
  readEnsembleInformation();
  //readServiceInformation(dabServiceInformation, serviceId);
  //writeTextField(dabServiceInformation.serviceLabel);
}

void startService(unsigned long &serviceId, unsigned long &componentId, const unsigned char serviceType)
{
  unsigned char cmd[1];
  unsigned char arg[11];
  unsigned char buf[4] = {0xff, 0xff, 0xff, 0xff};

  cmd[0] = START_DIGITAL_SERVICE;

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

void stopService(unsigned long &serviceId, unsigned long &componentId, const unsigned char serviceType)
{
  unsigned char cmd[1];
  unsigned char arg[11];
  unsigned char buf[4] = {0xff, 0xff, 0xff, 0xff};

  cmd[0] = STOP_DIGITAL_SERVICE;

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

dabServiceData_t dabGetServiceData(unsigned char statusOnly, unsigned char ack)
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
  dabServiceData_t dabServiceData;
  unsigned char cmd[1];
  unsigned char arg[1];
  unsigned char buf[24];
  //initalize
  for (unsigned short i = 0; i < 24; i++) buf[i] = 0xff;


  cmd[0]  = GET_DIGITAL_SERVICE_DATA;
  arg[0] = ((statusOnly  & 1 ) << 4) | (ack & 1);

  writeCommandArgument(cmd, sizeof(cmd), arg, sizeof(arg));
  delayMicroseconds(10000);
  readReply(buf, sizeof(buf));

  dabServiceData.errorInterrupt     =  buf[4] >> 2 & 1;
  dabServiceData.overflowInterrupt  =  buf[4] >> 1 & 1;
  dabServiceData.packetInterrupt    =  buf[4] & 1;
  dabServiceData.bufferCount        =  buf[5];
  dabServiceData.statusService      =  buf[6];//3 : Indicates the this data packet represents the beginning of a new data object.
  dabServiceData.dataSource         =  buf[7] >> 6 & 3;//2 : Indicates that the payload is DLS PAD and DATA_TYPE is 0.
  dabServiceData.dataType           =  buf[7] & 0x3F;
  dabServiceData.serviceId          =  (unsigned long) buf[8] | (unsigned long) buf[9] << 8 | (unsigned long) buf[10] << 16 | (unsigned long) buf[11] << 24;
  dabServiceData.componentId        =  (unsigned long) buf[12] | (unsigned long) buf[13] << 8 | (unsigned long) buf[14] << 16 | (unsigned long) buf[15] << 24;
  //dabServiceData.rfu              =  buf[16] | buf[17] << 8;
  dabServiceData.dataLength         =  buf[18] | buf[19] << 8;
  dabServiceData.segmentNumber      =  buf[20] | buf[21] << 8;
  dabServiceData.numberSegments     =  buf[22] | buf[23] << 8;

  /*nothing in buffer */
  if (dabServiceData.bufferCount == 0) return dabServiceData;
  /*length = 0 */
  if (dabServiceData.dataLength == 0)return dabServiceData;
  /*No DLS or DLS+ */
  //if (dabServiceData.dataSource != 2)return dabServiceData;

  /*Create buffer with 24 + payload*/
  unsigned char payloadBuffer[24 + dabServiceData.dataLength];

  //initalize buffer
  for (unsigned short i = 0; i < (24 + dabServiceData.dataLength); i++) payloadBuffer[i] = 0xff;

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
    char dls[dabServiceData.dataLength];
    memcpy(dls, &payloadBuffer[26], dabServiceData.dataLength); // Bytes 26...dataLength
    /*label terminated by '/0'*/
    //dabEnsembleInfo.ensembleLabel[dabServiceData.dataLength+1] = '\0';

    serialPrintSi468x::dabPrintDynamicLabelSegment(dls);
  }

  //Dynamic Label+
  else if (prefixDls.c == 1 && prefixDls.command == 0b0010)
  {
    //Create dls string
    char tags[dabServiceData.dataLength];
    Serial.println(F("DLS+:"));
    Serial.println(payloadBuffer[26]);

    memcpy(tags, &payloadBuffer[27], dabServiceData.dataLength); // Bytes 27...dataLength
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

  return dabServiceData;
}

//Get ensemble header
void getEnsembleHeader(ensembleHeader_t &ensembleHeader, unsigned char serviceType)
{
  //ensemble list availabel ?
  eventInformation_t eventInformation = readEventInformation();

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

  writeCommandArgument(cmd, sizeof(cmd));
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
void getEnsemble(ensembleHeader_t &ensembleHeader, uint8_t serviceType)
{
  //free memory from previous Ensemble List Data Structure
  freeMemoryFromEnsembleList(ensembleHeader);

  //Read Header
  getEnsembleHeader(ensembleHeader, serviceType);

  //Check services
  if (ensembleHeader.numServices == 0 )
  {
#ifdef DEBUG_PARSE_ENSEMBLE
    Serial.print(F("No services"));
#endif
    return;
  }

  //Set pointer to a new first serviceList and allocate memory
  ensembleHeader.serviceList = (serviceList_t*) calloc (ensembleHeader.numServices, sizeof(serviceList_t));

  //Realloc pointer to serviceList and create dynamic array of right size and initialize with 0
  //ensembleHeader.serviceList = (service_t*) realloc (ensembleHeader.serviceList, ensembleHeader.numServices * sizeof(service_t));

  //Debugging
  /*
    Serial.print(F("RAM Allocated:\t"));
    Serial.println(ensembleHeader.numServices * sizeof(serviceList_t));
    Serial.print(F("Services:\t"));
    Serial.println(ensembleHeader.numServices);
    Serial.print(F("serviceList_t:\t"));
    Serial.println(sizeof(serviceList_t));
    Serial.print(F("componentList_t:\t"));
    Serial.println(sizeof(componentList_t));
    Serial.print(F("Free RAM:\t"));
    Serial.println(freeRam());
    Serial.println();
  */

  //No memory left
  if (ensembleHeader.serviceList == nullptr)
  {
    freeMemoryFromEnsembleList(ensembleHeader);
#ifdef DEBUG_PARSE_ENSEMBLE
    Serial.print(F("No memory left"));
#endif
    return;
  }

  //Read service list
  //Read again response with readReplyOffset
  uint8_t cmd[2];
  cmd[0] = GET_DIGITAL_SERVICE_LIST;
  cmd[1] = serviceType & 1;
  writeCommandArgument(cmd, sizeof(cmd));
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

    //Create dynamic array of right size and initialize with 0 for component list
    ensembleHeader.serviceList[i].componentList = (componentList_t*) realloc(ensembleHeader.serviceList[i].componentList, ensembleHeader.serviceList[i].numComponents * sizeof(componentList_t));

    //No memory left
    if (ensembleHeader.serviceList[i].componentList == nullptr)
    {
      //free memory from previous realloc
      free(ensembleHeader.serviceList[i].componentList);
      //Set pointer to nullptr
      ensembleHeader.serviceList[i].componentList = nullptr;
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

#ifdef DEBUG_PARSE_ENSEMBLE
  Serial.println(F("Ensemble List created"));
  Serial.print(F("Free RAM:\t"));
  Serial.println(freeRam());
  Serial.println();
#endif
}

//Free memory from Ensemble List Data Structure
void freeMemoryFromEnsembleList(ensembleHeader_t &ensembleHeader)
{
  //pointer available ?
  if (ensembleHeader.serviceList != nullptr)
  {

#ifdef DEBUG_PARSE_ENSEMBLE
    Serial.println(F("Delete serviceList"));
#endif DEBUG_PARSE_ENSEMBLE

    //start with first service in list
    for (uint8_t i = 0; i < ensembleHeader.numServices; i++)
    {
      if (ensembleHeader.serviceList[i].componentList != nullptr)
      {
#ifdef DEBUG_PARSE_ENSEMBLE
        Serial.print(F("Delete component:\t"));
        Serial.println(i);
#endif DEBUG_PARSE_ENSEMBLE
        //clear memory from componentList
        free(ensembleHeader.serviceList[i].componentList);
        //Set pointer in component list to nullptr
        ensembleHeader.serviceList[i].componentList = nullptr;
      }
    }
    //clear memory from service
    free(ensembleHeader.serviceList);
  }

  ensembleHeader.serviceList = nullptr;

#ifdef DEBUG_PARSE_ENSEMBLE
  Serial.println(F("Ensemble List deleted"));
  Serial.print(F("Free RAM:\t"));
  Serial.println(freeRam());
  Serial.println();
#endif DEBUG_PARSE_ENSEMBLE
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
#endif DEBUG_PARSE_ENSEMBLE

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
#endif DEBUG_PARSE_ENSEMBLE
      found = true;
      break;
    }
  }

  return found;
}

//Start next service in ensemble
unsigned char nextService(unsigned long &serviceId, unsigned long &componentId)
{
  //in actual ensemble ?
  bool found = false;

  //no services in ensembleHeader
  if (ensembleHeader.numServices == 0)
  {
    //parse ensemble
    getEnsemble(ensembleHeader);
    //no reception, no service list ready return
    if (ensembleHeader.numServices == 0)  return 0;
  }

  //lookup in ensemble because of changed frequency
  found = searchService(serviceId, componentId);

  //Found in ensemble
  if (found)
  {
    //Wrap around
    if (ensembleHeader.actualService == ensembleHeader.numServices - 1)
      ensembleHeader.actualService = 0;
    else  ensembleHeader.actualService++;
  }
  //Set to first entry
  else
  {
    ensembleHeader.actualService = 0;
  }

  //global var tbi
  componentId = ensembleHeader.serviceList[ensembleHeader.actualService].componentList[0].componentId;
  serviceId = ensembleHeader.serviceList[ensembleHeader.actualService].serviceId;

  startService(serviceId, componentId);

  //return position in list
  return ensembleHeader.actualService;
}

//Start previous service in ensemble
unsigned char previousService(unsigned long &serviceId, unsigned long &componentId)
{
  //in actual ensemble ?
  bool found = false;

  //no services in ensembleHeader
  if (ensembleHeader.numServices == 0)
  {
    //parse ensemble
    getEnsemble(ensembleHeader);
    //no reception, no service list ready return
    if (ensembleHeader.numServices == 0)  return 0;
  }

  //lookup in ensemble because of changed frequency
  found = searchService(serviceId, componentId);

  //Wrap around
  if (ensembleHeader.actualService == 0)
    ensembleHeader.actualService = ensembleHeader.numServices;
  else ensembleHeader.actualService--;

  //Found in ensemble
  if (found)
  {
    //global var tbi
    componentId = ensembleHeader.serviceList[ensembleHeader.actualService].componentList[0].componentId;
    serviceId = ensembleHeader.serviceList[ensembleHeader.actualService].serviceId;
  }
  //Set to first entry
  else
  {
    ensembleHeader.actualService = 0;
  }

  startService(serviceId, componentId);

  //return position in list
  return ensembleHeader.actualService;
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

    eventInformation = readEventInformation();

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
unsigned char tuneIndex(unsigned char index, unsigned short varCap, unsigned char injection)
{
  /*
    Tunes the DAB Receiver to tune to a frequency between 168.16 and 239.20 MHz defined by the frequency table
    through DAB_SET_FREQ_LIST.
    INJECTION[1:0] Injection selection  0 : Automatic injection selection 1 : Low-side injection 2 : High-side injection
    FREQ_INDEX[7:0] Frequency index for the tuned frequency, see the DAB_SET_FREQ_LIST command that sets the frequency table
    ANTCAP[15:0] Antenna tuning capacitor value in 250 fF units (31.75 pF Max) Range: 0-128
    0 : Automatically determines the cap setting
  */
  //Validity index 0...47
  if (index >= MAX_INDEX) index = MAX_INDEX - 1;

  unsigned char cmd[1];
  unsigned char arg[5];
  unsigned char buf[4] = {0xff, 0xff, 0xff, 0xff};

  cmd[0] = DAB_TUNE_FREQ;

  arg[0] = injection & 3;
  arg[1] = index;
  arg[2] = 0;
  arg[3] = varCap & 0xFF;
  arg[4] = varCap >> 8;

  writeCommandArgument(cmd, sizeof(cmd), arg, sizeof(arg));

  //STC ? 600ms = 60 * 10000us
  for (uint8_t i = 0; i < 10; i++)
  {
    for (uint8_t j = 0; j < 20; j++)
      delayMicroseconds(DURATION_TUNE);
    readReply(buf, sizeof(buf));
    Serial.print('.');
    if ((buf[0] & 1) == 1)
    {
      break;
    }
  }
  Serial.println();

  return buf[0] & 1;
}

//0xB2 DAB_DIGRAD_STATUS Get status information about the received signal quality
rsqInformation_t readRsqInformation(unsigned char clearDigradInterrupt, unsigned char rssiAtTune, unsigned char clearStcInterrupt)
{
  
  rsqInformation_t rsqInformation;
  
  unsigned char cmd[1] = {DAB_DIGRAD_STATUS};
  unsigned char arg[1];
  arg[0] = ((clearDigradInterrupt & 1) << 3) | ((rssiAtTune & 1) << 2) | (clearStcInterrupt & 1);

  unsigned char buf[23];
  //initalize buffer
  for (unsigned char i = 0; i < 23; i++) buf[i] = 0xff;

  writeCommandArgument(cmd, sizeof(cmd), arg, sizeof(arg));
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

  return rsqInformation;
}

//0xB3 DAB_GET_EVENT_STATUS Gets information about the various events related to the DAB radio
eventInformation_t readEventInformation(unsigned char eventAck = 0)
{
  eventInformation_t eventInformation;
  unsigned char cmd[1];
  unsigned char arg[1];
  unsigned char buf[8] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

  cmd[0] = DAB_GET_EVENT_STATUS;
  arg[0] = eventAck & 1;

  writeCommandArgument(cmd, sizeof(cmd), arg, sizeof(arg));
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

  return eventInformation;
}

//0xB4 DAB_GET_ENSEMBLE_INFO Gets information about the current ensemble
ensembleInformation_t readEnsembleInformation()
{
  ensembleInformation_t dabEnsembleInformation;
  unsigned char cmd[1];
  unsigned char arg[1];
  unsigned char buf[26];
  for (unsigned short i = 0; i < 26; i++) buf[i] = 0xff;

  cmd[0] = DAB_GET_ENSEMBLE_INFO;
  arg[0] = 0;

  writeCommandArgument(cmd, sizeof(cmd), arg, sizeof(arg));
  delayMicroseconds(10000);
  readReply(buf, sizeof(buf));

  dabEnsembleInformation.ensembleId        = (unsigned short)buf[5] << 8 | buf[4];
  /*16 characters for the ensemble label terminated by '/0'*/
  memcpy(dabEnsembleInformation.ensembleLabel, &buf[6], 16); /*buf[6]....buf[21]*/
  dabEnsembleInformation.ensembleLabel[16] = '\0';
  dabEnsembleInformation.ecc               = buf[22];
  dabEnsembleInformation.charSet           = buf[23];
  dabEnsembleInformation.abbreviationMask  = (unsigned short)buf[25] << 8 | buf[24];

  return dabEnsembleInformation;
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

  uint8_t cmd[1];
  uint8_t arg[3 + 4 * numFreq]; //4 arg per frequency
  uint8_t buf[4] = {0xff, 0xff, 0xff, 0xff};

  cmd[0]  = DAB_SET_FREQ_LIST;

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

  writeCommandArgument(cmd, sizeof(cmd), arg, sizeof(arg));
  delayMicroseconds(10000);
  readReply(buf, sizeof(buf));

#ifdef DEBUG_DAB_FREQUENCY_TABLE
  serialPrintSi468x::printResponseHex(buf, sizeof(buf));
#endif
}

//0xB9 DAB_GET_FREQ_LIST Get frequency table
unsigned long* readFrequencyTable()
{
  //free memory from previous table
  if (frequencyTable != nullptr)
  {
    free(frequencyTable);
    frequencyTable = nullptr;
  }

  //read number of frequencies
  unsigned char numFreq = readNumberFrequencies();

#ifdef DEBUG_DAB_FREQUENCY_TABLE
  Serial.print(F("RAM: "));
  Serial.println(getFreeRam());
#endif

  //Dynamically allocate memory and initialize with 0
  frequencyTable = (uint32_t*) calloc(numFreq, (sizeof(uint32_t)));

  //Error
  if (frequencyTable == nullptr) return nullptr;

  uint8_t cmd[2]  = {DAB_GET_FREQ_LIST, 0};
  //4 Statusbytes + 4 Bytes from command DAB_GET_FREQ_LIST
  uint8_t freq[8] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
  writeCommandArgument(cmd, sizeof(cmd));
  delayMicroseconds(10000);

  //Fill index table
  //Start reading 4 Bytes per frequency
  //OFFSET parameter must be modulo four
  uint16_t offset = 0;
  readReplyOffset(freq, sizeof(freq), offset);

  for (uint8_t i = 0; i < numFreq; i++)
  {
    offset = offset + 4 ; //increase offset to next frequency
    readReplyOffset(freq, sizeof(freq), offset);

    frequencyTable[i] = (uint32_t) freq[7] << 24 | (uint32_t) freq[6] << 16 | (uint32_t) freq[5] << 8 | freq[4];

    //initalize for next frequency
    for (uint8_t j = 0;  j < 8; j++) freq[j] = 0xff;
  }

#ifdef DEBUG_DAB_FREQUENCY_TABLE
  Serial.print(F("RAM: "));
  Serial.println(getFreeRam());
#endif

  return  frequencyTable;
}


//0xBB DAB_GET_COMPONENT_INFO Get information about the component application data
dabComponentInformation_t dabGetComponentInformation(unsigned long &serviceId, unsigned long &componentId)
{
  dabComponentInformation_t dabComponentInformation;
  unsigned char cmd[1];
  unsigned char arg[11];
  unsigned char buf[31];
  //initalize buffer
  for (unsigned short i = 0; i < 31; i++) buf[i] = 0xff;

  cmd[0] = DAB_GET_COMPONENT_INFO;

  arg[0] = 0x00;
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
  delayMicroseconds(10000);
  readReply(buf, sizeof(buf));

  dabComponentInformation.globalId            = buf[4];
  dabComponentInformation.language            = buf[6] & 0x3F;
  dabComponentInformation.characterSet        = buf[7] & 0x3F;
  memcpy(dabComponentInformation.label, &buf[8], 16); //[8]....[23]
  dabComponentInformation.label[16] = '\0';
  dabComponentInformation.abbreviationMask    = (unsigned short) buf[25] << 8 | buf[24];
  dabComponentInformation.numberUserAppTypes  = buf[26];
  dabComponentInformation.lenUserApp          = buf[27];
  dabComponentInformation.userAppType         = buf[29] << 8 | buf[28];
  dabComponentInformation.lenUserAppData      = buf[30];

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

  return dabComponentInformation;
}

//0xBC DAB_GET_TIME Gets the ensemble time adjusted for the local time offset or the UTC
dabTime_t dabGetDateTimeInformation(unsigned char timeType)
{

  dabTime_t dabTime;

  unsigned char cmd[1];
  unsigned char arg[1];
  unsigned char buf[11];
  //initalize buffer
  for (unsigned short i = 0; i < 11; i++) buf[i] = 0xff;

  cmd[0] = DAB_GET_TIME;
  arg[0] = timeType & 1;

  writeCommandArgument(cmd, sizeof(cmd), arg, sizeof(arg));
  delayMicroseconds(10000);
  readReply(buf, sizeof(buf));

  dabTime.year    = (unsigned short)buf[5] << 8 | buf[4];
  dabTime.month   = buf[6];
  dabTime.day     = buf[7];
  dabTime.hour    = buf[8];
  dabTime.minute  = buf[9];
  dabTime.second  = buf[10];
  dabTime.type    = timeType;

  return dabTime;
}

audioInformation_t readAudioInformation()
{

  audioInformation_t audioInformation;
  unsigned char cmd[1];
  unsigned char arg[1];
  unsigned char buf[10];

  //initalize buffer
  for (unsigned char i = 0; i < 10; i++) buf[i] = 0xff;

  cmd[0] = DAB_GET_AUDIO_INFO;
  arg[0] = 0x00;

  writeCommandArgument(cmd, sizeof(cmd), arg, sizeof(arg));
  delayMicroseconds(10000);
  delayMicroseconds(10000);
  readReply(buf, sizeof(buf));

  audioInformation.audioBitRate =     (unsigned short) buf[5] << 8 | buf[4];
  audioInformation.audioSampleRate =  (unsigned short) buf[7] << 8 | buf[6];
  audioInformation.audioPsFlag =  buf[8] >> 2 & 1;
  audioInformation.audioSbrFlag = buf[8] >> 1 & 1;
  audioInformation.audioMode =    buf[8] & 1;
  audioInformation.audioDrcGain = buf[9] & 1;

  return audioInformation;
}

dabComponentTechnicalInformation_t dabGetComponentTechnicalInformation(unsigned long &serviceId, unsigned long &componentId)
{
  dabComponentTechnicalInformation_t dabComponentTechnicalInformation;
  unsigned char cmd[1] = {DAB_GET_SUBCHAN_INFO};
  unsigned char arg[11];
  unsigned char buf[12];
  //initalize buffer
  for (unsigned short i = 0; i < 12; i++) buf[i] = 0xff;

  arg[0] = 0x00;
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
  delayMicroseconds(10000);
  delayMicroseconds(10000);
  readReply(buf, sizeof(buf));

  dabComponentTechnicalInformation.serviceMode    = buf[4];
  dabComponentTechnicalInformation.protectionInfo = buf[5];
  dabComponentTechnicalInformation.bitRate        = (unsigned short) buf[7] << 8 | buf[6];
  dabComponentTechnicalInformation.numberCU       = (unsigned short) buf[9] << 8 | buf[8];
  dabComponentTechnicalInformation.addressCU      = (unsigned short) buf[11] << 8 | buf[10];

  return dabComponentTechnicalInformation;
}

//0xBF DAB_GET_FREQ_INFO Gets ensemble frequency information list
unsigned long dabGetEnsembleFrequencyInformationList()
{
  unsigned char cmd[1] = {DAB_GET_FREQ_INFO};
  unsigned char arg[1] = {0};
  unsigned char buf[8];
  //initalize buffer
  for (unsigned short i = 0; i < 8; i++) buf[i] = 0xff;

  writeCommandArgument(cmd, sizeof(cmd), arg, sizeof(arg));
  delayMicroseconds(10000);
  delayMicroseconds(10000);
  readReply(buf, sizeof(buf));

  return (unsigned long)buf[7] << 24 | (unsigned long)buf[7] << 16 | (unsigned long)buf[7] << 8 | buf[4];
}

serviceInformation_t readServiceInformation(unsigned long &serviceId)
{
  serviceInformation_t dabServiceInformation;
  unsigned char cmd[1] = {DAB_GET_SERVICE_INFO};
  unsigned char arg[7];
  unsigned char buf[26];
  //initalize buffer
  for (unsigned short i = 0; i < 26; i++) buf[i] = 0xff;

  arg[0] = 0;
  arg[1] = 0;
  arg[2] = 0;
  arg[3] = serviceId & 0xFF;
  arg[4] = serviceId >> 8 & 0xFF;
  arg[5] = serviceId >> 16 & 0xFF;
  arg[6] = serviceId >> 24 & 0xFF;

  writeCommandArgument(cmd, sizeof(cmd), arg, sizeof(arg));
  delayMicroseconds(10000);
  delayMicroseconds(10000);
  readReply(buf, sizeof(buf));

  //serviceInfo1
  dabServiceInformation.serviceLinkingInfoFlag = buf[4] >> 6 & 1;
  dabServiceInformation.pType                  = buf[4] >> 1 & 0x1F;
  dabServiceInformation.pdFlag                 = buf[4] & 1;

  //serviceInfo2
  dabServiceInformation.localFlag              = buf[5] >> 7 & 1;
  dabServiceInformation.caId                   = buf[5] >> 4 & 0x7;
  dabServiceInformation.numComponents          = buf[5] & 0xF;

  //serviceInfo3
  dabServiceInformation.characterSet           = buf[6] & 0xF;
  dabServiceInformation.ecc                    = buf[7];

  //Service Label
  memcpy(dabServiceInformation.serviceLabel, &buf[8], 16); // Bytes 16...36
  dabServiceInformation.serviceLabel[16] = '\0';

  dabServiceInformation.abbreviationMask       = (unsigned short) buf[25] << 8 | buf[24];

  return dabServiceInformation;
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
  unsigned char numFrequencies = readNumberFrequencies();

  //Scan trough index table - start with index 0
  for (unsigned char index = 0; index < numFrequencies; index++)
  {
    tuneIndex(index);
    //Check receive signal quality
    rsqInformation = readRsqInformation();
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
unsigned char tune(bool up)
{
  //read actual index
  rsqInformation_t rsqInformation = readRsqInformation();

  //remember actual index
  uint8_t index = rsqInformation.index;
  //get actual number of indices
  uint8_t numberIndices = readNumberFrequencies();

  if (up)
  {
    //wrap around at _numberIndices
    if (index == numberIndices - 1) index = 0;
    //only increase if smaller 48 or _numberIndices
    else if (index < numberIndices - 1) index++;
  }
  else
  {
    //wrap around at 0 to numberIndices -1
    if (index == 0)  index = numberIndices - 1;
    //only decrease if min 1
    else if (index >= 1)  index--;
  }

  //tune index
  tuneIndex(index);

  //read result
  rsqInformation = readRsqInformation();
  return rsqInformation.index;
}

//Read number of frequencies
unsigned char readNumberFrequencies()
{
  unsigned char cmd[2]  = {DAB_GET_FREQ_LIST, 0};
  unsigned char buf[12] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

  writeCommandArgument(cmd, sizeof(cmd));
  delayMicroseconds(DURATION_10000_MIKROS);
  readReply(buf, sizeof(buf));

  //Number of frequencies in table
  unsigned char numberFrequencies = buf[4];

  //Validity ?
  if (numberFrequencies > MAX_INDEX) numberFrequencies = 0;

  return numberFrequencies;
}

//Scan next valid frequency
unsigned char scan(bool up)
{
  //remember start index
  rsqInformation_t rsqInformation = readRsqInformation();
  unsigned char indexStart = rsqInformation.index;

  do
  {
    //tune index
    if (up) tune(true);
    else(tune(false));

    //get receive quality
    rsqInformation = readRsqInformation();
    Serial.print(F("Index:\t"));
    Serial.println(rsqInformation.index);

    //DAB found and valid if threshold for dab detected and greater than 4
    if ((rsqInformation.fastDect > 4) && rsqInformation.valid == 1)
    {
      //nothing found
      Serial.println(F("Valid index found"));
      return rsqInformation.index;
    }
  }
  //around
  while (rsqInformation.index != indexStart);
  Serial.println(F("Nothing found"));
  //nothing found
  return rsqInformation.index;
}

//Test varactor tuning capacitor
unsigned short dabTestVaractorCap(unsigned char index, unsigned char injection, unsigned char numberMeasurments)
{
  unsigned short rssi = 0;
  /*max rssi*/
  unsigned short rssiMax = 0;
  /*number of measurements*/
  unsigned char numberVar = 129;
  /*list of rssi measurments*/
  unsigned short varCapList[numberVar] = {0};

  unsigned char varCapMax = 0;

  //count all values from 0 ..128
  for (unsigned char varCap = 0; varCap < numberVar; varCap++)
  {
    //Serial.print(F("Varactor capacity: "));
    //Serial.println(varCap);
    //Serial.print(((varCap - 1) * 0.25));
    //Serial.print(F("4.2f pF \n",(cap-1)*0.25);
    //31.75 pF Max
    tuneIndex(index, varCap, injection);
    
    rssi = readRssi();

    /*save in list*/
    varCapList[varCap] = rssi;

    //check if rssi better
    if (rssi > rssiMax)
    {
      //save new values
      rssiMax = rssi;
      varCapMax = varCap;
    }
    //Serial.print(F("Rssi Avgerage: "));
    Serial.print(varCap);
    Serial.print(",");
    Serial.println(rssi / 256.00);
    //Serial.println();

  }

  Serial.println();
  Serial.print(F("Index:\t"));
  Serial.println(index);
  Serial.print(F("Max Rssi:\t"));
  Serial.print(rssiMax / 256.0);
  Serial.println(F(" dBuV"));
  Serial.print(F("Varactor count:"));
  Serial.print(varCapMax);
  Serial.print(F(" pf:"));
  Serial.println(varCapMax / 4);
  Serial.println();

  return varCapMax;
}
