/*Testing*/
//Print device function names
//#define DEBUG_FUNCTION_NAMES_DEVICE
//Print function names DAB
//#define DEBUG_FUNCTION_NAMES_DAB

//Print overview for flash memory
//#define DEBUG_FLASH
//Print detailed messages for flash memory package transfer
//#define DEBUG_FLASH2
/*Print detailed info about dabGetFrequencyTable() and dabSetFrequencyTable()*/
//#define DEBUG_DAB_FREQUENCY_TABLE
/*Print detailed info about dabParseEnsemble()*/
//#define DEBUG_DAB_PARSE_ENSEMBLE

/*Calculate available free RAM*/
#include "ramTesting.h"
#define DEBUG_FREE_RAM

/*to test for error handling*/
#include "errno.h"

/*Tuner shield DAB*/
#include "SI468x.h"

/*SPI communication layer*/
#include "comDriverSpi_0.1.0.h"
ComDriverSpi tuner(pinSlaveSelectTuner);

/*properties of tuner circuit*/
#include "properties.h"

/*flash functions*/
#include "flashSst26_0.2.0.h"

/*Serial Monitor Print Functions*/
#include "printSerial.h"


/*Global variables of device*/
/*Image*/
unsigned char deviceImage = 0;

/*Firmware Information*/
struct deviceFirmwareInformation_t deviceFirmwareInformation = {0, 0, 0, 0, 0, 0, 0, 0};

/*Property id*/
unsigned short devicePropertyId = 0;

/*Property value*/
long devicePropertyValue = 0;

/*Receive signal strength indicator*/
unsigned short deviceRssi = 0;

/*Tuning capacitor value*/
unsigned short varCap = 0;

/*Device status information*/
struct deviceStatusInformation_t deviceStatusInformation = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

/*Device part number*/
struct devicePartNumber_t devicePartNumber = {0, 0, 0};

/*Device power up arguments*/
struct devicePowerUpArguments_t devicePowerUpArguments = {

  //CLK_MODE[5:4]
  //devicePowerUpArguments.clockMode
  //0 //Oscillator and buffer are powered down.
  1, //Reference clock generator is in crystal mode.
  //2 //Oscillator is off and circuit acts as single ended buffer.
  //3 //Oscillator is off and circuit acts as differential buffer.

  //TR_SIZE[3:0]
  //devicePowerUpArguments.trSize
  //Range 0-15
  //TR_SIZE(19,2MHz)=  7 see table
  //TR_SIZE(30MHz)  = 11
  //TR_SIZE = lowerTR_SIZE + (higherTR_SIZE â€“ lowerTR_SIZE) x (f â€“ fLower)/(fHigher â€“ fLower)
  //TR_SIZE(30MHz)   =  0x0A + (0x0F - 0x0A) x (30 - 27)/(37,2 - 27) = 0x0A + 0x04 x (3/10,2) = 0x0B = 11
  11,//ok
  //15,//ok

  //IBIAS[6:0]
  //devicePowerUpArguments.iBiasStart
  //Range 0-127, 10 uA steps, 0 to 1270 uA
  127,//ok
  //60//ok
  //30//ok
  //5//ok
  //1//ok

  //XTAL Frequency in Hz
  //devicePowerUpArguments.xtalFreq
  //The supported crystal frequencies are: 6,12,18,24,30 MHz [5.4 MHz - 6.6 MHz]
  //[10.8 MHz - 13.2 MHz] [16.8 MHz - 19.8 MHz] [21.6 MHz - 26.4 MHz] [27 MHz - 46.2 MHz]
  //Range 5,4 - 46,2 MHz
  //19200000UL;
  //30000002UL//ok
  30000000UL,//ok
  //29999996UL//ok

  //devicePowerUpArguments.cTune
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

  //devicePowerUpArguments.cTune
  //63,//ok Max
  48,//ok
  //24,//ok
  //12,//nok

  //IBIAS_RUN[6:0]
  //devicePowerUpArguments.iBiasRun
  //To reduce the impact of appearance of unwanted spurs in tuner chain for,
  //it is recommended to reduce the biasing current by 2x
  //If set to 0, will use the same value as iBiasStart
  //Range 0-127, 10 uA steps, 10 to 1270 uA

  //devicePowerUpArguments.iBiasRun = 127;//ok
  60//ok
  //devicePowerUpArguments.iBiasRun = 30;//ok
  //devicePowerUpArguments.iBiasRun = 10;//ok
  //devicePowerUpArguments.iBiasRun = 5;//nok
  //devicePowerUpArguments.iBiasRun = 0;//ok

};

/*Device property value list*/
struct devicePropertyValueList_t devicePropertyValueList[numberDeviceProperties] =
{
  //General Properties
  //2xReserved DEVNTIEN 5xReserved CTSIEN ERR_CMDIEN DACQIEN DSRVIEN Reserved Reserved Reserved STCIEN
  //Default: 0
  {INT_CTL_ENABLE,  1 << 7 | 1 << 6 | 1 << 5 | 1 << 4 | 1},
  //2xReserved DEVNTREP 7xReserved DACQREP DSRVREP 3xReserved STCREP
  //Default: 0
  {INT_CTL_REPEAT, 0},

  //{AUDIO_ANALOG_VOLUME, 63},
  {AUDIO_ANALOG_VOLUME, 55},
  {AUDIO_MUTE, 0},
  {AUDIO_OUTPUT_CONFIG, 0},

  //Digital Output properties
  //MASTER 14xReserved
  {DIGITAL_IO_OUTPUT_SELECT, (unsigned short)1 << 15},
  {DIGITAL_IO_OUTPUT_SAMPLE_RATE, 48000},
  {DIGITAL_IO_OUTPUT_FORMAT, 0x1800},
  {DIGITAL_IO_OUTPUT_FORMAT_OVERRIDES_1, 0},
  {DIGITAL_IO_OUTPUT_FORMAT_OVERRIDES_2, 0},
  {DIGITAL_IO_OUTPUT_FORMAT_OVERRIDES_3, 0},
  {DIGITAL_IO_OUTPUT_FORMAT_OVERRIDES_4, 0},

  //INTBOUTEN Reserved I2SOUTEN DACOUTEN
  {PIN_CONFIG_ENABLE, (unsigned short) 1 << 15 | 1},

  //WAKE TONE Properties
  {WAKE_TONE_ENABLE, 1},
  {WAKE_TONE_PERIOD, 250},//0 , 50 ...2000
  {WAKE_TONE_FREQ, 750},
  {WAKE_TONE_AMPLITUDE, 8},//0...31

  {TEST_BER_CONFIG, 0},
  {HD_TEST_DEBUG_AUDIO, 0}
};


//Global variables DAB definitions
//Frequency table - dynamic memory allocation
long unsigned* dabFreqTable = NULL;
//Available number of frequencies in table
unsigned char dabNumFreq = 0;
//Maximal number of frequencies in table
const unsigned char dabMaxNumFreq = 48;

//Actual index
unsigned char dabIndex = 2;//CHAN_5C = 178352;//DR Deutschland
//unsigned char dabIndex = 25; //CHAN_11A = 216928;//SWR RP
//unsigned char dabIndex = 28; //CHAN_11D = 220352;//DR Hessen

//Actual Digital Service
//sunshine live
unsigned long dabServiceId = 0x15DC;
//Actual Digital Component
unsigned long dabComponentId = 0x0015;
//Radio TEDDY
//dabServiceId = 0x1B2E;
//dabComponentId = 0xD;
//SWR3
//dabServiceId = 0xD3A3;
//dabComponentId = 0x4;

//Table of valid frequencies
unsigned char* dabValidIndexList = NULL;
//Number of valid indices
unsigned char dabNumValidIndex = 0;

//Actual Service is running
bool dabServiceRunning = false;
//Audio is muted
bool mute = false;
//Audio volume
unsigned char volume = 55;


//Actual Service List Size
unsigned short dabListSize = 0;
//Actual Service List Version
unsigned short dabListVersion = 0;
//Actual Number of Services
unsigned char dabNumServices = 0;

/*Pointer to actual Ensemble List*/
struct dabEnsemble_t* dabEnsembleList = NULL;
/*Actual Service in ensemble list*/
int dabActualService = 0;
/*Ensemble information*/

struct dabEnsembleInformation_t dabEnsembleInformation = { 0, "0", 0, 0 , 0};
/*Service Information*/
struct dabServiceInformation_t dabServiceInformation;
/*Component Information*/
struct dabComponentInformation_t dabComponentInformation;

//Event information
struct dabEventInformation_t dabEventInformation;
//Index status information
struct dabRsqInformation_t dabRsqInformation;
//Component Technical Information
struct dabComponentTechnicalInformation_t dabComponentTechnicalInformation;
//Audio Component Information
struct dabAudioComponentInformation_t dabAudioComponentInformation;

/*Service list header with information about listsize, version and num of services*/
struct dabServiceListHeader_t dabServiceListHeader;

//DAB service data block
struct dabServiceData_t dabServiceData;

//DAB property value list
struct dabPropertyValueList_t dabPropertyValueList[dabNumberProperties] =
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

//Loads data from Flash Memory into host and than into device
bool hostLoadDataTuner(unsigned long startAddress, unsigned long fileSize)
{
#ifdef DEBUG_FUNCTION_NAMES_DEVICE
  Serial.println(F("hostLoadDataTuner()"));
#endif

  /*
     Max Package size of tuner circuit 0x1000
     Defines the size of packages load from flash to Arduino to device
     Careful because of RAM effect
     Tested with
     0x100 ok
     0x200 ok
     0x300 nok
  */
  const unsigned short  maxPackageSize = 0x200;
  unsigned int packages = fileSize / maxPackageSize;
  unsigned char dataBuffer[maxPackageSize];

  //Flash Memory
  Sst26 flashSst26(pinSlaveSelectFlash);

  Sst26::id_t id = flashSst26.readId();
  Serial.print(F("Jedec Id:\t"));
  Serial.println(id.jedecId, HEX);

  Sst26::uniqueId_t idUnique = flashSst26.readUniqueId();
  Serial.print(F("Unique Id:\t0x"));
  Serial.print(idUnique.uniqueIdUpper, HEX);
  Serial.println(idUnique.uniqueIdLower, HEX);
  Serial.println();

  deviceLoadInit();

#ifdef DEBUG_FLASH
  Serial.println();
  Serial.print(F("Start Adress: 0x"));
  Serial.println(startAddress, HEX);
#endif

  for (unsigned long i = 0 ; i <= packages; i++) //force i as long important //<=cycles +1 cylces for the rest
  {
    //initalize buffer with 0xff
    for (unsigned short i = 0; i < maxPackageSize; i++) dataBuffer[i] = 0xff;

    flashSst26.readData(startAddress + i * maxPackageSize, dataBuffer, maxPackageSize);

#ifdef DEBUG_FLASH2
    for (unsigned short j = 0 ; j < maxPackageSize; j++)
    {
      Serial.println(startAddress + i * maxPackageSize, HEX);
      //Serial.print(F("Address: "));
      Serial.print(startAddress + j, HEX);
      //Serial.print(F(" Byte: "));
      Serial.println(dataBuffer[j], HEX);
    }
#endif

    deviceHostLoad(dataBuffer, maxPackageSize);
  }

  //delay(tGeneralDelay);

  return true;
}

//Prints hex answer of chip
bool printHexStr(unsigned char str[], unsigned long len)
{
  char string[7];
  if (len == 0) return false;
  for (unsigned long i = 0; i < len; i++)
  {
    snprintf(string, 7, "0x%02x ", str[i]);
    Serial.print(string);
    delay(1);
  }
  Serial.println();//next line

  return true;
}

//Run setup functions before firmware
bool deviceSetUpPreFirmware()
{
  //Print version information
  serial::printVersionInformation();

  //Reset device
  deviceReset();

  //Print device status information
  serial::devicePrintStatus(deviceGetStatus());

  //Power up device
  devicePowerUp(devicePowerUpArguments);

  //Print device status information
  serial::devicePrintStatus(deviceGetStatus());

  return true;

}

