#ifndef DAB_MENU_SERIAL_H
#define DAB_MENU_SERIAL_H

/*Serial DAB Menu functions*/

/*Serial Monitor*/
bool callSerialMonitorApplication();

/*Print Device Menu*/
bool dabMenuDeviceSerial(char ch);
/*Print Main Menu*/
bool dabMenuMainSerial(char ch);
/*Print Technical Menu*/
bool dabMenuTechnicalSerial(char ch);
/*Print Scan Menu*/
bool dabMenuScanSerial(char ch);
 
#endif /*DAB_MENU_SERIAL_H*/
