#ifndef PROPERTIES_H
#define PROPERTIES_H

/*properties of tuner circuit*/

//General Properties
//0x0000 INT_CTL_ENABLE Interrupt enable property 0x0000
unsigned short const INT_CTL_ENABLE = 0x0000;
//0x0001 INT_CTL_REPEAT Interrupt repeat property 0x0000
unsigned short const INT_CTL_REPEAT = 0x0001;

//Digital Output properties
//0x0200 DIGITAL_IO_OUTPUT_SELECT Selects digital audio Master or Slave. 0
unsigned short const DIGITAL_IO_OUTPUT_SELECT = 0x0200;
//0x0201 DIGITAL_IO_OUTPUT_SAMPLE_RATE Sets output sample audio rate in units of 1Hz. 48000
unsigned short const DIGITAL_IO_OUTPUT_SAMPLE_RATE = 0x0201;
//0x0202 DIGITAL_IO_OUTPUT_FORMAT Configure digital output format. 0x1800
unsigned short const DIGITAL_IO_OUTPUT_FORMAT = 0x0202;
//0x0203 DIGITAL_IO_OUTPUT_FORMAT_OVERRIDES_1 Deviations from the standard framing mode 0
unsigned short const DIGITAL_IO_OUTPUT_FORMAT_OVERRIDES_1 = 0x0203;
//0x0204 DIGITAL_IO_OUTPUT_FORMAT_OVERRIDES_2 Deviations from the standard framing mode 0
unsigned short const DIGITAL_IO_OUTPUT_FORMAT_OVERRIDES_2 = 0x0204;
//0x0205 DIGITAL_IO_OUTPUT_FORMAT_OVERRIDES_3 Deviations from the standard framing mode 0
unsigned short const DIGITAL_IO_OUTPUT_FORMAT_OVERRIDES_3 = 0x0205;
//0x0206 DIGITAL_IO_OUTPUT_FORMAT_OVERRIDES_4 Deviations from the standard framing mode 0
unsigned short const DIGITAL_IO_OUTPUT_FORMAT_OVERRIDES_4 = 0x0206;

//0x0300 AUDIO_ANALOG_VOLUME Sets the audio analog volume. 63
unsigned short const AUDIO_ANALOG_VOLUME = 0x0300;
//0x0301 AUDIO_MUTE property mutes/unmutes each audio output independently.0x0000
unsigned short const AUDIO_MUTE = 0x0301;
//0x0302 AUDIO_OUTPUT_CONFIG is used to configure various settings of the audio output. 0x0000
unsigned short const AUDIO_OUTPUT_CONFIG = 0x0302;

//0x0800 PIN_CONFIG_ENABLE Pin configuration property 0x8001
unsigned short const PIN_CONFIG_ENABLE = 0x0800;

//WAKE TONE Properties
//0x0900 WAKE_TONE_ENABLE Enables the wake tone feature. 0
unsigned short const WAKE_TONE_ENABLE = 0x0900;
//0x0901 WAKE_TONE_PERIOD Sets the wake tone duty cycle. 250
unsigned short const WAKE_TONE_PERIOD = 0x0901;
//0x0902 WAKE_TONE_FREQ Sets the wake tone frequency. 750
unsigned short const WAKE_TONE_FREQ = 0x0902;
//0x0903 WAKE_TONE_AMPLITUDE Sets the wake tone amplitude. 8
unsigned short const WAKE_TONE_AMPLITUDE = 0x0903;

//0xE800 TEST_BER_CONFIG Sets up and enables the BER test 0
unsigned short const TEST_BER_CONFIG = 0xE800;
//0xE801 HD_TEST_DEBUG_AUDIO Used to put the analog audio output in to special test modes for debug purposes. 0
unsigned short const HD_TEST_DEBUG_AUDIO = 0xE801;


