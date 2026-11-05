param(
 [Parameter(Mandatory)][string]$PackageRoot,
 [Parameter(Mandatory)][string]$BuildRoot,
 [Parameter(Mandatory)][string]$Observer,
 [Parameter(Mandatory)][string]$CompilerDriver,
 [ValidateSet('normal','mismatch')][string]$Case='normal'
)
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Use a fresh repository-build evidence directory'}
$Observer=(Resolve-Path $Observer).Path
$CompilerDriver=(Resolve-Path $CompilerDriver).Path
$null=New-Item -ItemType Directory $root
$driver=$CompilerDriver
& nasm -f bin -o "$root/GENPROBE.EXE" "$PSScriptRoot/win16_generic_probe.asm"
if($LASTEXITCODE){throw 'NE build failed'}
$extra=if($Case -eq 'mismatch'){'/DTEST_GENERIC_CDECL'}else{'/DTEST_GENERIC_NORMAL'}
$export=if($Case -eq 'mismatch'){'/EXPORT:RunGenericProbe'}else{'/EXPORT:RunGenericProbe=_RunGenericProbe@12'}
& $driver cl /nologo /LD /O2 /MT $extra "/Fo$root/native.obj" "$PSScriptRoot/wow_generic_callback_probe.c" /link "/OUT:$root/GENHOST.DLL" "/IMPLIB:$root/native.lib" $export /EXPORT:ResolveGenericSegment=_ResolveGenericSegment@4 *> "$root/build.txt"
if($LASTEXITCODE){throw 'Native build failed'}
. "$repo/tests/observation/isolated_package_cleanup.ps1"
$runtime=New-PhysicalRuntimeStage $PackageRoot
$scope=New-IsolatedPackageScope $runtime
$saved=@{}
foreach($n in @('PATH','TEMP','TMP','MVDM_OBSERVER_PRIVATE_DESKTOP')){$saved[$n]=[Environment]::GetEnvironmentVariable($n)}
try {
 $null=New-Item -ItemType Directory "$runtime/tests" -Force
 Copy-Item "$root/GENPROBE.EXE","$root/GENHOST.DLL" "$runtime/tests"
 $env:PATH="$runtime/system32;"+$env:SystemRoot+'\System32'
 $env:TEMP=$runtime;$env:TMP=$runtime;$env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
 & $Observer "$runtime/system32/run16.exe" "$runtime/tests" "$root/observer.txt" --wait "$runtime/tests/GENPROBE.EXE" --observation-timeout-ms 30000
 if($LASTEXITCODE){throw 'Observer failed'}
 Copy-Item "$runtime/tests/GENPROBE.BIN" "$root/report.bin"
 $b=[IO.File]::ReadAllBytes("$root/report.bin")
 if($b.Length -ne 54){throw "Wrong record length $($b.Length)"}
 for($i=0;$i -lt 3;$i++) {
  $o=$i*18
  if([Text.Encoding]::ASCII.GetString($b,$o,4) -ne 'GEN1' -or [BitConverter]::ToUInt16($b,$o+4) -ne $i -or [BitConverter]::ToUInt16($b,$o+6) -ne 0){throw 'Guest report failed'}
 }
 if([BitConverter]::ToUInt16($b,36+14) -ne 3){throw 'Resource retirement failed'}
 for($i=1;$i -lt 3;$i++) {
  $o=$i*18
  if([BitConverter]::ToUInt16($b,$o+8) -ne 3 -or [BitConverter]::ToUInt16($b,$o+10) -ne 1 -or [BitConverter]::ToUInt16($b,$o+12) -ne 1){throw 'Nested callback counters failed'}
 }
 'PASS three guest phases, zero errors, three releases' | Set-Content "$root/result.txt"
} finally {
 try {Stop-IsolatedPackageScope $scope;Remove-PhysicalRuntimeStage $runtime}
 finally {foreach($n in $saved.Keys){[Environment]::SetEnvironmentVariable($n,$saved[$n])}}
}


