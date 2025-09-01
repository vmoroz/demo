
# Node-API C# Demo

This demo showcases a simple Node.js native module built using the Node-API C# interface. The module implements a basic greeter function that demonstrates how to create native addons using .NET that work across different JavaScript runtimes.

## Prerequisites

- .NET SDK 8.0
- Node.js (for npm)

## Building the Project

To compile the native module, run the following commands in sequence:

```
npm install
```

```
dotnet publish
```

The first command installs the required Node.js dependencies, and the second command builds the C# DLL and publishes it into the build folder as `greeter.node`.

## Running the Demo

Once built, you can run the demo with any of the following JavaScript runtimes:

### Node.js
```
node .\hello.cjs
```

### Hermes CLI
```
hermes-cli.exe .\hello.cjs
```

### Deno
```
deno --allow-read --allow-env --allow-ffi .\hello.cjs
```

### Bun
```
bun .\hello.cjs
```

## What it does

The demo creates a simple greeter module that:
- Exports a `sayHello()` function implemented in C#
- Demonstrates cross-runtime compatibility with Node-API
- Shows JavaScript engine information for the current runtime
- Shows currently running executable file path
- Shows currently running addon file path

## Project Structure

- `greeter.cs` - The C# source code implementing the native module
- `greeter.csproj` - .NET project configuration file
- `module_info.cs` - Additional module information utilities
- `hello.cjs` - JavaScript entry point that loads and uses the native module
- `package.json` - Node.js project metadata and dependencies