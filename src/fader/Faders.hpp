#pragma once
#include <Adc.hpp>

//FIXME add error handling


/** A collection of faders */
class Faders
{
public:
  static const uint8_t NUM_FADERS = Adc::NUM_ADC;
  
private:
    struct Fader
  {
    uint16_t rawValue = 0;
    bool isLogarithmic = false;
    
    /** converts to linear/logarithmic and range [0..127] */
    uint16_t getConvertedValue() const;
  };
  
private:
  Adc& adc;
  Fader faders[Adc::NUM_ADC];
  
public:
  
  Faders(Adc& adc);
  
  /** @note calls adc.update() */
  void update();
  
  /** @parma value True => fader is logarithmic, False => fader is linear. */
  void setFaderLog(uint8_t faderIndex, bool value);
  
  uint8_t getFaderValue(uint8_t faderIndex) const;
  
  
  
};
