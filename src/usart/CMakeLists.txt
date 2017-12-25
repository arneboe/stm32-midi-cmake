cmake_minimum_required(VERSION 3.0)


set(SOURCES Usart.cpp)
add_library(usart STATIC ${SOURCES})
target_link_libraries(usart opencm3_stm32f1)

include_directories(${LIBOPENCM3_DIR}/include)
