#include "Midi.hpp"
#include "Clock.hpp"
#include "Systick.hpp"
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>



int main(void)
{
  rcc_clock_setup_in_hse_8mhz_out_72mhz();


  rcc_periph_clock_enable(RCC_GPIOC);
  rcc_periph_clock_enable(RCC_GPIOB);

  gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_50_MHZ,
                GPIO_CNF_OUTPUT_PUSHPULL, GPIO13); 

  
  Systick::init();
  Midi m;
  
  uint32_t lastTime = 0;

  while(1)
  {
    m.update();
    if(Clock::ticks - lastTime > 5000)
    {
      lastTime = Clock::ticks;
      m.send();
      gpio_toggle(GPIOC, GPIO13);
      Clock::delayMs(200);
      gpio_toggle(GPIOC, GPIO13);
    }
    
  }
}