//FM Properties
//Frontend config
unsigned short const FM_TUNE_FE_VARM            = 0x1710;//0x1710 FM_TUNE_FE_VARM FM Front End Varactor configuration slope. 0x0000
unsigned short const FM_TUNE_FE_VARB            = 0x1711;//0x1711 FM_TUNE_FE_VARB FM Front End Varactor configuration intercept. 0x0000
unsigned short const FM_TUNE_FE_CFG             = 0x1712;//0x1712 FM_TUNE_FE_CFG Additional configuration options for the front end. 0x0000
//FM Band Config
unsigned short const FM_SEEK_BAND_BOTTOM        = 0x3100;//0x3100 FM_SEEK_BAND_BOTTOM Sets the lower seek boundary of the FM band in multiples of 10kHz. 8750
unsigned short const FM_SEEK_BAND_TOP           = 0x3101;//0x3101 FM_SEEK_BAND_TOP Sets the upper seek boundary for the FM band in multiples of 10kHz. 10790
unsigned short const FM_SEEK_FREQUENCY_SPACING  = 0x3102;//0x3102 FM_SEEK_FREQUENCY_SPACING Sets the frequency spacing for the FM band in multiples of 10kHz when performing a seek. 10
//FM Valid Tune
unsigned short const FM_VALID_MAX_TUNE_ERROR    = 0x3200;//0x3200 FM_VALID_MAX_TUNE_ERROR Sets the maximum frequency error allowed before setting the AFCRL indicator. 114
unsigned short const FM_VALID_RSSI_TIME         = 0x3201;//0x3201 FM_VALID_RSSI_TIME Sets the amount of time in ms to allow the RSSI/ISSI metrics to settle before evaluating. 15
unsigned short const FM_VALID_RSSI_THRESHOLD    = 0x3202;//0x3202 FM_VALID_RSSI_THRESHOLD Sets the RSSI threshold for a valid FM Seek/Tune. 17
unsigned short const FM_VALID_SNR_TIME          = 0x3203;//0x3203 FM_VALID_SNR_TIME Sets the amount of time in ms to allow the SNR metric to settle before evaluating. 40
unsigned short const FM_VALID_SNR_THRESHOLD     = 0x3204;//0x3204 FM_VALID_SNR_THRESHOLD Sets the SNR threshold for a valid FM Seek/Tune. 10
unsigned short const FM_VALID_HDLEVEL_THRESHOLD = 0x3206;//0x3206 FM_VALID_HDLEVEL_THRESHOLD Sets the HDLEVEL threshold for a valid FM Seek/Tune. 0

unsigned short const FM_RSQ_INTERRUPT_SOURCE = 0x3300;        //0x3300 FM_RSQ_INTERRUPT_SOURCE Configures interrupt related to Received Signal Quality metrics. 0
unsigned short const FM_RSQ_SNR_HIGH_THRESHOLD = 0x3301;      //0x3301 FM_RSQ_SNR_HIGH_THRESHOLD sets the high threshold, which triggers the RSQ interrupt if the SNR is above this threshold. 127
unsigned short const FM_RSQ_SNR_LOW_THRESHOLD = 0x3302;       //0x3302 FM_RSQ_SNR_LOW_THRESHOLD sets the low threshold, which triggers the RSQ interrupt if the SNR is below this threshold. -128
unsigned short const FM_RSQ_RSSI_HIGH_THRESHOLD = 0x3303;     //0x3303 FM_RSQ_RSSI_HIGH_THRESHOLD sets the high threshold, which triggers the RSQ interrupt if the RSSI is above this threshold. 127
unsigned short const FM_RSQ_RSSI_LOW_THRESHOLD = 0x3304;      //0x3304 FM_RSQ_RSSI_LOW_THRESHOLD Sets low threshold which triggers the RSQ interrupt if the RSSI is below this threshold. -128
unsigned short const FM_RSQ_HD_DETECTION = 0x3307;            //0x3307 FM_RSQ_HD_DETECTION Configures the Fast HD Detection routine.0x000d
unsigned short const FM_RSQ_HD_LEVEL_TIME_CONST = 0x3308;     //0x3308 FM_RSQ_HD_LEVEL_TIME_CONST Configures the Fast HD Detection Level Metric Filtering Time Constant. 32
unsigned short const FM_RSQ_HDDETECTED_THD = 0x3309;          //0x3309 FM_RSQ_HDDETECTED_THD Configures the HD Level Detected Threshold. 0x1e1e

