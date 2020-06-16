//include guard
#ifndef SI468X_H
#define SI468X_H

/*
  Tuner Shield DAB+
  by IGB

  Tuner circuit SI468x
  SPI MOSI:   Pin 11
  SPI MISO:   Pin 12
  SPI SCK:    Pin 13
  SPI SS:     Pin 8
  Interrupt:  Pin 6
  Reset:      Pin 3

  Flash memory circuit SST26W onboard
  SPI SS:     Pin 2

  Flash:  31252 Bytes (96%)
  RAM:    1019 Bytes (49%)
*/

/*Version*/
const char swVersion[] = "0.8.0";

enum SPI_FREQUENCY {SPI_FREQUENCY = 8000000UL};

enum PINS
{
  //Tuner circuit
  PIN_DEVICE_SLAVE_SELECT = 8,
  PIN_DEVICE_INTERRUPT = 6,
  PIN_DEVICE_RESET = 3,
  //Memory circuit
  PIN_FLASH_SLAVE_SELECT = 2,
};

//Max numbers of retry when chip is busy
enum MAX_RETRY {MAX_RETRY = 10};

//Device specific delay times
enum DURATIONS
{
  //Very critical for device to start up
  //https://github.com/arduino/Arduino/issues/129
  //delay does not work in ctor, delayMicroseconds() work in CTOR
  //max delayMicroseconds(16383) = delayMicroseconds(0x3FFF)

  DURATION_5000_MIKROS  = 5000,//3ms see flowchart; 5ms tRSTB_HI see timing
  DURATION_3000_MIKROS  = 3000,//20us see flowchart; 3ms tPOWER_UP see timing
  TIME_REPLY            = 3000,//?ms see flowchart; 1 ms see timing CTS polls @ 1 ms; tested 2500us
  TIME_LOAD_INIT        = 4000,//4ms see flowchart; ?ms see timing
  TIME_BOOT             = 10000,//350ms = 30 * 10000 us in loop, Boot time 63ms at analog FM, 198ms at DAB

  //DAB specific delay times
  //in milliseconds
  DURATION_20_MILLIS    = 20,//Get ensemble info
  DURATION_200_MILLIS   = 200 //Seek Tune Index 600ms
};


/*Global variables of device*/
/*Image*/
extern unsigned char deviceImage;

/*Firmware Information*/
extern struct deviceFirmwareInformation_t
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
} deviceFirmwareInformation;

/*Property id*/
extern unsigned short devicePropertyId;

/*Property value*/
extern long devicePropertyValue;

/*Receive signal strength indicator*/
extern unsigned short deviceRssi;

/*Tuning capacitor value*/
extern unsigned short varCap;

/*Device status information*/
extern struct deviceStatusInformation_t
{
  unsigned char cts:        1;  //Clear to Send

  unsigned char cmdErr:     1;  //Command Error. 0 : No error 1 : Error. The previous command failed
  unsigned char dacqInt:    1;  //Digital radio link change interrupt indicator
  unsigned char dsrvInt:    1;  //Digital service Interrupt
  unsigned char stcInt:     1;  //Seek/Tune complete
  unsigned char devNtInt:   1;  //Digital radio event change interrupt indicator

  unsigned char pUpState:   2;  //PUP_STATE 0 : waiting on the POWER_UP command. 1 : Reserved 2 : The bootloader is currently running. 3 : An application was successfully booted and is currently running.
  unsigned char dspErr:     1;  //DSPERR The DSP has encountered a frame overrun
  unsigned char repOfErr:   1;  //Reply overflow Error. SPI clock rate too fast
  unsigned char cmdOfErr:   1;  //Command overflow Error. SPI clock rate too fast
  unsigned char arbErr:     1;  //Arbiter error has occurred
  unsigned char nonRecErr:  1;  //Non recoverable error

  unsigned char cmdErrCode: 8;  //command error code
} deviceStatusInformation;

/*Device part number*/
extern struct devicePartNumber_t
{
  unsigned char chipRev;
  unsigned char romId;
  unsigned short partNumber;
} devicePartNumber;

