#include "Clock.hpp"

volatile uint32_t Clock::ticks = 0;

void Clock::tick(uint16_t ms)
{
  ticks += ms;
}

void Clock::delayMs(uint32_t ms)
{
  volatile const uint32_t start = ticks; //without volatile compiler optimizes to endless loop
  while(ticks - start < ms);
}