unsigned short const FM_ACF_INTERRUPT_SOURCE = 0x3400;        //0x3400 FM_ACF_INTERRUPT_SOURCE Enables the ACF interrupt sources. 0
unsigned short const FM_ACF_SOFTMUTE_THRESHOLD = 0x3401;      //0x3401 FM_ACF_SOFTMUTE_THRESHOLD Sets the softmute interrupt threshold in dB attenuation. 31
unsigned short const FM_ACF_HIGHCUT_THRESHOLD = 0x3402;       //0x3402 FM_ACF_HIGHCUT_THRESHOLD Sets the high cut interrupt threshold. 0
unsigned short const FM_ACF_BLEND_THRESHOLD = 0x3403;         //0x3403 FM_ACF_BLEND_THRESHOLD Sets the stereo blend interrupt threshold.0
unsigned short const FM_ACF_SOFTMUTE_TOLERANCE = 0x3404;      //0x3404 FM_ACF_SOFTMUTE_TOLERANCE Sets the distance from the final softmute value that triggers softmute convergence flag. 2
unsigned short const FM_ACF_HIGHCUT_TOLERANCE = 0x3405;       //0x3405 FM_ACF_HIGHCUT_TOLERANCE Sets the distance from the final high cut frequency that triggers the high cut convergence flag. 20
unsigned short const FM_ACF_BLEND_TOLERANCE = 0x3406;         //0x3406 FM_ACF_BLEND_TOLERANCE Sets the distance from the final blend state that triggers the blend convergence flag. 5

unsigned short const FM_SOFTMUTE_SNR_LIMITS = 0x3500;        //Sets the SNR limits for soft mute attenuation. 0x0602
unsigned short const FM_SOFTMUTE_SNR_ATTENUATION = 0x3501;   //Sets the SNR based softmute attenuation limits. 0x0008
unsigned short const FM_SOFTMUTE_SNR_ATTACK_TIME = 0x3502;   //Sets the soft mute attack time. 16
unsigned short const FM_SOFTMUTE_SNR_RELEASE_TIME = 0x3503;  //Sets the soft mute release time. 4000


unsigned short const FM_AUDIO_DE_EMPHASIS = 0x3900;           //0x3900 FM_AUDIO_DE_EMPHASIS Sets the FM Receive de-emphasis. 0 = 75 ÃƒÅ½Ã‚Â¼s
//RDS config
unsigned short const FM_RDS_INTERRUPT_SOURCE = 0x3C00;    //0x3C00 FM_RDS_INTERRUPT_SOURCE This property configures interrupt related to RDS. 0x0000
unsigned short const FM_RDS_INTERRUPT_FIFO_COUNT = 0x3C01; //0x3C01 FM_RDS_INTERRUPT_FIFO_COUNT Configures minimum received data groups in fifo before interrupt. 0x0000
unsigned short const FM_RDS_CONFIG = 0x3C02;              //0x3C02 FM_RDS_CONFIG configures RDS settings to enable RDS processing (RDSEN) and set RDS block error thresholds. 0x0000
unsigned short const FM_RDS_CONFIDENCE = 0x3C03;          //0x3C03 FM_RDS_CONFIDENCE Configures rds block confidence threshold. 0x1111


