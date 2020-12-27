#ifndef DAB_MENU_SERIAL_H
#define DAB_MENU_SERIAL_H

//Menu functions

// States of menu
enum state
{
  main,
  scanFrequency,
  scanEnsemble,
  technical,
  device
};

void callSerialMonitorApplication();

void menuMain(char ch);
void menuScanEnsemble(char ch);
void menuScanFrequency(char ch);
void menuTechnical(char ch);
void menuDevice(char ch);

//Get free RAM
unsigned short getFreeRam();
//Get status of mute
unsigned char readMute();
//Mute 0,1,2,3; No, Left, Rright, Both, default No
void writeMute(unsigned char channelMuted = 0);

//Volume
unsigned char volumeUp();
unsigned char volumeDown();

//Test varactor tuning capacitor
void testVaractorCap(unsigned char& index, unsigned char injection = 0);


#endif //DAB_MENU_SERIAL_H