//Run setup functions after loading firmware
bool deviceSetUpAfterFirmware()
{
  //Print device status information
  //serial::devicePrintStatus(deviceStatusInfo);

  //Boot device
  deviceBoot();

  //Print device status information
  serial::devicePrintStatus(deviceGetStatus());

  //Set device properties
  deviceSetAllProperties(devicePropertyValueList, numberDeviceProperties);

  //Get Device Image
  deviceGetImage(deviceImage);

  //Print Device Image
  serial::devicePrintImage(deviceImage);

  return true;
}

/*Initalize device*/
bool deviceInitalize()
{
  //SPI.begin();
  /*Tuner slave select pin*/
  //pinMode(pinSlaveSelectTuner, OUTPUT);
  //digitalWrite(pinSlaveSelectTuner, HIGH);
  /*Tuner reset pin*/
  pinMode(pinResetTuner, OUTPUT);
  digitalWrite(pinResetTuner, HIGH);
  /*Tuner interrupt pin*/
  pinMode(pinInterruptTuner, INPUT);
  //digitalWrite(pinInterruptTuner, HIGH);
  /*flash memory slave select pin*/
  pinMode(pinSlaveSelectFlash, OUTPUT);
  digitalWrite(pinSlaveSelectFlash, HIGH);

  return true;
}

/*Reset device*/
bool deviceReset(unsigned char resetPin, unsigned char durationReset, unsigned char timeAfterReset)
{
#ifdef DEBUG_FUNCTION_NAMES_DEVICE
  Serial.println(F("devReset"));
#endif

  digitalWrite(resetPin, LOW);
  delay(durationReset);
  digitalWrite(resetPin, HIGH);
  delay(timeAfterReset);

  return true;
}

/*Check if device is busy*/
bool deviceReady(unsigned char buf[], unsigned short len, unsigned short tDelay, bool printBuffer)
{
  unsigned char cmd[1] = {RD_REPLY};

  /*retry until device not busy anymore*/
  for (unsigned char retry = 0; retry < maxRetry; retry++)
  {
    /*delay before reading*/
    delay(tDelay);
    /*Read reply*/
    tuner.writeSpi(cmd, sizeof(cmd), ComDriverSpi::transferStart);
    tuner.readSpi(buf, len, ComDriverSpi::transferEnd);

    if (printBuffer)
    {
      printHexStr(buf, len);
    }
    /*Clear to send and no error*/
    if (buf[0] >> 6 == 0x02) break;
    if (retry == maxRetry)
    {
      printHexStr(buf, len);
      return false;
    }
  }
  return true;
}

/*Check if device has seek tune completed*/
bool deviceSeekTuneComplete(unsigned char buf[], unsigned short len, unsigned short tDelay, bool printBuffer)
{
  unsigned char cmd[1] = {RD_REPLY};

  /*retry until maxRetry*/
  for (unsigned char retry = 0; retry < maxRetry; retry++)
  {
    /*delay before reading*/
    delay(tDelay);
    tuner.writeSpi(cmd, sizeof(cmd), ComDriverSpi::transferStart);
    /*Read reply*/
    tuner.readSpi(buf, len, ComDriverSpi::transferEnd);

    if (printBuffer)
    {
      printHexStr(buf, len);
    }
    /*Clear to send (CTS) and Seek tune complete (STC) and no error*/
    if (((buf[0] & 1) == 1) && (buf[0] >> 6 == 0x02)) break;
    if (retry == maxRetry)
    {
      printHexStr(buf, len);
      return false;
    }
  }
  return true;
}

/*0x00 RD_REPLY Get Device Status Information*/
deviceStatusInformation_t deviceGetStatus()
{
#ifdef DEBUG_FUNCTION_NAMES_DEVICE
  Serial.println(RD_REPLY, HEX);
#endif
  /*
      RD_REPLY command must be called to return the status byte and data for the last command sent to the device.
      This command is also used to poll the status byte as needed. To poll the status byte, send the RD_REPLY
      command and read the status byte. This can be done regardless of the state of the CTS bit in the status register.
      Please refer to individual command descriptions for the format of returned data. RD_REPLY is a hardware
      command and can be issued while device is powered down. For commands where the size of the response is
      returned, the user should send the RD_REPLY command to read the SIZE first. Each time the RD_REPLY
      command is sent, the STAUS bytes will still be returned.
  */
  unsigned char cmd[1] = {RD_REPLY};
  unsigned char buf[4] = {0xff, 0xff, 0xff, 0xff};

  tuner.writeSpi(cmd, sizeof(cmd), ComDriverSpi::transferStart);

  tuner.readSpi(buf, sizeof(buf), ComDriverSpi::transferEnd);

  deviceStatusInformation.cts      = buf[0] >> 7 & 1;
  deviceStatusInformation.cmdErr   = buf[0] >> 6 & 1;
  deviceStatusInformation.dacqInt  = buf[0] >> 5 & 1;
  deviceStatusInformation.dsrvInt  = buf[0] >> 4 & 1;
  deviceStatusInformation.stcInt   = buf[0] & 1;
  deviceStatusInformation.devNtInt = buf[1] >> 5 & 1;
  deviceStatusInformation.pUpState = buf[3] >> 6 & 3;
  deviceStatusInformation.dspErr   = buf[3] >> 4 & 1;
  deviceStatusInformation.repOfErr = buf[3] >> 3 & 1;
  deviceStatusInformation.cmdOfErr = buf[3] >> 2 & 1;
  deviceStatusInformation.arbErr   = buf[3] >> 1 & 1;
  deviceStatusInformation.nonRecErr = buf[3] & 1;

  /*error*/
  if (deviceStatusInformation.cmdErr == 1)
  {
    unsigned char cmd[1] = {RD_REPLY};
    unsigned char errBuf[5] = {0xff, 0xff, 0xff, 0xff, 0xff};
    tuner.writeSpi(cmd, sizeof(cmd), ComDriverSpi::transferStart);
    tuner.readSpi(buf, sizeof(buf), ComDriverSpi::transferEnd);
    printHexStr(errBuf, sizeof(errBuf));
  }

  return deviceStatusInformation;
}

/*Sets the power up arguments*/
bool deviceSetPowerUpArguments(devicePowerUpArguments_t &devicePowerUpArguments)
{
#ifdef DEBUG_FUNCTION_NAMES_DEVICE
  Serial.println(F("devSetPowerUpArguments"));
#endif

  //CLK_MODE[5:4]
  //0 : Oscillator and buffer are powered down.
  //1 : Reference clock generator is in crystal mode.
  //2 : Oscillator is off and circuit acts as single ended buffer.
  //3 : Oscillator is off and circuit acts as differential buffer.
  devicePowerUpArguments.clockMode = 1;

  //TR_SIZE[3:0]
  //Range 0-15
  //TR_SIZE(19,2MHz)=  7 see table
  //TR_SIZE(30MHz)  = 11
  //TR_SIZE = lowerTR_SIZE + (higherTR_SIZE â€“ lowerTR_SIZE) x (f â€“ fLower)/(fHigher â€“ fLower)
  //TR_SIZE(30MHz)   =  0x0A + (0x0F - 0x0A) x (30 - 27)/(37,2 - 27) = 0x0A + 0x04 x (3/10,2) = 0x0B = 11
  devicePowerUpArguments.trSize = 11;//ok
  //devicePowerUpArguments.trSize = 15;//ok

  //IBIAS[6:0]
  //Range 0-127, 10 uA steps, 0 to 1270 uA
  devicePowerUpArguments.iBiasStart = 127;//ok
  //devicePowerUpArguments.iBiasStart = 60;//ok
  //devicePowerUpArguments.iBiasStart = 30;//ok
  //devicePowerUpArguments.iBiasStart = 5;//ok
  //devicePowerUpArguments.iBiasStart = 1;//ok

  //XTAL Frequency in Hz. The supported crystal frequencies are: 6,12,18,24,30 MHz [5.4 MHz - 6.6 MHz]
  //[10.8 MHz - 13.2 MHz] [16.8 MHz - 19.8 MHz] [21.6 MHz - 26.4 MHz] [27 MHz - 46.2 MHz]
  //Range 5,4 - 46,2 MHz
  //devicePowerUpArguments.xtalFreq = 19200000UL;
  devicePowerUpArguments.xtalFreq = 30000000UL;//ok
  //devicePowerUpArguments.xtalFreq = 30000002UL;//ok
  //devicePowerUpArguments.xtalFreq = 29999996UL;//ok

  //CTUN programs a pair of internal tuning capacitances
  //Cload       = capacity of crystal from manufacturers datasheet
  //Cparasitic  = capacity from pcb etc.
  //Cx          = external discrete capacitance if Cload >14pF

  //CTUN= 2*(Cload - Cparasitic)- Cx
  //63 steps for CTUN starting at 0x00...0x3F = 0...63 = 0-24pF
  //0,375 pF per step

  //Cload = 18pF, Cparasitic = 1pF, Cx=0, CTUN = 34pF = 91
  //Maximum CTUN = 24pF = 63
  //Cload = 13pF, Cparasitic = 1pF, Cx=0, CTUN = 24pF = 63
  //Cload = 10pF, Cparasitic = 1pF, Cx=0, CTUN = 18pF = 48

  //Crystal with Cload <=13pF is necessary

  //devicePowerUpArguments.cTune = 63;//Max
  devicePowerUpArguments.cTune = 48;//ok
  //devicePowerUpArguments.cTune = 24;//ok
  //devicePowerUpArguments.cTune = 12;//nok

  //IBIAS_RUN[6:0]
  //To reduce the impact of appearance of unwanted spurs in tuner chain for,
  //it is recommended to reduce the biasing current by 2x
  //If set to 0, will use the same value as iBiasStart
  //Range 0-127, 10 uA steps, 10 to 1270 uA

  //devicePowerUpArguments.iBiasRun = 127;//ok
  devicePowerUpArguments.iBiasRun = 60;//ok
  //devicePowerUpArguments.iBiasRun = 30;//ok
  //devicePowerUpArguments.iBiasRun = 10;//ok
  //devicePowerUpArguments.iBiasRun = 5;//nok
  //devicePowerUpArguments.iBiasRun = 0;//ok

  return true;
}


/*0x01 POWER_UP Power-up the device and set system settings*/
bool devicePowerUp(devicePowerUpArguments_t devicePowerUpArguments, unsigned char ctsInterruptEnabled)
{
  /*
    The POWER_UP initiates the boot process to move the device from power down to power up mode. There are two
    possible boot scenarios: Host image load and FLASH image load. When the host is loading the image the host first
    executes the POWER_UP command to set the system settings REF_CLK, etc). A LOAD_INIT command then
    prepares the bootloader to receive a new image. After the LOAD_INIT command, using the HOST_LOAD
    command loads the image into the device RAM. After the RAM is loaded the host issues the BOOT command.
    When booting a FLASH image the host issues the POWER_UP command to set the system settings. Then issues
    the FLASH_LOAD command to select and load the image from FLASH. Once the image is loaded the host sends
    the BOOT command to boot the application. Power-up is complete when the CTS bit is set. This command may
    only be sent while the device is powered down. Note: FLASH_LOAD is not supported in A0A or A0B revisions.
  */
#ifdef DEBUG_FUNCTION_NAMES_DEVICE
  Serial.println(POWER_UP, HEX);
#endif

  unsigned char cmd[1] = {POWER_UP};
  unsigned char data[15];
  unsigned char buf[4] = {0xff, 0xff, 0xff, 0xff};

  data[0] = ctsInterruptEnabled & 1;                      //Arg 1   CTSIEN 0 0 0 0 0 0 0  Clear to Sent Interrupt Enabled
  data[1] = devicePowerUpArguments.clockMode << 4 | devicePowerUpArguments.trSize; //Arg 2 0 0 CLK_MODE[1:0] TR_SIZE[3:0]
  data[2] = devicePowerUpArguments.iBiasStart;            //ARG3 0 IBIAS[6:0]
  data[3] = devicePowerUpArguments.xtalFreq & 0xFF;       //Arg4 XTAL_FREQ[7:0]
  data[4] = devicePowerUpArguments.xtalFreq >> 8 & 0xFF;  //Arg5 XTAL_FREQ[15:8]
  data[5] = devicePowerUpArguments.xtalFreq >> 16 & 0xFF; //Agr6 XTAL_FREQ[23:16]
  data[6] = devicePowerUpArguments.xtalFreq >> 24 & 0xFF; //Agr7 XTAL_FREQ[31:24]
  data[7] = devicePowerUpArguments.cTune;                 //Arg8 0 0 CTUN[5:0]
  data[8] = 1 << 4;                                       //ARG9 0 0 0 1 0 0 0 0
  data[9] = 0;                                            //Arg10 0
  data[10] = 0;                                           //Arg11 0
  data[11] = 0;                                           //Arg12 0
  data[12] = devicePowerUpArguments.iBiasRun;             //Arg13
  data[13] = 0;                                           //Arg14 0
  data[14] = 0;                                           //Arg15 0

  if (!tuner.writeSpi(cmd, sizeof(cmd), ComDriverSpi::transferStart)) return false;
  if (!tuner.writeSpi(data, sizeof(data), ComDriverSpi::transferEnd)) return false;

  if (!deviceReady(buf, sizeof(buf))) return false;

  return true;
}

