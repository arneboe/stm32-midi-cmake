#include "Faders.hpp"



/**taken from arduino:
  * https://playground.arduino.cc/Main/MultiMap */
static int multiMap(uint16_t value)
{
  //adc has only 12 bit resolution, everything above has to be garbage and shouldnt be there 
  if(value > 4095) value = 4095;
  
  //last 2 values are special cases to make sure that we always reach the highest midi value.
  //This is neccessary to compensate for integer arithmetics
  static uint16_t potiVals[] = {0, 44, 120, 222, 345, 470, 590, 895, 1715, 2465, 3220, 4050, 4094, 4095};
  static uint16_t midiVals[] = {0, 10, 21, 31, 42, 52, 63, 74, 84, 95, 105, 116, 127, 127};

  // search right interval
  uint8_t pos = 0; 
  while(value > potiVals[pos]) pos++;

  // this will handle all exact "points" in the potiVals array
  if (value == potiVals[pos]) return midiVals[pos];
  
  //if the code reaches here pos is always > 0
  // interpolate in the right segment for the rest
  return (value - potiVals[pos-1]) * (midiVals[pos] - midiVals[pos-1]) / (potiVals[pos] - potiVals[pos-1]) + midiVals[pos-1];
}


Faders::Faders(Adc& adc) : adc(adc)
{
  
}

//from arduino
int32_t map(int32_t x, int32_t in_min, int32_t in_max, int32_t out_min, int32_t out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

uint16_t Faders::Fader::getConvertedValue() const
{
  if(isLogarithmic)
  {
    //faders are logarithmic by default, just map to correct range
    //NOTE in_max and out_max are increased by one to achieve even scaling
    //     see: https://www.jetmore.org/john/blog/2011/09/arduinos-map-function-and-numeric-distribution/
    return map(rawValue, 0, 4096, 0, 128);
  }
  
  //map to linear values
  return multiMap(rawValue);
  
}

uint8_t Faders::getFaderValue(uint8_t faderIndex) const
{
  if(faderIndex < numFaders())
  {
    return faders[faderIndex].getConvertedValue();
  }
  return 0;
}

uint8_t Faders::numFaders() const
{
  return Adc::NUM_ADC;
}

void Faders::setFaderLog(uint8_t faderIndex, bool value)
{
  if(faderIndex < numFaders())
  {
    faders[faderIndex].isLogarithmic = value;
  }
}

void Faders::update()
{
  adc.update();
  
  for(int i = 0; i < numFaders(); ++i)
  {
    faders[i].rawValue = adc.values[i];
  }
}

