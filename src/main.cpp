#include <Midi.hpp>
#include <Clock.hpp>
#include <Adc.hpp>
#include <Faders.hpp>
#include "Systick.hpp"
#include <Usart.hpp>
#include <Control.hpp>
#include <stdio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/adc.h>
#include <libopencm3/stm32/dma.h>
#include <math.h>


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
  printf("midi error\n");
}

int main(void)
{
  rcc_clock_setup_in_hse_8mhz_out_72mhz();//this also sets the adc prescaler

  rcc_periph_clock_enable(RCC_GPIOC);
  rcc_periph_clock_enable(RCC_GPIOB);

  gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_50_MHZ,
                GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);

  //Also initializes the clock
  Systick::init();//has to be done asap because most libs use it during init
  Usart::init();

  Midi midi(midiErrorHandler);
  Clock::delayMs(500); //wait for midi end points to be created (see comment in Midi.cpp for why we need this)
  //also initializing the adc while midi is still initializing seems to freeze the stm32... man i dont understand this hardware :D

  Adc adc;
  Faders faders(adc);
  
  //this starts the main logic "thread"
  Control ctrl(midi, faders);

  //main thread updates usb as fast as possible, will get interrupted by timers
  while(1)
  {
    midi.update();
  }
}
