//include guard
#ifndef SI468X_DRIVER_H
#define SI468X_DRIVER_H

/*
  Driver for tuner circuit SI468x by IGB

  SPI MOSI:   Pin 11
  SPI MISO:   Pin 12
  SPI SCK:    Pin 13

  SPI SS:     Pin 8
  Interrupt:  Pin 6
  Reset:      Pin 3

  Flash memory circuit SST26W onboard
  SPI SS:     Pin 2

  Flash:  30820 Bytes (95%)
  RAM:    853 Bytes (41%)
*/

/*
  Done:
  replace NULL with nullptr - done
  use enum for constants to save memory - done

  use reference & in function parameters to save memory - open
  use write/read instead of get/set in function names - open
  use standard uint8_t datatypes - open
  create namespace driverSi468x to avoid conflicts - open
  loadFirmware generic message handler to avoid PrintSerialFlashSst26 object - open
  loadFirmware works only with 0x100 because of flash (pagewise)- open
  readStorage() handle first 4 bytes of answer - open
  readServiceInformation() delay tbd - open
  dabGetEnsembleFrequencyInformationList()- open
  delay tbd - open
  use onboard flash/firmware location for different data like favorites, properties, program type - open
*/


//Version
const char version[8] = "0.08.04";

enum SPI_FREQUENCY {SPI_FREQUENCY = 8000000UL};

enum PINS
{
  //Tuner circuit
  PIN_DEVICE_SLAVE_SELECT = 8,
  PIN_DEVICE_INTERRUPT    = 6,
  PIN_DEVICE_RESET        = 3,
  //Memory circuit
  PIN_FLASH_SLAVE_SELECT  = 2,
};

//Max numbers of retry when chip is busy
enum MAX_RETRY {MAX_RETRY = 10};

//Device specific delay times
enum DURATIONS_DEVICE
{
  //Very critical for device to start up
  //https://github.com/arduino/Arduino/issues/129
  //delay does not work in ctor, delayMicroseconds() work in CTOR
  //max delayMicroseconds(16383) = delayMicroseconds(0x3FFF)

  DURATION_RESET        = 5000,//3ms see flowchart; 5ms tRSTB_HI see timing
  DURATION_POWER_UP     = 3000,//20us see flowchart; 3ms tPOWER_UP see timing
  DURATION_REPLY        = 3000,//?ms see flowchart; 1 ms see timing CTS polls @ 1 ms; tested 2500us
  DURATION_LOAD_INIT    = 4000,//4ms see flowchart; ?ms see timing
  DURATION_BOOT         = 10000,//350ms = 30 * 10000 us in loop, Boot time 63ms at analog FM, 198ms at DAB
  DURATION_PROPERTY     = 10000//write / read PropertyValue
};

//Status register 2 Byte
struct statusRegister_t
{
  unsigned char cts:        1;  //Clear to Send

  unsigned char cmdErr:     1;  //Command Error. 0 : No error 1 : Error. The previous command failed
  unsigned char dacqInt:    1;  //Digital radio link change interrupt indicator
  unsigned char dsrvInt:    1;  //Digital service Interrupt
  unsigned char stcInt:     1;  //Seek/Tune complete
  unsigned char eventInt:   1;  //Digital radio event change interrupt indicator

  unsigned char state:      2;  //PUP_STATE 0 : waiting on the POWER_UP command. 1 : Reserved 2 : The bootloader is currently running. 3 : An application was successfully booted and is currently running.
  unsigned char rfFeErr:    1;  //When set indicates that the RF front end of the system is in an unexpected state.

  unsigned char dspErr:     1;  //DSPERR The DSP has encountered a frame overrun
  unsigned char repOfErr:   1;  //Reply overflow Error. SPI clock rate too fast
  unsigned char cmdOfErr:   1;  //Command overflow Error. SPI clock rate too fast
  unsigned char arbErr:     1;  //Arbiter error has occurred
  unsigned char nonRecErr:  1;  //Non recoverable error

  unsigned char cmdErrCode: 8;  //command error code
};

