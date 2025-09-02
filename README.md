# Demo Repository

This repository contains various demonstration projects showcasing different technologies and concepts. Each demo is designed to be educational and serves as a practical example for specific use cases.

## 📁 Demos

### [Hermes Node-API Greeter](hermes-node-api/greeter/) 
**React Universe 2025 Conference Demo**

A practical exploration of Node-API compatibility across different JavaScript runtimes and engines. This demo shows how to build native modules that work seamlessly with Node.js, Hermes, Deno, and Bun.

**Key Features:**
- ✅ **Cross-Runtime Compatibility** - Same native module runs on Node.js, Hermes, Deno, and Bun
- 🔧 **Multiple Implementation Examples** - C, C++, and C# APIs
- 🎯 **Presentation-Focused** - Simplified code designed for stage demonstrations
- 📦 **Easy Setup** - Automated environment configuration scripts

> **⚠️ Important Note**: These demos are intentionally simplified for presentation clarity. Error handling, return status checking, and other production considerations are omitted for brevity. For production-ready Node-API examples with proper error handling, refer to the [official Node-API examples](https://github.com/nodejs/node-addon-examples).

**What's Included:**
- **[C API Implementation](hermes-node-api/greeter/c-api/)** - Pure C Node-API example
- **[C++ API Implementation](hermes-node-api/greeter/cpp-api/)** - Modern C++ Node-API wrapper
- **[C# API Implementation](hermes-node-api/greeter/cs-api/)** - .NET 8 Node-API bindings
- **[Hermes CLI Tool](hermes-node-api/greeter/hermes-cli/)** - Custom JavaScript runtime powered by Hermes

**Quick Start:**
```powershell
# Navigate to the demo
cd hermes-node-api\greeter

# Set up development environment (VS 2022 + hermes-cli)
.\start-vs-code.ps1

# Build all demos
.\build-all.ps1

# Test with different runtimes
cd c-api
node .\hello.cjs           # Node.js
hermes-cli.exe .\hello.cjs # Hermes
deno --allow-read --allow-env --allow-ffi .\hello.cjs  # Deno
bun .\hello.cjs            # Bun
```

## 🛠️ Prerequisites

To run the demos in this repository, you'll need:

- **Windows 10/11** (primary development environment)
- **Visual Studio 2022** with C++ build tools and .NET 8 SDK
- **Node.js 22.x or 24.x**
- **Python 3.11** (required for node-gyp)
- **PowerShell** (for automation scripts)

**Optional (for extended testing):**
- **Deno** - Alternative JavaScript runtime
- **Bun** - Fast JavaScript runtime and bundler

**Easy Installation with WinGet:**
```powershell
winget install OpenJS.NodeJS
winget install Python.Python.3.11
winget install DenoLand.Deno
winget install Oven-sh.Bun
```

## 📖 Getting Started

1. **Clone the repository:**
   ```powershell
   git clone https://github.com/vmoroz/demo.git
   cd demo
   ```

2. **Choose a demo** and follow its specific README instructions

3. **For the Hermes Node-API demo**, use the automated setup:
   ```powershell
   cd hermes-node-api\greeter
   .\start-vs-code.ps1
   ```

## 📝 License

See [LICENSE](LICENSE) file for details.

## 🤝 Contributing

Feel free to submit issues, feature requests, or pull requests to improve these demos or add new ones.
