cmake_minimum_required(VERSION 4.1)

project(trace LANGUAGES CXX)

add_executable(trace
        src/main.cpp
        src/tracer.cpp
        src/options.cpp
        src/syscall.cpp
        src/utils.cpp
        src/syscall_table.cpp
        #        src/process.cpp
#        src/decoder.cpp
#        src/child_memory.cpp
#        src/formatter.cpp
#        src/cli.cpp
)

target_include_directories(trace
        PRIVATE
        include
)

target_compile_features(trace PRIVATE cxx_std_20)

target_compile_options(trace PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Wconversion
        -Wshadow
)

target_compile_options(trace PRIVATE
        $<$<CONFIG:Debug>:-fsanitize=address,undefined>
)

target_link_options(trace PRIVATE
        $<$<CONFIG:Debug>:-fsanitize=address,undefined>
)