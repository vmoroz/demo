// Show the JS engine type.
// It is for the demo purpose only.
// Do not use this code in production.

function yellow(text) {
  return "\x1b[33m" + text + "\x1b[0m";
}

exports.printJSEngineInfo = function () {
  if (global.HermesInternal) {
    console.log(`${yellow("JS Engine:")} Hermes`);
  } else if (process.versions && process.versions.v8) {
    // Bun also reports "v8 version", but it uses JSC
    if (process.versions.bun) {
      console.log(`${yellow("JS Engine:")} JavaScriptCore (JSC)`);
    } else {
      console.log(`${yellow("JS Engine:")} V8, version: ${process.versions.v8}`);
    }
  } else {
    console.log(`${yellow("JS Engine:")} Unknown`);
  }
};