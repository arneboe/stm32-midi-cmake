#include "Control.hpp"
#include <Clock.hpp>
#include <Midi.hpp>
#include <Faders.hpp>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/rcc.h>
#include <stdio.h>


//FIXME this is done to use the timer, but it is very bad style?! use callback or something instead?
static Control* pControl = nullptr;

extern "C"
{
  static uint32_t lll = Clock::ticks;
  void tim2_isr(void)
  {
    TIM_SR(TIM2) &= ~TIM_SR_UIF; /* Clear interrrupt flag. otherwise isr will trigger again */
    
    if(pControl != nullptr)
    {
      pControl->sendFaderData();
    }
    
    lll = Clock::ticks;
    printf("took: %d\n", Clock::ticks - lll);
  }
}

Control::Control(Midi& midi, Faders& faders) : midi(midi), faders(faders)
{
  //FIXME this is not the opencm3 way to initialize the timer.
  //      use the opencm3 lib functions instead?
  rcc_periph_clock_enable(RCC_TIM2);

  nvic_enable_irq(NVIC_TIM2_IRQ);
  nvic_set_priority(NVIC_TIM2_IRQ, 1);
  /* Set timer start value. */
  TIM_CNT(TIM2) = 1;
  /* Set timer prescaler. 72MHz/1440 => 50000 counts per second. */
  TIM_PSC(TIM2) = 1440;

  /* End timer value. If this is reached an interrupt is generated.
  * 50000/250 = 200hz
  */
  TIM_ARR(TIM2) = 1000; //FIXME 1000 just for testing

  /* Update interrupt enable. */
  TIM_DIER(TIM2) |= TIM_DIER_UIE;
  /* Start timer. */
  TIM_CR1(TIM2) |= TIM_CR1_CEN;
  
  pControl = this;
  
}

void Control::sendFaderData() const
{
  //message buffer is reused every time
  static CCMessage messages[Faders::NUM_FADERS];
  
  //used to remember the last values and avoid spam,
  //255 can never happen in midi thus it is a good initial value
  static uint8_t lastValues[Faders::NUM_FADERS] = {255}; 

  uint8_t nextMessageIndex = 0;

  faders.update();
  for(int i = 0; i < Faders::NUM_FADERS; ++i)
  {
    const uint8_t value = faders.getFaderValue(i);
    
    if(value != lastValues[i])
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
    midi.sendCC(messages, nextMessageIndex);
  }
}

