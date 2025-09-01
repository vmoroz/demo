# Greeter Demo for React Universe 2025

This folder contains greeter implementations using different APIs to demonstrate that Node-API modules can be shared across different JavaScript runtimes and engines.

## Project Structure

- **[c-api/](c-api/README.md)** - Node-API C implementation
- **[cpp-api/](cpp-api/README.md)** - Node-API C++ implementation  
- **[cs-api/](cs-api/README.md)** - Node-API C# implementation
- **[hermes-cli/](hermes-cli/README.md)** - Hermes CLI tool for running JavaScript with Hermes engine
- **[common/](common/README.md)** - Shared utility files used across demos
- `start-vs-code.ps1` - PowerShell script for environment setup
- `build-all.ps1` - PowerShell script to build all demo projects

Each demo folder contains its own README with specific build instructions and prerequisites.

## Quick Start with VS Code

To set up your development environment with VS 2022 tools and hermes-cli in PATH, use the provided PowerShell script.

### Prerequisites

- Visual Studio 2022 with C++ build tools and .NET 8 SDK installed
- VS Code installed
- PowerShell execution policy allows running scripts (see below if needed)
- Node.js 22.x or 24.x
- Python 3.11
- Deno (optional)
- Bun (optional)

> **Note**: Node.js is required for building the demos. Deno and Bun are optional for testing cross-runtime compatibility.

**Tip**:  Use WinGet to isntall the prerequisites:
- **Node.js**: `winget install OpenJS.NodeJS`
- **Python 3.11**: `winget install Python.Python.3.11`
- **Deno**: `winget install DenoLand.Deno`
- **Bun**: `winget install Oven-sh.Bun`

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

### Build All Demos

After launching VS Code with the proper environment, you can build all demo projects at once:

```powershell
.\build-all.ps1
```

For verbose output:
```powershell
.\build-all.ps1 -Verbose
```

To clean and rebuild everything:
```powershell
.\build-all.ps1 -Clean
```

The build script will automatically build all four demo projects:
- Hermes CLI
- C API demo
- C++ API demo  
- C# API demo

## Building hermes-cli (Individual Build)

If you need to build only hermes-cli, it needs to be built within the VS 2022 environment. See the [hermes-cli README](hermes-cli/README.md) for detailed build instructions.

**Quick build steps:**

1. **First, launch VS Code with the proper environment:**
   ```powershell
   .\start-vs-code.ps1
   ```

2. **Navigate to hermes-cli folder and build:**
   ```powershell
   cd hermes-cli
   .\build-hermes-cli.ps1
   ```

> **Note**: The build script must be run from within the VS 2022 Developer environment that `start-vs-code.ps1` provides.

## Running the Demos

After building the projects (either with `.\build-all.ps1` or individually), each demo can be run with multiple JavaScript runtimes:

- **Node.js**: `node .\hello.cjs`
- **Hermes CLI**: `hermes-cli.exe .\hello.cjs`
- **Deno**: `deno --allow-read --allow-env --allow-ffi .\hello.cjs`
- **Bun**: `bun .\hello.cjs`

Navigate to any demo folder (c-api, cpp-api, cs-api) and run these commands to test cross-runtime compatibility.

> **Tip**: For specific build instructions for each demo, see their respective README files linked in the Project Structure section above.