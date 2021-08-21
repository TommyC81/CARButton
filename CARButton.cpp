/////////////////////////////////////////////////////////////////
/*
	CARButton.cpp - Arduino Library to simplify working with buttons.
	Created by Tommy Carlsson.
*/
/////////////////////////////////////////////////////////////////

#include <CARButton.h>

/////////////////////////////////////////////////////////////////

CARButton::CARButton(byte p_inputPin, byte p_buttonMode /* = INPUT_PULLUP */, boolean p_activeLow /* = true */, byte p_debounceTimeout /* = DEBOUNCE_MS */) {
	_inputPin = p_inputPin;
	_pressedState = p_activeLow ? LOW : HIGH;
	_previousState = _currentState = p_activeLow ? HIGH : LOW;
	_pressedTriggered = false;
	setDebounceTime(p_debounceTimeout);
	pinMode(p_inputPin, p_buttonMode);
}

bool CARButton::operator==(CARButton &rhs) {
	return (this==&rhs);
}

void CARButton::setDebounceTime(byte ms) {
	_debounceTimeMs = ms;
}

void CARButton::setButtonHandler(CallbackFunction f) {
	_buttonCallback = f; 
}

boolean CARButton::isPressed() {
	return (_currentState == _pressedState);
}

boolean CARButton::isPressedRaw() {
	return (digitalRead(_inputPin) == _pressedState);
}

byte CARButton::getNumberOfClicks() {
	return _clickCount;
}

unsigned int CARButton::wasPressedFor() {
	return _timePressedMs;
}

void CARButton::loop() {
	_previousState = _currentState;
	_currentState = digitalRead(_inputPin);
	// is button _pressedState?
	if (_currentState == _pressedState) {
		// was the button _pressedState now?
		if (_previousState != _pressedState) {
			_elapsedMillis_sincePressed = 0;
			_elapsedMillis_sinceLongclick = 0;
			_pressedTriggered = false;
			_longclickDetected = false;
		// trigger _pressedState event (after debounce has passed)
		} else if (!_pressedTriggered && _elapsedMillis_sincePressed >= _debounceTimeMs) {
			_pressedTriggered = true;
			_clickCount++;
			// trigger _pressedState
			_buttonCallback(*this, BUTTON_PRESSED);
		// set _longclickDetected after button has been _pressedState for LONGCLICK_MS
		// then repeat the longclick events
		} else if (_pressedTriggered && _clickCount == 1) {
			if (!_longclickDetected && _elapsedMillis_sincePressed >= LONGCLICK_MS) {
				_timePressedMs = _elapsedMillis_sincePressed;
				_longclickDetected = true;
				_elapsedMillis_sinceLongclick = 0;
				_buttonCallback(*this, BUTTON_LONG_FIRST);
			} else if (_longclickDetected && _elapsedMillis_sinceLongclick >= LONGCLICK_REPEAT_MS) {
				_timePressedMs = _elapsedMillis_sincePressed;
				_elapsedMillis_sinceLongclick = 0;
				_buttonCallback(*this, BUTTON_LONG_REPEAT);
			}
		}
	// was _pressedTriggered (and button is no longer in _pressedState)?
	} else if (_pressedTriggered) {
		// was the button released now?
		if (_previousState == _pressedState) {
			_timePressedMs = _elapsedMillis_sincePressed;
			// was it the end of a longclick? (preceeds any other clicks/released events)
			// however, once multiple clicks have started, don't change to a long click
			if (_clickCount == 1 && _longclickDetected) {
				// trigger long-click
				_buttonCallback(*this, BUTTON_LONG_FINISH);
				_longclickDetected = false;
				_pressedTriggered = false;
				_clickCount = 0;
			} else {
				// trigger release if it was not the end of a longclick
				_buttonCallback(*this, BUTTON_CLICK_RELEASED);
			}
		} else if (_clickCount > 0 && _elapsedMillis_sincePressed >= MULTICLICK_MS) {
			// trigger click
			_buttonCallback(*this, BUTTON_CLICK_FINISH);
			_pressedTriggered = false;
			_clickCount = 0;
		}
	}
}

void CARButton::reset() {
	_elapsedMillis_sincePressed = 0;
	_elapsedMillis_sinceLongclick = 0;
	_pressedTriggered = false;
	_longclickDetected = false;
	_clickCount = 0;
	_buttonCallback = NULL;
}

/////////////////////////////////////////////////////////////////