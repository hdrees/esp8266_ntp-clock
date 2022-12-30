#ifndef Led_h
#define Led_h

#include "Arduino.h"

class Led {

public:
  Led(byte pin);
  void off();
  void on();
  byte status();
  void toggle();

private:
  void init();
  byte pin;
};

#endif