/*0x04 HOST_LOAD Loads an image from HOST over command interface*/
bool deviceHostLoad(unsigned char package[], unsigned short len)
{
  /*
    HOST_LOAD loads an image from HOST over command interface. It sends up to 4096 bytes of application image
    to the bootloader. Note: This command is much more efficient when the image is sent as multiples of 4 bytes. The
    command is complete when the CTS bit (and optional interrupt) is set. The ERR bit (and optional interrupt) is set if
    an invalid argument is sent. Note that only a single interrupt occurs if both the CTS and ERR bits are set. The
    command may only be sent in powerup mode.
  */

#ifdef DEBUG_FUNCTION_NAMES_DEVICE
  //Serial.println(HOST_LOAD, HEX);
#endif
  /*
      4096 bytes of application image
  */
  if (len > 0x1000) return false;

  unsigned char cmd[4];

  cmd[0] = HOST_LOAD;
  cmd[1] = 0x00; /*ARG1*/
  cmd[2] = 0x00; /*ARG2*/
  cmd[3] = 0x00; /*ARG3*/

  if (!tuner.writeSpi(cmd, sizeof(cmd), ComDriverSpi::transferStart)) return false;
  if (!tuner.writeSpi(package, len, ComDriverSpi::transferEnd)) return false;

  //if (!deviceReady(buf, sizeof(buf))) return false;

  return true;
}

/*0x05 FLASH_LOAD Loads an image from external FLASH over secondary SPI bus*/
bool deviceFlashLoad(unsigned long address, unsigned char subCommand)
{
  /*
    FLASH_LOAD loads the firmware image from an externally attached SPI flash over the secondary SPI bus. The
    image must be contiguous on the flash. The command is complete when the CTS bit (and optional interrupt) is set.
    The ERR bit (and optional interrupt) is set if an invalid argument is sent. Note that only a single interrupt occurs if
    both the CTS and ERR bits are set. The command may only be sent in powerup mode. Note: FLASH_LOAD is only
    supported after patching the bootloader.
  */

#ifdef DEBUG_FUNCTION_NAMES_DEVICE
  Serial.println(FLASH_LOAD, HEX);
#endif

  unsigned char cmd[1] = {FLASH_LOAD};
  unsigned char data[11];
  unsigned char buf[4] = {0xff, 0xff, 0xff, 0xff};

  data[0] = subCommand; //SUBCMD1
  data[1] = 0x00;       //SUBCMD2
  data[2] = 0x00;       //SUBCMD3
  data[3] = address & 0xFF;      //ARG4 FLASH_ADDR [7:0]
  data[4] = address >> 8 & 0xFF; //ARG5 FLASH_ADDR [15:8]
  data[5] = address >> 16 & 0xFF;//ARG6 FLASH_ADDR [23:16]
  data[6] = address >> 24 & 0xFF;//ARG7 FLASH_ADDR [31:24]
  data[7] = 0x00;   //ARG8 0x00
  data[8] = 0x00;   //ARG9 0x00
  data[9] = 0x00;   //ARG10 0x00
  data[10] = 0x00;  //ARG11 0x00

  if (!tuner.writeSpi(cmd, sizeof(cmd), ComDriverSpi::transferStart)) return false;
  if (!tuner.writeSpi(data, sizeof(data), ComDriverSpi::transferEnd)) return false;

  if (!deviceReady(buf, sizeof(buf))) return false;

  return true;
}

/*0x06 LOAD_INIT Prepares the bootloader to receive a new image*/
bool deviceLoadInit()
{
  /*
    LOAD_INIT prepares the bootloader to receive a new image. It will force the bootloader state to waiting for a new
    LOAD command (HOST_LOAD or FLASH_LOAD.) LOAD_INIT command must always be sent prior to a
    HOST_LOAD or a FLASH_LOAD command. The command is complete when the CTS bit (and optional interrupt)
    is set. The ERR bit (and optional interrupt) is set if an invalid argument is sent. Note that only a single interrupt
    occurs if both the CTS and ERR bits are set. The command may only be sent in powerup mode.
  */

#ifdef DEBUG_FUNCTION_NAMES_DEVICE
  Serial.println(LOAD_INIT, HEX);
#endif

  unsigned char cmd[2];
  unsigned char buf[4] = {0xff, 0xff, 0xff, 0xff};

  cmd[0] = LOAD_INIT;
  cmd[1] = 0x00;

  if (!tuner.writeSpi(cmd, sizeof(cmd))) return false;

  if (!deviceReady(buf, sizeof(buf))) return false;

  return true;
}

/*0x07 BOOT Boots the image currently loaded in RAM*/
bool deviceBoot()
{
  /*
    BOOT command boots the image currently loaded in RAM. The command is complete when the CTS bit (and
    optional interrupt) is set. The ERR bit (and optional interrupt) is set if an invalid argument is sent. Note that only a
    single interrupt occurs if both the CTS and ERR bits are set. The command may only be sent in powerup mode.
  */

  unsigned char cmd[2];
  unsigned char buf[5] = {0xff, 0xff, 0xff, 0xff, 0xff};

#ifdef DEBUG_FUNCTION_NAMES_DEVICE
  Serial.println(BOOT, HEX);
#endif

  cmd[0] = BOOT;
  cmd[1] = 0;

  if (!tuner.writeSpi(cmd, sizeof(cmd))) return false;

  if (!deviceReady(buf, sizeof(buf), tDeviceBoot)) return false;

  return true;
}

/*0x08 GET_PART_INFO Get Device Part Number*/
bool deviceGetPartNumber(devicePartNumber_t &devicePartNumber)
{
  /*
    GET_PART_INFO reports basic information about the device such as Part Number, Part Version, ROM ID, etc.
    This command will hold CTS until the reply is available. The command is complete when the CTS bit (and optional
    interrupt) is set. The ERR bit (and optional interrupt) is set if an invalid argument is sent. Note that only a single
    interrupt occurs if both the CTS and ERR bits are set. The command may only be sent in powerup mode. Note:
    GET_PART_INFO command is not supported in firmware revision A0A.
  */
#ifdef DEBUG_FUNCTION_NAMES_DEVICE
  Serial.println(GET_PART_INFO, HEX);
#endif

  unsigned char cmd[2];
  unsigned char buf[23] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

  cmd[0] = GET_PART_INFO;
  cmd[1] = 0;

  if (!tuner.writeSpi(cmd, sizeof(cmd))) return false;

  if (!deviceReady(buf, sizeof(buf))) return false;

  devicePartNumber.chipRev    = buf[4];
  devicePartNumber.romId      = buf[5];
  devicePartNumber.partNumber = buf[9] << 8 | buf[8];

  return true;
}

