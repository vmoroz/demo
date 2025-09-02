const greeter = require("bindings")("greeter");
const common = require("../common");

console.log(common.format(greeter.sayHello("world")));

common.printJSEngineInfo();