/*Device power up arguments*/
extern struct devicePowerUpArguments_t
{
  unsigned char clockMode:  2;//Range 0-3
  unsigned char trSize:     4;//Range 0-15
  unsigned char iBiasStart: 7;//Range 0-127
  unsigned long xtalFreq:   32;//5,4 - 46,2 MHz
  unsigned char cTune:      6;//Range 0-63
  unsigned char iBiasRun:   7;//Range 0-127, 10 uA steps, 10 to 1270 uA. If set to 0, will use the same value as iBiasStart
} devicePowerUpArguments;

/*Number of device properties*/
const unsigned char numberDeviceProperties = 19;

/*Device property value list*/
extern struct devicePropertyValueList_t
{
  unsigned short devicePropertyId;
  unsigned short devicePropertyValue;
} devicePropertyValueList[numberDeviceProperties];



/*0x00 RD_REPLY Get Device Status Information*/
deviceStatusInformation_t deviceGetStatus();
/*Sets the power up arguments*/
bool deviceSetPowerUpArguments(devicePowerUpArguments_t &devicePowerUpArguments);
/*0x01 POWER_UP Power-up the device and set system settings*/
void devicePowerUp(devicePowerUpArguments_t devicePowerUpArguments, unsigned char ctsInterruptEnabled = 1);
//0x04 HOST_LOAD Loads an image from HOST over command interface
void deviceHostLoad(unsigned char package[], unsigned short len);
//0x05 FLASH_LOAD Loads an image from external FLASH over secondary SPI bus
void deviceFlashLoad(unsigned long address, unsigned char subCommand = 0);
/*0x06 LOAD_INIT Prepares the bootloader to receive a new image*/
void deviceLoadInit();
//0x07 BOOT Boots the image currently loaded in RAM
void deviceBoot();
/*0x08 GET_PART_INFO Get Device Part Number*/
bool deviceGetPartNumber(devicePartNumber_t &devicePartNumber);
/*0x09 GET_SYS_STATE Get Device Image*/
bool deviceGetImage(unsigned char &deviceImage);
/*0x0A GET_POWER_UP_ARGS Reports basic information about the device such as arguments used during POWER_UP*/
devicePowerUpArguments_t deviceGetPowerUpArguments();
/*0x10 READ_OFFSET Reads a portion of response buffer from an offset*/
bool deviceReadOffset(unsigned short offset, unsigned char data[]);
/*0x12 GET_FUNC_INFO Get Firmware Information*/
bool deviceGetFirmwareInformation(deviceFirmwareInformation_t &deviceFirmwareInfo);
/*0x13 SET_PROPERTY Sets the value of a property*/
bool deviceSetProperty(unsigned short devicePropertyId, unsigned short devicePropertyValue);
/*0x14 GET_PROPERTY Retrieve the value of a property*/
bool deviceGetProperty(unsigned short devicePropertyId, unsigned short &devicePropertyValue, unsigned char count = 1);
/*0x15 WRITE_STORAGE Writes data to the on board storage area at a specified offset*/
bool deviceWriteStorage(unsigned short offset, unsigned char data[], unsigned char len);
/*0x16 READ_STORAGE Reads data from the on board storage area from a specified offset*/
bool deviceReadStorage(unsigned short offset, unsigned char data[], unsigned char len);
/*0xE5 TEST_GET_RSSI returns the reported RSSI in 8.8 format*/
unsigned short deviceGetRssi();
/*0xE800 TEST_GET_BER_INFO Reads the current BER rate*/
bool deviceGetBitErrorRate(unsigned short &deviceBitErrorRate);

//Helper functions device
void deviceInitalize();
void deviceReset(unsigned char resetPin = PIN_DEVICE_RESET);
/*Check if device is busy*/
bool deviceReady(unsigned char buf[], unsigned short len, unsigned short tDelay = 0, bool printBuffer = false);
/*Check if device has seek tune completed*/
bool deviceSeekTuneComplete(unsigned char buf[], unsigned short len, unsigned short tDelay = DURATION_200_MILLIS, bool printBuffer = false);
/*Get all device property values*/
bool deviceGetAllProperties(struct devicePropertyValueList_t devicePropertyValueList[], unsigned char num = numberDeviceProperties);
/*Set all device properties*/
bool deviceSetAllProperties(struct devicePropertyValueList_t devicePropertyValueList[], unsigned char num = numberDeviceProperties);
/*Get average rssi*/
unsigned short deviceGetAverageRssi(unsigned char numberMeasurements = 10);

