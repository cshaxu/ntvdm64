param([Parameter(Mandatory)][int[]]$ProcessIds,[Parameter(Mandatory)][string]$BuildRoot,
      [string]$Desktop='', [switch]$IgnoreSerialWarnings, [switch]$IgnoreDeniedDiskWarning,
      [switch]$IgnoreIllegalOpcodeWarning)
$ErrorActionPreference='Stop'
$root=(Resolve-Path $BuildRoot).Path
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
if(!$root.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase)){throw 'Build-owned output required'}
$assembly="$root/WindowInspector-$PID.dll"
Add-Type -OutputAssembly $assembly -TypeDefinition @'
using System;
using System.Text;
using System.Runtime.InteropServices;
public static class WindowInspector {
 public delegate bool Visit(IntPtr w,IntPtr p);
 [DllImport("user32.dll")] static extern bool EnumWindows(Visit v,IntPtr p);
 [DllImport("user32.dll")] static extern bool EnumChildWindows(IntPtr w,Visit v,IntPtr p);
 [DllImport("user32.dll",CharSet=CharSet.Unicode,SetLastError=true)] static extern IntPtr OpenDesktop(string n,uint f,bool inherit,uint access);
 [DllImport("user32.dll")] static extern bool EnumDesktopWindows(IntPtr d,Visit v,IntPtr p);
 [DllImport("user32.dll")] static extern bool CloseDesktop(IntPtr d);
 [DllImport("user32.dll")] static extern int GetDlgCtrlID(IntPtr w);
 [DllImport("user32.dll")] static extern IntPtr GetDlgItem(IntPtr w,int id);
 [DllImport("user32.dll",CharSet=CharSet.Unicode)] static extern IntPtr SendMessageTimeout(IntPtr w,uint msg,IntPtr wp,IntPtr lp,uint flags,uint timeout,out IntPtr result);
 [DllImport("user32.dll")] static extern uint GetWindowThreadProcessId(IntPtr w,out uint p);
 [DllImport("user32.dll",CharSet=CharSet.Unicode)] static extern int GetWindowText(IntPtr w,StringBuilder s,int n);
 [DllImport("user32.dll")] static extern bool IsWindowVisible(IntPtr w);
 static string Text(IntPtr w){var s=new StringBuilder(2048);GetWindowText(w,s,s.Capacity);return s.ToString();}
 public static string Read(int[] ids,string desktop,bool ignoreSerial,bool ignoreDisk,bool ignoreIllegalOpcode){var result=new StringBuilder();
  Visit visit=(w,p)=>{uint pid;GetWindowThreadProcessId(w,out pid);if(Array.IndexOf(ids,(int)pid)<0)return true;
   result.AppendLine("PID="+pid+" visible="+IsWindowVisible(w)+" title="+Text(w));
   bool serial=false,disk=false,illegalOpcode=false;
   Visit child=(c,x)=>{string text=Text(c);result.AppendLine("  id="+GetDlgCtrlID(c)+" "+text);
    for(int port=1;port<=4;port++)if(text.Contains("The system cannot open COM"+port+" port requested by the application."))serial=true;
    if(text.Contains("An application has attempted to directly access the hard disk, which cannot be supported. This may cause the application to function incorrectly."))disk=true;
    if(text.Contains("The NTVDM CPU has encountered an illegal instruction."))illegalOpcode=true;
    return true;};EnumChildWindows(w,child,IntPtr.Zero);
   if((ignoreSerial && serial) || (ignoreDisk && disk) || (ignoreIllegalOpcode && illegalOpcode)){IntPtr button=GetDlgItem(w,102),reply;
    if(button!=IntPtr.Zero && Text(button)=="&Ignore"){string action=serial ? "serial-ignore=" : (disk ? "denied-disk-ignore=" : "illegal-opcode-ignore=");result.AppendLine(action+(SendMessageTimeout(button,0xF5,IntPtr.Zero,IntPtr.Zero,2,1000,out reply)!=IntPtr.Zero));}}
   return true;};
  if(String.IsNullOrEmpty(desktop))EnumWindows(visit,IntPtr.Zero);
  else {IntPtr d=OpenDesktop(desktop,0,false,0x41);if(d==IntPtr.Zero)throw new System.ComponentModel.Win32Exception();
   try{EnumDesktopWindows(d,visit,IntPtr.Zero);}finally{CloseDesktop(d);}}
  return result.ToString();}
}
'@
Add-Type -Path $assembly
if(($IgnoreSerialWarnings -or $IgnoreDeniedDiskWarning -or $IgnoreIllegalOpcodeWarning) -and !$Desktop.StartsWith('NTVDMConsoleTest-')){throw 'Dialog action is restricted to the explicit private test desktop'}
$text=[WindowInspector]::Read($ProcessIds,$Desktop,$IgnoreSerialWarnings,$IgnoreDeniedDiskWarning,$IgnoreIllegalOpcodeWarning)
[IO.File]::AppendAllText("$root/windows.txt",[DateTime]::UtcNow.ToString('o')+"`r`n"+$text)
$text
