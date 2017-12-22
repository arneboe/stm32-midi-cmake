#pragma once
#include <stdint.h>

class Clock
{
public:
  static volatile uint32_t ticks;

  static void tick(uint16_t ms)
  {
    ticks += ms;
  }

  static void delayMs(uint32_t ms)
  {
    volatile const uint32_t start = ticks; //without volatile compiler optimizes to endless loop
    while(ticks - start < ms);
  }

};


