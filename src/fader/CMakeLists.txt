cmake_minimum_required(VERSION 3.0)

set(SOURCES 
    Faders.cpp)

add_library(fader STATIC ${SOURCES})
target_link_libraries(fader adc)
