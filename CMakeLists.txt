cmake_minimum_required(VERSION 4.1)

project(trace LANGUAGES C CXX)

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

target_include_directories(trace PRIVATE include)

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

file(GLOB_RECURSE EXAMPLE_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_SOURCE_DIR}/examples/*.c"
)

set(EXAMPLE_TARGETS)

foreach(example_source ${EXAMPLE_SOURCES})
    get_filename_component(example_name ${example_source} NAME_WE)

    add_executable(${example_name} ${example_source})

    target_compile_features(${example_name} PRIVATE c_std_11)

    target_compile_options(${example_name} PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -O0
    )

    target_link_options(${example_name} PRIVATE
            -static
    )

    set_target_properties(${example_name} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/examples/bin"
    )

    list(APPEND EXAMPLE_TARGETS ${example_name})
endforeach()

add_custom_target(examples ALL
        DEPENDS ${EXAMPLE_TARGETS}
)

add_dependencies(trace examples)