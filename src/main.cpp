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

//FIXME move midi send logic into class
Faders* pFaders = nullptr;
Midi* pMidi = nullptr;

/** This function should be called from inside the systick.
 *  It checks for changed fader values every 5 ms and sends them through midi if they changed
 *
 * */
void sendFaderData()
{
  if(pFaders == nullptr || pMidi == nullptr)
    return; //not yet initialized

  //run every 5 ms
  static uint32_t lastTime = Clock::ticks;
  if(Clock::ticks - lastTime < 5) //every 5 ms => 200hz
    return;

  lastTime = Clock::ticks;

  //message buffer is reused every time
  static Midi::CCMessage messages[Adc::NUM_ADC];
  //used to remember the last values and avoid spam
  static uint8_t lastValues[Adc::NUM_ADC] = {255}; //255 can never happen in midi

  uint8_t nextMessageIndex = 0;

  pFaders->update();
  //FIXME assert that pFaders->numFaders == Adc::NUM_ADC
  for(int i = 0; i < pFaders->numFaders(); ++i) //FIXME Only updates 1 fader
  {
    const uint8_t value = pFaders->getFaderValue(i);
    
    //FIXME values shitter too much, this is just a hack for debugging:
    if(abs(((int)value) - ((int)lastValues[i])) > 10)
//     if(value != lastValues[i])
    {
      lastValues[i] = value;
      messages[nextMessageIndex].controlChannel = i;
      messages[nextMessageIndex].value = value;
      ++nextMessageIndex;
    }
  }

  //if anything changed, send it
  if(nextMessageIndex > 0)
  {
    pMidi->sendCC(messages, nextMessageIndex);
  }
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

  Midi m(midiErrorHandler);
  Clock::delayMs(500); //wait for midi end points to be created (see comment in Midi.cpp for why we need this)
  //also initializing the adc while midi is still initializing seems to freeze the stm32... man i dont understand this hardware :D

  Adc adc;
  Faders faders(adc);

  pMidi = &m;
  pFaders = &faders;

  //main logic is run in systick
  Systick::addCallback(&sendFaderData);

  //main thread updates usb as fast as possible, will get interrupted by systick
  while(1)
  {
    m.update();
  }
}
