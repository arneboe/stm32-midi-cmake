#include "Faders.hpp"

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
  //FIXME scale to log/linear
  
  //NOTE in_max and out_max are increased by one to achieve even scaling
  //     see: https://www.jetmore.org/john/blog/2011/09/arduinos-map-function-and-numeric-distribution/
  return map(rawValue, 0, 4096, 0, 128);
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