/*


  0x3600 FM_HIGHCUT_RSSI_LIMITS RSSI based high cut limits. 0x0C06
  0x3601 FM_HIGHCUT_RSSI_CUTOFF_FREQ RSSI based high cut cutoff frequencies. 0xC828
  0x3602 FM_HIGHCUT_RSSI_ATTACK_TIME RSSI based high cut attack time. 16
  0x3603 FM_HIGHCUT_RSSI_RELEASE_TIME RSSI based high cut release time. 4000
  0x3604 FM_HIGHCUT_SNR_LIMITS SNR based high cut limits. 0x0903
  0x3605 FM_HIGHCUT_SNR_CUTOFF_FREQ SNR based high cut cutoff frequencies. 0xc828
  0x3606 FM_HIGHCUT_SNR_ATTACK_TIME SNR based high cut attack time. 16
  0x3607 FM_HIGHCUT_SNR_RELEASE_TIME SNR based high cut release time. 4000
  0x3608 FM_HIGHCUT_MULTIPATH_LIMITS Multipath based high cut limits. 0x2D3C
  0x3609 FM_HIGHCUT_MULTIPATH_CUTOFF_FREQ Multipath based high cut cutoff frequencies. 0xc828
  0x360A FM_HIGHCUT_MULTIPATH_ATTACK_TIME Multipath based high cut attack time. 16
  0x360B FM_HIGHCUT_MULTIPATH_RELEASE_TIME Multipath based high cut release time. 4000

  0x3700 FM_BLEND_RSSI_LIMITS This property sets the RSSI limits for RSSI controlled stereo separation. 0x2010
  0x3702 FM_BLEND_RSSI_ATTACK_TIME RSSI based stereo to mono attack time in ms. 16
  0x3703 FM_BLEND_RSSI_RELEASE_TIME RSSI based mono to stereo release time in ms. 4000
  0x3704 FM_BLEND_SNR_LIMITS Sets the SNR limits for SNR controlled stereo separation. 0x180F
  0x3706 FM_BLEND_SNR_ATTACK_TIME SNR based stereo to mono attack time in ms. 16
  0x3707 FM_BLEND_SNR_RELEASE_TIME SNR based mono to stereo release time in ms. 4000

  0x3708 FM_BLEND_MULTIPATH_LIMITS Sets the multipath limits for multipath controlled stereo separation. 0x2D3C
  0x370A FM_BLEND_MULTIPATH_ATTACK_TIME Multi-path based stereo to mono attack time in ms. 16
  0x370B FM_BLEND_MULTIPATH_RELEASE_TIME Multi-path based mono to stereo release time in ms. 4000


  //HD Properties

  0x9101 HD_BLEND_OPTIONS This property provides options to control HD/analog audio blend behavior.This property is only valid for Hybrid (non-All-Digital HD) Broadcasts. 0x000A
  0x9102 HD_BLEND_ANALOG_TO_HD_TRANSITION_-TIME This property sets the amount of time it takes in ms to blend from analog to HD. This property only applies to primary service channel. 750
  0x9103 HD_BLEND_HD_TO_ANALOG_TRANSITION_-TIME This property sets the amount of time it takes in ms to blend from HD to analog. This property only applies to primary service channel. 100
  0x9106 HD_BLEND_DYNAMIC_GAIN This property sets the digital audio dynamic linear scaling factor. Setting DGAIN_OVERRIDE bit to 1 will override the broadcaster specified digital gain. The DGAIN field is a signed 8 bit fractional number where the linear gain is equal to DGAIN/128. 0
  0x9109 HD_BLEND_BLEND_DECISION_ANALOG_TO_DIGITAL_THD This property defines the analog to digital blend threshold. 58
  0x910A HD_BLEND_BLEND_DECISION_ANALOG_TO_DIGITAL_DELAY This property defines the analog to digital blend delay. 5000
  0x910B HD_BLEND_SERV_LOSS_RAMP_UP_TIME Sets the service re-acquisition unmute time in ms. 750
  0x910C HD_BLEND_SERV_LOSS_RAMP_DOWN_-TIME Sets the service switching mute time in ms. 250
  0x910D HD_BLEND_SERV_LOSS_NOISE_RAMP_UP_TIME Sets the comfort noise unmute time in ms. 1000
  0x910E HD_BLEND_SERV_LOSS_NOISE_RAMP_DOWN_TIME Sets the comfort noise mute time in ms. 250
  0x910F HD_BLEND_SERV_LOSS_NOISE_LEVEL Sets the service loss noise level. 512
  0x9110 HD_BLEND_SERV_LOSS_NOISE_DAAI_-THRESHOLD Sets the DAAI threshold below which comfort noise will engage. 40
  0x9111 HD_BLEND_SERV_LOSS_NOISE_AUDIO_START_DELAY Sets the audio restart delay used by the comfort noise ramping algorithm. 4
  0x9112 HD_BLEND_SERV_SWITCH_RAMP_UP_TIME Sets the service switching unmute time in ms. 184
  0x9113 HD_BLEND_SERV_SWITCH_RAMP_DOWN_-TIME Sets the service switching mute time in ms. 184
  0x9200 HD_DIGRAD_INTERRUPT_SOURCE Configures interrupts related to digital receivers HD_DIGRAD_STATUS command. 0
  0x9201 HD_DIGRAD_CDNR_LOW_THRESHOLD Configures CDNR LOW interrupt threshold. 0
  0x9202 HD_DIGRAD_CDNR_HIGH_THRESHOLD Configures CDNR HIGH interrupt threshold. 127
  0x9300 HD_EVENT_INTERRUPT_SOURCE Configures interrupts related to digital receiver (HD_GET_EVENT_STATUS). 0
  0x9301 HD_EVENT_SIS_CONFIG This property configures which basic SIS information is returned by the HD_GET_STATION_INFO command BASICSIS option. 0x0017
  0x9302 HD_EVENT_ALERT_CONFIG This property configures HD alerts. Alert information is returned by the HD_GET_ALERT_MSG command. 0x0001
  0x9500 HD_PSD_ENABLE Sets which audio services will provide program service data. 0
  0x9501 HD_PSD_FIELD_MASK Sets which PSD fields will be decoded. 0xFFFF

  0x9700 HD_AUDIO_CTRL_FRAME_DELAY Controls the value of the delay of decoded digital audio samples relative to the output of the audio quality indicator. 6
  0x9701 HD_AUDIO_CTRL_PROGRAM_LOSS_-THRESHOLD Controls the duration before reverting to MPS audio after an SPS audio program is removed or lost. 0
  0x9702 HD_AUDIO_CTRL_BALL_GAME_ENABLE Selects the audio output for hybrid waveforms when the TX Blend Control Status (BCTL) bits are set to 01. 1
  0x9900 HD_CODEC_MODE_0_BLEND_THRESHOLD Blend threshold for Codec Mode 0 3
  0x9901 HD_CODEC_MODE_0_SAMPLES_DELAY Digital Audio Delay for Codec Mode 0 3693
  0x9902 HD_CODEC_MODE_0_BLEND_RATE Blend Rate for Codec Mode 0 1
  0x9903 HD_CODEC_MODE_2_BLEND_THRESHOLD Blend threshold for Codec Mode 2 3
  0x9904 HD_CODEC_MODE_2_SAMPLES_DELAY Digital Audio Delay for Codec Mode 2 0
  0x9905 HD_CODEC_MODE_2_BLEND_RATE Blend Rate for Codec Mode 2 1
  0x9906 HD_CODEC_MODE_10_BLEND_THRESHOLD Blend threshold for Codec Mode 10 3
  0x9907 HD_CODEC_MODE_10_SAMPLES_DELAY Digital Audio Delay for Codec Mode 10 0
  0x9908 HD_CODEC_MODE_10_BLEND_RATE Blend Rate for Codec Mode 10 1
  0x9909 HD_CODEC_MODE_13_BLEND_THRESHOLD Blend threshold for Codec Mode 13 3
  0x990A HD_CODEC_MODE_13_SAMPLES_DELAY Digital Audio Delay for Codec Mode 13 0
  0x990B HD_CODEC_MODE_13_BLEND_RATE Blend Rate for Codec Mode 13 1
  0x990C HD_CODEC_MODE_1_BLEND_THRESHOLD Blend threshold for Codec Mode 1 3
  0x990D HD_CODEC_MODE_1_SAMPLES_DELAY Digital Audio Delay for Codec Mode 1 0
  0x990E HD_CODEC_MODE_1_BLEND_RATE Blend Rate for Codec Mode 1 1
  0x990F HD_CODEC_MODE_3_BLEND_THRESHOLD Blend threshold for Codec Mode 3 3
  0x9910 HD_CODEC_MODE_3_SAMPLES_DELAY Digital Audio Delay for Codec Mode 3 0
  0x9911 HD_CODEC_MODE_3_BLEND_RATE Blend Rate for Codec Mode 3 1
  0x9A00 HD_SERVICE_MODE_CONTROL_MP11_ENABLE This property Enables MP11 mode support. If MP11 support is disabled using this property the receiver will fall back to MP3 mode of operation when tuned to a station that is transmitting the MP11 subcarriers. 0x0000
  0x9B00 HD_EZBLEND_ENABLE This property enables and disables HD EZ blend. 0
  0x9B01 HD_EZBLEND_MPS_BLEND_THRESHOLD This property sets the threshold for determining when to blend between digital audio and analog audio for Hybrid MPS. 3
  0x9B02 HD_EZBLEND_MPS_BLEND_RATE This property configures the hysteresis in the blending process for Hybrid MPS. 3
  0x9B03 HD_EZBLEND_MPS_SAMPLES_DELAY This property is used to perform audio alignment between analog and Hybrid MPS digital audio. 3693
  0x9B04 HD_EZBLEND_SPS_BLEND_THRESHOLD This property sets the threshold for determining when to blend between digital audio and mute for SPS programs as well as All Digital MPS programs. 4
  0x9B05 HD_EZBLEND_SPS_BLEND_RATE This property configures the hysteresis in the blending process for SPS programs and All Digital MPS programs. 1
  0xE800 HD_TEST_BER_CONFIG Enables the HD BER test. 0
  0xE801 HD_TEST_DEBUG_AUDIO Used to put the analog audio output in to special test modes for debug purposes. 0
*/


