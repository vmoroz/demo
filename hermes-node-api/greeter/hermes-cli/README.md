# Hermes CLI

A C++ console application that demonstrates integration with the Microsoft.JavaScript.Hermes NuGet package. The CLI can load and execute JavaScript files using the Hermes JavaScript engine.

## Usage

```cmd
hermes-cli.exe <script.js>
```

Example:
```cmd
hermes-cli.exe test.js
```

## Features

- **Command Line Interface**: Accepts JavaScript file as argument
- **File Reading**: Reads JavaScript source code from files
- **Hermes Integration**: Links with hermes.dll and re-exports Node-API functions
- **Error Handling**: Provides clear error messages for missing files or arguments

## Building

### Prerequisites
- CMake 3.20 or later
- Visual Studio 2019/2022 or equivalent C++ compiler
- NuGet CLI tool

### Build Steps

1. Create a build directory:
```cmd
mkdir build
cd build
```

2. Generate build files:
```cmd
cmake .. -G "Visual Studio 17 2022" -A x64
```

3. Build the project:
```cmd
cmake --build . --config Release
```

The executable will be created in `build/bin/hermes-cli.exe` along with the required `hermes.dll`.

### Alternative Build (using Developer Command Prompt)

```cmd
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

## Dependencies

- **Microsoft.JavaScript.Hermes** (0.0.0-2508.18001-6668da2d): JavaScript engine
  - Provides `hermes.dll` and `hermes.lib`
  - Located in `build/native/win32/x64/` within the package

## Project Structure

- `CMakeLists.txt`: CMake build configuration
- `main.cpp`: Main application source code
- `README.md`: This file

## Notes

- The project automatically downloads the Hermes NuGet package during build
- The `hermes.dll` is copied to the output directory for runtime dependency
- Currently implements basic DLL loading verification
- Extend `main.cpp` to use actual Hermes API functions as needed
