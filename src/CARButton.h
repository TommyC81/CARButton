/////////////////////////////////////////////////////////////////
/*
  CARButton.h - Arduino Library to simplify working with buttons.
  Created by Tommy Carlsson.
*/
/////////////////////////////////////////////////////////////////

#pragma once

#ifndef CARButton_h
#define CARButton_h

/////////////////////////////////////////////////////////////////

#include "Arduino.h"
#include "elapsedMillis.h"

/////////////////////////////////////////////////////////////////

#ifndef DEBOUNCE_MS
    #define DEBOUNCE_MS			50
#endif
#ifndef LONGCLICK_MS 
  #define LONGCLICK_MS		250
#endif
#ifndef LONGCLICK_REPEAT_MS 
  #define LONGCLICK_REPEAT_MS	250
#endif
#ifndef MULTICLICK_MS
  #define MULTICLICK_MS		300
#endif

#define BUTTON_PRESSED 0
#define BUTTON_RELEASED 1
#define BUTTON_CLICK_FINISH 2
#define BUTTON_LONG_FIRST 3
#define BUTTON_LONG_REPEAT 4
#define BUTTON_LONG_FINISH 5

/////////////////////////////////////////////////////////////////

class CARButton {
  protected:
    byte pin;
    byte prev_state;
    byte current_state;
    byte pressed_state;
    byte click_count = 0;
    elapsedMillis elapsedMillis_since_pressed;
    elapsedMillis elapsedMillis_since_longclick;
    unsigned int time_pressed_ms = 0;
    byte debounce_time_ms;
    bool pressed_triggered = false;
    bool longclick_detected = false;
    
    typedef void (*CallbackFunction) (CARButton&, byte callback_type);
    
    CallbackFunction button_cb = NULL;
    
  public:
    CARButton(byte attachTo, byte buttonMode = INPUT_PULLUP, boolean activeLow = true, byte debounceTimeout = DEBOUNCE_MS);
    void setDebounceTime(byte ms);
    void reset();
    
    void setButtonHandler(CallbackFunction f);
    
    boolean isPressed();
    boolean isPressedRaw();
    
    byte getNumberOfClicks();
    unsigned int wasPressedFor();
    
    bool operator==(CARButton &rhs);
    
    void loop();
};
/////////////////////////////////////////////////////////////////
#endif
/////////////////////////////////////////////////////////////////