//Communication functions
//Load Firmware from flash memory to device
bool hostLoadDataTuner(unsigned long startAddress, unsigned long fileSize);

//Run setup functions before firmware
bool deviceLoadFirmware();

//Sets the power up arguments
bool deviceSetPowerUpArguments(devicePowerUpArguments_t &devicePowerUpArguments);

//Print Hex answer of device
bool printHexStr(unsigned char str[], unsigned long len);


//Datatypes DAB
//Very memory intensive for Uno !
//To use about 500 Bytes of RAM
const unsigned char dabMaxNumServices   = 20; //to ETSI standard<=32
const unsigned char dabMaxNumComponents = 4;  //to ETSI standard<=15

//Global variables DAB
//Actual index
extern unsigned char dabIndex;
//Number of valid indices
extern unsigned char dabNumValidIndex;
//Table of valid frequencies
extern unsigned char* dabValidIndexList;

//Frequency table - dynamic memory allocation
extern long unsigned* dabFreqTable;
//Available number of frequencies in table
extern unsigned char dabNumFreq;
//Maximal number of frequencies in table
extern const unsigned char dabMaxNumFreq;

//Actual Service is running
extern bool dabServiceRunning;
//Audio is muted
extern bool mute;
//Audio volume
extern unsigned char volume;

//Actual Digital Service
extern unsigned long dabServiceId;
//Actual Digital Component
extern unsigned long dabComponentId;

//Actual Service List Size
extern unsigned short dabListSize;
//Actual Service List Version
extern unsigned short dabListVersion;
//Actual Number of Services
extern unsigned char dabNumServices;

//Component Information
struct dabComponent_t
{
  //Component ID
  unsigned char componentId;
  //unsigned char tmId:        2;//Transmission mode 0...3
  //unsigned char reserved1:   8;
  //unsigned char subChannelId:6;
  //unsigned char fidcId:      6;
  //unsigned char dgFlag:      1;//data service is transmitted in data groups
  //unsigned char reserved2:   1;
  //unsigned char scId:        12;

  //Component Info
  unsigned char psFlag:         1;//primary (0) or a secondary (1) component of a given service
  unsigned char caFlag:         1;//Conditional access control
  unsigned char type:           6;//ASCTyÃƒÂ¢Ã¢â€šÂ¬Ã¢â‚¬ï¿½Audio Service Component TypeÃƒÂ¯Ã¢â€šÂ¬Ã‚Â DSCTyÃƒÂ¢Ã¢â€šÂ¬Ã¢â‚¬ï¿½Data Service Component Type

  //Valid Flags
  //unsigned char reserved3:    7;
  unsigned char valid:          1;
};

/*Ensemble type with Service Id, component id, number of components and pinter to components*/
struct dabEnsemble_t
{
  unsigned char serviceNumber;
  unsigned long serviceId;
  /*Audio program or data flag*/
  unsigned char pdFlag:         1;
  /*Number of components in service (M <= 15)*/
  unsigned char numComponents:  4;
  /*Pointer to component list*/
  unsigned long* componentList;
};

/*Pointer to actual Ensemble List*/
extern struct dabEnsemble_t* dabEnsembleList;
/*Actual Service in ensemble list*/
extern int dabActualService;


//Number of dab properties
const unsigned char dabNumberProperties = 26;
//DAB property value list
extern struct dabPropertyValueList_t
{
  unsigned short dabPropertyId;
  unsigned short dabPropertyValue;
} dabPropertyValueList[dabNumberProperties];


/*Service list header with information about listsize, version and num of services*/
extern struct dabServiceListHeader_t
{
  unsigned short listSize;
  unsigned short version;
  unsigned char numServices;
  //unsigned char reserved1;
  //unsigned char reserved2;
  //unsigned char reserved3;
} dabServiceListHeader;

