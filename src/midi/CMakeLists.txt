cmake_minimum_required(VERSION 3.0)


set(SOURCES 
    Midi.cpp)

add_library(midi STATIC ${SOURCES})
    
#add_dependencies(midi libopencm3)
target_link_libraries(midi opencm3_stm32f1 clock)

include_directories(${LIBOPENCM3_DIR}/include)