//Firmware Revision 8 Byte
struct firmwareInformation_t
{
  unsigned char revisionNumberMajor:  8;//REVEXT[7:0] Major revision number (first part of 1.2.3).
  unsigned char revisionNumberMinor:  8;//[7:0] Minor revision number (second part of 1.2.3).
  unsigned char revisionNumberBuild:  8;//REVINT[7:0] Build revision number (third part of 1.2.3).
  unsigned char noSvnFlag:            1;//NOSVN If set the build was created with no SVN info. This image cannot be tracked back to the SVN repo.
  unsigned char location:             2;//LOCATION[5:4] The location from which the image was built (Trunk, Branch or Tag).
  //0x0 : The image was built from an SVN tag. Revision numbers are valid.
  //0x1 : The image was built from an SVN branch. Revision numbers will be 0.
  //0x2 : The image was built from the trunk. Revision number will be 0.
  unsigned char mixedRevFlag:         1;//MIXEDREV If set, the image was built with mixed revisions.
  unsigned char localModFlag:         1;//LOCALMOD If set, the image has local modifications.
  unsigned long svnId:               32;//SVNID[31:0] SVN ID from which the image was built.
};

//Device part info 4 Byte
struct partInfo_t
{
  unsigned char chipRev;
  unsigned char romId;
  unsigned short partNumber;
};

//Device power up arguments 8 Byte
struct powerUpArguments_t
{
  unsigned char cts:        1;//0-1
  unsigned char clockMode:  2;//0-3
  unsigned char trSize:     4;//0-15
  unsigned char iBiasStart: 7;//0-127
  unsigned long xtalFreq:   32;//5,4 - 46,2 MHz
  unsigned char cTune:      6;//0-63
  unsigned char iBiasRun:   7;// 0-127, 10 uA steps, 10 to 1270 uA. If set to 0, will use the same value as iBiasStart
};

//properties
//Number of device properties
enum NUM_PROPERTIES_DEVICE {NUM_PROPERTIES_DEVICE = 17};
//2 dim list for device properties 68 Byte
extern unsigned short propertyValueListDevice[NUM_PROPERTIES_DEVICE][2];

//Read 2 dimensional propery value list - a bit complicated :)
//https://stackoverflow.com/questions/3716595/returning-multidimensional-array-from-function
//unsigned short (*readPropertyValueList())[2];//reads the values from device if given the right id
unsigned short (*readPropertyValueList(unsigned short propertyValueList[][2], unsigned char numberProperties))[2];//reads the values from device if given the right id
//writes 2 dimensional property value list with numberProperties to device
void writePropertyValueList(unsigned short propertyValueList[][2], unsigned char numberProperties);


//Device functions
//0x00 RD_REPLY Read answer of device
statusRegister_t readReply(unsigned char reply[], unsigned long len);
//0x01 POWER_UP Power-up the device and set system settings
void powerUp(powerUpArguments_t powerUpArguments);
//0x04 HOST_LOAD Loads an image from HOST over command interface
void hostLoad(unsigned char package[], unsigned short len);
//0x05 FLASH_LOAD Loads an image from external FLASH over secondary SPI bus
void flashLoad(unsigned long address, unsigned char subCommand = 0);
//0x06 LOAD_INIT Prepares the bootloader to receive a new image
void loadInit();
//0x07 BOOT Boots the image currently loaded in RAM
void boot();
//0x08 GET_PART_INFO Get Device Part Info
partInfo_t readPartInfo();
//0x09 GET_SYS_STATE reports basic system state information such as which mode is active; FM, DAB, etc.
unsigned char readSystemState();
//0x0A GET_POWER_UP_ARGS Reports basic information about the device such as arguments used during POWER_UP
powerUpArguments_t readPowerUpArguments();
//0x10 READ_OFFSET Reads a portion of response buffer from an offset
statusRegister_t readReplyOffset(unsigned char reply[], unsigned short len, unsigned short offset);
//0x12 GET_FUNC_INFO Get Firmware Information
firmwareInformation_t readFirmwareInformation();
//0x13 SET_PROPERTY Sets the value of a property
void writePropertyValue(unsigned short id, unsigned short value);
//0x14 GET_PROPERTY Retrieve the value of a property
unsigned short readPropertyValue(unsigned short id);
//0x15 WRITE_STORAGE Writes data to the on board storage area at a specified offset
void writeStorage(unsigned char data[], unsigned char len, unsigned short offset);
//0x16 READ_STORAGE Reads data from the on board storage area from a specified offset
void readStorage(unsigned char data[], unsigned char len, unsigned short offset);
//0xE5 TEST_GET_RSSI returns the reported RSSI in 8.8 format
unsigned short readRssi();


//Helper functions device

//Write command and argument
void writeCommandArgument(unsigned char cmd[], unsigned long lenCmd, unsigned char arg[] = nullptr, unsigned long lenArg = 0);
//Run setup functions before firmware
void deviceBegin();
//Read status register
statusRegister_t readStatusRegister();
//Initalize pins
void initalize();
//Reset
void reset(unsigned char resetPin = PIN_DEVICE_RESET);
//Load Firmware from flash memory to device
void loadFirmware(unsigned long addressFirmware, unsigned long sizeFirmware);
//Get free RAM
unsigned short getFreeRam();

