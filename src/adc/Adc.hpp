#pragma once
#include <stdint.h>

class Adc
{
public:
  Adc();
  
   /**After a call to update, @ref values will contain the newest readings */
   void update();
   
   static const uint8_t NUM_ADC = 8;
   uint16_t values[NUM_ADC];
};
