cmake_minimum_required(VERSION 3.5.0)
project(stm32-midi)

IF(NOT CMAKE_CROSSCOMPILING)
    message(FATAL_ERROR "Cross compiling only. Please use -DCMAKE_TOOLCHAIN_FILE=/PATH/TO/TOOLCHAIN_FILE .")
ENDIF(NOT CMAKE_CROSSCOMPILING)

add_definitions(-DSTM32F1)

set(LIBOPENCM3_DIR ${CMAKE_SOURCE_DIR}/libopencm3)
add_custom_target(libopencm3 make WORKING_DIRECTORY ${LIBOPENCM3_DIR})
link_directories(${LIBOPENCM3_DIR}/lib)

include_directories(${LIBOPENCM3_DIR}/include)
        
#change this if you want to compile for a different stm32
set(LD_SCRIPT "${CMAKE_SOURCE_DIR}/libopencm3/lib/stm32/f1/stm32f103x8.ld")
        
set(STM32F1_FLAGS "-mcpu=cortex-m3 -mthumb -msoft-float")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall ${STM32F1_FLAGS} -std=c99 -Os -ffunction-sections -fdata-sections ")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall --std=c++14 ${STM32F1_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T ${LD_SCRIPT} -nostartfiles -lopencm3_stm32f1 -lc -lnosys -Wl,--gc-sections --specs=rdimon.specs") 

function(add_bin_from_elf bin elf)
    add_custom_target(${bin}
            ALL ${CMAKE_OBJCOPY}
            -Obinary ${elf} ${bin} 
            DEPENDS ${elf}
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
            
    install(CODE "execute_process(COMMAND ${CMAKE_SOURCE_DIR}/flash.sh ${CMAKE_BINARY_DIR}/${bin})")
            
endfunction(add_bin_from_elf)

add_subdirectory(src)

#todo add custom target flash