//Mute
//0,1,2,3; No, Left, Rright, Both, default No
void writeMute(unsigned char channelMuted = 0);
//Get status of mute
unsigned char readMute();

//Volume
unsigned char volumeUp();
unsigned char volumeDown();


//Component list type 4 Byte
struct componentList_t
{
  //Component ID
  unsigned short componentId;
  //unsigned char tmId:        2;//Transmission mode 0...3
  //unsigned char reserved1:   8;
  //unsigned char subChannelId:6;
  //unsigned char fidcId:      6;
  //unsigned char dgFlag:      1;//data service is transmitted in data groups
  //unsigned char reserved2:   1;
  //unsigned char scId:        12;

  //Component Info
  unsigned char secondaryFlag:          1;//primary (0) or a secondary (1) component of a given service
  unsigned char conditionalAccessFlag:  1;//Conditional access control
  unsigned char serviceType:            6;//Audio Service Component Type

  //Valid Flags
  //unsigned char reserved3:            7;
  unsigned char validFlag:              1;
};

//Servicelist type with information about number of components and type and pointer to component list 5+2 = 7 Byte
struct serviceList_t
{
  unsigned long serviceId;

  //dataFlag == 0
  //unsigned short serviceReference:      12;
  //unsigned char countryId:              4;
  //unsigned short reserved0              16;

  //Service Info 1
  //unsigned char reserved:             1;
  //unsigned char serviceLinkingFlag:   1;
  //unsigned char programType:          5;
  unsigned char dataFlag:               1;//to find audio service

  //Service Info 2
  //unsigned char localFlag:            1;
  //unsigned char conditionalAccess:    3;//0 = unscrambled, 1 = NR-MSK, 2 Eurocrypt EN 50094
  unsigned char numComponents:          4;//Number of components in service (M <= 15)

  //Service Info 3
  //unsigned char reservedServiceInfo3: 4;
  //unsigned char characterSet:         4;

  //The name of this service
  //char serviceLabel[17];

  componentList_t* componentList;  //Pointer to component list
};

//Ensemble header type with information about listsize, version and num of services
//and pointer to first servicelist element 9 = 7+2 Byte
//Ensemble -> ServiceList > ComponentList
struct ensembleHeader_t
{
  unsigned char actualService;//number in serviceList
  unsigned char actualComponent;//number in componentList

  unsigned short listSize;//Max = 2694 Bytes, not including List Size
  unsigned short version;
  unsigned char numServices;//N<=32
  //unsigned char reserved1;
  //unsigned char reserved2;
  //unsigned char reserved3;
  serviceList_t* serviceList;
};

//Time 8 Byte
struct dabTime_t
{
  unsigned short  year;
  unsigned char   month;
  unsigned char   day;
  unsigned char   hour;
  unsigned char   minute;
  unsigned char   second;
  unsigned char   type : 1; //UTC or local
};

//Audio component information 4 Byte
struct audioInformation_t
{
  unsigned short audioBitRate:    16;// AUDIO_BIT_RATE[15:0] Audio bit rate of the current audio service (kbps).
  unsigned short audioSampleRate: 16;//AUDIO_SAMPLE_RATE[15:0] Sample rate of the current audio service (Hz).
  unsigned char audioPsFlag:      1; //Audio Parametric Stereo flag
  unsigned char audioSbrFlag:     1; //Spectral Band Replication flag enhance sound for low bitrates
  unsigned char audioMode:        2; //0 : dual 1 : mono 2 : stereo 3 : joint stereo
  unsigned char audioDrcGain:     6; //dynamic range control from 0 to 63, representing 0 to 15.75dB
};

//Ensemble Information 7 Byte
struct ensembleInformation_t
{
  unsigned short ensembleId;      //EID[15:0] The ensemble ID EID. See section 6.4 of ETSI EN 300401.
  char ensembleLabel[17];         //16 characters for the ensemble label terminated by '/0'
  unsigned char ecc;              //The ensemble Extended Country Code (ECC)
  unsigned char charSet;          //The character set for the component label
  unsigned short abbreviationMask;//The component label abbreviation mask.
};

//Service Information
struct serviceInformation_t
{
  unsigned long serviceId;

  //serviceInfo1
  unsigned char serviceLinkingInfoFlag: 1; //service linking info
  unsigned char pType:                  5; //Program type
  unsigned char pdFlag:                 1; //Audio program or data flag

