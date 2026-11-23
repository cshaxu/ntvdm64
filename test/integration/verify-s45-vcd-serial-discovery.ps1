[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)] [string]$BuildRoot,
    [string]$RuntimeRoot = 'O:\winnt'
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$build = (Resolve-Path -LiteralPath $BuildRoot).Path
$runtime = (Resolve-Path -LiteralPath $RuntimeRoot).Path
$repository = (Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$probe = Join-Path $build 'S45VCD.COM'
$trace = Join-Path $runtime 'logs\s45-vcd-serial-discovery.log'
$paths = @('run16.exe', 'ntsrv.exe', 'ntvdm.exe') | ForEach-Object {
    Join-Path $runtime $_
}

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

$producer=Start-Process -FilePath $env:ComSpec -ArgumentList ('/d /s /c ""'+(Join-Path $repository 'tools/build-fixtures.cmd')+'" x86 serial-discovery '+$mask+'"') -WorkingDirectory $repository -WindowStyle Hidden -PassThru -RedirectStandardOutput (Join-Path $build 'serial-build.log') -RedirectStandardError (Join-Path $build 'serial-build.stderr.log')
try {
    "pid=$($producer.Id) started=$($producer.StartTime.ToUniversalTime().ToString('o')) mask=$mask" | Set-Content (Join-Path $build 'serial-producer.txt')
    if(!$producer.WaitForExit(300000)){
        $producer.Kill($true);$producer.WaitForExit()
        throw 'Serial producer timed out'
    }
    "exit=$($producer.ExitCode)" | Add-Content (Join-Path $build 'serial-producer.txt')
    if($producer.ExitCode){throw 'Serial producer failed'}
} finally {$producer.Dispose()}
Copy-Item (Join-Path $repository 'build/.ninja/fixtures-x86/guest-probes/serial-discovery/S45VCD.COM') $probe
New-Item -ItemType Directory -Force -Path (Join-Path $runtime 'tests'),(Join-Path $runtime 'logs') | Out-Null
foreach ($name in @('run16.exe','ntsrv.exe','ntvdm.exe','ntmon.exe')) {
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
