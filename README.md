# stm32f103c8t6 usb midi 

Cmake scripts based on https://github.com/StanislavLakhtin/stm32-DS18B20

usb midi based on:
https://github.com/UA3MQJ/usb-midi-key



# Build

 1. mkdir build
 2. cd build
 3. cmake .. -DCMAKE_TOOLCHAIN_FILE=~/git/stm32-midi-cmake/libopencm3.cmake -DTOOLCHAIN_DIR=/path/to/gcc-arm-none-eabi-toolchain
 4. make
 5. make install (will install using openocd)