  //serviceInfo2
  unsigned char localFlag:              1; //Indicates if the service is available over the entire (0) or part (1) of the ensemble service area
  unsigned char caId:                   3; //Conditional Access Identifier (0 = unscrambled, 1 = NR-MSK, 2 Eurocrypt EN 50094
  unsigned char numComponents:          4; //Number of components in service (M <= 15)

  //serviceInfo3
  unsigned char characterSet:           4;//Character sets (Charset)
  unsigned char ecc;                      //The ensemble Extended Country Code (ECC)

  char serviceLabel[17];                  //Service Label
  unsigned short abbreviationMask;        //The component label abbreviation mask.
};

struct dabComponentInformation_t
{
  unsigned char globalId:           8;//The global reference for the component
  unsigned char language:           6;//The language of the component
  unsigned char characterSet:       6;//The character set for the component label
  char label[17];                   //The component label
  unsigned short abbreviationMask:  16;//The component label abbreviation mask.
  unsigned char numberUserAppTypes: 8;//NUMUA[7:0] The number of user application types.
  unsigned char lenUserApp:         8;//LENUA[7:0] The total length (in byte) of the UATYPE, UADATALEN and UADATA fields, including the padding bytes which is described in UADATAN field.
  unsigned short userAppType:       16;//UATYPE[15:0] The user application type. If multiple UA Types exist, all UATTYPE fields will be aligned on a 16-bit (2 byte) boundary.
  unsigned char lenUserAppData:     8;//The user application data field length, excluding the padding byte which is described in UADATAN field.
  char* userAppData;                //UADATA0[7:0] The first user application data byte.
};

struct eventInformation_t
{
  unsigned char ensembleReconfigInterrupt:          1;
  unsigned char ensembleReconfigWarningInterrupt:   1;
  unsigned char announcementInterrupt:              1;
  unsigned char otherServiceInterrupt:              1;
  unsigned char serviceLinkingInterrupt:            1;
  unsigned char frequencyInterrupt:                 1;
  unsigned char serviceListInterrupt:               1;
  unsigned char announcementAvailable:              1;
  unsigned char otherServiceAvailable:              1;
  unsigned char serviceLinkingAvailable:            1;
  unsigned char frequencyAvailable:                 1;
  unsigned char serviceListAvailable:               1;
  unsigned short currentServiceListVersion:         16;
};

struct rsqInformation_t
{
  unsigned char hardMuteInterrupt:  1;//Indicates that the audio had been muted This is likely due to poor signal conditions
  unsigned char ficErrorInterrupt:  1;//Indicates the FIC decoder has encountered unrecoverable errors. This is likely due to poor signal conditions
  unsigned char acqInterrupt:       1;//Indicates a change in the ensemble acquisition state
  unsigned char rssiHighInterrupt:  1;//Indicates RSSI below DAB_DIGRAD_RSSI_LOW_THRESHOLD
  unsigned char rssiLowInterrupt:   1;//Indicates RSSI above DAB_DIGRAD_RSSI_HIGH_THRESHOLD

  unsigned char hardmute:           1;//When set to 0 the audio is unmuted. When set to 1 the audio is hard muted
  unsigned char ficError:           1;//When set to 1 the ensemble is experiencing FIC errors. Signal quality has been degraded and acquisition may be lost
  unsigned char acq:                1;//When set to 1 the ensemble is acquired
  unsigned char valid:              1;

  char rssi:                        8;//Received signal strength indicator. -128-63
  char snr:                         8;//Indicates the current estimate of the digital SNR in dB. -128-63
  unsigned char ficQuality:         8;//Indicates the current estimate of the ensembles FIC quality. Range: 0-100
  unsigned char cnr:                8;//Indicates the current estimate of the CNR in dB. The CNR is the ratio of the OFDM signal level during the on period and during the off (null) period. Range: 0-54
  unsigned short fibErrorCount:    16;//Indicates the num of Fast Information Blocks received with errors.
  unsigned long frequency:         32;//indicates the currently tuned frequency in kHz.
  unsigned char index:              8;//Indicates the currently tuned frequency index. Range: 0-47
  unsigned char fftOffset:          8;//Indicates the frequency offset of the DQPSK tones of the OFDM signal relative to the center of the FFT bins of the digital demod.
  unsigned short varactorCap:      16;//Returns the antenna tuning varactor cap value.
  unsigned short cuLevel:          16;//Returns the Capacity Unit usage indicator (number of currently decoded CUs)Range: 0-470
  unsigned char fastDect:           8;//Returns the statistical metric for DAB fast detect. The metric is a confidence level that dab signal is detected. The threshold for dab detected is greater than 4.
};