//DAB properties

//0x1710 DAB_TUNE_FE_VARM DAB/DMB Front End Varactor configuration slope 0
unsigned short const DAB_TUNE_FE_VARM = 0x1710;
//0x1711 DAB_TUNE_FE_VARB DAB/DMB Front End Varactor configuration intercept 0
unsigned short const DAB_TUNE_FE_VARB = 0x1711;
//0x1712 DAB_TUNE_FE_CFG Additional configuration options for the front end. 0x0001
unsigned short const DAB_TUNE_FE_CFG = 0x1712;

//0x8100 DIGITAL_SERVICE_INT_SOURCE Configures the interrupt sources for digital services 0x0000
unsigned short const DIGITAL_SERVICE_INT_SOURCE = 0x8100;
//0x8101 DIGITAL_SERVICE_RESTART_DELAY sets the delay time (in milliseconds) to restart digital service when recovering from acquisition loss 200
unsigned short const DIGITAL_SERVICE_RESTART_DELAY = 0x8101;

//0xB000 DAB_DIGRAD_INTERRUPT_SOURCE Configures interrupts related to digital receiver. 0
unsigned short const DAB_DIGRAD_INTERRUPT_SOURCE = 0xB000;

//0xB001 DAB_DIGRAD_RSSI_HIGH_THRESHOLD DAB_DIGRAD_RSSI_HIGH_- THRESHOLD sets the high threshold,which triggers the DIGRAD interrupt if the RSSI is above this threshold. 127
unsigned short const DAB_DIGRAD_RSSI_HIGH_THRESHOLD = 0xB001;
//0xB002 DAB_DIGRAD_RSSI_LOW_THRESHOLD DAB_DIGRAD_RSSI_LOW_THRESHOLD sets the low threshold, which triggers the DIGRAD interrupt if the RSSI is below this threshold. -128
unsigned short const DAB_DIGRAD_RSSI_LOW_THRESHOLD = 0xB002;

