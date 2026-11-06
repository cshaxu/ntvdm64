param([ValidatePattern('^[A-Z]$')][string]$Drive='F',[Parameter(Mandatory)][string]$Output)
$ErrorActionPreference='Stop'
Add-Type @"
using System;
using System.Runtime.InteropServices;
using Microsoft.Win32.SafeHandles;
public static class OpticalTocProbe {
 [DllImport("kernel32.dll",CharSet=CharSet.Unicode,SetLastError=true)] static extern SafeFileHandle CreateFileW(string n,uint a,uint s,IntPtr sa,uint d,uint f,IntPtr t);
 [DllImport("kernel32.dll",SetLastError=true)] static extern bool DeviceIoControl(SafeFileHandle h,uint c,IntPtr i,uint il,byte[] o,uint ol,out uint n,IntPtr v);
 public static byte[] Read(string drive) {
  using(var h=CreateFileW(@"\\.\"+drive+":",0x80000000,1,IntPtr.Zero,3,0,IntPtr.Zero)) {
   if(h.IsInvalid) throw new System.ComponentModel.Win32Exception(Marshal.GetLastWin32Error());
   var b=new byte[804];uint n;
   if(!DeviceIoControl(h,0x24000,IntPtr.Zero,0,b,(uint)b.Length,out n,IntPtr.Zero)) throw new System.ComponentModel.Win32Exception(Marshal.GetLastWin32Error());
   Array.Resize(ref b,(int)n);return b;
  }
 }
}
"@
$b=[OpticalTocProbe]::Read($Drive)
$o=4+8*[int]$b[3]+4
$frames=[int]$b[$o+1]*4500+[int]$b[$o+2]*75+[int]$b[$o+3]
@{drive=$Drive;firstTrack=$b[2];lastTrack=$b[3];leadoutAddress=@($b[$o..($o+3)]);originalVolumeFrames=$frames;raw=@($b)}|ConvertTo-Json -Depth 3|Set-Content $Output
