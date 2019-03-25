/*tactile button functions*/
#include "TactileSwitch.h"

TactileSwitch::TactileSwitch(uint8_t buttonPin, uint8_t number, uint8_t mode, bool enablePullUp)
{
  _event = BTN_NO_ACTION;
  _buttonPin  = buttonPin;
  _number = number;
  _mode = mode;
  _enablePullUp = enablePullUp;

  if (enablePullUp) pinMode(_buttonPin, INPUT_PULLUP);
  else pinMode(_buttonPin, INPUT);

  digitalWrite(_buttonPin, _mode);

  //Button variables
  _buttonVal  = HIGH;
  _buttonLast = HIGH;
  _dcWaiting  = false;
  _dcOnUp     = false;
  _singleOK   = true;
  _timePressedDown = -1;
  _timeReleased    = 0;
  _ignoreUp   = false;
  _waitForUp  = false;
  _holdEventPast     = false;
  _longHoldEventPast = false;

}

uint8_t TactileSwitch::getNumber()
{
  return _number;
}

/*Function to get event of button*/
TactileSwitch::action TactileSwitch::getEvent()
{
  return _event;
}

TactileSwitch::action TactileSwitch::checkUserAction()
{
  //initalize
  _event = BTN_NO_ACTION;
  //read value
  _buttonVal = digitalRead(_buttonPin);

  //button pressed down and _timeDebounce elapsed
  if (_buttonVal == !_mode && _buttonLast == _mode && (millis() - _timeReleased) > _durationDebounce)
    //if (_buttonVal == LOW && _buttonLast == HIGH && (millis() - _timeReleased) > _durationDebounce)
  {
    //save time when button was pressed down
    _timePressedDown = millis();

    _singleOK = true;
    _ignoreUp = false;
    _waitForUp = false;
    _holdEventPast = false;
    _longHoldEventPast = false;

    //_doubleClickTime elapsed since button released
    if ((millis() - _timeReleased) < _durationDoubleClick && _dcOnUp == false && _dcWaiting == true)  _dcOnUp = true;
    else  _dcOnUp = false;

    _dcWaiting = false;
  }

  //button released and _timeDebounce elapsed
  else if (_buttonVal == _mode && _buttonLast == !_mode && (millis() - _timeReleased) > _durationDebounce)
    //else if (_buttonVal == HIGH && _buttonLast == LOW && (millis() - _timeReleased) > _durationDebounce)
  {
    if (! _ignoreUp)
    {
      //save time when button was released
      _timeReleased = millis();

      //waiting further for double click
      if (_dcOnUp == false) _dcWaiting = true;
      // double click
      else
      {
        _event = BTN_DBL_PUSH;
        _dcOnUp = false;
        _dcWaiting = false;
        _singleOK = false;
      }
    }
  }

  // Test for normal click event: _doubleClickTime expired
  if ( _buttonVal == _mode && (millis() - _timeReleased) >= _durationDoubleClick && _dcWaiting == true && _dcOnUp == false && _singleOK == true && _event != BTN_DBL_PUSH)
    //if ( _buttonVal == HIGH && (millis() - _timeReleased) >= _durationDoubleClick && _dcWaiting == true && _dcOnUp == false && _singleOK == true && event != BTN_DBL_PUSH)
  {
    _event = BTN_PUSH;
    _dcWaiting = false;
  }

  // Test for hold event
  if (_buttonVal == !_mode && (millis() - _timePressedDown) >= _durationHoldShort)
    //if (_buttonVal == LOW && (millis() - _timePressedDown) >= _durationHoldShort)
  {
    // Trigger "normal" hold
    if (! _holdEventPast)
    {
      _timePressedDown = millis();
      _event = BTN_HOLD;
      _waitForUp = true;
      _ignoreUp = true;
      _dcOnUp = false;
      _dcWaiting = false;
      _holdEventPast = true;
    }
    // Trigger "long" hold
    if ((millis() - _timePressedDown) >= _durationHoldLong)
    {
      if (! _longHoldEventPast) {
        _event = BTN_HOLD_LONG;
        _longHoldEventPast = true;
      }
    }
  }

  /*Save last state*/
  _buttonLast = _buttonVal;

  return _event;
}



