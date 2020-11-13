/////////////////////////////////////////////////////////////////
/*
  CARButton.cpp - Arduino Library to simplify working with buttons.
  Created by Tommy Carlsson.
*/
/////////////////////////////////////////////////////////////////

#include "Arduino.h"
#include "elapsedMillis.h"
#include "CARButton.h"

/////////////////////////////////////////////////////////////////

CARButton::CARButton(byte attachTo, byte buttonMode /* = INPUT_PULLUP */, boolean activeLow /* = true */, byte debounceTimeout /* = DEBOUNCE_MS */) {
  pin = attachTo;
  setDebounceTime(debounceTimeout);
  pinMode(attachTo, buttonMode);
  pressed_state = activeLow ? LOW : HIGH;
  current_state = activeLow ? HIGH : LOW;
  pressed_triggered = false;
}

/////////////////////////////////////////////////////////////////

bool CARButton::operator==(CARButton &rhs) {
  return (this==&rhs);    
}

/////////////////////////////////////////////////////////////////

void CARButton::setDebounceTime(byte ms) {
  debounce_time_ms = ms;
}

/////////////////////////////////////////////////////////////////

void CARButton::setButtonHandler(CallbackFunction f) {
  button_cb = f; 
}

/////////////////////////////////////////////////////////////////

boolean CARButton::isPressed() {
  return (current_state == pressed_state);
}

/////////////////////////////////////////////////////////////////

boolean CARButton::isPressedRaw() {
  return (digitalRead(pin) == pressed_state);
}

/////////////////////////////////////////////////////////////////

byte CARButton::getNumberOfClicks() {
  return click_count;
}

/////////////////////////////////////////////////////////////////

unsigned int CARButton::wasPressedFor() {
  return time_pressed_ms;
}

/////////////////////////////////////////////////////////////////

void CARButton::loop() {
  prev_state = current_state;
  current_state = digitalRead(pin);

  // is button pressed_state?
  if (current_state == pressed_state) {
    // was the button pressed_state now?
    if (prev_state != pressed_state) {		  
      elapsedMillis_since_pressed = 0;
      elapsedMillis_since_longclick = 0;
      pressed_triggered = false;
      longclick_detected = false;
      
      // trigger pressed_state event (after debounce has passed)
    } else if (!pressed_triggered && elapsedMillis_since_pressed >= debounce_time_ms) {
      pressed_triggered = true;
      click_count++;
      // trigger pressed_state
      button_cb(*this, BUTTON_PRESSED);
      
      // set longclick_detected after button has been pressed_state for LONGCLICK_MS
      // then repeat
    } else if (pressed_triggered && click_count == 1) {
      if (!longclick_detected && elapsedMillis_since_pressed >= LONGCLICK_MS) {
        time_pressed_ms = elapsedMillis_since_pressed;
        longclick_detected = true;
        elapsedMillis_since_longclick = 0;
        button_cb(*this, BUTTON_LONG_FIRST);
      } else if (longclick_detected && elapsedMillis_since_longclick >= LONGCLICK_REPEAT_MS) {
        time_pressed_ms = elapsedMillis_since_pressed;
        elapsedMillis_since_longclick = 0;
        button_cb(*this, BUTTON_LONG_REPEAT);
      }
	}

  // is button not pressed_state?
  } else if (pressed_triggered) {
    // was the button released now?
    if (prev_state == pressed_state) {
      time_pressed_ms = elapsedMillis_since_pressed;
      // trigger release
      button_cb(*this, BUTTON_RELEASED);
      // was it a longclick? (preceeds multi clicks)
      // however, once multiple clicks have started, don't change to a long click
      if (click_count == 1 && longclick_detected) {
        // trigger long-click
        button_cb(*this, BUTTON_LONG_FINISH);
        longclick_detected = false;
        pressed_triggered = false;
        click_count = 0;
      }
    } else if (click_count > 0 && elapsedMillis_since_pressed >= MULTICLICK_MS) {
      // trigger click
      button_cb(*this, BUTTON_CLICK_FINISH);
      pressed_triggered = false;
      click_count = 0;
    }
  }
}

/////////////////////////////////////////////////////////////////

void CARButton::reset() {
  elapsedMillis_since_pressed = 0;
  elapsedMillis_since_longclick = 0;
  pressed_triggered = false;
  longclick_detected = false;
  click_count = 0;
  button_cb = NULL;
}

/////////////////////////////////////////////////////////////////
