#include "Systick.hpp"
#include "Clock.hpp"
#include <libopencm3/cm3/systick.h>

extern "C" {
  void sys_tick_handler(void)
  {
    Clock::tick(1);
  }
}

void Systick::init()
{
  /* clock rate / 1000 to get 1mS interrupt rate */
  systick_set_reload(72000);
  systick_set_clocksource(STK_CSR_CLKSOURCE_AHB);
  systick_counter_enable();
  /* this done last */
  systick_interrupt_enable();
}
