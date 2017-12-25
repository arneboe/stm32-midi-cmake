#include "Systick.hpp"
#include "Clock.hpp"
#include <libopencm3/cm3/systick.h>


struct Callback
{
  Systick::SystickCallback cb;
  Callback* pNext = nullptr;
};

static Callback* systickCbs = nullptr;

extern "C" {
  void sys_tick_handler(void)
  {
    //FIXME maybe move clock tick to callback? is a waste of resources but the design is better?
    Clock::tick(1);
    
    Callback* callback = systickCbs;
    while(callback)
    {
      callback->cb();
      callback = callback->pNext;
    }
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

void Systick::addCallback(SystickCallback cb)
{
  //FIXME this leaks callbacks
  Callback* callback = new Callback;
  callback->cb = cb;
  callback->pNext = nullptr;
  
  if(systickCbs == nullptr)
  {
    systickCbs = callback;
  }
  else
  {
    Callback* currentCallback = systickCbs;
    while(currentCallback->pNext != nullptr)
      currentCallback = currentCallback->pNext;
    currentCallback->pNext = callback;
  }
}