//Audio component information
extern struct dabAudioComponentInformation_t
{
  unsigned short audioBitRate:    16;// AUDIO_BIT_RATE[15:0] Audio bit rate of the current audio service (kbps).
  unsigned short audioSampleRate: 16;//AUDIO_SAMPLE_RATE[15:0] Sample rate of the current audio service (Hz).
  unsigned char audioPsFlag:      1; //Audio Parametric Stereo flag
  unsigned char audioSbrFlag:     1; //Spectral Band Replication flag enhance sound for low bitrates
  unsigned char audioMode:        2; //0 : dual 1 : mono 2 : stereo 3 : joint stereo
  unsigned char audioDrcGain:     6; //dynamic range control from 0 to 63, representing 0 to 15.75dB
} dabAudioComponentInformation;


//Ensemble Information
extern struct dabEnsembleInformation_t
{
  unsigned short ensembleId;      //EID[15:0] The ensemble ID EID. See section 6.4 of ETSI EN 300401.
  char ensembleLabel[17];         //16 characters for the ensemble label terminated by '/0'
  unsigned char ecc;              //The ensemble Extended Country Code (ECC)
  unsigned char charSet;          //The character set for the component label
  unsigned short abbreviationMask;//The component label abbreviation mask.
} dabEnsembleInformation;

//Service Information
extern struct dabServiceInformation_t
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

} dabServiceInformation;

//Component Information
extern struct dabComponentInformation_t
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
} dabComponentInformation;

//Event Information
extern struct dabEventInformation_t
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
} dabEventInformation;

//Received Signal Quality Information
extern struct dabRsqInformation_t
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
} dabRsqInformation;

//Component Technical Information
extern struct dabComponentTechnicalInformation_t
{
  unsigned char serviceMode:        8;//Indicates the service mode of the sub-channel
  unsigned char protectionInfo:     8;//Indicates the protection profile of the sub-channel
  unsigned short bitRate:           16;//Sub-channel bit rate (kpbs)
  unsigned short numberCU:          16;//The number of Capacity units assigned to this service component
  unsigned short addressCU:         16;//The CU starting address of this subchannel within the CIF
} dabComponentTechnicalInformation;


//DAB service data block
extern struct dabServiceData_t
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
} dabServiceData;

/*DAB functions*/
/*Start DAB*/
bool dabStart();

/*0x81 START_DIGITAL_SERVICE Starts an audio or data service*/
bool dabStartDigitalService(unsigned long dabServiceId, unsigned long dabComponentId, unsigned char dabServiceType = 0);
/*0x82 STOP_DIGITAL_SERVICE Stops an audio or data service*/
bool dabStopDigitalService(unsigned long dabServiceId, unsigned long dabComponentId, unsigned char dabServiceType = 0);
/*0x84 GET_DIGITAL_SERVICE_DATA Gets a block of data associated with one of the enabled data components of a digital services*/
bool dabGetServiceData(struct dabServiceData_t &dabServiceData, unsigned char statusOnly = 1, unsigned char ack = 0);
/*0xB0 Tunes to frequency index
    injection[1:0] Injection selection  0 : Automatic injection selection 1 : Low-side injection 2 : High-side injection
    index[7:0] Frequency index for the tuned frequency, see the DAB_SET_FREQ_LIST command that sets the frequency table
    varCap[15:0] Antenna tuning capacitor value in 250 fF units (31.75 pF Max) Range: 0-128 0 : Automatically determines the cap setting
*/
bool dabTuneIndex(unsigned char index, unsigned short varCap = 0, unsigned char injection = 0);
/*0xB2 DAB_DIGRAD_STATUS Get status information about the received signal quality*/
bool dabGetRsqStatus(dabRsqInformation_t &dabRsqInformation, unsigned char clearDigradInterrupt = 0, unsigned char rssiAtTune = 0, unsigned char clearStcInterrupt = 0);
/*0xB3 DAB_GET_EVENT_STATUS Gets information about the various events related to the DAB radio*/
bool dabGetEventStatus(struct dabEventInformation_t &dabEventInfo, unsigned char eventAck = 0);
/*0xB4 DAB_GET_ENSEMBLE_INFO Gets information about the current ensemble*/
dabEnsembleInformation_t dabGetEnsembleInfo();
/*0xB8 DAB_SET_FREQ_LIST Set frequency table*/
bool dabSetFrequencyTable(unsigned long* &dabFreqTable, unsigned char &dabNumFreq, const unsigned long* dabNewFreqTable, unsigned char dabNewNumFreq);
/*0xB9 DAB_GET_FREQ_LIST Get frequency table*/
bool dabGetFrequencyTable(unsigned long* &dabFreqTable, unsigned char &dabNumFreq);
/*0xBB DAB_GET_COMPONENT_INFO Get information about the component application data*/
bool dabGetComponentInformation(unsigned long dabServiceId, unsigned long dabComponentId, dabComponentInformation_t &dabComponentInformation);
/*0xBC DAB_GET_TIME Gets the ensemble time adjusted for the local time offset or the UTC*/
bool dabGetDateTimeInformation(unsigned short &year, unsigned char &month, unsigned char &day, unsigned char &hour, unsigned char &minute, unsigned char &second, unsigned char timeType = 1);
/*0xBD DAB_GET_AUDIO_INFO Gets audio service info*/
bool dabGetComponentAudioInfo(dabAudioComponentInformation_t &dabAudioComponentInformation);
/*0xBE DAB_GET_SUBCHAN_INFO Get technical information about the component*/
bool dabGetComponentTechnicalInformation(unsigned long dabServiceId, unsigned long dabComponentId, dabComponentTechnicalInformation_t &dabComponentTechnicalInformation);
/*0xC0 DAB_GET_SERVICE_INFO Get digital service information*/
bool dabGetDigitalServiceInformation(dabServiceInformation_t &dabServiceInformation, unsigned long dabServiceId);

