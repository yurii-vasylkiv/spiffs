cmake_minimum_required(VERSION 3.15)
project(spiffs)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wattributes -Wno-unused-variable -Wno-unused-parameter")
set(CMAKE_C_STANDARD 11)

file(GLOB SRC
        "src/*.c"
        "Inc/*.h"
        )

add_compile_options(-DNO_TEST)
add_compile_options(-DSPIFFS_USE_MAGIC)

include_directories(src)
add_executable(spiffs src/main.c ${SRC} src/spiffs_config.h src/params_test.h)