using System; using System.Runtime.InteropServices;
public static class TabstopReader {
 [DllImport("kernel32.dll", SetLastError=true)] public static extern IntPtr OpenProcess(uint access, bool inherit, int pid);
 [DllImport("kernel32.dll", SetLastError=true)] public static extern bool ReadProcessMemory(IntPtr h, IntPtr address, byte[] bytes, UIntPtr count, out UIntPtr got);
 [DllImport("kernel32.dll")] public static extern bool CloseHandle(IntPtr h);
}
