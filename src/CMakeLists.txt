cmake_minimum_required(VERSION 3.0)


#FIXME use target_include_directories. better style
include_directories(clock)
include_directories(midi)
include_directories(adc)
include_directories(fader)
include_directories(usart)
include_directories(control)


add_subdirectory(clock)
add_subdirectory(adc)
add_subdirectory(fader)
add_subdirectory(midi)
add_subdirectory(usart)
add_subdirectory(control)


set(SOURCES 
    main.cpp
    Systick.cpp)

add_executable(${PROJECT_NAME}.elf ${SOURCES})
#add_dependencies(${PROJECT_NAME}.elf libopencm3)
target_link_libraries(${PROJECT_NAME}.elf
                      opencm3_stm32f1
                      adc
                      fader
                      clock
                      midi
                      usart
                      control)

include_directories(${LIBOPENCM3_DIR}/include)

add_bin_from_elf(${PROJECT_NAME}.bin ${PROJECT_NAME}.elf)
