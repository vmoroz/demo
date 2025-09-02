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
- Visual Studio Developer environment (or run from VS Code launched with start-vs-code.ps1)

### Quick Build

Use the provided PowerShell build script:

```powershell
.\build-hermes-cli.ps1
```

For verbose output:
```powershell
.\build-hermes-cli.ps1 -Verbose
```

To clean and rebuild:
```powershell
.\build-hermes-cli.ps1 -Clean
```

### Manual Build Steps

If you prefer to build manually:

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

The executable will be created in `build/bin/Release/hermes-cli.exe` along with the required `hermes.dll`.

## Dependencies

- **Microsoft.JavaScript.Hermes** (0.0.0-2508.18001-6668da2d): JavaScript engine
  - Provides `hermes.dll` and `hermes.lib`
  - Located in `build/native/win32/x64/` within the package

## Project Structure

### Core Files
- `CMakeLists.txt`: CMake build configuration
- `build-hermes-cli.ps1`: PowerShell build script for easy compilation
- `hermes-cli.def`: Windows DEF file for exporting functions
- `test.js`: Sample JavaScript file for testing the CLI
- `README.md`: This file

### Source Files
The hermes-cli implementation is adapted from the Hermes Node-API unit tests (see [reference](https://github.com/microsoft/hermes-windows/tree/main/unittests/NodeApi)):

- `node_lite.cpp` / `node_lite.h`: Core Node-API lite implementation
- `node_lite_hermes.cpp`: Hermes-specific Node-API integration
- `node_lite_windows.cpp`: Windows-specific implementation details
- `child_process.cpp` / `child_process.h`: Child process management utilities
- `string_utils.cpp` / `string_utils.h`: String manipulation utilities
- `threadsafe_function.cpp`: Thread-safe function call implementations
- `compat.h`: Compatibility definitions and macros

## Notes

- The project automatically downloads the Hermes NuGet package during build
- The `hermes.dll` is copied to the output directory for runtime dependency
- Implementation is based on the Hermes Node-API unit tests from the [microsoft/hermes-windows repository](https://github.com/microsoft/hermes-windows/tree/main/unittests/NodeApi)
- Provides a lightweight Node-API compatible runtime for executing JavaScript with Hermes