//0xB200 DAB_VALID_RSSI_TIME Sets the time to allow the RSSI metric to settle before evaluating. 30
unsigned short const DAB_VALID_RSSI_TIME = 0xB200;
//0xB201 DAB_VALID_RSSI_THRESHOLD Sets the RSSI threshold for a valid DAB Seek/Tune 12
unsigned short const DAB_VALID_RSSI_THRESHOLD = 0xB201;
//0xB202 DAB_VALID_ACQ_TIME Set the time to wait for acquisition before evaluating 2000
unsigned short const DAB_VALID_ACQ_TIME = 0xB202;
//0xB203 DAB_VALID_SYNC_TIME Sets the time to wait for ensemble synchronization. 1200
unsigned short const DAB_VALID_SYNC_TIME = 0xB203;
//0xB204 DAB_VALID_DETECT_TIME Sets the time to wait for ensemble fast signal detection. 35
unsigned short const DAB_VALID_DETECT_TIME = 0xB204;

//0xB300 DAB_EVENT_INTERRUPT_SOURCE Configures which dab events will set the DEVENTINT status bit. 0
unsigned short const DAB_EVENT_INTERRUPT_SOURCE = 0xB300;
//0xB301 DAB_EVENT_MIN_SVRLIST_PERIOD Configures how often service list notifications can occur. 10
unsigned short const DAB_EVENT_MIN_SVRLIST_PERIOD = 0xB301;
//0xB302 DAB_EVENT_MIN_SVRLIST_PERIOD_RECONFIG Configures how often service list notifications can occur during reconfiguration events. 10
unsigned short const DAB_EVENT_MIN_SVRLIST_PERIOD_RECONFIG = 0xB302;
//0xB303 DAB_EVENT_MIN_FREQINFO_PERIOD Configures how often frequency informationnotifications can occur. 5
unsigned short const DAB_EVENT_MIN_FREQINFO_PERIOD = 0xB303;