/*0x09 GET_SYS_STATE Get Device Image*/
bool deviceGetImage(unsigned char &deviceImage)
{
  /*
    GET_SYS_STATE reports basic system state information such as which mode is active; FM, DAB, etc. The
    command is complete when the CTS bit (and optional interrupt) is set. The ERR bit (and optional interrupt) is set if
    an invalid argument is sent. Note that only a single interrupt occurs if both the CTS and ERR bits are set. The
    command may only be sent in powerup mode. Note: GET_SYS_STATE command is not supported in firmware
    revision A0A.
  */
#ifdef DEBUG_FUNCTION_NAMES_DEVICE
  Serial.println(GET_SYS_STATE, HEX);
#endif

  unsigned char cmd[2];
  unsigned char buf[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

  cmd[0] = GET_SYS_STATE;
  cmd[1] = 0;

  if (!tuner.writeSpi(cmd, sizeof(cmd))) return false;

  if (!deviceReady(buf, sizeof(buf))) return false;

  deviceImage = buf[4];

  return true;
}

/*0x0A GET_POWER_UP_ARGS Reports basic information about the device such as arguments used during POWER_UP*/
devicePowerUpArguments_t deviceGetPowerUpArguments()
{
  /*
    GET_POWER_UP_ARGS reports basic information about the device such as which parameters were used during
    power up. This command will hold CTS until the reply is available. The command is complete when the CTS bit
    (and optional interrupt) is set. The ERR bit (and optional interrupt) is set if an invalid argument is sent. Note that
    only a single interrupt occurs if both the CTS and ERR bits are set. The command may only be sent in powerup
    mode.
  */

#ifdef DEBUG_FUNCTION_NAMES_DEVICE
  Serial.println(GET_POWER_UP_ARGS, HEX);
#endif

  unsigned char cmd[2];
  unsigned char buf[18] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

  cmd[0] = GET_POWER_UP_ARGS;
  cmd[1] = 0;

  tuner.writeSpi(cmd, sizeof(cmd));
  deviceReady(buf, sizeof(buf));

  devicePowerUpArguments.clockMode  = buf[6] >> 4 & 3;
  devicePowerUpArguments.trSize     = buf[6] & 0xF;
  devicePowerUpArguments.iBiasStart = buf[7] & 0x7F;
  devicePowerUpArguments.xtalFreq   = (unsigned long) buf[11] << 24 | (unsigned long)buf[10] << 16 | (unsigned long) buf[9] << 8 | buf[8];
  devicePowerUpArguments.cTune      = buf[12] & 0x3F;
  devicePowerUpArguments.iBiasRun   = buf[17] & 0x7F;

  return devicePowerUpArguments;
}

/*0x12 GET_FUNC_INFO Get Firmware Information*/
bool deviceGetFirmwareInformation(deviceFirmwareInformation_t &deviceFirmwareInfo)
{
  /*
    GET_FUNC_INFO returns the function revision number for currently loaded firmware (FMHD, AM etc.) as opposed
    to GET_PART_INFO command that provides the revision number for the combo firmware. For example,
    GET_PART_INFO would return A0B is the firmware revision while GET_FUNC_INFO would return 1.0.4 for FM
    function revision if the currently running firmware function is FM. The command is complete when the CTS bit (and
    optional interrupt) is set. The ERR bit (and optional interrupt) is set if an invalid argument is sent. Note that only a
    single interrupt occurs if both the CTS and ERR bits are set. The command may only be sent in powerup mode.
  */
#ifdef DEBUG_FUNCTION_NAMES_DEVICE
  Serial.println(GET_FUNC_INFO, HEX);
#endif

  unsigned char cmd[2];
  unsigned char buf[12] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

  cmd[0] = GET_FUNC_INFO;
  cmd[1] = 0;

  if (!tuner.writeSpi(cmd, sizeof(cmd))) return false;

  if (!deviceReady(buf, sizeof(buf))) return false;

  deviceFirmwareInfo.revisionNumberMajor = buf[4];
  deviceFirmwareInfo.revisionNumberMinor = buf[5];
  deviceFirmwareInfo.revisionNumberBuild = buf[6];
  deviceFirmwareInfo.noSvnFlag = buf[7] >> 7 & 1;
  deviceFirmwareInfo.location = buf[7] >> 4 & 2;
  deviceFirmwareInfo.mixedRevFlag = buf[7] >> 1 & 1;
  deviceFirmwareInfo.localModFlag = buf[7] & 1;
  deviceFirmwareInfo.svnId = buf[8] | (unsigned long)buf[9] << 8 | (unsigned long)buf[10] << 16 | (unsigned long)buf [11] << 24;

  return true;
}

/*0x13 SET_PROPERTY Sets the value of a property*/
bool deviceSetProperty(unsigned short devicePropertyId, unsigned short devicePropertyValue)
{
  /*
    SET_PROPERTY sets the value of a property. The command is complete when the CTS bit (and optional interrupt)
    is set. The ERR bit (and optional interrupt) is set if an invalid argument is sent. Note that only a single interrupt
    occurs if both the CTS and ERR bits are set. The command may only be sent in powerup mode.
  */

#ifdef DEBUG_FUNCTION_NAMES_DEVICE
  Serial.println(SET_PROPERTY, HEX);
#endif

  unsigned char cmd[1];
  unsigned char arg[5];
  unsigned char buf[4] = {0xff, 0xff, 0xff, 0xff};

  cmd[0] = SET_PROPERTY;

  arg[0] = 0x00;
  arg[1] = devicePropertyId & 0xff;
  arg[2] = devicePropertyId >> 8;
  arg[3] = devicePropertyValue & 0xff;
  arg[4] = devicePropertyValue >> 8;

  if (!tuner.writeSpi(cmd, sizeof(cmd), ComDriverSpi::transferStart)) return false;
  if (!tuner.writeSpi(arg, sizeof(arg), ComDriverSpi::transferEnd)) return false;

  if (!deviceReady(buf, sizeof(buf))) return false;

  return true;
}

/*0x14 GET_PROPERTY Retrieve the value of a property*/
bool deviceGetProperty(unsigned short devicePropertyId, unsigned short &devicePropertyValue, unsigned char count)
{
  /*
    GET_PROPERTY retrieves the value of a property or properties. The host may read as many properties as desired
    up to the end of a given property group. An attempt to read passed the end of the property group will result in zeros
    being read. The command is complete when the CTS bit (and optional interrupt) is set. The ERR bit (and optional
    interrupt) is set if an invalid argument is sent. Note that only a single interrupt occurs if both the CTS and ERR bits
    are set. The command may only be sent in powerup mode.
  */
#ifdef DEBUG_FUNCTION_NAMES_DEVICE
  Serial.println(GET_PROPERTY, HEX);
#endif

  unsigned char cmd[1];
  unsigned char arg[3];
  unsigned char buf[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

  cmd[0] = GET_PROPERTY;

  arg[0] = count & 0xFF;
  arg[1] = devicePropertyId & 0xff;
  arg[2] = devicePropertyId >> 8 & 0xff;

  if (!tuner.writeSpi(cmd, sizeof(cmd), ComDriverSpi::transferStart)) return false;
  if (!tuner.writeSpi(arg, sizeof(arg), ComDriverSpi::transferEnd)) return false;

  if (!deviceReady(buf, sizeof(buf))) return false;

  devicePropertyValue = (int)buf[5] << 8 | buf[4];

  return true;
}

/*0x15 WRITE_STORAGE Writes data to the on board storage area at a specified offset*/
bool deviceWriteStorage(unsigned short offset, unsigned char data[], unsigned char len)
{
  /*
    WRITE_STORAGE writes data to the no board storage area at the specified offset. the largest block of data that
    can be written at one time is 256 bytes. The command is complete when the CTS bit (and optional interrupt) is set.
    The ERR bit (and optional interrupt) is set if an invalid argument is sent. Note that only a single interrupt occurs if
    both the CTS and ERR bits are set. The command may only be sent in powerup mode.
  */
#ifdef DEBUG_FUNCTION_NAMES_DEVICE
  Serial.println(WRITE_STORAGE, HEX);
#endif

  unsigned char cmd[1] = {WRITE_STORAGE};
  unsigned char arg[8];
  unsigned char buf[4] = {0xff, 0xff, 0xff, 0xff};

  arg[0] = 0;             //0x00
  arg[1] = offset & 0xff; //ARG2 OFFSET[7:0]
  arg[2] = offset >> 8;   //ARG3 OFFSET[15:8]
  arg[3] = len & 0xff;    //ARG4 LENGTH[7:0]
  arg[4] = len >> 8;      //ARG5 LENGTH[15:8]
  arg[5] = 0;             //ARG6 0x00
  arg[6] = 0;             //ARG7 0x00

  if (!tuner.writeSpi(cmd, sizeof(cmd), ComDriverSpi::transferStart)) return false;
  if (!tuner.writeSpi(arg, sizeof(arg), ComDriverSpi::transferStart)) return false;

  if (!deviceReady(buf, sizeof(buf))) return false;
  if (!tuner.writeSpi(data, len, ComDriverSpi::transferEnd)) return false;

  if (!deviceReady(buf, sizeof(buf))) return false;

  return true;
}

/*0x16 READ_STORAGE Reads data from the on board storage area from a specified offset*/
bool deviceReadStorage(unsigned short offset, unsigned char data[], unsigned char len)
{
  /*
    READ_STORAGE reads data from the on board storage area from a specified offset. The command is complete
    when the CTS bit (and optional interrupt) is set. The ERR bit (and optional interrupt) is set if an invalid argument is
    sent. Note that only a single interrupt occurs if both the CTS and ERR bits are set. The command may only be sent
    in powerup mode.
  */
#ifdef DEBUG_FUNCTION_NAMES_DEVICE
  Serial.println(READ_STORAGE, HEX);
#endif

  unsigned char cmd[1];
  unsigned char arg[3];
  unsigned char buf[4] = {0xff, 0xff, 0xff, 0xff};

  cmd[0] = READ_STORAGE;

  arg[0] = 0;
  arg[1] = offset & 0xff;//ARG2 OFFSET[7:0]
  arg[2] = offset >> 8;  //ARG3 OFFSET[15:8]

  if (!tuner.writeSpi(cmd, sizeof(cmd), ComDriverSpi::transferStart)) return false;
  if (!tuner.writeSpi(arg, sizeof(arg), ComDriverSpi::transferEnd)) return false;

  if (!deviceReady(buf, sizeof(buf)))return false;

  tuner.readSpi(buf, sizeof(buf), ComDriverSpi::transferStart);
  tuner.readSpi(data, len, ComDriverSpi::transferEnd);

  return true;
}

/*0x97 HD_PLAY_ALERT_TONE Plays the HD Alert Tone*/
bool deviceAlertTone()
{
#ifdef DEBUG_FUNCTION_NAMES_DEVICE
  Serial.println(PLAY_ALERT_TONE, HEX);
#endif

  unsigned char cmd[1];
  unsigned char arg[1];
  unsigned char buf[4] = {0xff, 0xff, 0xff, 0xff};

  cmd[0] = PLAY_ALERT_TONE;
  arg[0] = 0;

  if (!tuner.writeSpi(cmd, sizeof(cmd), ComDriverSpi::transferStart)) return false;
  if (!tuner.writeSpi(arg, sizeof(arg), ComDriverSpi::transferEnd)) return false;

  if (!deviceReady(buf, sizeof(buf))) return false;

  return true;
}

/*0xE5 TEST_GET_RSSI returns the reported RSSI in 8.8 format*/
unsigned short deviceGetRssi()
{
  /*
    TEST_GET_RSSI returns the reported RSSI in 8.8 format. This command is used to help calibrate the frontend
    tracking circuit. It returns the RSSI value in dBuV to 1/256 of a dB. The command is complete when the CTS bit
    (and optional interrupt) is set. The ERR bit (and optional interrupt) is set if an invalid argument is sent. Note that
    only a single interrupt occurs if both the CTS and ERR bits are set. The command may only be sent in powerup
    mode. Note: TEST_GET_RSSI command is not supported in firmware revision A0A or A0B.
  */
#ifdef DEBUG_FUNCTION_NAMES_DEVICE
  Serial.println(TEST_GET_RSSI, HEX);
#endif
  unsigned short deviceRssi = 0;

  unsigned char cmd[1];
  unsigned char arg[1];
  unsigned char buf[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

  cmd[0] = TEST_GET_RSSI;
  arg[0] = 0;

  if (!tuner.writeSpi(cmd, sizeof(cmd), ComDriverSpi::transferStart)) return 0;
  if (!tuner.writeSpi(arg, sizeof(arg), ComDriverSpi::transferEnd)) return 0;

  if (!deviceReady(buf, sizeof(buf))) return 0;

  deviceRssi = buf[5] << 8 | buf[4];

  return deviceRssi;
}

/*Get all device property values*/
bool deviceGetAllProperties(struct devicePropertyValueList_t devicePropertyValueList[], unsigned char num)
{
#ifdef DEBUG_FUNCTION_NAMES_DEVICE
  Serial.println(F("deviceGetAllProperties()"));
#endif

  for (unsigned char i = 0; i < num; i++)
  {
    deviceGetProperty(devicePropertyValueList[i].devicePropertyId, devicePropertyValueList[i].devicePropertyValue);
  }

  return true;
}


/*Set all device properties*/
bool deviceSetAllProperties(struct devicePropertyValueList_t devicePropertyValueList[], unsigned char num)
{
#ifdef DEBUG_FUNCTION_NAMES_DEVICE
  Serial.println(F("deviceSetAllProperties()"));
#endif

  for (unsigned char i = 0; i < num; i++)
  {
    deviceSetProperty(devicePropertyValueList[i].devicePropertyId, devicePropertyValueList[i].devicePropertyValue);
  }

  return true;
}

/*0x81 START_DIGITAL_SERVICE Starts an audio or data service*/
bool dabStartDigitalService(unsigned long dabServiceId, unsigned long dabComponentId, unsigned char dabServiceType)
{
  /*
    START_DIGITAL_SERVICE starts an audio or data service. This command is used for DAB audio and data
    services. To determine what services exist in an ensemble please use the GET_DIGITAL_SERVICE_LIST
    command. In the case of starting an audio service, it is not required to stop a currently running audio service/
    program before starting a new one. The currently running audio service will be stopped automatically when the new
    service is requested. The command is complete when the CTS bit (and optional interrupt) is set. The ERR bit (and
    optional interrupt) is set if an invalid argument is sent. Note that only a single interrupt occurs if both the CTS and
    ERR bits are set. The command may only be sent in powerup mode.
  */

#ifdef DEBUG_FUNCTION_NAMES_DAB
  Serial.println(START_DIGITAL_SERVICE, HEX);
#endif

  unsigned char cmd[1];
  unsigned char arg[11];
  unsigned char buf[4] = {0xff, 0xff, 0xff, 0xff};

  cmd[0] = START_DIGITAL_SERVICE;

  arg[0] = dabServiceType & 1;
  arg[1] = 0x00;
  arg[2] = 0x00;
  arg[3] = dabServiceId & 0xFF;
  arg[4] = dabServiceId >> 8 & 0xFF;
  arg[5] = dabServiceId >> 16 & 0xFF;
  arg[6] = dabServiceId >> 24 & 0xFF;
  arg[7] = dabComponentId & 0xFF;
  arg[8] = dabComponentId >> 8 & 0xFF;
  arg[9] = dabComponentId >> 16 & 0xFF;
  arg[10] = dabComponentId >> 24 & 0xFF;

  if (!tuner.writeSpi(cmd, sizeof(cmd), ComDriverSpi::transferStart)) return false;
  if (!tuner.writeSpi(arg, sizeof(arg), ComDriverSpi::transferEnd)) return false;

  if (!deviceReady(buf, sizeof(buf), tDabGeneralDelay))return false;

  return true;
}

/*0x82 STOP_DIGITAL_SERVICE Stops an audio or data service*/
bool dabStopDigitalService(unsigned long dabServiceId, unsigned long dabComponentId, unsigned char dabServiceType)
{
  /*
    STOP_DIGITAL_SERVICE stops an audio or data service. The command is complete when the CTS bit (and
    optional interrupt) is set. The ERR bit (and optional interrupt) is set if an invalid argument is sent. Note that only a
    single interrupt occurs if both the CTS and ERR bits are set. The command may only be sent in powerup mode.
  */

#ifdef DEBUG_FUNCTION_NAMES_DAB
  Serial.println(STOP_DIGITAL_SERVICE, HEX);
#endif

  unsigned char cmd[1];
  unsigned char arg[11];
  unsigned char buf[4] = {0xff, 0xff, 0xff, 0xff};

  cmd[0] = STOP_DIGITAL_SERVICE;

  arg[0] = dabServiceType & 1;
  arg[1] = 0x00;
  arg[2] = 0x00;
  arg[3] = dabServiceId & 0xFF;
  arg[4] = dabServiceId >> 8 & 0xFF;
  arg[5] = dabServiceId >> 16 & 0xFF;
  arg[6] = dabServiceId >> 24 & 0xFF;
  arg[7] = dabComponentId & 0xFF;
  arg[8] = dabComponentId >> 8 & 0xFF;
  arg[9] = dabComponentId >> 16 & 0xFF;
  arg[10] = dabComponentId >> 24 & 0xFF;

  if (!tuner.writeSpi(cmd, sizeof(cmd), ComDriverSpi::transferStart)) return false;
  if (!tuner.writeSpi(arg, sizeof(arg), ComDriverSpi::transferEnd)) return false;

  if (!deviceReady(buf, sizeof(buf), tDabGeneralDelay)) return false;

  return true;
}

/*0x84 GET_DIGITAL_SERVICE_DATA Gets a block of data associated with one of the enabled data components of a digital services*/
bool dabGetServiceData(struct dabServiceData_t &dabServiceData, unsigned char statusOnly, unsigned char ack)
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
#ifdef DEBUG_FUNCTION_NAMES_DAB
  Serial.println(GET_DIGITAL_SERVICE_DATA, HEX);
#endif

  unsigned char cmd[1];
  unsigned char arg[1];
  unsigned char buf[24];

  /*initalize*/
  cmd[0]  = GET_DIGITAL_SERVICE_DATA;
  arg[0] = ((statusOnly  & 1 ) << 4) | (ack & 1);
  for (unsigned short i = 0; i < 24; i++) buf[i] = 0xff;

  if (!tuner.writeSpi(cmd, sizeof(cmd), ComDriverSpi::transferStart)) return false;
  if (!tuner.writeSpi(arg, sizeof(arg), ComDriverSpi::transferEnd)) return false;

  if (!deviceReady(buf, sizeof(buf), tDabGeneralDelay, true)) return false;

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
  if (dabServiceData.bufferCount == 0) return false;
  /*length = 0 */
  if (dabServiceData.dataLength == 0)return false;
  /*No DLS or DLS+ */
  //if (dabServiceData.dataSource != 2)return false;

  /*Create buffer with 24 + payload*/
  unsigned char payloadBuffer[24 + dabServiceData.dataLength];

  /*initalize buffer*/
  for (unsigned short i = 0; i < (24 + dabServiceData.dataLength); i++) payloadBuffer[i] = 0xff;

  if (!deviceReady(payloadBuffer, sizeof(payloadBuffer), tDabGeneralDelay, true)) return false;

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

    serial::dabPrintDynamicLabelSegment(dls);
  }

  //Dynamic Label+
  else if (prefixDls.c == 1 && prefixDls.command == 0b0010)
  {
    //Create dls string
    char tags[dabServiceData.dataLength];
    Serial.println(F("DLS+:"));
    Serial.println(payloadBuffer[26]);

    memcpy(tags, &payloadBuffer[27], dabServiceData.dataLength); // Bytes 27...dataLength
    serial::dabPrintDynamicLabelSegment(tags);
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

  return true;
}

/*Get service list header*/
bool dabGetDigitalServiceListHeader(struct dabServiceListHeader_t &dabServiceListHeader, unsigned char dabServiceType)
{
#ifdef DEBUG_FUNCTION_NAMES_DAB
  Serial.println(GET_DIGITAL_SERVICE_LIST, HEX);
#endif

  unsigned char cmd[1];
  unsigned char arg[1];
  /*12 bytes = 4 status bytes 0...3, 2 list size, 2 version, 1 number of services, 3 align pads*/
  unsigned char buf[12];

  /*initalize*/
  cmd[0] = GET_DIGITAL_SERVICE_LIST;
  arg[0] = dabServiceType & 1;
  for (unsigned short i = 0; i < 12; i++) buf[i] = 0xff;



  if (!tuner.writeSpi(cmd, sizeof(cmd), ComDriverSpi::transferStart)) return false;
  if (!tuner.writeSpi(arg, sizeof(arg), ComDriverSpi::transferEnd)) return false;

  if (!deviceReady(buf, sizeof(buf), tDabGeneralDelay, true)) return false;

  /*Read 12 bytes:
    0...3 status
    4...5 list size
    6...7 bytes list version
    8 number of services
    9...11 align pads
  */

  /*Indicates the number of bytes in the digital service list (Max = 2047 bytes, not including list size of 2 bytes)*/
  dabServiceListHeader.listSize = (unsigned short) buf[5] << 8 | buf[4];

  /*version of list*/
  dabServiceListHeader.version = (unsigned short)buf[7] << 8 | buf[6];

  /*Read and save number of services*/
  dabServiceListHeader.numServices = buf[8];

  /*Align Pad 3 Bytes reserved for future use*/
  /*dabServiceListHeader.reserved1 = buf[9];
    dabServiceListHeader.reserved2 = buf[10];
    dabServiceListHeader.reserved3 = buf[11];
  */

  /*Check if list exists or too big */
  if (dabListSize < 34 || dabListSize > 2694)
  {
    return false;
  }

  return true;
}

/*Parse ensemble an fill array with serviceId and component Id*/
bool dabParseEnsemble(struct dabEnsemble_t* &dabEnsembleList, unsigned char &dabNumServices, unsigned char dabServiceType)
{
#ifdef DEBUG_DAB_PARSE_ENSEMBLE
  Serial.println(GET_DIGITAL_SERVICE_LIST, HEX);
#endif

  unsigned char cmd[1];
  unsigned char arg[1];
  /*12 bytes = 4 status bytes 0...3, 2 list size, 2 version, 1 number of services, 3 align pads*/
  unsigned char header[12];

  /*initalize*/
  cmd[0] = GET_DIGITAL_SERVICE_LIST;
  arg[0] = dabServiceType & 1;
  for (unsigned short i = 0; i < 12; i++) header[i] = 0xff;

  unsigned char serviceInfo[24];
  unsigned char componentInfo[4];
  unsigned char numComponents;

  /*free memory from previous realloc*/
  for (unsigned char i = 0; i < dabNumServices; i++)
  {
    /*components*/
    free(dabEnsembleList[i].componentList);
    /*Set pointer to NULL*/
    dabEnsembleList[i].componentList = NULL;
  }


#ifdef DEBUG_DAB_PARSE_ENSEMBLE
  Serial.print(F("RAM1: "));
  Serial.println(freeRam());
#endif

  /*free memory from previous realloc*/
  if (dabEnsembleList != NULL)
  {
    /*Services*/
    free(dabEnsembleList);
    /*Set pointer to NULL*/
    dabEnsembleList = NULL;
    /*Set number of services =0*/
    dabNumServices = 0;
    /*Set service list size = 0*/
    dabListSize = 0;
    /*Service list version = 0*/
    dabListVersion = 0;
  }

  /*Set pointer to a new first dabEnsembleList*/
  dabEnsembleList = (struct dabEnsemble_t*) calloc (1, sizeof(struct dabEnsemble_t));


#ifdef DEBUG_DAB_PARSE_ENSEMBLE
  Serial.print(F("RAM2: "));
  Serial.println(freeRam());
#endif

  if (!tuner.writeSpi(cmd, sizeof(cmd), ComDriverSpi::transferStart)) return false;
  if (!tuner.writeSpi(arg, sizeof(arg), ComDriverSpi::transferEnd)) return false;

  if (!deviceReady(header, sizeof(header), tDabGeneralDelay, true)) return false;

  dabListSize = (unsigned short) header[5] << 8 | header[4];
  dabListVersion = (unsigned short)header[7] << 8 | header[6];
  dabNumServices = header[8];

  /*Check if list exists or too big */
  if (dabListSize < 34 || dabListSize > 2694)
  {
    //Serial.print(F("Error1"));
    return false;
  }

  /*Too many services or no services*/
  if (dabNumServices > dabMaxNumServices || dabNumServices == 0 )
  {
    /*free memory from previous calloc*/
    /*Services*/
    free(dabEnsembleList);
    /*Set pointer to NULL*/
    dabEnsembleList = NULL;
    /*Set number of services =0*/
    dabNumServices = 0;
    /*Set service list size = 0*/
    dabListSize = 0;
    /*Service list version = 0*/
    dabListVersion = 0;

    //Serial.print(F("Error2"));

    return false;
  }


#ifdef DEBUG_DAB_PARSE_ENSEMBLE
  Serial.print(F("RAM3: "));
  Serial.println(freeRam());
#endif

  /*Realloc pointer to dabEnsembleList and create dynamic array of right size and initialize with 0*/
  dabEnsembleList = (struct dabEnsemble_t*) realloc (dabEnsembleList, dabNumServices * sizeof(struct dabEnsemble_t));


#ifdef DEBUG_DAB_PARSE_ENSEMBLE
  Serial.print(F("RAM4: "));
  Serial.println(freeRam());
#endif

  /*No memory left*/
  if (dabEnsembleList == NULL)
  {
    /*free memory from previous calloc*/
    /*Services*/
    free(dabEnsembleList);
    /*Set pointer to NULL*/
    dabEnsembleList = NULL;
    /*Set number of services =0*/
    dabNumServices = 0;
    /*Set service list size = 0*/
    dabListSize = 0;
    /*Service list version = 0*/
    dabListVersion = 0;

    return false;
  }

  /*Start*/
  cmd[0] = RD_REPLY;

  if (!tuner.writeSpi(cmd, sizeof(cmd), ComDriverSpi::transferStart)) return false;
  if (!tuner.readSpi(header, sizeof(header), ComDriverSpi::transferStart)) return false;

  /*Cycle services*/
  for (unsigned char i = 0; i < dabNumServices && i < dabMaxNumServices; i++)
  {

    /*24 bytes for one service*/
    if (!tuner.readSpi(serviceInfo, sizeof(serviceInfo), ComDriverSpi::transferStart)) return false;

#ifdef DEBUG_DAB_PARSE_ENSEMBLE
    Serial.print(F("Service: "));
    Serial.println(i);
    printHexStr(serviceInfo, sizeof(serviceInfo));
#endif

    //Fill Service List
    dabEnsembleList[i].serviceNumber = i;
    //Bytes 0...3
    dabEnsembleList[i].serviceId = ((unsigned long) serviceInfo[3] << 24) | ((unsigned long)serviceInfo[2] << 16) | ((unsigned long)serviceInfo[1] << 8) | (unsigned long)serviceInfo[0];
    //Byte 4 Service Info 1
    //dabServiceList.dabServiceInformation[serviceNum].reservedServiceInfo1   = serviceInfo[4] >> 7 & 1;  //RFU
    //dabServiceList.dabServiceInformation[serviceNum].serviceLinkingInfoFlag = serviceInfo[4] >> 6 & 1;  //Service linking info
    //dabServiceList.dabServiceInformation[serviceNum].pType                  = serviceInfo[4] >> 1 & 0x1F; //Program type
    dabEnsembleList[i].pdFlag                                                 = serviceInfo[4] & 1;       //Audio program or data flag
    //byte 5 Service Info 2
    //dabServiceList.dabServiceInformation[serviceNum].localFlag              = serviceInfo[5] >> 7 & 1; //Indicates if the service is available over the entire (0) or part (1) of the ensemble service area
    //dabServiceList.dabServiceInformation[serviceNum].caId                   = serviceInfo[5] >> 4 & 0x7; //Conditional Access Identifier (0 = unscrambled, 1 = NR-MSK, 2 Eurocrypt EN 50094

    dabEnsembleList[i].numComponents                                          = serviceInfo[5] & 0x0F;//Number of components in service (M <= 15)

    //byte 6 Service Info 3
    // dabServiceList.dabServiceInformation[serviceNum].reservedServiceInfo3= serviceInfo[6] >> 4 & 0x0F;; //RFU
    //dabServiceList.dabServiceInformation[serviceNum].characterSet           = serviceInfo[6] & 0x0F;
    //Service Label
    //memcpy(serviceLabel, &serviceInfo[8], 16); //[8]....[23]
    //serviceLabel[16] = '\0';
    //memcpy(dabServiceList.dabServiceInformation[serviceNum].serviceLabel, &serviceInfo[8], 16); //[8]....[23]
    //dabServiceList.dabServiceInformation[serviceNum].serviceLabel[16] = '\0';

    //Components
    numComponents = dabEnsembleList[i].numComponents;

    //Create dynamic array of right size and initialize with 0
    dabEnsembleList[i].componentList = (unsigned long*) calloc (numComponents, sizeof(unsigned long));

    //No memory left
    if (dabEnsembleList[i].componentList == NULL)
    {
      //free memory from previous realloc
      free(dabEnsembleList[i].componentList);
      //Set pointer to NULL
      dabEnsembleList[i].componentList = NULL;
      if (!tuner.writeSpi(cmd, sizeof(cmd), ComDriverSpi::transferEnd))
        return false;
    }

    //Very memory intensive for Uno !
    //Dont exceed dabMaxNumComponents
    for (unsigned char j = 0; (j < numComponents) && (j < dabMaxNumComponents); j++)
    {
      /*4 bytes per component*/
      tuner.readSpi(componentInfo, sizeof(componentInfo), ComDriverSpi::transferStart);

#ifdef DEBUG_DAB_PARSE_ENSEMBLE
      Serial.print(F("Component: "));
      Serial.println(j);
      printHexStr(componentInfo, sizeof(componentInfo));
#endif

      //dabEnsembleList[i].componentId = (unsigned short)componentInfo[1] << 8 | componentInfo[0];
      //Fill Pointer to components
      dabEnsembleList[i].componentList[j] = (unsigned short)componentInfo[1] << 8 | componentInfo[0];

      //dabComponentList.dabComponent[componentNum].caFlag      = componentInfo[2] & 1;
      //dabComponentList.dabComponent[componentNum].psFlag      = componentInfo[2] >> 1 & 1;
      //dabComponentList.dabComponent[componentNum].type        = componentInfo[2] >> 2 & 0x7F;
      //dabComponentList.dabComponent[componentNum].valid       = componentInfo[3] & 1;

      //Serial.print(F("Component Id: "));
      //Serial.println(dabEnsembleList[i].components[j], HEX);

#ifdef DEBUG_DAB_PARSE_ENSEMBLE
      Serial.print(F("RAM5: "));
      Serial.println(freeRam());
#endif

    }
  }

  /*end spi */
  cmd[0] = RD_REPLY;
  if (!tuner.writeSpi(cmd, sizeof(cmd), ComDriverSpi::transferEnd))return false;

  return true;
}

/*0xB0 Tunes to frequency index*/
bool dabTuneIndex(unsigned char index, unsigned short varCap, unsigned char injection)
{
  /*
    Tunes the DAB Receiver to tune to a frequency between
    168.16 and 239.20 MHz defined by the frequency table
    through DAB_SET_FREQ_LIST.
    INJECTION[1:0] Injection selection  0 : Automatic injection selection 1 : Low-side injection 2 : High-side injection
    FREQ_INDEX[7:0] Frequency index for the tuned frequency, see the DAB_SET_FREQ_LIST command that sets the frequency table
    ANTCAP[15:0] Antenna tuning capacitor value in 250 fF units (31.75 pF Max) Range: 0-128
    0 : Automatically determines the cap setting
  */
#ifdef DEBUG_FUNCTION_NAMES_DAB
  Serial.println(DAB_TUNE_FREQ, HEX);
#endif

  /*index 0...47*/
  if (index >= 47) index = 47;
  else if (index <= 0) index = 0;

  unsigned char cmd[1];
  unsigned char arg[5];
  unsigned char buf[4] = {0xff, 0xff, 0xff, 0xff};

  cmd[0] = DAB_TUNE_FREQ;

  arg[0] = injection & 3;
  arg[1] = index;
  arg[2] = 0;
  arg[3] = varCap & 0xFF;
  arg[4] = varCap >> 8;

  if (!tuner.writeSpi(cmd, sizeof(cmd), ComDriverSpi::transferStart)) return false;
  if (!tuner.writeSpi(arg, sizeof(arg), ComDriverSpi::transferEnd)) return false;

  if (!deviceSeekTuneComplete(buf, sizeof(buf), tDabSeekTuneIndex))return false;

  return true;
}


/*0xB2 DAB_DIGRAD_STATUS Get status information about the received signal quality*/
bool dabGetRsqStatus(dabRsqInformation_t &dabRsqInformation, unsigned char clearDigradInterrupt, unsigned char rssiAtTune, unsigned char clearStcInterrupt)
{
  /*
    DAB_DIGRAD_STATUS returns status information about the digital radio and ensemble including a change in
    ensemble acquisition state, current estimates for ensemble's MSC (Main Service Channel) BER (bit error rate),
    FIC (Fast Information Channel) BER along with number of FIBs (Fast Information Block) that failed a CRC check
    and number of Reed-Solomon decoder errors (DAB+ and DMB only). The bits RSSILINT, RSSIHINT, ACQINT are
    sticky meaning they will remain set until DIGRAD_ACK is set. If the condition is still true after the interrupt is
    cleared another interrupt will fire assuming that bit is enabled in DAB_DIGRAD_INTERRUPT_SOURCE. The
    command is complete when the CTS bit (and optional interrupt) is set. The ERR bit (and optional interrupt) is set if
    an invalid argument is sent. Note that only a single interrupt occurs if both the CTS and ERR bits are set. The
    command may only be sent in powerup mode.
  */
#ifdef DEBUG_FUNCTION_NAMES_DAB
  Serial.println(DAB_DIGRAD_STATUS, HEX);
#endif

  unsigned char cmd[1] = {DAB_DIGRAD_STATUS};
  unsigned char arg[1];
  arg[0] = clearDigradInterrupt << 3 | rssiAtTune << 2 | clearStcInterrupt;
  unsigned char buf[23];
  /*initalize buffer*/
  for (unsigned char i = 0; i < 23; i++) buf[i] = 0xff;

  if (!tuner.writeSpi(cmd, sizeof(cmd), ComDriverSpi::transferStart)) return false;
  if (!tuner.writeSpi(arg, sizeof(arg), ComDriverSpi::transferEnd)) return false;

  if (!deviceReady(buf, sizeof(buf))) return false;

  dabRsqInformation.hardMuteInterrupt =  buf[4] >> 4 & 1;
  dabRsqInformation.ficErrorInterrupt =  buf[4] >> 3 & 1;
  dabRsqInformation.acqInterrupt =       buf[4] >> 2 & 1;
  dabRsqInformation.rssiHighInterrupt =  buf[4] >> 1 & 1;
  dabRsqInformation.rssiLowInterrupt =   buf[4] & 1;

  dabRsqInformation.hardmute = buf[5] >> 4 & 1;
  dabRsqInformation.ficError = buf[5] >> 3 & 1;
  dabRsqInformation.acq =      buf[5] >> 2 & 1;
  dabRsqInformation.valid =    buf[5] & 1;

  dabRsqInformation.rssi        = buf[6]; //Received signal strength indicator. Range: -128-63
  dabRsqInformation.snr         = buf[7];
  dabRsqInformation.ficQuality  = buf[8];
  dabRsqInformation.cnr         = buf[9];
  dabRsqInformation.fibErrorCount = (unsigned short) buf[11] << 8 | buf[10];

  dabRsqInformation.frequency   = (unsigned long)((buf[12] | (unsigned long)buf[13] << 8 | (unsigned long)buf[14] << 16 | (unsigned long)buf[15] << 24));
  dabRsqInformation.index       = buf[16];
  dabRsqInformation.fftOffset   = buf[17];
  dabRsqInformation.varactorCap = (unsigned short) buf[19] << 8 | buf[18];
  dabRsqInformation.cuLevel     = (int) buf[21] << 8 | buf[20]; // 0-470
  dabRsqInformation.fastDect    = buf[22];

  return true;
}


/*0xB3 DAB_GET_EVENT_STATUS Gets information about the various events related to the DAB radio*/
bool dabGetEventStatus(struct dabEventInformation_t &dabEventInfo, unsigned char eventAck)
{
  /*
    DAB_GET_EVENT_STATUS gets information about the various events related to the DAB radio. These events
    include signaling the reception of new PAD (Programme-Associated Data) data, service lists and announcements.
    The bits SVRLISTINT, ANNOINT, RECFGWRNINT, and RECFGINT are sticky meaning they will remain set until
    EVENT_ACK is set. If the condition is still true after the interrupt is cleared another interrupt will fire assuming that
    bit is enabled in DAB_EVENT_INTERRUPT_SOURCE. The command is complete when the CTS bit (and optional
    interrupt) is set. The ERR bit (and optional interrupt) is set if an invalid argument is sent. Note that only a single
    interrupt occurs if both the CTS and ERR bits are set. The command may only be sent in powerup mode.
  */
#ifdef DEBUG_FUNCTION_NAMES_DAB
  Serial.println(DAB_GET_EVENT_STATUS, HEX);
#endif

  unsigned char cmd[1];
  unsigned char arg[1];
  unsigned char buf[8] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

  cmd[0] = DAB_GET_EVENT_STATUS;
  arg[0] = eventAck & 1;

  if (!tuner.writeSpi(cmd, sizeof(cmd), ComDriverSpi::transferStart)) return false;
  if (!tuner.writeSpi(arg, sizeof(arg), ComDriverSpi::transferEnd)) return false;

  if (!deviceReady(buf, sizeof(buf), tDabGeneralDelay, true)) return false;

  dabEventInfo.ensembleReconfigInterrupt        = buf[4] >> 7 & 1;
  dabEventInfo.ensembleReconfigWarningInterrupt = buf[4] >> 6 & 1;

  dabEventInfo.announcementInterrupt            = buf[4] >> 4 & 1;
  dabEventInfo.otherServiceInterrupt            = buf[4] >> 3 & 1;
  dabEventInfo.serviceLinkingInterrupt          = buf[4] >> 2 & 1;
  dabEventInfo.frequencyInterrupt               = buf[4] >> 1 & 1;
  dabEventInfo.serviceListInterrupt             = buf[4] & 1;

  dabEventInfo.announcementAvailable            = buf[5] >> 4 & 1;
  dabEventInfo.otherServiceAvailable            = buf[5] >> 3 & 1;
  dabEventInfo.serviceLinkingAvailable          = buf[5] >> 2 & 1;
  dabEventInfo.frequencyAvailable               = buf[5] >> 1 & 1;
  dabEventInfo.serviceListAvailable             = buf[5] & 1;

  dabEventInfo.currentServiceListVersion         = (unsigned short)buf[7] << 8 | buf[6];

  return true;
}


/*0xB4 DAB_GET_ENSEMBLE_INFO Gets information about the current ensemble*/
dabEnsembleInformation_t dabGetEnsembleInfo()
{
  /*
    DAB_GET_ENSEMBLE_INFO gets information about the current ensemble such as the ensemble ID and label.
    The command is complete when the CTS bit (and optional interrupt) is set. The ERR bit (and optional interrupt) is
    set if an invalid argument is sent. Note that only a single interrupt occurs if both the CTS and ERR bits are set. The
    command may only be sent in powerup mode.
  */
#ifdef DEBUG_FUNCTION_NAMES_DAB
  Serial.println(DAB_GET_ENSEMBLE_INFO, HEX);
#endif
  unsigned char cmd[1];
  unsigned char arg[1];
  unsigned char buf[26];

  /*initalize*/
  cmd[0] = DAB_GET_ENSEMBLE_INFO;
  arg[0] = 0;
  for (unsigned short i = 0; i < 26; i++) buf[i] = 0xff;

  tuner.writeSpi(cmd, sizeof(cmd), ComDriverSpi::transferStart);
  tuner.writeSpi(arg, sizeof(arg), ComDriverSpi::transferEnd);

  deviceReady(buf, sizeof(buf), tDabGetEnsembleInfo);

  dabEnsembleInformation.ensembleId        = (unsigned short)buf[5] << 8 | buf[4];
  /*16 characters for the ensemble label terminated by '/0'*/
  memcpy(dabEnsembleInformation.ensembleLabel, &buf[6], 16); /*buf[6]....buf[21]*/
  dabEnsembleInformation.ensembleLabel[16] = '\0';
  dabEnsembleInformation.ecc               = buf[22];
  dabEnsembleInformation.charSet           = buf[23];
  dabEnsembleInformation.abbreviationMask  = (unsigned short)buf[25] << 8 | buf[24];

  return dabEnsembleInformation;
}

/*0xB8 DAB_SET_FREQ_LIST Set Frequency table*/
bool dabSetFrequencyTable(unsigned long* &dabFreqTable, unsigned char &dabNumFreq, const unsigned long* dabNewFreqTable, unsigned char dabNewNumFreq)
{
  /*
    DAB_SET_FREQ_LIST command sets the DAB frequency table. The frequencies are in units of 1 kHz. The table
    can be populated with a single entry or a regional list (for example 5 or 6 entries). It is recommended to make the
    list regional to increase scanning speed. The command is complete when the CTS bit (and optional interrupt) is
    set. The ERR bit (and optional interrupt) is set if an invalid argument is sent. Note that only a single interrupt occurs
    if both the CTS and ERR bits are set. The command may only be sent in powerup mode.
  */
#ifdef DEBUG_FUNCTION_NAMES_DAB
  Serial.println(DAB_SET_FREQ_LIST, HEX);
#endif

  /*free memory from previous malloc for dabGetFrequencyTable*/
  free(dabFreqTable);
  /*Set previous pointer to NULL*/
  dabFreqTable = NULL;
  /*Set number of frequencies in table = 0*/
  dabNumFreq = 0;

  unsigned char cmd[1];
  unsigned char arg[3 + 4 * dabNewNumFreq]; /*4 arg per frequency*/
  unsigned char buf[4] = {0xff, 0xff, 0xff, 0xff};

  cmd[0]  = DAB_SET_FREQ_LIST;

  arg[0] = dabNewNumFreq;
  arg[1] = 0;
  arg[2] = 0;

  /*fill arg array with frequencies*/
  for (unsigned char i = 0; i < dabNewNumFreq && i < dabMaxNumFreq; i++)
  {
    arg[3 + 4 * i] = dabNewFreqTable[i] & 0xFF;
    arg[4 + 4 * i] = dabNewFreqTable[i] >> 8 & 0xFF;
    arg[5 + 4 * i] = dabNewFreqTable[i] >> 16 & 0xFF;
    arg[6 + 4 * i] = dabNewFreqTable[i] >> 24 & 0xFF;
  }

  if (!tuner.writeSpi(cmd, sizeof(cmd), ComDriverSpi::transferStart)) return false;
  if (!tuner.writeSpi(arg, sizeof(arg), ComDriverSpi::transferEnd)) return false;

  if (!deviceReady(buf, sizeof(buf))) printHexStr(buf, sizeof(buf));

  return true;
}

/*0xB9 DAB_GET_FREQ_LIST Get frequency table*/
bool dabGetFrequencyTable(unsigned long* &dabFreqTable, unsigned char &dabNumFreq)
{
  /*
    DAB_GET_FREQ_LIST gets the DAB frequency table. All frequencies are in units of 1 kHz. The command is
    complete when the CTS bit (and optional interrupt) is set. The ERR bit (and optional interrupt) is set if an invalid
    argument is sent. Note that only a single interrupt occurs if both the CTS and ERR bits are set. The command may
    only be sent in powerup mode.
  */
#ifdef DEBUG_FUNCTION_NAMES_DAB
  Serial.println(DAB_GET_FREQ_LIST, HEX);
#endif

  unsigned char cmd[1]  = {DAB_GET_FREQ_LIST};
  unsigned char arg[1]  = {0x00};
  unsigned char buf[8]  = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
  unsigned char freq[4] = {0xff, 0xff, 0xff, 0xff};

  if (!tuner.writeSpi(cmd, sizeof(cmd), ComDriverSpi::transferStart)) return false;
  if (!tuner.writeSpi(arg, sizeof(arg), ComDriverSpi::transferEnd)) return false;

  if (!deviceReady(buf, sizeof(buf), tDabGeneralDelay)) return false;

  /*Number of frequencies*/
  dabNumFreq = buf[4];

#ifdef DEBUG_DAB_FREQUENCY_TABLE
  Serial.print(F("RAM1: "));
  Serial.println(freeRam());
#endif

  /*Frequencies available and not more than dabMaxNumFreq*/
  if (dabNumFreq != 0 && !(dabNumFreq > dabMaxNumFreq))
  {
    /*free memory from previous table*/
    free(dabFreqTable);
    /*Set pointer to NULL*/
    dabFreqTable = NULL;

    /*Dynamically allocate memory and initialize with 0*/
    dabFreqTable = (unsigned long*)calloc(dabNumFreq, (sizeof(unsigned long)));

  }
  else return false;

#ifdef DEBUG_DAB_FREQUENCY_TABLE
  Serial.print(F("RAM2: "));
  Serial.println(freeRam());
#endif

  /*Fill frequency table*/
  unsigned char data[9] = {0, 0xff,  0xff,  0xff,  0xff,  0xff,  0xff, 0xff, 0xff};
  /*Start reading */
  if (!tuner.writeSpi(data, sizeof(data), ComDriverSpi::transferStart)) return false;


  for (unsigned char i = 0; i < dabNumFreq; i++)
  {
    /*last element end reading*/
    if (i == dabNumFreq - 1)tuner.readSpi(freq, sizeof(freq), ComDriverSpi::transferEnd);
    /*not last element read further*/
    else tuner.readSpi(freq, sizeof(freq), ComDriverSpi::transferStart);

    dabFreqTable[i] = (unsigned long) freq[3] << 24 | (unsigned long) freq[2] << 16 | (unsigned long) freq[1] << 8 | freq[0];

#ifdef DEBUG_DAB_FREQUENCY_TABLE
    printHexStr(freq, sizeof(freq));
#endif

  }

#ifdef DEBUG_DAB_FREQUENCY_TABLE
  Serial.print(F("RAM3: "));
  Serial.println(freeRam());
#endif

  return true;
}

/*0xBB DAB_GET_COMPONENT_INFO Get information about the component application data*/
bool dabGetComponentInformation(unsigned long dabServiceId, unsigned long dabComponentId, dabComponentInformation_t &dabComponentInformation)
{
  /*
    DAB_GET_COMPONENT_INFO gets information about components within the ensemble if available. The
    command is complete when the CTS bit (and optional interrupt) is set. The ERR bit (and optional interrupt) is set if
    an invalid argument is sent. Note that only a single interrupt occurs if both the CTS and ERR bits are set. The
    command may only be sent in powerup mode.
  */
#ifdef DEBUG_FUNCTION_NAMES_DAB
  Serial.println(DAB_GET_COMPONENT_INFO, HEX);
#endif

  unsigned char cmd[1];
  unsigned char arg[11];
  unsigned char buf[31];

  /*initalize buffer*/
  for (unsigned short i = 0; i < 31; i++) buf[i] = 0xff;

  cmd[0] = DAB_GET_COMPONENT_INFO;

  arg[0] = 0x00;
  arg[1] = 0x00;
  arg[2] = 0x00;
  arg[3] = dabServiceId & 0xFF;
  arg[4] = dabServiceId >> 8 & 0xFF;
  arg[5] = dabServiceId >> 16 & 0xFF;
  arg[6] = dabServiceId >> 24 & 0xFF;
  arg[7] = dabComponentId & 0xFF;
  arg[8] = dabComponentId >> 8 & 0xFF;
  arg[9] = dabComponentId >> 16 & 0xFF;
  arg[10] = dabComponentId >> 24 & 0xFF;

  if (!tuner.writeSpi(cmd, sizeof(cmd), ComDriverSpi::transferStart)) return false;
  if (!tuner.writeSpi(arg, sizeof(arg), ComDriverSpi::transferEnd)) return false;

  if (!deviceReady(buf, sizeof(buf), tDabGeneralDelay, true)) return false;

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

  return true;
}

/*0xBC DAB_GET_TIME Gets the ensemble time adjusted for the local time offset or the UTC*/
bool dabGetDateTimeInformation(unsigned short &year, unsigned char &month, unsigned char &day, unsigned char &hour, unsigned char &minute, unsigned char &second, unsigned char timeType)
{
  /*
    DAB_GET_TIME gets the ensemble time adjusted for the local time offset. The command is complete when the
    CTS bit (and optional interrupt) is set. The ERR bit (and optional interrupt) is set if an invalid argument is sent. Note
    that only a single interrupt occurs if both the CTS and ERR bits are set. The command may only be sent in
    powerup mode.
  */
#ifdef DEBUG_FUNCTION_NAMES_DAB
  Serial.println(DAB_GET_TIME, HEX);
#endif

  unsigned char cmd[1];
  unsigned char arg[1];
  unsigned char buf[11];

  /*initalize buffer*/
  for (unsigned short i = 0; i < 11; i++) buf[i] = 0xff;

  cmd[0] = DAB_GET_TIME;
  arg[0] = timeType & 1;

  if (!tuner.writeSpi(cmd, sizeof(cmd), ComDriverSpi::transferStart)) return false;
  if (!tuner.writeSpi(arg, sizeof(arg), ComDriverSpi::transferEnd)) return false;

  if (!deviceReady(buf, sizeof(buf), tDabGeneralDelay, true)) return false;

  year    = (unsigned short)buf[5] << 8 | buf[4];
  month   = buf[6];
  day     = buf[7];
  hour    = buf[8];
  minute  = buf[9];
  second  = buf[10];

  return true;
}

/*0xBD DAB_GET_AUDIO_INFO Gets audio service info*/
bool dabGetComponentAudioInfo(dabAudioComponentInformation_t &dabAudioComponentInformation)
{
  /*
    DAB_GET_AUDIO_INFO gets information about the current audio service (decoder bps, audio mode). The
    command is complete when the CTS bit (and optional interrupt) is set. The ERR bit (and optional interrupt) is set if
    an invalid argument is sent. Note that only a single interrupt occurs if both the CTS and ERR bits are set. The
    command may only be sent in powerup mode.
  */
#ifdef DEBUG_FUNCTION_NAMES_DAB
  Serial.println(DAB_GET_AUDIO_INFO, HEX);
#endif

  unsigned char cmd[1];
  unsigned char arg[1];
  unsigned char buf[10];

  /*initalize buffer*/
  for (unsigned short i = 0; i < 10; i++) buf[i] = 0xff;

  cmd[0] = DAB_GET_AUDIO_INFO;
  arg[0] = 0x00;

  if (!tuner.writeSpi(cmd, sizeof(cmd), ComDriverSpi::transferStart)) return false;
  if (!tuner.writeSpi(arg, sizeof(arg), ComDriverSpi::transferEnd)) return false;

  if (!deviceReady(buf, sizeof(buf), tDabGeneralDelay, true)) return false;

  /*
    AUDIO_BIT_RATE[15:0] Audio bit rate of the current audio service (kbps).
    AUDIO_SAMPLE_RATE[15:0] Sample rate of the current audio service (Hz).
    AUDIO_PS_FLAG Audio PS flag. only applicable to DAB+. Set to 0 for DAB
    0 : SBR is not used
    1 : SBR is used
    AUDIO_SBR_FLAG only applicable to DAB+. Set to 0 for DAB
    0 : SBR is not used
    1 : SBR is used
    AUDIO_MODE[1:0]
    Audio mode
    0 : dual
    1 : mono
    2 : stereo
    3 : joint stereo
    AUDIO_DRC_GAIN[7:0]
    The transmitter specified dynamic range control (DRC) gain of the current audio
    service. The range of this field is from 0 to 63, representing 0 to 15.75dB in increment
    of 0.25dB.
  */

  dabAudioComponentInformation.audioBitRate =     (unsigned short) buf[5] << 8 | buf[4];
  dabAudioComponentInformation.audioSampleRate =  (unsigned short) buf[7] << 8 | buf[6];
  dabAudioComponentInformation.audioPsFlag =  buf[8] >> 2 & 1;
  dabAudioComponentInformation.audioSbrFlag = buf[8] >> 1 & 1;
  dabAudioComponentInformation.audioMode =    buf[8] & 1;
  dabAudioComponentInformation.audioDrcGain = buf[9] & 1;

  return true;
}

/*0xBE DAB_GET_SUBCHAN_INFO Get technical information about the component*/
bool dabGetComponentTechnicalInformation(unsigned long dabServiceId, unsigned long dabComponentId, dabComponentTechnicalInformation_t &dabComponentTechnicalInformation)
{
#ifdef DEBUG_FUNCTION_NAMES_DAB
  Serial.println(DAB_GET_SUBCHAN_INFO, HEX);
#endif

  unsigned char cmd[1] = {DAB_GET_SUBCHAN_INFO};
  unsigned char arg[11];
  unsigned char buf[12];

  /*initalize buffer*/
  for (unsigned short i = 0; i < 12; i++) buf[i] = 0xff;

  arg[0] = 0x00;
  arg[1] = 0x00;
  arg[2] = 0x00;
  arg[3] = dabServiceId & 0xFF;
  arg[4] = dabServiceId >> 8 & 0xFF;
  arg[5] = dabServiceId >> 16 & 0xFF;
  arg[6] = dabServiceId >> 24 & 0xFF;
  arg[7] = dabComponentId & 0xFF;
  arg[8] = dabComponentId >> 8 & 0xFF;
  arg[9] = dabComponentId >> 16 & 0xFF;
  arg[10] = dabComponentId >> 24 & 0xFF;

  if (!tuner.writeSpi(cmd, sizeof(cmd), ComDriverSpi::transferStart)) return false;
  if (!tuner.writeSpi(arg, sizeof(arg), ComDriverSpi::transferEnd)) return false;

  if (!deviceReady(buf, sizeof(buf), tDabGeneralDelay)) return false;

  dabComponentTechnicalInformation.serviceMode    = buf[4];
  dabComponentTechnicalInformation.protectionInfo = buf[5];
  dabComponentTechnicalInformation.bitRate        = (unsigned short) buf[7] << 8 | buf[6];
  dabComponentTechnicalInformation.numberCU       = (unsigned short) buf[9] << 8 | buf[8];
  dabComponentTechnicalInformation.addressCU      = (unsigned short) buf[11] << 8 | buf[10];

  return true;
}

/*0xBF DAB_GET_FREQ_INFO Gets ensemble frequency information list*/
bool dabGetEnsembleFrequencyInformationList()
{
  /*
    DAB_GET_FREQ_INFO gets radio Frequency Information (FI) about the ensemble. The command is complete
    when the CTS bit (and optional interrupt) is set. The ERR bit (and optional interrupt) is set if an invalid argument is
    sent. Note that only a single interrupt occurs if both the CTS and ERR bits are set. The command may only be sent
    in powerup mode.
  */

#ifdef DEBUG_FUNCTION_NAMES_DAB
  Serial.println(DAB_GET_FREQ_INFO, HEX);
#endif

  unsigned char cmd[1] = {DAB_GET_FREQ_INFO};
  unsigned char arg[1] = {0};
  unsigned char buf[20];

  /*initalize buffer*/
  for (unsigned short i = 0; i < 20; i++) buf[i] = 0xff;

  if (!tuner.writeSpi(cmd, sizeof(cmd), ComDriverSpi::transferStart)) return false;
  if (!tuner.writeSpi(arg, sizeof(arg), ComDriverSpi::transferEnd)) return false;

  if (!deviceReady(buf, sizeof(buf), tDabGeneralDelay, true)) return false;

  return true;
}

/*0xC0 DAB_GET_SERVICE_INFO Get digital service information*/
bool dabGetDigitalServiceInformation(struct dabServiceInformation_t &dabServiceInformation, unsigned long dabServiceId)
{
  /*
    DAB_GET_SERVICE_INFO gets information about a service. The command is complete when the CTS bit (and
    optional interrupt) is set. The ERR bit (and optional interrupt) is set if an invalid argument is sent. Note that only a
    single interrupt occurs if both the CTS and ERR bits are set. The command may only be sent in powerup mode.
  */

#ifdef DEBUG_FUNCTION_NAMES_DAB
  Serial.println(DAB_GET_SERVICE_INFO, HEX);
#endif

  unsigned char cmd[1] = {DAB_GET_SERVICE_INFO};
  unsigned char arg[7];
  unsigned char buf[26];

  /*initalize buffer*/
  for (unsigned short i = 0; i < 26; i++) buf[i] = 0xff;

  arg[0] = 0;
  arg[1] = 0;
  arg[2] = 0;
  arg[3] = dabServiceId & 0xFF;
  arg[4] = dabServiceId >> 8 & 0xFF;
  arg[5] = dabServiceId >> 16 & 0xFF;
  arg[6] = dabServiceId >> 24 & 0xFF;

  if (!tuner.writeSpi(cmd, sizeof(cmd), ComDriverSpi::transferStart)) return false;
  if (!tuner.writeSpi(arg, sizeof(arg), ComDriverSpi::transferEnd)) return false;

  if (!deviceReady(buf, sizeof(buf), tDabGeneralDelay, true)) return false;

  /*serviceInfo1*/
  dabServiceInformation.serviceLinkingInfoFlag = buf[4] >> 6 & 1;
  dabServiceInformation.pType                  = buf[4] >> 1 & 0x1F;
  dabServiceInformation.pdFlag                 = buf[4] & 1;

  /*serviceInfo2*/
  dabServiceInformation.localFlag              = buf[5] >> 7 & 1;
  dabServiceInformation.caId                   = buf[5] >> 4 & 0x7;
  dabServiceInformation.numComponents          = buf[5] & 0xF;

  /*serviceInfo3*/
  dabServiceInformation.characterSet           = buf[6] & 0xF;
  dabServiceInformation.ecc                    = buf[7];

  /*Service Label*/
  memcpy(dabServiceInformation.serviceLabel, &buf[8], 16); // Bytes 16...36
  dabServiceInformation.serviceLabel[16] = '\0';

  dabServiceInformation.abbreviationMask       = (unsigned short) buf[25] << 8 | buf[24];

  return true;
}


/*Scan all indices of frequency table*/
bool dabBandScan(unsigned char &dabValidNumFreq, unsigned char* &dabValidFreqTable)
{
#ifdef DEBUG_FUNCTION_NAMES_DAB
  Serial.println(F("dabBandScan()"));
#endif

  /*free memory from previous table*/
  free(dabValidFreqTable);
  /*Set pointer to NULL*/
  dabValidFreqTable = NULL;
  /*Set to valid frequencies = 0*/
  dabValidNumFreq = 0;

  /*Dynamically allocate memory and initialize with 0*/
  //dabFreqTable = (unsigned long*)calloc(dabNumFreq, (sizeof(unsigned long)));

  /*Get actual frequency table*/
  if (!dabGetFrequencyTable(dabFreqTable, dabNumFreq))return false;

  /*Scan trough index table*/
  for (unsigned char index = 0; index < dabNumFreq; index++)
  {
    if (dabTuneIndex(index))
    {
      dabGetRsqStatus(dabRsqInformation);
      //serial::dabPrintRsqStatus(dabRsqInfo);//Print status
    }

    //DAB found and valid save The threshold for dab detected is greater than 4.
    if ((dabRsqInformation.fastDect > 4) && dabRsqInformation.valid)
    {
      //serial::dabPrintIndex(index);
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
    //Set pointer to NULL
    dabValidFreqTable = NULL;
    //Set to valid frequencies = 0
    dabValidNumFreq = 0;

    return false;
  }

  return true;
}

/*Tune to next entry in frequency table*/
bool dabScanUp(unsigned char &index)
{
#ifdef DEBUG_FUNCTION_NAMES_DAB
  Serial.println(F("dabScanUp()"));
#endif

  index++;
  /*wrap around*/
  if (index == 48) index = 0;

  /*tune index*/
  dabTuneIndex(index);

  dabGetRsqStatus(dabRsqInformation);

  return true;
}

/*Tune to previous entry in frequency table*/
bool dabScanDown(unsigned char &index)
{
#ifdef DEBUG_FUNCTION_NAMES_DAB
  Serial.println(F("dabScanDown()"));
#endif

  index--;
  /*wrap around*/
  if (index == 0) index = 47;

  /*tune index*/
  dabTuneIndex(index);

  dabGetRsqStatus(dabRsqInformation);

  return false;
}

/*Volume up*/
bool dabVolumeUp(unsigned char &volume)
{
  //Serial.println(volume);

  //volume 0...63;
  if (volume < 63) volume ++;
  else volume = 63;
  deviceSetProperty(AUDIO_ANALOG_VOLUME, volume);
  return true;
}

/*Volume down*/
bool dabVolumeDown(unsigned char &volume) {
  //volume 0...63;
  if (volume > 0) volume --;
  else volume = 0;
  deviceSetProperty(AUDIO_ANALOG_VOLUME, volume);
  return true;
}
/*Volume mute*/
bool dabVolumeMute(bool mute) {
  if (mute == true)
    deviceSetProperty(AUDIO_MUTE, 3);
  else if (mute == false)
    deviceSetProperty(AUDIO_MUTE, 0);
  return true;
}

/*Get all property values*/
bool dabGetAllProperties(struct dabPropertyValueList_t dabPropertyValueList[], unsigned char num)
{
#ifdef DEBUG_FUNCTION_NAMES_DAB
  Serial.println(F("dabGetAllProperties()"));
#endif

  for (unsigned char i = 0; i < num; i++)
  {
    deviceGetProperty(dabPropertyValueList[i].dabPropertyId, dabPropertyValueList[i].dabPropertyValue);
  }

  return true;
}

/*Set all property vlaues*/
bool dabSetAllProperties(struct dabPropertyValueList_t dabPropertyValueList[], unsigned char num)
{
#ifdef DEBUG_FUNCTION_NAMES_DAB
  Serial.println(F("dabSetAllProperties()"));
#endif

  for (unsigned char i = 0; i < num; i++)
  {
    deviceSetProperty(dabPropertyValueList[i].dabPropertyId, dabPropertyValueList[i].dabPropertyValue);
  }
  return true;
}


/*Test varactor tuning capacitor*/
unsigned short dabTestVaractorCap(unsigned char index, unsigned char injection, unsigned char numberMeasurments)
{
  /*average rssi*/
  unsigned short rssiAvg = 0;
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
    dabTuneIndex(index, varCap, injection);
    /*Get average rssi*/
    rssiAvg = deviceGetAverageRssi(numberMeasurments);

    /*save in list*/
    varCapList[varCap] = rssiAvg;

    //check if rssi better
    if (rssiAvg > rssiMax)
    {
      //save new values
      rssiMax = rssiAvg;
      varCapMax = varCap;
    }
    //Serial.print(F("Rssi Avgerage: "));
    Serial.print(varCap);
    Serial.print(",");
    Serial.println(rssiAvg / 256.00);
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

/*Get average rssi*/
unsigned short deviceGetAverageRssi(unsigned char numberMeasurements)
{
  /*list of measurements*/
  unsigned short rssiN[numberMeasurements];
  /*initalize*/
  for (unsigned char i = 0; i < numberMeasurements; i++) rssiN[i] = 0;
  /*summation of all measurements*/
  unsigned long sum = 0;
  /*Average rssi in dBuV in 1/256*/
  unsigned short rssiAvg = 0;
  
  /*multiple measurements*/
  for (unsigned char j = 0; j < numberMeasurements; j++)
  {
    /*save in list*/
    rssiN[j] = deviceGetRssi();
    /*summation of all measurements*/
    sum = sum + rssiN[j];
  }
  
  /*calculate average*/
  rssiAvg = sum / numberMeasurements;

  return rssiAvg;
}