struct dabComponentTechnicalInformation_t
{
  unsigned char serviceMode:        8;//Indicates the service mode of the sub-channel
  unsigned char protectionInfo:     8;//Indicates the protection profile of the sub-channel
  unsigned short bitRate:           16;//Sub-channel bit rate (kpbs)
  unsigned short numberCU:          16;//The number of Capacity units assigned to this service component
  unsigned short addressCU:         16;//The CU starting address of this subchannel within the CIF
};

struct dabServiceData_t
{
  unsigned char errorInterrupt:     1;
  unsigned char overflowInterrupt:  1;
  unsigned char packetInterrupt:    1;
  unsigned char bufferCount:        8;
  unsigned char statusService:      8;
  unsigned char dataSource:         2;
  unsigned short dataType:          6;
  //Source = Data/PAD/Audio = 00: Standard data channel for data services not related to audio
  //TYPE = 0, RFU
  //Source = Data/PAD/Audio = 01: Data over PAD , Non-DLS, DLS+
  //TYPE: conforms to Table 2: DSCTy types found in [2], selected values are
  //TYPE = 0: unspecified data //TYPE = 1: TMC //TYPE = 5: TDC/TPEG //TYPE =60: MOT
  //Source = Data/PAD/Audio = 10: DLS/DL+ over PAD for DLS services
  //Type = 0, RFU
  //Source = Data/PAD/Audio = 11: Audio (RFU; Audio Currently not transported over the DSRV interface)
  //Type = 0 for MPEG I or II foreground //Type = 1 for MPEG I or II Background //Type = 2 for multichannel MPEG II
  unsigned long serviceId;
  unsigned long componentId;
  //unsigned short rfu;
  unsigned short dataLength;
  unsigned short segmentNumber;
  unsigned short numberSegments;
};


//DAB specific delay times
enum DURATIONS_DAB
{
  //in mikroseconds
  DURATION_STOP_START_SERVICE   = 10000,
  DURATION_TUNE                 = 10000,//Seek Tune Index 600ms
  DURATION_15000_MIKROS         = 15000,
  DURATION_10000_MIKROS         = 10000,//Get ensemble info
};

enum CONSTANTS_DAB
{
  MAX_INDEX = 48,//Maximal number of indices in table
  //Very memory intensive for Uno !
  //To use about 500 Bytes of RAM
  MAX_NUMBER_SERVICES   = 20, //to ETSI standard<=32
  MAX_NUMBER_COMPONENTS = 4  //to ETSI standard<=15
};


//Actual Digital Service
extern unsigned long serviceId;
//Actual Digital Component
extern unsigned long componentId;

//Actual index
extern unsigned char dabIndex;
//Number of valid indices
extern unsigned char dabNumValidIndex;
//Table of valid frequencies
extern unsigned char* dabValidIndexList;

/*
    uint8_t               _scannedIndexListSize;//Number of scanned valid indices
    uint8_t*              _scannedIndexList;//Pointer to table of scanned valid indices
    uint8_t               _positionScannedIndexList;//Position in table
*/

//Frequency table - dynamic memory allocation
extern long unsigned* frequencyTable;

//0xB9 DAB_GET_FREQ_LIST Get frequency table
unsigned long* readFrequencyTable();
//Read number of frequencies
unsigned char readNumberFrequencies();

//Number properties DAB
enum NUM_PROPERTIES_DAB {NUM_PROPERTIES_DAB = 26};
//Property value list DAB
extern unsigned short propertyValueListDab[NUM_PROPERTIES_DAB][2];


//DAB functions

//Constructor
void dabBegin();


//Ensemble dynamic allocation
extern struct ensembleHeader_t ensembleHeader;
//Get ensemble header
void getEnsembleHeader(ensembleHeader_t &ensembleHeader, unsigned char serviceType = 0);
//Get ensemble an fill serviceList and componentList
void getEnsemble(ensembleHeader_t &ensembleHeader, unsigned char serviceType = 0);
//Free memory from Ensemble List Data Structure
void freeMemoryFromEnsembleList(ensembleHeader_t &ensembleHeader);

//Start next service in ensemble
unsigned char nextService(unsigned long &serviceId, unsigned long &componentId);
//Start previous service in ensemble
unsigned char previousService(unsigned long &serviceId, unsigned long &componentId);
//Search _serviceId and _componentId in ensemble and save in actualService, if found returns true
bool searchService(unsigned long &serviceId, unsigned long &componentId);
//Start first serviceType (0 = audio) in ensemble
void startFirstService(unsigned long &serviceId, unsigned long &componentId, unsigned char serviceType = 0);


