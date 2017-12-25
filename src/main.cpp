#include <Midi.hpp>
#include <Clock.hpp>
#include <Adc.hpp>
#include <Faders.hpp>
#include "Systick.hpp"
#include <Usart.hpp>
#include <stdio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/adc.h>
#include <libopencm3/stm32/dma.h>


//for printf
extern "C" 
{
  int _write(int file, char* ptr, int len)
  {
    return Usart::write(ptr, len);
  }

}

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
// void midiTest(Midi& m);




int main(void)
{
  rcc_clock_setup_in_hse_8mhz_out_72mhz();//this also sets the adc prescaler
  
  rcc_periph_clock_enable(RCC_GPIOC);
  rcc_periph_clock_enable(RCC_GPIOB);

  gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_50_MHZ,
                GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);
  
  Systick::init();//has to be done asap because most libs use it during init
  
  Usart::init();
  
  Midi m(midiErrorHandler);
  Clock::delayMs(500); //wait for midi end points to be created (see comment in Midi.cpp for why we need this)
  //also initializing the adc while midi is still initializing seems to freeze the stm32... man i dont understand this hardware :D

  Adc adc;
  Faders faders(adc);
  
  Midi::CCMessage messages[8];
  for(int i = 0; i < 8; ++i)
  {
    messages[i] = Midi::CCMessage(0, 0, i, 0);
  }
  
  uint32_t lastTime = 0;
  while(1)
  { 
    m.update();
    if(Clock::ticks - lastTime > 500)
    {
      Usart::write("AA\n", 3);
      
      lastTime = Clock::ticks;
      faders.update();
      
      printf("raw / mapped: %d / %d\n", adc.values[0], faders.getFaderValue(0));
      
      for(int i = 0; i < faders.numFaders(); ++i)
      {
        messages[i].value = faders.getFaderValue(i);
      }
      m.sendCC(messages, 8);
    }
  }
}
