#include "Midi.hpp"
#include "Clock.hpp"
#include "Systick.hpp"
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>


void midiErrorHandler(Midi::MidiError err)
{
  //simply blink fast in case of midi error
  gpio_toggle(GPIOC, GPIO13);
  Clock::delayMs(100);
  gpio_toggle(GPIOC, GPIO13);
  Clock::delayMs(100);
  gpio_toggle(GPIOC, GPIO13);
  Clock::delayMs(100);
  gpio_toggle(GPIOC, GPIO13);
  Clock::delayMs(100);
  gpio_toggle(GPIOC, GPIO13);
  
  gpio_set(GPIOC, GPIO13); //led off
}


//spam lots of midi messages to test functionality
void midiTest(Midi& m);


int main(void)
{
  rcc_clock_setup_in_hse_8mhz_out_72mhz();
  
  rcc_periph_clock_enable(RCC_GPIOC);
  rcc_periph_clock_enable(RCC_GPIOB);

  gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_50_MHZ,
                GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);

  Systick::init();
  Midi m(midiErrorHandler);

  Clock::delayMs(500); //wait for midi end points to be created (see comment in Midi.cpp for why we need this)

  midiTest(m);

  while(1)
  {
    m.update();
  }
  
}

void midiTest(Midi& m)
{
  uint32_t lastTime = 0;

  uint8_t channel = 0;
  
  while(1)
  {
    m.update();
    if(Clock::ticks - lastTime > 500)
    {
      lastTime = Clock::ticks;
      
      m.sendCC(0, channel, 0, 42);
      ++channel;
      if(channel > 20) break;
    }
  }
  
  uint8_t controlChannel = 0;
  while(1)
  {
    m.update();
    if(Clock::ticks - lastTime > 50)
    {
      lastTime = Clock::ticks;
      
      m.sendCC(0, 0, controlChannel, 42);
      ++controlChannel;
      if(controlChannel > 140) break;
    }
  }
  
  uint8_t value = 0;
  while(1)
  {
    m.update();
    if(Clock::ticks - lastTime > 50)
    {
      lastTime = Clock::ticks;
      
      m.sendCC(0, 0, 42, value);
      ++value;
      if(value > 140) break;
    }
  }
}


