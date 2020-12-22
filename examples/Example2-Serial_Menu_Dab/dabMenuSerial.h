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
 
#endif //DAB_MENU_SERIAL_H
