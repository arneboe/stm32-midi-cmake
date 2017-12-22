#include "Midi.hpp"
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
int main(void)
{
  rcc_clock_setup_in_hse_8mhz_out_72mhz();


  rcc_periph_clock_enable(RCC_GPIOC);
  rcc_periph_clock_enable(RCC_GPIOB);

  gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_50_MHZ,
                GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);

  gpio_set(GPIOC, GPIO13); // led off

  Midi m;

  while(1)
  {
    m.update();
  }
}
