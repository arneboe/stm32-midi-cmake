#include "Usart.hpp"
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>



//change these defines if you want to use another port
#define USART_CONSOLE USART3
#define USART_TX_PIN GPIO_USART3_TX
#define USART_RCC RCC_USART3


void Usart::init()
{
  rcc_periph_clock_enable(USART_RCC);
  
  gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ,
                GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, USART_TX_PIN);

  usart_set_baudrate(USART_CONSOLE, 115200);
  usart_set_databits(USART_CONSOLE, 8);
  usart_set_stopbits(USART_CONSOLE, USART_STOPBITS_1);
  usart_set_mode(USART_CONSOLE, USART_MODE_TX);
  usart_set_parity(USART_CONSOLE, USART_PARITY_NONE);
  usart_set_flow_control(USART_CONSOLE, USART_FLOWCONTROL_NONE);
  usart_enable(USART_CONSOLE);
}

int Usart::write(const char* ptr, int len)
{
  int i;

  for(i = 0; i < len; i++)
  {
    if(ptr[i] == '\n')
    {
      usart_send_blocking(USART_CONSOLE, '\r');
    }
    usart_send_blocking(USART_CONSOLE, ptr[i]);
  }
  return i;
}


