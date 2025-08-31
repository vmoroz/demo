#include <node_api.h>
#include <windows.h>
#include <iostream>

// Declare the external pseudo-variable
extern "C" IMAGE_DOS_HEADER __ImageBase;

#define YELLOW(text) "\x1b[33m" text "\x1b[0m"

EXTERN_C_START

void PrintModuleInfo() {
  // Get the current process's file path
  char exePath[MAX_PATH];
  if (GetModuleFileNameA(NULL, exePath, MAX_PATH)) {
    std::cout << YELLOW("Current EXE: ") << exePath << std::endl;
  } else {
    std::cerr << "Failed to retrieve the EXE file name. Error: "
              << GetLastError() << std::endl;
  }

  // Get the DLL's file path
  char dllPath[MAX_PATH];
  if (GetModuleFileNameA(
          reinterpret_cast<HINSTANCE>(&__ImageBase), dllPath, MAX_PATH)) {
    std::cout << YELLOW("Current DLL: ") << dllPath << std::endl;
  } else {
    std::cerr << "Failed to retrieve the DLL file name. Error: "
              << GetLastError() << std::endl;
  }
}

// DLL entry point - called automatically when DLL is loaded/unloaded
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
  switch (fdwReason) {
    case DLL_PROCESS_DETACH:
      // Print the DLL info in the end on unload in our demo
      PrintModuleInfo();
      break;
  }
  return TRUE;
}

EXTERN_C_END
