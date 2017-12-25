#pragma once


/** Simple static usart debug output */
struct Usart
{
  static void init();
  static int write(const char* ptr, int len);
};
