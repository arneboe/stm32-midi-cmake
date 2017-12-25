cmake_minimum_required(VERSION 3.0)

include_directories(clock)
include_directories(midi)

add_subdirectory(clock)
add_subdirectory(midi)
add_subdirectory(adc)


set(SOURCES 
    main.cpp
    Systick.cpp)

add_executable(${PROJECT_NAME}.elf ${SOURCES})
#add_dependencies(${PROJECT_NAME}.elf libopencm3)
target_link_libraries(${PROJECT_NAME}.elf
                      opencm3_stm32f1
                      clock
                      midi
                      adc)

include_directories(${LIBOPENCM3_DIR}/include)

add_bin_from_elf(${PROJECT_NAME}.bin ${PROJECT_NAME}.elf)
