cmake_minimum_required(VERSION 3.14)
project(MyWasmProject)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

# Check if we're building for WebAssembly
if(EMSCRIPTEN)
    # WebAssembly build configuration
    set(CMAKE_CXX_COMPILER "em++")
    set(CMAKE_CXX_FLAGS "-O0")
    set(CMAKE_VERBOSE_MAKEFILE ON)
    set(CMAKE_EXE_LINKER_FLAGS "-s WASM=1 -s MODULARIZE=1 -s EXPORT_NAME=MyModule -s ALLOW_MEMORY_GROWTH=1 --bind")

    # Specify the source files for the WebAssembly build
    set(WASM_SOURCES 
        Cpu.cpp
        Bus.cpp
        Bindings.cpp
    )

    # Add the WebAssembly executable
    add_executable(my_wasm_project ${WASM_SOURCES})

    # Set the output directory for the wasm and JavaScript files
    set_target_properties(my_wasm_project PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        OUTPUT_NAME "my_wasm_project"
        SUFFIX ".js"
    )
else()
    # Native build configuration for testing

    # Google Test setup for testing
    enable_testing()
    find_package(GTest REQUIRED)
    find_package(Threads REQUIRED)
    include_directories(${GTEST_INCLUDE_DIRS})

    # Test executable
    set(TEST_SOURCES
        Cpu_test.cpp
        Bus.cpp
        Cpu.cpp
    )
    add_executable(cpu_tests ${TEST_SOURCES})
    target_compile_definitions(cpu_tests PRIVATE PROJECT_ROOT_DIR="${CMAKE_SOURCE_DIR}")
    target_link_libraries(cpu_tests ${GTEST_LIBRARIES} gtest_main Threads::Threads)

    # Register tests
    add_test(NAME cpu_tests COMMAND cpu_tests)
endif()
