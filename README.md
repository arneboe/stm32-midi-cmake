# stm32f1 usb midi 

Code for a custom usb midi controller using the super cheap stm32f103c8t6 boards.

Uses libopencm3 because it contains an usb stack and all examples I could find used libopencm3.


Cmake scripts based on:
 * https://github.com/StanislavLakhtin/stm32-DS18B20

usb midi based on:
 * https://github.com/UA3MQJ/usb-midi-key
 * https://github.com/DarkSunHD/STM32F1_MIDI_Serial_Bridge/blob/master/src/usbmidi.c



# Build

 1. mkdir build
 2. cd build
 3. cmake .. -DCMAKE_TOOLCHAIN_FILE=~/git/stm32-midi-cmake/libopencm3.cmake -DTOOLCHAIN_DIR=/path/to/gcc-arm-none-eabi-toolchain
 4. make
 5. make install (will call flash.sh)
