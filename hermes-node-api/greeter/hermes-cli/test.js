// Simple JavaScript test file
console.log("Hello from JavaScript!");

function greet(name) {
    return "Hello, " + name + "!";
}

console.log(greet("Hermes"));

// Test some basic JavaScript features
const numbers = [1, 2, 3, 4, 5];
const doubled = numbers.map(n => n * 2);
for (const num of doubled) {
  console.log(`Doubled number: ${num}`);
}
