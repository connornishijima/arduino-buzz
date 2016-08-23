/*
  Buzz.h - Library for detecting motion via AC electricity
  Created by Connor Nishijima, August 22nd 2016.
  Released under the GPLv3 license.
*/
#ifndef buzz_h
#define buzz_h

#include "Arduino.h"

class Buzz
{
  public:
    Buzz();
    void begin(uint8_t pin, uint8_t hz, uint16_t coolDown);
    void end();
    void printData();
    void setAlarm(void (*action)(), uint16_t thresh, uint16_t hold);
    void checkAlarm();
    int16_t level();
};

#endif
