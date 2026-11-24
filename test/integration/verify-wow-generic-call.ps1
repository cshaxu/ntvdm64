param(
 [Parameter(Mandatory)][string]$PackageRoot,
 [Parameter(Mandatory)][string]$BuildRoot,
 [Parameter(Mandatory)][string]$Observer,
 [ValidateSet('normal','mismatch')][string]$Case='normal'
)
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Use a fresh repository-build evidence directory'}
$Observer=(Resolve-Path $Observer).Path
$null=New-Item -ItemType Directory $root
& "$repo/tools/build-fixtures.cmd" x86 "wow-generic-$Case" *> "$root/build.txt"
if($LASTEXITCODE){throw 'Generic probe native build failed'}
$cache="$repo/build/.ninja/fixtures-x86/guest-probes"
& "$repo/tools/prepare-test-inputs.cmd" $root "$cache/mine/generic.exe" GENPROBE.EXE "$cache/generic-$Case/GENHOST.DLL" GENHOST.DLL
if($LASTEXITCODE){throw 'Generic input preparation failed'}
. "$repo/test/observation/isolated_package_cleanup.ps1"
$runtime=New-PhysicalRuntimeStage $PackageRoot -TestFiles @("$root/GENPROBE.EXE","$root/GENHOST.DLL")
$scope=New-IsolatedPackageScope $runtime
$saved=@{}
foreach($n in @('PATH','TEMP','TMP','MVDM_OBSERVER_PRIVATE_DESKTOP')){$saved[$n]=[Environment]::GetEnvironmentVariable($n)}
try {
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