//Scan all indices of frequency table
bool dabBandScan(unsigned char &dabValidNumFreq, unsigned char* &dabValidFreqTable);

//Scan next valid frequency
unsigned char scan(bool up = true);
//Tune up = true/down = false
unsigned char tune(bool up = true);

//Test varactor tuning capacitor
unsigned short dabTestVaractorCap(unsigned char index, unsigned char injection = 0, unsigned char numberMeasurments = 10);


//0x81 START_DIGITAL_SERVICE Starts an audio or data service
void startService(unsigned long &serviceId, unsigned long &componentId, const unsigned char serviceType = 0);
//0x82 STOP_DIGITAL_SERVICE Stops an audio or data service
void stopService(unsigned long &serviceId, unsigned long &componentId, const unsigned char serviceType = 0);
//0x84 GET_DIGITAL_SERVICE_DATA Gets a block of data associated with one of the enabled data components of a digital services*/
dabServiceData_t dabGetServiceData(unsigned char statusOnly = 1, unsigned char ack = 0);
//0xB0 Tunes to frequency index
unsigned char tuneIndex(unsigned char index, unsigned short varCap = 0, unsigned char injection = 0);
//0xB2 DAB_DIGRAD_STATUS Get status information about the received signal quality
rsqInformation_t readRsqInformation(unsigned char clearDigradInterrupt = 0, unsigned char rssiAtTune = 0, unsigned char clearStcInterrupt = 0);
//0xB3 DAB_GET_EVENT_STATUS Gets information about the various events related to the DAB radio
eventInformation_t readEventInformation(unsigned char eventAck = 0);
//0xB4 DAB_GET_ENSEMBLE_INFO Gets information about the current ensemble
ensembleInformation_t readEnsembleInformation();
//0xB8 DAB_SET_FREQ_LIST Set frequency table
void writeFrequencyTable(const unsigned long frequencyTable[], const unsigned char numFreq);

//0xBB DAB_GET_COMPONENT_INFO Get information about the component application data
dabComponentInformation_t dabGetComponentInformation(unsigned long &serviceId, unsigned long &componentId);
//0xBC DAB_GET_TIME Gets the ensemble time adjusted for the local time offset (0) or the UTC (1)
dabTime_t dabGetDateTimeInformation(unsigned char timeType = 1);
//0xBD DAB_GET_AUDIO_INFO Gets audio information
audioInformation_t readAudioInformation();
//0xBE DAB_GET_SUBCHAN_INFO Get technical information about the component
dabComponentTechnicalInformation_t dabGetComponentTechnicalInformation(unsigned long &serviceId, unsigned long &componentId);
//0xC0 DAB_GET_SERVICE_INFO Get digital service information
serviceInformation_t readServiceInformation(unsigned long &serviceId);
//0xBF DAB_GET_FREQ_INFO Gets ensemble frequency information list
unsigned long dabGetEnsembleFrequencyInformationList();

//to do
//0xB7 DAB_GET_SERVICE_LINKING_INFO Provides service linking info for the passed in service ID
bool readServiceLinkingInfo(unsigned long &serviceId);


//Tuner commands
enum COMMANDS_DEVICE
{
  READ_REPLY        = 0x00,//0x00 RD_REPLY Returns the status byte and data for the last command sent to the device
  POWER_UP          = 0x01,//0x01 POWER_UP Power-up the device and set system settings

  HOST_LOAD         = 0x04,//0x04 HOST_LOAD Loads an image from HOST over command interface
  FLASH_LOAD        = 0x05,//0x05 FLASH_LOAD Loads an image from external FLASH over secondary SPI bus
  LOAD_INIT         = 0x06,//0x06 LOAD_INIT Prepares the bootloader to receive a new image
  BOOT              = 0x07,//0x07 BOOT Boots the image currently loaded in RAM
  GET_PART_INFO     = 0x08,//0x08 GET_PART_INFO Reports basic information about the device
  GET_SYS_STATE     = 0x09,//0x09 GET_SYS_STATE Reports system state information
  GET_POWER_UP_ARGS = 0x0A,//0x0A GET_POWER_UP_ARGS Reports basic information about the device such as arguments used during POWER_UP
  READ_OFFSET       = 0x10,//0x10 READ_OFFSET Reads a portion of response buffer from an offset.

