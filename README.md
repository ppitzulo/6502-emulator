# MOS 6502 Emulator (WebAssembly)

This project is a MOS 6502 emulator that is compiled to WebAssembly (WASM) using CMake. The WebAssembly version of the emulator can be used in a web-based environment to simulate the 6502 processor. Additionally, the emulator attempts to accurately simulate illegal opcodes, which are undocumented behaviors of the 6502 CPU, making it useful for emulating software that relies on these instructions. The project also includes a native version for testing purposes using Google Test.
## Features
* WebAssembly Build: The project is configured to compile to WebAssembly for web-based applications.
* Native Build for Testing: A native version of the emulator is built using Google Test to run unit tests.
* Flexible Build Configuration: The project automatically detects whether it's being built for WebAssembly or a native environment and configures the build accordingly.

## Requirements
* CMake (minimum version 3.14)
* Emscripten (for WebAssembly build)
* Google Test (for native testing)
* C++17

## Build Instructions
### WebAssembly Build
1. Install Emscripten from the official website.
2. Configure the project for WebAssembly:
   ```bash
   mkdir build
   cd build
   emcmake cmake ..
   cmake --build .```
3. The WebAssembly output (`my_wasm_project.wasm` and `my_wasm_project.js`) will be generated in the `build/bin` directory.
### Native Build (for Testing)
1. Install [Google Test](https://github.com/google/googletest)
2. Configure and build the project:
   ```
   mkdir build
   cd build
   cmake ..
   cmake --build .
   ```
3. Run the tests:
   ```
   ./cpu_tests
   ```
## Project Structure
* `Cpu.cpp`, `Bus.cpp`: Core components of the 6502 emulator.
* `Bindings.cpp`: WASM bindings for javascript integration.
* `Cpu_test.cpp`: Unit tests for the 6502 CPU.

## Credits
Massive thanks to the [65x02 Single Step Tests](https://github.com/SingleStepTests/65x02) contributors it made debugging the processor a much easier task.
