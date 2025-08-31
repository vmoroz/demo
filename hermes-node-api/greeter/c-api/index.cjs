const greeter = require("bindings")("greeter");

console.log(`\n${greeter.sayHello("world")} \n`);

require("../common/jsEngineInfo.cjs").printJSEngineInfo();
