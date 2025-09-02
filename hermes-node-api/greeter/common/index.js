// Show the JS engine type.
// It is for the demo purpose only. Do not use this code in production.

exports.printJSEngineInfo = function () {
  console.log("");
  const prefix = yellow("JS Engine: ");

  if (global.HermesInternal) {
    console.log(prefix + green("Hermes"));
  } else if (process.versions && process.versions.v8) {
    // Bun reports v8 version, but it uses JSC
    if (process.versions.bun) {
      console.log(prefix + green("JavaScriptCore (JSC)"));
    } else {
      console.log(prefix + green("V8, version: " + process.versions.v8));
    }
  } else {
    console.log(prefix + "Unknown");
  }
};

function yellow(text) {
  return "\x1b[33m" + text + "\x1b[0m";
}

function green(text) {
  return "\x1b[32m" + text + "\x1b[0m";
}

exports.format = function (message) {
  return "\n===============================\n" +
    `\x1b[32m${message}\x1b[0m` +
    "\n===============================";
};
