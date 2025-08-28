# Greeter Demo for React Universe 2025

This folder contains greeter implementations using different APIs to demonstrate Hermes integration with Node.js.

## Quick Start with VS Code

To set up your development environment with VS 2022 tools and hermes-cli in PATH, use the provided PowerShell script:

### Prerequisites

- Visual Studio 2022 with C++ build tools installed
- VS Code installed
- PowerShell execution policy allows running scripts (see below if needed)

### Launch VS Code with Proper Environment

```powershell
# Navigate to this folder
cd hermes-node-api\greeter

# Run the setup script
.\start-vs-code.ps1

# For verbose output to see what's happening
.\start-vs-code.ps1 -Verbose
```

### What the Script Does

The `start-vs-code.ps1` script automatically:

1. **Finds Visual Studio 2022**: Locates your VS 2022 installation using Microsoft's `vswhere.exe` tool
2. **Sets up Build Environment**: Configures the VS 2022 Developer command prompt environment
3. **Adds hermes-cli to PATH**: Makes the hermes-cli executable available from any terminal
4. **Launches VS Code**: Opens VS Code in this directory with the proper environment

### PowerShell Execution Policy

If you get an execution policy error, you can temporarily allow script execution:

```powershell
# Check current policy
Get-ExecutionPolicy

# Allow scripts for current user (recommended)
Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser

# Or allow for current session only
Set-ExecutionPolicy -ExecutionPolicy Bypass -Scope Process
```

### Manual Setup (Alternative)

If you prefer to set up the environment manually:

1. Open VS 2022 Developer Command Prompt
2. Add hermes-cli to PATH:
   ```cmd
   set PATH=%PATH%;%cd%\hermes-cli\build\bin\Release
   ```
3. Launch VS Code from that command prompt:
   ```cmd
   code .
   ```

## Project Structure

- `c-api/` - C API implementation
- `cpp-api/` - C++ API implementation  
- `cs-api/` - C# API implementation
- `hermes-cli/` - Hermes CLI tool for running JavaScript with Hermes engine
- `start-vs-code.ps1` - PowerShell script for environment setup

## Building hermes-cli

The hermes-cli needs to be built within the VS 2022 environment. Here's the recommended workflow:

1. **First, launch VS Code with the proper environment:**
   ```powershell
   .\start-vs-code.ps1
   ```

2. **Then, build hermes-cli from VS Code's integrated terminal:**
   ```cmd
   cd hermes-cli
   mkdir build
   cd build
   cmake ..
   cmake --build . --config Release
   ```

The PowerShell script will automatically add the hermes-cli build path to your PATH environment variable, so once built, `hermes-cli.exe` will be available from any terminal within VS Code.

> **Note**: Don't try to build hermes-cli from a regular PowerShell prompt - you need the VS 2022 Developer environment that the script provides to have access to CMake and the C++ compiler.