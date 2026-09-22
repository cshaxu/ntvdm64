[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)] [string]$BuildRoot,
    [string]$RuntimeRoot = 'O:\winnt',
    [string]$Nasm = 'C:\Users\neko\AppData\Local\Microsoft\WinGet\Packages\BrechtSanders.WinLibs.POSIX.MSVCRT_Microsoft.Winget.Source_8wekyb3d8bbwe\mingw64\bin\nasm.exe'
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$build = (Resolve-Path -LiteralPath $BuildRoot).Path
$runtime = (Resolve-Path -LiteralPath $RuntimeRoot).Path
$source = Join-Path $PSScriptRoot 'dpmi_vcd_serial_ports.asm'
$probe = Join-Path $build 'S45VCD.COM'
$trace = Join-Path $runtime 'logs\s45-vcd-serial-discovery.log'
$paths = @('run16.exe', 'basesrv.exe', 'ntvdm.exe') | ForEach-Object {
    Join-Path $runtime $_
}

if (!(Test-Path -LiteralPath $Nasm)) { throw "NASM not found: $Nasm" }
if (@(Get-Process ntvdm,basesrv,run16 -ErrorAction SilentlyContinue).Count) {
    throw 'S45 VCD test requires no existing run16/basesrv/ntvdm session.'
}

if (-not ('S45QueryDosDevice' -as [type])) {
    Add-Type @'
using System;
using System.Runtime.InteropServices;
public static class S45QueryDosDevice {
  [DllImport("kernel32.dll", EntryPoint="QueryDosDeviceA", SetLastError=true)]
  public static extern UInt32 QueryDosDevice(
      IntPtr deviceName, [Out] byte[] targetPath, UInt32 maxChars);
  public static UInt32 GetSerialMask() {
    byte[] names = null;
    UInt32 length = 0;
    for (int size = 32768; size <= 1048576; size *= 2) {
      names = new byte[size];
      length = QueryDosDevice(IntPtr.Zero, names, (UInt32)names.Length);
      if (length != 0) break;
      int error = Marshal.GetLastWin32Error();
      if (error != 122) throw new System.ComponentModel.Win32Exception(error);
    }
    if (length == 0) throw new System.ComponentModel.Win32Exception(122);
    UInt32 mask = 0;
    for (int start = 0; start < length && names[start] != 0; ) {
      int end = start;
      while (end < length && names[end] != 0) ++end;
      string name = System.Text.Encoding.ASCII.GetString(names, start, end - start);
      if (name.Length == 4 && name.StartsWith("COM", StringComparison.OrdinalIgnoreCase) &&
          name[3] >= '1' && name[3] <= '9') mask |= 1u << (name[3] - '1');
      start = end + 1;
    }
    return mask;
  }
}
'@
}
$mask = [S45QueryDosDevice]::GetSerialMask()

$nasmArguments = @('-f', 'bin', ('-DEXPECT_PORT_MASK=' + $mask), $source, '-o', $probe)
& $Nasm @nasmArguments
if ($LASTEXITCODE -ne 0) { throw 'NASM could not build the S45 VCD probe.' }
New-Item -ItemType Directory -Force -Path (Join-Path $runtime 'tests'),(Join-Path $runtime 'logs') | Out-Null
foreach ($name in @('run16.exe','basesrv.exe','ntvdm.exe','dtmgr.exe')) {
    Copy-Item -LiteralPath (Join-Path $build $name) -Destination (Join-Path $runtime $name) -Force
}
Copy-Item -LiteralPath $probe -Destination (Join-Path $runtime 'tests\S45VCD.COM') -Force
Remove-Item -LiteralPath $trace -Force -ErrorAction SilentlyContinue

try {
    $env:NTVDM_SHADOW_REGISTRY_TRACE = $trace
    foreach ($arguments in @(@('tests\S45VCD.COM'), @('COMMAND.COM','/c','COMMAND.COM','/c','tests\S45VCD.COM'))) {
        $process = Start-Process -FilePath $paths[0] -ArgumentList $arguments -WorkingDirectory $runtime `
            -WindowStyle Hidden -PassThru
        if (!$process.WaitForExit(30000)) {
            Stop-Process -Id $process.Id -Force
            throw "VCD probe timed out: $($arguments -join ' ')"
        }
        if ($process.ExitCode -ne 0) {
            throw "VCD port array mismatch: expected 0x{0:X}, exit={1}, route={2}" -f $mask,$process.ExitCode,($arguments -join ' ')
        }
    }
    $markers = @(Get-Content -LiteralPath $trace -ErrorAction Stop)
    foreach ($marker in @('SERIAL_DISCOVERY_OK','SERIAL_ENUM_OK','CLOSE')) {
        if ($markers -notcontains $marker) { throw "VCD trace lacks $marker" }
    }
    Write-Output ('S45_VCD_PUBLIC_SERIAL_DISCOVERY_OK mask=0x{0:X}' -f $mask)
}
finally {
    Get-Process ntvdm,basesrv,run16 -ErrorAction SilentlyContinue | Stop-Process -Force
    Remove-Item -LiteralPath (Join-Path $runtime 'tests\S45VCD.COM') -Force -ErrorAction SilentlyContinue
}
