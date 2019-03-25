#ifndef FIRMWARE_H
#define FIRMWARE_H

//===============================================================================================================
//   Index  Bytes   Startaddress  CRC32       Filename                 Version    Comment
//===============================================================================================================
//    0     65536   0x00000000    n/a         n/a                      n/a        Customer Specific Data
//    1       940   0x00010000    0x8944b5ae  "rom00_patch_mini.bin"              Mini Patch Bootloader 
//    2      5796   0x00020000    0xa93227b5  "rom00_patch.016.bin"               Fullpatch Bootloader
//    3    530180   0x00030000    0x40fbbd93  "fmhd_radio_5_0_4.bin"   5.0.4      Firmware Image Mode 1 FMHD
//    4    521448   0x000C0000    0xb5edae86  "dab_radio_5_0_5.bin"    5.0.5      Firmware Image Mode 2 DAB
//    5    529356   0x00140000    0x375e4a88  "amhd_radio_2_0_11.bin"  2.0.11     Firmware Image Mode 3 AMHD
//    6       n/a   0x001D0000    n/a         n/a                      n/a        Customer Specific Data
//===============================================================================================================

//Mini Patch Bootloader
const unsigned long addrBootloaderPatchMini=0x00010000;
const unsigned long sizeBootloaderPatchMini=0x000003AC;
const char nameBootloaderPatchMini[] = "rom00_patch_mini.003.bin";
const unsigned short checkSumBootloaderPatchMini = 0x36E9;
const unsigned long crc32BootloaderPatchMini = 0x8944b5ae;

//Fullpatch Bootloader
const unsigned long addrBootloaderPatchFull=0x00020000;
const unsigned long sizeBootloaderPatchFull=0x000016A4;
const char nameBootloaderPatchFull[] = "rom00_patch.016.bin";
const unsigned short checkSumBootloaderPatchFull = 0x02BF;
const unsigned long crc32BootloaderPatchFull = 0xa93227b5;

//Firmware Image Mode 1
const unsigned long addrFirmwareFm=0x00030000;
const unsigned long sizeFirmwareFm=0x00081704;
const char nameFirmwareFm[] = "fmhd_radio_5_0_4.bin";
const unsigned short checkSumFirmwareFm = 0x3A10;
const unsigned long crc32FirmwareFm = 0x40fbbd93;

//Firmware Image Mode 2
const unsigned long addrFirmwareDab = 0x000C0000;
const unsigned long sizeFirmwareDab = 0x0007F4E8;
const char nameFirmwareDab[] = "dab_radio_5_0_5.bin";
const unsigned short checkSumFirmwareDab = 0x37A4;
const unsigned long crc32FirmwareDab = 0xb5edae86;

//Firmware Image Mode 3
const unsigned long addrFirmwareAm = 0x00140000;
const unsigned long sizeFirmwareAm = 0x000813CC;
const char nameFirmwareAm[] = "amhd_radio_2_0_11.bin";
const unsigned short checkSumFirmwareAm = 0x59A7;
const unsigned long crc32FirmwareAm = 0x375e4a88;

#endif //FIRMWARE_H
