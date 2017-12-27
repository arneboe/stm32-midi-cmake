#pragma once

class Midi;
class Faders;



/** Main midi controller logic */
class Control
{
private:
  Midi& midi;
  Faders& faders;
  
  
public:
  void sendFaderData() const;
  Control(Midi& midi, Faders& faders);
};
