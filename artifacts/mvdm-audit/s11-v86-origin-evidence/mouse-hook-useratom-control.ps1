Add-Type -TypeDefinition @"
using System;
using System.Runtime.InteropServices;
public static class ClassCheck {
 [StructLayout(LayoutKind.Sequential, CharSet=CharSet.Unicode)] public struct WNDCLASS {public uint style;public IntPtr proc;public int extra;public int winextra;public IntPtr instance;public IntPtr icon;public IntPtr cursor;public IntPtr brush;public string menu;public string name;}
 [DllImport("kernel32.dll",CharSet=CharSet.Unicode,SetLastError=true)] public static extern IntPtr GetModuleHandleW(string value);
 [DllImport("kernel32.dll",CharSet=CharSet.Ansi,SetLastError=true)] public static extern IntPtr GetProcAddress(IntPtr module,string value);
 [DllImport("user32.dll",CharSet=CharSet.Unicode,SetLastError=true)] public static extern ushort RegisterClassW(ref WNDCLASS wc);
 [DllImport("user32.dll",CharSet=CharSet.Unicode,SetLastError=true)] public static extern int GetClipboardFormatNameW(uint id,System.Text.StringBuilder output,int length);
 [DllImport("user32.dll",CharSet=CharSet.Unicode,SetLastError=true)] public static extern bool UnregisterClassW(string name,IntPtr instance);
}
"@
$name='T473-Class-Control-'+[guid]::NewGuid().ToString('N'); $c=[ClassCheck+WNDCLASS]::new(); $c.name=$name; $c.instance=[ClassCheck]::GetModuleHandleW($null); $c.proc=[ClassCheck]::GetProcAddress([ClassCheck]::GetModuleHandleW('user32.dll'),'DefWindowProcW'); $a=[ClassCheck]::RegisterClassW([ref]$c); $e=[Runtime.InteropServices.Marshal]::GetLastWin32Error(); "class_atom=$a error=$e"; if($a){$n=[Text.StringBuilder]::new(260); $count=[ClassCheck]::GetClipboardFormatNameW($a,$n,260); "clipboard_lookup_length=$count same_own_class=$($n.ToString() -eq $name)"; $ok=[ClassCheck]::UnregisterClassW($name,$c.instance); "unregistered=$ok"; if(!$ok){exit 2}} else {exit 1}
