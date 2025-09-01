// Show the JS engine type.
// It is for the demo purpose only. Do not use this code in production.

exports.printJSEngineInfo = function () {
  console.log("");
  const prefix = yellow("JS Engine: ");

  if (global.HermesInternal) {
    console.log(prefix + "Hermes");
  } else if (process.versions && process.versions.v8) {
    // Bun reports v8 version, but it uses JSC
    if (process.versions.bun) {
      console.log(prefix + "JavaScriptCore (JSC)");
    } else {
      console.log(prefix + "V8, version: " + process.versions.v8);
    }
  } else {
    console.log(prefix + "Unknown");
  }
};

function yellow(text) {
  return "\x1b[33m" + text + "\x1b[0m";
}
