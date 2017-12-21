cmake_minimum_required(VERSION 3.0)


set(SOURCES main.cpp)

add_executable(${PROJECT_NAME}.elf ${SOURCES})
add_dependencies(${PROJECT_NAME}.elf libopencm3)
target_link_libraries(${PROJECT_NAME}.elf opencm3_stm32f1)

add_bin_from_elf(${PROJECT_NAME}.bin ${PROJECT_NAME}.elf )
