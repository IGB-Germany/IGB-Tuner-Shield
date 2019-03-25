/*tactile button functions*/
#ifndef TACTILE_SWITCH_H
#define TACTILE_SWITCH_H

#include "arduino.h" //HIGH, LOW, boolean, uint8_t

class TactileSwitch
{
  public:
    /*constructor with pin, number, internal resistor, mode: high or low driver side */
    TactileSwitch(uint8_t buttonPin, uint8_t number, uint8_t mode = HIGH, bool enablePullUp = false);
    
    /*user defined actions*/
    enum action {BTN_NO_ACTION, BTN_PUSH, BTN_DBL_PUSH, BTN_HOLD, BTN_HOLD_LONG, BTN_ERROR};

    /*Function which returns the action of the button*/
    action checkUserAction();

    /*Function to get number of button*/
    uint8_t getNumber();

    /*Function to get event of button*/
    action getEvent();
    
  private:
    /*timing variables*/
    static const uint8_t _durationDebounce    = 20;//ms debounce period to prevent flickering when pressing or releasing the button
    static const uint8_t _durationDoubleClick = 150;//ms between clicks for a double click event
    static const uint16_t _durationHoldShort  = 1000;//ms short hold period: how long to wait for press+hold event
    static const uint16_t _durationHoldLong   = 3000;//ms long hold period: how long to wait for press+hold event

    //Button variables
    action _event;
   
    uint8_t _number;//Unique number
    uint8_t _buttonPin;   //Pin
    uint8_t _mode;        //HIGH side or LOW side button
    bool _enablePullUp;   //Enable internal 20k resistor
    
    int32_t _timePressedDown;    //time the button was pressed down
    uint32_t _timeReleased;      //time the button was released

    //uint32_t _timeError; //to do: time we are waiting for an event until error  

    bool _buttonVal;    // value read from button
    bool _buttonLast;   // buffered value of the button's previous state
    
    bool _dcOnUp;       // whether to register a double click on next release, or whether to wait and click
    bool _singleOK;     // whether it's OK to do a single click

    bool _dcWaiting;    // whether we're waiting for a double click (down)
    
    bool _ignoreUp;     // whether to ignore the button release because the click+hold was triggered
    bool _waitForUp;    // when held, whether to wait for the up event
    bool _holdEventPast;// whether or not the hold event happened already
    bool _longHoldEventPast;  // whether or not the long hold event happened already
  
};

#endif /*TACTILE_SWITCH_H*/

