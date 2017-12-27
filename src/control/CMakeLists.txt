cmake_minimum_required(VERSION 3.0)


set(SOURCES 
    Control.cpp)

add_library(control STATIC ${SOURCES})
    
#add_dependencies(midi libopencm3)
target_link_libraries(control opencm3_stm32f1 clock midi fader)

include_directories(${LIBOPENCM3_DIR}/include)
