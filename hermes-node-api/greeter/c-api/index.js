const greeter = require("bindings")("greeter");

console.log(`\n ${greeter.sayHello("world")} \n`);

if (global.HermesInternal) {
  console.log("Running inside Hermes\n");
} else {
  console.log("Running outside Hermes\n");
}
