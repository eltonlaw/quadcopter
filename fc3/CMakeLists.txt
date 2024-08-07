cmake_minimum_required(VERSION 3.16)

option(DEBUG "Enable debug symbols" ON)

# create compile_commands.json for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

set(FAMILY "G0" CACHE STRING "Target STM32 family")
set(PROJ_NAME fc3)
set(TARGET_NAME ${PROJ_NAME}-${FAMILY})
set(HAL_COMP_LIST
    CORTEX
    GPIO
    PWR
    RCC
    TIM
    UART
    )
set(CMSIS_COMP_LIST "")
message(STATUS "Configuring for STM32${FAMILY} series")
if(${FAMILY} STREQUAL "G0")
    set(FC3_G0 ON CACHE BOOL "Compile for G0" FORCE)
else()
    set(FC3_G0 OFF CACHE BOOL "Compile for G0" FORCE)
endif()
if(${FAMILY} STREQUAL "F1")
    set(FC3_F1 ON CACHE BOOL "Compile for F1" FORCE)
else()
    set(FC3_F1 OFF CACHE BOOL "Compile for F1" FORCE)
endif()

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/../ext/stm32-cmake/cmake/stm32_gcc.cmake)
project(${PROJ_NAME} C ASM)

add_compile_options(
    -std=gnu11 # specify the C standard
    -Wall
    -ffunction-sections # place each function in its own section
    -fdata-sections # place each data item in its own section
    -fstack-usage
    -mfloat-abi=soft
    -DUSE_HAL_DRIVER # define USE_HAL_DRIVER macro
    # -fcyclomatic-complexity # enable cyclomatic complexity
)
if(${FAMILY} STREQUAL "G0")
    set(FC3_G0 ON CACHE BOOL "Compile for G0" FORCE)
    set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/STM32G070RBTX_FLASH.ld)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T${LINKER_SCRIPT} --specs=nano.specs -mfloat-abi=soft -mthumb -Wl,--gc-sections -static -u _printf_float -Wl,--start-group -lc -lm -Wl,--end-group")
	add_compile_options(
		-mcpu=cortex-m0plus
		-DSTM32G070xx
        -mthumb
	)
endif()

if(DEBUG)
    # If DEBUG is ON, add debug symbols and potentially lower optimization
    add_compile_options(-g3 -O0)
    add_definitions(-DDEBUG) # Define DEBUG preprocessor macro
    message(STATUS "Debug mode is enabled")
else()
    # If DEBUG is OFF, optimize for release
    add_compile_options(-O3)
    message(STATUS "Debug mode is disabled, optimized build")
endif()
add_definitions(-DFC3_${FAMILY})

list(APPEND CMSIS_COMP_LIST STM32${FAMILY})
list(APPEND HAL_COMP_LIST STM32${FAMILY})
set(STM32_CUBE_${FAMILY}_PATH $ENV{STM32_CUBE_${FAMILY}_PATH} CACHE PATH "Path to STM32Cube${FAMILY}")

message(STATUS "CMAKE_TOOLCHAIN_FILE is set to ${CMAKE_TOOLCHAIN_FILE}")
message(STATUS "STM32_CUBE_${FAMILY}_PATH is set to ${STM32_CUBE_${FAMILY}_PATH}")
message(STATUS "STM32_CMAKE_DIR is set to ${STM32_CMAKE_DIR}")
message(STATUS "CMAKE_MODULE_PATH is set to ${CMAKE_MODULE_PATH}")
find_package(HAL COMPONENTS "${HAL_COMP_LIST}" REQUIRED)
find_package(CMSIS COMPONENTS "${CMSIS_COMP_LIST}" REQUIRED)
file(GLOB STLIB_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/stlib/Src/*.c")

# FIXME: add FC3_F1
if(FC3_G0)
    # FIXME: put hal_conf into headerfile
    add_executable(${TARGET_NAME}
        src/main.c
        # src/system_stm32g0xx.c
        src/stm32g0xx_it.c
        src/stm32g0xx_it.h
        src/stm32g0xx_hal_conf.h
        ${STLIB_SOURCES}
    )
    target_link_libraries(${TARGET_NAME}
        HAL::STM32::G0::CORTEX
        HAL::STM32::G0::GPIO
        HAL::STM32::G0::PWR
        HAL::STM32::G0::PWREx
        HAL::STM32::G0::RCC
        HAL::STM32::G0::TIM
        HAL::STM32::G0::TIMEx
        HAL::STM32::G0::UART
        HAL::STM32::G0::UARTEx
        CMSIS::STM32::G070RB
        STM32::NoSys
    )
	target_include_directories(${TARGET_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/../stlib/Inc)
    stm32_print_size_of_target(${TARGET_NAME})
    stm32_generate_hex_file(${TARGET_NAME})
    stm32_generate_binary_file(${TARGET_NAME})
endif()
add_custom_target(flash
    COMMAND STM32_Programmer_CLI -c port=SWD -w ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.bin 0x8000000 -v -rst
    COMMENT "Flashing the board..."
)