  GET_FUNC_INFO     = 0x12,//0x12 GET_FUNC_INFO Returns the Function revision information of the device
  SET_PROPERTY      = 0x13,//0x13 SET_PROPERTY Sets the value of a property
  GET_PROPERTY      = 0x14,//0x14 GET_PROPERTY Retrieve the value of a property
  WRITE_STORAGE     = 0x15,//0x15 WRITE_STORAGE Writes data to the on board storage area at a specified offset.
  READ_STORAGE      = 0x16,//0x16 READ_STORAGE Reads data from the

  TEST_GET_RSSI     = 0xE5,//0xE5 TEST_GET_RSSI Returns the reported RSSI in 8.8 format.
};


//Comands DAB
enum COMMANDS_DAB
{
  GET_DIGITAL_SERVICE_LIST          = 0x80,  //0x80 GET_DIGITAL_SERVICE_LIST Gets a service list of the ensemble.
  START_DIGITAL_SERVICE             = 0x81,  //0x81 START_DIGITAL_SERVICE Starts an audio or data service.
  STOP_DIGITAL_SERVICE              = 0x82,  //0x82 STOP_DIGITAL_SERVICE Stops an audio or data service.
  GET_DIGITAL_SERVICE_DATA          = 0x84,//0x84 GET_DIGITAL_SERVICE_DATA Gets a block of data associated with one of the enabled data components of a digital services.

  DAB_TUNE_FREQ                     = 0xB0,//0xB0 DAB_TUNE_FREQ Tunes the DAB Receiver to tune to a frequency between 168.16 and 239.20 MHz defined by the frequency table through DAB_SET_FREQ_LIST.
  DAB_DIGRAD_STATUS                 = 0xB2,//0xB2 DAB_DIGRAD_STATUS Returns status information about the digital radio and ensemble.
  DAB_GET_EVENT_STATUS              = 0xB3,  //0xB3 DAB_GET_EVENT_STATUS Gets information about the various events related to the DAB radio.
  DAB_GET_ENSEMBLE_INFO             = 0xB4,  //GxB4 DAB_GET_ENSEMBLE_INFO Gets information about the current ensemble
  DAB_GET_ANNOUNCEMENT_SUPPORT_INFO = 0xB5,  //Gets the announcement support information
  DAB_GET_ANNOUNCEMENT_INFO         = 0xB6,  //gets announcement information from the announcement queue
  DAB_GET_SERVICE_LINKING_INFO      = 0xB7,  //0xB7 DAB_GET_SERVICE_LINKING_INFO Provides service linking info for the passed in service ID.
  DAB_SET_FREQ_LIST                 = 0xB8,//0xB8 DAB_SET_FREQ_LIST Sets the DAB frequency table. The frequencies are in units of kHz.
  DAB_GET_FREQ_LIST                 = 0xB9,  //0xB9 DAB_GET_FREQ_LIST Gets the DAB frequency table
  DAB_GET_COMPONENT_INFO            = 0xBB,  //0xBB DAB_GET_COMPONENT_INFO Gets information about components within the ensemble if available.
  DAB_GET_TIME                      = 0xBC,  //0xBC DAB_GET_TIME Gets the ensemble time adjusted for the local time offset or the UTC.
  DAB_GET_AUDIO_INFO                = 0xBD,  //0xBD DAB_GET_AUDIO_INFO Gets audio service info
  DAB_GET_SUBCHAN_INFO              = 0xBE,  //0xBE DAB_GET_SUBCHAN_INFO Gets sub-channel info
  DAB_GET_FREQ_INFO                 = 0xBF,  //0xBF DAB_GET_FREQ_INFO Gets ensemble freq info

  DAB_GET_SERVICE_INFO              = 0xC0,  //0xC0 DAB_GET_SERVICE_INFO Gets information about a service
  DAB_GET_OE_SERVICES_INFO          = 0xC1,//Provides other ensemble (OE) services (FIG 0/24)information for the passed in service ID
  DAB_ACF_STATUS                    = 0xC2,//Returns status information about automatically controlled features

  DAB_TEST_GET_BER_INFO             = 0xE8//Reads the current BER rate
};

//Frequencies between 168,16 MHz and 239,20 MHz
//Frequency distance = 1712Hz

