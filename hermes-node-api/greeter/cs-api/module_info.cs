using System;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Text;
using Microsoft.JavaScript.NodeApi;

using static Microsoft.JavaScript.NodeApi.Runtime.JSRuntime;

public static partial class Greeter
{
    [JSExport("printModuleInfo")]
    public static unsafe void PrintModuleInfo()
    {
        string exePath = Environment.ProcessPath
            ?? Process.GetCurrentProcess().MainModule?.FileName;
        Console.WriteLine($"{Yellow("Current EXE: ")}{exePath}");

        IntPtr moduleHandle = IntPtr.Zero;
        GetModuleHandleEx(
            GetModuleHandleExFlags.FromAddress | GetModuleHandleExFlags.UnchangedRefCount,
            (IntPtr)s_moduleInitialize, out moduleHandle);

        StringBuilder sb = new StringBuilder(512);
        GetModuleFileName(moduleHandle, sb, sb.Capacity);
        string dllPath = sb.ToString();
        Console.WriteLine($"{Yellow("Current DLL: ")}{dllPath}");
    }

    private static string Yellow(string text) => $"\x1b[33m{text}\x1b[0m";

    // Exported function defined in this DLL. It is defined in the generated code.
    private static readonly unsafe delegate* unmanaged
        <napi_env, napi_value, napi_value> s_moduleInitialize = &Microsoft.JavaScript.NodeApi.Generated.Module._Initialize;

    [DllImport("kernel32.dll", CharSet = CharSet.Unicode, SetLastError = true)]
    private static extern uint GetModuleFileName(IntPtr hModule, StringBuilder lpFilename, int nSize);

    [DllImport("kernel32.dll", SetLastError = true)]
    private static extern bool GetModuleHandleEx(
         GetModuleHandleExFlags dwFlags,
         IntPtr lpAddress,   // treat second param as address when FromAddress is set
         out IntPtr phModule);

    [Flags]
    private enum GetModuleHandleExFlags : uint
    {
        Pin = 0x00000001,                    // GET_MODULE_HANDLE_EX_FLAG_PIN
        UnchangedRefCount = 0x00000002,      // GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT
        FromAddress = 0x00000004             // GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS
    }
}