#include <windows.h>
#include <fstream>
#include <iostream>
#include <string>

#include "hermes_api.h"

// Forward declarations for Hermes functions (if available)
// Note: You may need to include actual Hermes headers when they become
// available For now, we'll demonstrate basic DLL loading

// Function to execute JavaScript code using Hermes
// TODO: Implement actual Hermes JavaScript execution
void runJavaScript(const std::string& jsCode) {
  std::cout << "JavaScript code to execute:" << std::endl;
  std::cout << "----------------------------------------" << std::endl;
  std::cout << jsCode << std::endl;
  std::cout << "----------------------------------------" << std::endl;
  std::cout << "TODO: Execute this code with Hermes engine" << std::endl;

  jsr_config config{};
  jsr_runtime runtime{};
  napi_env env{};
  jsr_create_config(&config);
  jsr_config_enable_gc_api(config, true);
  jsr_create_runtime(config, &runtime);
  jsr_delete_config(config);
  jsr_runtime_get_node_api_env(runtime, &env);

  napi_value str;
  napi_create_string_utf8(env, jsCode.c_str(), NAPI_AUTO_LENGTH, &str);
  napi_run_script(env, str, NULL);

  // Microsoft::NodeApiJsi::NodeApiEnvScope envScope{env};

  jsr_delete_runtime(runtime);
}

// Function to read file contents into a string
std::string readFileContents(const std::string& filename) {
  std::ifstream file(filename);
  if (!file.is_open()) {
    throw std::runtime_error("Unable to open file: " + filename);
  }

  std::string content;
  std::string line;
  while (std::getline(file, line)) {
    content += line + "\n";
  }

  return content;
}

int main(int argc, char* argv[]) {
  std::cout << "Hermes CLI - JavaScript Engine" << std::endl;

  // Check command line arguments
  if (argc != 2) {
    std::cerr << "Error: JavaScript file name is required." << std::endl;
    std::cerr << "Usage: " << argv[0] << " <script.js>" << std::endl;
    return 1;
  }

  std::string jsFilename = argv[1];
  std::cout << "JavaScript file: " << jsFilename << std::endl;

  // Try to load the Hermes DLL to verify it's available
  HMODULE hermesModule = LoadLibraryA("hermes.dll");
  if (hermesModule != nullptr) {
    std::cout << "Successfully loaded hermes.dll" << std::endl;

    // Here you would typically get function pointers from the DLL
    // For example:
    // auto hermesFunction = GetProcAddress(hermesModule, "SomeFunctionName");

    FreeLibrary(hermesModule);
  } else {
    std::cerr << "Failed to load hermes.dll. Error: " << GetLastError()
              << std::endl;
    return 1;
  }

  try {
    // Read the JavaScript file
    std::string jsCode = readFileContents(jsFilename);

    // Execute the JavaScript code
    runJavaScript(jsCode);

    std::cout << "Hermes CLI completed successfully!" << std::endl;
    return 0;

  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }
}