enum VHF_Band_III
{
  CHAN_5A = 174928,
  CHAN_5B = 176640,
  CHAN_5C = 178352,//DR Deutschland D__00188
  CHAN_5D = 180064,
  CHAN_6A = 181936,
  CHAN_6B = 183648,
  CHAN_6C = 185360,
  CHAN_6D = 187072,
  CHAN_7A = 188928,
  CHAN_7B = 190640,//hr Radio
  CHAN_7C = 192352,
  CHAN_7D = 194064,
  CHAN_8A = 195936,
  CHAN_8B = 197648,
  CHAN_8C = 199360,//Mittelfranken
  CHAN_8D = 201072,
  CHAN_9A = 202928,
  CHAN_9B = 204640,
  CHAN_9C = 206352,
  CHAN_9D = 208064,
  CHAN_10A = 209936,
  CHAN_10N = 210096,
  CHAN_10B = 211648,
  CHAN_10C = 213360,
  CHAN_10D = 215072,
  CHAN_11A = 216928,//SWR RP D__00217
  CHAN_11N = 217088,
  CHAN_11B = 218640,//DRS BW
  CHAN_11C = 220352,
  CHAN_11D = 222064,//BR Bayern
  CHAN_12A = 223936,
  CHAN_12N = 224096,
  CHAN_12B = 225648,
  CHAN_12C = 227360,//Hessen Süd
  CHAN_12D = 229072,
  CHAN_13A = 230784,
  CHAN_13B = 232496,
  CHAN_13C = 234208,
  CHAN_13D = 235776,
  CHAN_13E = 237488,
  CHAN_13F = 239200
};

//MAX_INDEX = 48
const unsigned long frequencyTableDefault[41] =
{
  CHAN_5A, CHAN_5B, CHAN_5C, CHAN_5D,
  CHAN_6A, CHAN_6B, CHAN_6C, CHAN_6D,
  CHAN_7A, CHAN_7B, CHAN_7C, CHAN_7D,
  CHAN_8A, CHAN_8B, CHAN_8C, CHAN_8D,
  CHAN_9A, CHAN_9B, CHAN_9C, CHAN_9D,
  CHAN_10A, CHAN_10N, CHAN_10B, CHAN_10C, CHAN_10D,
  CHAN_11A, CHAN_11N, CHAN_11B, CHAN_11C, CHAN_11D,
  CHAN_12A, CHAN_12N, CHAN_12B, CHAN_12C, CHAN_12D,
  CHAN_13A, CHAN_13B, CHAN_13C, CHAN_13D, CHAN_13E, CHAN_13F
};

const unsigned long frequencyTableNrw[] =    {CHAN_5C, CHAN_11D};
const unsigned long frequencyTableBayern[] = {CHAN_5C, CHAN_12D, CHAN_11D, CHAN_9C, CHAN_10C, CHAN_11A, CHAN_11C, CHAN_12A, CHAN_6A};
const unsigned long frequencyTableBw[] =     {CHAN_5C, CHAN_8D, CHAN_9D, CHAN_11B};
const unsigned long frequency_list_bb[] =     {CHAN_5C, CHAN_7B, CHAN_7D};
const unsigned long frequency_list_hb[] =     {CHAN_5C, CHAN_7B, CHAN_12A};
const unsigned long frequency_list_hh[] =     {CHAN_5C, CHAN_7A};
const unsigned long frequency_list_he[] =     {CHAN_5C, CHAN_7B, CHAN_11C};
const unsigned long frequency_list_mv[] =     {CHAN_5C, CHAN_12B};
const unsigned long frequency_list_ni[] =     {CHAN_5C, CHAN_6A, CHAN_6D, CHAN_11B, CHAN_12A};

const unsigned long frequency_list_sl[] =     {CHAN_5C, CHAN_9A};
const unsigned long frequency_list_sn[] =     {CHAN_5C, CHAN_6C, CHAN_8D, CHAN_9A, CHAN_12A};
const unsigned long frequency_list_st[] =     {CHAN_5C, CHAN_11C, CHAN_12C};
const unsigned long frequency_list_sh[] =     {CHAN_5C, CHAN_9C};
const unsigned long frequency_list_th[] =     {CHAN_5C, CHAN_7B, CHAN_9C, CHAN_12B};

const unsigned long frequency_list_ch[7] =     {CHAN_12A, CHAN_12C, CHAN_12D, CHAN_7D, CHAN_7A, CHAN_9D, CHAN_8B};

const unsigned long frequencyTableRheinlandPfalz[]  = {CHAN_5C, CHAN_11A};

const unsigned long frequencyTableItalienRas[6]      = {CHAN_10B, CHAN_10C, CHAN_10D, CHAN_12A, CHAN_12B, CHAN_12C};

const unsigned long frequencyTableItalienTrentino[2] = {CHAN_10A, CHAN_12D};

const unsigned long frequencyTableUnitedKingdom[3] = {CHAN_11A, CHAN_11D, CHAN_12B};


#endif //SI468X_DRIVER_H