//0xB400 DAB_XPAD_ENABLE Selects which XPAD data will forwarded to the host. 1
unsigned short const DAB_XPAD_ENABLE = 0xB400;
//0xB401 DAB_DRC_OPTION defines option to apply DRC (dynamic range control) gain. 0
unsigned short const DAB_DRC_OPTION = 0xB401;

//0xB500 DAB_CTRL_DAB_MUTE_ENABLE enables the feature of hard muting audio when signal level is low 1
unsigned short const DAB_CTRL_DAB_MUTE_ENABLE = 0xB500;
//0xB501 DAB_CTRL_DAB_MUTE_SIGNAL_LEVEL_THRESHOLD Set the signal quality threshold, which is evaluated by fic_quality. 98
unsigned short const DAB_CTRL_DAB_MUTE_SIGNAL_LEVEL_THRESHOLD = 0xB501;
//0xB502 DAB_CTRL_DAB_MUTE_WIN_THRESHOLD Set the time window threshold (in milliseconds)to mute audio 1000
unsigned short const DAB_CTRL_DAB_MUTE_WIN_THRESHOLD = 0xB502;
//0xB503 DAB_CTRL_DAB_UNMUTE_WIN_THRESHOLD Set the time window threshold (in milliseconds) to unmute audio 1500
unsigned short const DAB_CTRL_DAB_UNMUTE_WIN_THRESHOLD = 0xB503;
//0xB504 DAB_CTRL_DAB_MUTE_SIGLOSS_THRESHOLD Set the signal RSSI threshold to mute audio. RSSI below this threshold indicates that signal is lost. In this case, audio will be muted.6
unsigned short const DAB_CTRL_DAB_MUTE_SIGLOSS_THRESHOLD = 0xB504;
//0xB505 DAB_CTRL_DAB_MUTE_SIGLOW_THRESHOLD Set the signal SNR threshold. The fic_quality based audio mute operation only engages when signal SNR is below this threshold. 9
unsigned short const DAB_CTRL_DAB_MUTE_SIGLOW_THRESHOLD = 0xB505;
//0xB506 DAB_ACF_SOFTMUTE_ATTACK_Time Sets unmute time in ms. Default: 100 Units: ms.
unsigned short const DAB_ACF_SOFTMUTE_ATTACK_Time = 0xB506;

//0xB508 DAB_ACF_CMFTNOISE_LEVEL Sets the comfort noise level. Range: 0-4000 dB Default: 0x0200 
unsigned short const DAB_ACF_CMFTNOISE_LEVEL = 0xB508;
//0xB509 DAB_ACF_CMFTNOISE_ATTACK_TIME Sets comfort noise attack time in ms. Default: 100 Units: ms
unsigned short const DAB_ACF_CMFTNOISE_ATTACK_TIME = 0xB509;
//0xB50A DAB_ACF_CMFTNOISE_RELEASE_TIME Sets comfort noise release time in ms. Default: 4000 Units: ms
unsigned short const DAB_ACF_CMFTNOISE_RELEASE_TIME = 0xB50A;

//0xB700 DAB_ANNOUNCEMENT_ENABLE enables announcement types.Default: 0x07FF
unsigned short const DAB_ANNOUNCEMENT_ENABLE = 0xB700;

//AM Properties
/*
  0x0500 AM_AVC_MIN_GAIN Sets the minimum gain for automatic volume control. -2048
  0x0501 AM_AVC_MAX_GAIN Sets the maximum gain for automatic volume control. 10220

  0x2200 AM_CHBW_SQ_LIMITS The SNR/RSSI threshold for maximum and minimum channel filter bandwidth 0x1E0F
  0x2201 AM_CHBW_SQ_CHBW The maximum and minimum channel filter bandwidth. 0x2314
  0x2202 AM_CHBW_SQ_WIDENING_TIME SNR/RSSI based channel filter bandwidth widening time 2048
  0x2203 AM_CHBW_SQ_NARROWING_TIME SNR/RSSI based channel filter bandwidth narrowing time 16
  0x2204 AM_CHBW_OVERRIDE_BW Overrides the automatically controlled channel filter setting. 0
*/

//0x4100 AM_SEEK_BAND_BOTTOM Sets the lower seek boundary of the AM band in multiples of 1kHz. 520
unsigned short const AM_SEEK_BAND_BOTTOM = 0x4100;
//0x4101 AM_SEEK_BAND_TOP Sets the upper seek boundary for the AM band in multiples of 1kHz. 1710
unsigned short const AM_SEEK_BAND_TOP = 0x4101;


