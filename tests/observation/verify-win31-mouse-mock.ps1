param([Parameter(Mandatory)][string]$DriverBuild,
      [Parameter(Mandatory)][string]$PackageRoot,
      [Parameter(Mandatory)][string]$Observer,
      [Parameter(Mandatory)][string]$BuildRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$driver=(Resolve-Path -LiteralPath $DriverBuild).Path
if(!$driver.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase)) {
    throw 'Build-owned test artifact required'
}
$package=(Resolve-Path -LiteralPath $PackageRoot).Path
$tests=Join-Path $package 'tests'
if(!(Test-Path -LiteralPath $tests -PathType Container)){throw 'Existing package tests directory required'}
$fixture=Join-Path $tests 'M31TEST.COM'
Copy-Item -LiteralPath "$driver/M31TEST.COM" -Destination $fixture -Force
& "$PSScriptRoot/observe-win31-enhanced.ps1" -Observer $Observer -PackageRoot $package `
    -Pif $fixture -BuildRoot $BuildRoot -ObservationTimeoutMs 10000 |Out-Null
$report=Get-Content "$BuildRoot/observation.txt" -Raw
$screen=Get-Content "$BuildRoot/observation.txt.console.txt" -Raw
if($report -notmatch '(?m)^result=exited\r?$' -or $report -notmatch '(?m)^exit=0x00000000\r?$' -or
   $screen -notmatch 'M31MOCK:PASS 50 checks') {
    throw 'Guest mock requires real completion, zero status and this occurrence of the complete pass marker'
}
[ordered]@{role='CCPU40-guest-mock-not-real-DPMI-or-Windows-pass';
    fixtureSha256=(Get-FileHash $fixture).Hash;checks=50;completed=$true;
    exitCode=0;source='tests/observation/win31_mouse_driver_guest.asm'} |
    ConvertTo-Json|Set-Content "$BuildRoot/mock-verdict.json"
'PASS real x86 CCPU40 execution of 50 driver/mock assertions, output and completion'
