#pragma once
#include <stdint.h>

class Clock
{
public:
  static volatile uint32_t ticks;
  static void tick(uint16_t ms);
  static void delayMs(uint32_t ms);
};


