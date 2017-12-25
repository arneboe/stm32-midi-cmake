cmake_minimum_required(VERSION 3.0)


set(SOURCES Adc.cpp)
add_library(adc STATIC ${SOURCES})
target_link_libraries(adc opencm3_stm32f1 clock)
