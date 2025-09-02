#include <node_api.h>
#include <windows.h>
#include <iostream>

// Declare the external pseudo-variable
extern "C" IMAGE_DOS_HEADER __ImageBase;

#define YELLOW(text) "\x1b[33m" text "\x1b[0m"

EXTERN_C_START

void PrintModuleInfo() {
  char exePath[MAX_PATH];
  GetModuleFileNameA(NULL, exePath, MAX_PATH);
  std::cout << YELLOW("Current EXE: ") << exePath << std::endl;

  char dllPath[MAX_PATH];
  GetModuleFileNameA(
      reinterpret_cast<HINSTANCE>(&__ImageBase), dllPath, MAX_PATH);
  std::cout << YELLOW("Current DLL: ") << dllPath << std::endl;
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