/*to do*/
/*0xBF DAB_GET_FREQ_INFO Gets ensemble frequency information list*/
bool dabGetEnsembleFrequencyInformationList();
/*0xB7 DAB_GET_SERVICE_LINKING_INFO Provides service linking info for the passed in service ID*/
bool dabGetDigitalServiceLinkingInfo(unsigned long dabServiceId);
/*Scan all indices of frequency table*/
bool dabBandScan(unsigned char &dabValidNumFreq, unsigned char* &dabValidFreqTable);
/*Tune to next entry in frequency table*/
bool dabScanUp(unsigned char &index);
/*Tune to previous entry in frequency table*/
bool dabScanDown(unsigned char &index);

/*DAB helper functions*/
/*Volume up*/
bool dabVolumeUp(unsigned char &volume);
/*Volume down*/
bool dabVolumeDown(unsigned char &volume);
/*Volume mute and unmute*/
bool dabVolumeMute(bool mute);
/*Get all property values*/
bool dabGetAllProperties(struct dabPropertyValueList_t dabPropertyValueList[], unsigned char num = dabNumberProperties);
/*Set all property vlaues*/
bool dabSetAllProperties(struct dabPropertyValueList_t dabPropertyValueList[], unsigned char num = dabNumberProperties);
/*Get service list header*/
bool dabGetDigitalServiceListHeader(struct dabServiceListHeader_t &dabServiceListHeader, unsigned char dabServiceType = 0);
/*Parse ensemble an fill array with serviceId and component Id*/
bool dabParseEnsemble(struct dabEnsemble_t* &dabEnsembleList, unsigned char &dabNumServices, unsigned char dabServiceType = 0);
/*Test varactor tuning capacitor*/
unsigned short dabTestVaractorCap(unsigned char index, unsigned char injection = 0, unsigned char numberMeasurments = 10);


//Tuner commands
enum COMMANDS_DEVICE
{
  RD_REPLY          = 0x00,//0x00 RD_REPLY Returns the status byte and data for the last command sent to the device
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

//dabMaxNumFreq = 48
const unsigned long frequencyTableAll[41] = {
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

const unsigned long frequency_list_ch[] =     {CHAN_12A, CHAN_12C, CHAN_12D, CHAN_7D, CHAN_7A, CHAN_9D, CHAN_8B};

const unsigned long frequencyTableRheinlandPfalz[]  = {CHAN_5C, CHAN_11A};

const unsigned long frequencyTableItalienRas[]      = {CHAN_10B, CHAN_10C, CHAN_10D, CHAN_12A, CHAN_12B, CHAN_12C};

const unsigned long frequencyTableItalienTrentino[] = {CHAN_10A, CHAN_12D};

const unsigned long frequencyTableUnitedKingdom[] = {CHAN_11A, CHAN_11D, CHAN_12B};

#endif //SI468X_H
