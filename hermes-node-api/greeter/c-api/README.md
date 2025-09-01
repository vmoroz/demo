# Node-API C Demo

This demo showcases a simple Node.js native module built using the Node-API C interface. The module implements a basic greeter function that demonstrates how to create native addons that work across different JavaScript runtimes.

## Building the Project

To compile the native module, run:

```
npm install
```

This will build the C extension using node-gyp and install the required dependencies.

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
- Exports a `sayHello()` function implemented in C
- Demonstrates cross-runtime compatibility with Node-API
- Shows JavaScript engine information for the current runtime
- Shows currently running executable file path
- Shows currently running addon file path

## Project Structure

- `greeter.c` - The C source code implementing the native module
- `binding.gyp` - Build configuration for node-gyp
- `hello.cjs` - JavaScript entry point that loads and uses the native module
- `package.json` - Project metadata and dependencies