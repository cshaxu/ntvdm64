param([Parameter(Mandatory)][string]$OutputPath)
$ErrorActionPreference='Stop'
Add-Type @"
using System;
using System.Runtime.InteropServices;
public static class TitleWitness {
 [DllImport("kernel32.dll",SetLastError=true)] public static extern bool FreeConsole();
 [DllImport("kernel32.dll",SetLastError=true)] public static extern bool AllocConsole();
 [DllImport("kernel32.dll",CharSet=CharSet.Ansi,SetLastError=true)] public static extern bool SetConsoleTitleA(string title);
 [DllImport("kernel32.dll",SetLastError=true)] public static extern uint GetConsoleTitleA([Out] byte[] bytes,uint size);
 [DllImport("kernel32.dll")] public static extern void SetLastError(uint error);
}
"@
# This is a hidden, test-owned process. Never query or alter the caller's title.
[void][TitleWitness]::FreeConsole()
if(![TitleWitness]::AllocConsole()){throw 'Owned Console allocation failed'}
try {
 $rows=@(foreach($title in @('TITLE-TRANSPORT','','AB')) {
  if(![TitleWitness]::SetConsoleTitleA($title)){throw 'Set title failed'}
  foreach($capacity in @(0,1,4,32)) {
   [byte[]]$bytes=New-Object byte[] 32
   [Array]::Fill[byte]($bytes,126)
   [TitleWitness]::SetLastError(0)
   $count=[TitleWitness]::GetConsoleTitleA($bytes,$capacity)
   $errorCode=[Runtime.InteropServices.Marshal]::GetLastWin32Error()
   [ordered]@{title=$title;capacity=$capacity;count=$count;error=$errorCode;bytes=[Convert]::ToHexString($bytes)}
  }
 })
 $rows|ConvertTo-Json|Set-Content -LiteralPath $OutputPath
} finally {[void][TitleWitness]::FreeConsole()}
