#pragma once

class Systick
{
public:
  typedef void (*SystickCallback)();
  
  static void init();
  
  static void addCallback(SystickCallback cb);
  
  
};
