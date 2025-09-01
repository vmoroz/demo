const greeter = require("bindings")("greeter");

console.log(`${greeter.sayHello("world")}`);

require("../common/jsEngineInfo.cjs").printJSEngineInfo();