/*

  0x4102 AM_SEEK_FREQUENCY_SPACING Sets the frequency spacing for the AM band in multiples of 1kHz when performing a seek. 10
  0x4200 AM_VALID_MAX_TUNE_ERROR Sets the maximum frequency error allowed before setting the AFCRL indicator. 75
  0x4201 AM_VALID_RSSI_TIME Sets the amount of time in ms to allow the RSSI/ISSI metrics to settle before evaluating. 8
  0x4202 AM_VALID_RSSI_THRESHOLD Sets the RSSI threshold for a valid AM Seek/Tune. 35
  0x4203 AM_VALID_SNR_TIME Sets the amount of time in ms to allow the SNR metric to settle before evaluating.40
  0x4204 AM_VALID_SNR_THRESHOLD Sets the SNR threshold for a valid AM Seek/Tune. 4
  0x4205 AM_VALID_HDLEVEL_THRESHOLD Sets the HDLEVEL threshold for a valid AM Seek/Tune. 0
  0x4300 AM_RSQ_INTERRUPT_SOURCE Configures interrupt related to Received Signal Quality metrics. 0
  0x4301 AM_RSQ_SNR_HIGH_THRESHOLD AM_RSQ_SNR_HIGH_THRESHOLD sets the high threshold, which triggers the RSQ interrupt if the SNR is above this threshold. 127
  0x4302 AM_RSQ_SNR_LOW_THRESHOLD AM_RSQ_SNR_LOW_THRESHOLD sets the low threshold, which triggers the RSQ interrupt if the SNR is below this threshold. -128
  0x4303 AM_RSQ_RSSI_HIGH_THRESHOLD AM_RSQ_RSSI_HIGH_THRESHOLD sets the high threshold, which triggers the RSQ interrupt if the RSSI is above 127
  0x4304 AM_RSQ_RSSI_LOW_THRESHOLD Sets low threshold which triggers the RSQ interrupt if the RSSI is below this threshold. -128
  0x4305 AM_RSQ_HD_DETECTION Configures the Fast HD Detection routine. 48
  0x4306 AM_RSQ_HD_LEVEL_TIME_CONST Configures the Fast HD Detection Level Metric Filtering Time Constant. 32
  0x4307 AM_RSQ_HDDETECTED_THD Configures the HD Level Detected Threshold. 0x1E1E
  0x4400 AM_ACF_INTERRUPT_SOURCE Enables the ACF interrupt sources. 0
  0x4401 AM_ACF_SOFTMUTE_THRESHOLD Sets the softmute interrupt threshold in dB attenuation. 31
  0x4402 AM_ACF_HIGHCUT_THRESHOLD Sets the high cut interrupt threshold. 0
  0x4403 AM_ACF_SOFTMUTE_TOLERANCE Sets the distance from the final softmute value that triggers softmute convergence flag. 2
  0x4404 AM_ACF_HIGHCUT_TOLERANCE Sets the distance from the final high cut frequency that triggers the high cut convergence flag. 20
  0x4405 AM_ACF_CONTROL_SOURCE Determines what metrics control various ACF features. 0
  0x4500 AM_SOFTMUTE_SQ_LIMITS Sets the SNR limits for soft mute attenuation. 0x0800
  0x4501 AM_SOFTMUTE_SQ_ATTENUATION Sets the SNR based softmute attenuation limits. 0x000C
  0x4502 AM_SOFTMUTE_SQ_ATTACK_TIME Sets the soft mute attack time. 120
  0x4503 AM_SOFTMUTE_SQ_RELEASE_TIME Sets the soft mute release time. 500
  0x4600 AM_HIGHCUT_SQ_LIMITS SQ based high cut high threshold. 0x0a06
  0x4601 AM_HIGHCUT_SQ_CUTOFF_FREQ Max and Min audio frequencies 0x280A
  0x4602 AM_HIGHCUT_SQ_ATTACK_TIME Hi-cut cutoff frequency attack time in ms. 16
  0x4603 AM_HIGHCUT_SQ_RELEASE_TIME Hi-cut cutoff frequency release time in ms. 2000
  0x4800 AM_DEMOD_AFC_RANGE Specify the AFC range 0x0000
*/

#endif //PROPERTIES_H
