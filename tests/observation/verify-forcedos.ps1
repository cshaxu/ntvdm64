param(
 [Parameter(Mandatory=$true)][string]$PackageRoot,
 [Parameter(Mandatory=$true)][string]$BuildRoot,
 [Parameter(Mandatory=$true)][string]$Observer,
 [Parameter(Mandatory=$true)][string]$NativeParent,
 [Parameter(Mandatory=$true)][string]$GenericHostDll,
 [ValidateSet('forced','normal','nested','native','sequence')][string]$Case='forced',
 [switch]$Ansi
)
$ErrorActionPreference='Stop'
$r=(Resolve-Path "$PSScriptRoot/../..").Path
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($r+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Use a fresh repository-build evidence directory'}
$Observer=(Resolve-Path $Observer).Path
$NativeParent=(Resolve-Path $NativeParent).Path
$GenericHostDll=(Resolve-Path $GenericHostDll).Path
$null=New-Item -ItemType Directory $root
$defs=@('-DFORCEDOS_STUB');if($Case.Contains('nested')){$defs+='-DFORCEDOS_PARENT'}
& nasm -f bin @defs -o "$root/DUAL.EXE" "$PSScriptRoot/win16_generic_probe.asm"
if($LASTEXITCODE){throw 'Assembly failed'}
if($Case.Contains('nested')) { & nasm -f bin -DFORCEDOS_STUB -o "$root/FCHILD.EXE" "$PSScriptRoot/win16_generic_probe.asm" }
if($LASTEXITCODE){throw 'Assembly failed'}
. "$r/tests/observation/isolated_package_cleanup.ps1"
$runtime=New-PhysicalRuntimeStage $PackageRoot
$scope=New-IsolatedPackageScope $runtime
$saved=@{};foreach($n in @('PATH','TEMP','TMP','MVDM_OBSERVER_PRIVATE_DESKTOP')){$saved[$n]=[Environment]::GetEnvironmentVariable($n)}
try {
 $null=New-Item -ItemType Directory "$runtime/tests" -Force
 Copy-Item "$root/DUAL.EXE" "$runtime/tests"
 if($Case.Contains("nested")){Copy-Item "$root/FCHILD.EXE" "$runtime/tests"}
 Copy-Item $GenericHostDll "$runtime/tests/GENHOST.DLL"
 Copy-Item $NativeParent "$runtime/tests/PARENT.EXE"
 $target=if($Case.Contains("native")){"$runtime/tests/PARENT.EXE"}else{"$runtime/tests/DUAL.EXE"}
 $flags=if($Case -ne 'normal'){'0x2000'}else{'0'}
 $env:PATH="$runtime/system32;"+$env:SystemRoot+'\System32';$env:TEMP=$runtime;$env:TMP=$runtime;$env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
 $api=if($Case -eq 'sequence'){'sequence'}elseif($Ansi){'ansi'}else{'unicode'}
 & $Observer "$runtime/system32/run16.exe" "$runtime/tests" "$root/observer.txt" --wait "$runtime/tests/PARENT.EXE" $target $flags "$runtime/tests/output.txt" $api --observation-timeout-ms 30000
 if($LASTEXITCODE){throw 'Observer failed'}
 Copy-Item "$runtime/tests/output.txt" "$root/output.txt"
 $report=Get-Content "$root/observer.txt" -Raw
 if($Case.Contains('native')) {
  if($report -notmatch 'exit=0x0000005a'){throw 'Native PE route changed'}
 } elseif($flags -ne '0' -and $Case -ne 'sequence') {
  $expected=if($Case.Contains('nested')){'exit=0x00000037'}else{'exit=0x00000035'}
  if($report -notmatch $expected -or (Get-Content "$root/output.txt" -Raw) -notmatch 'FORCEDOS_STUB_MARKER'){throw 'Forced DOS result mismatch'}
  $markers=([regex]::Matches((Get-Content "$root/output.txt" -Raw),'FORCEDOS_STUB_MARKER')).Count
  if($markers -ne $(if($Case -eq 'nested'){2}else{1})){throw 'DOS marker count mismatch'}
  if(Test-Path "$runtime/tests/GENPROBE.BIN"){throw 'Unexpected Win16 execution'}
 } else {
  if($Case -eq 'sequence' -and (Get-Content "$root/output.txt" -Raw) -notmatch 'FORCEDOS_STUB_MARKER'){throw 'First forced request did not run DOS'}
  $limit=[DateTime]::UtcNow.AddSeconds(5)
  while(!(Test-Path "$runtime/tests/GENPROBE.BIN") -and [DateTime]::UtcNow -lt $limit){Start-Sleep -Milliseconds 100}
  Copy-Item "$runtime/tests/GENPROBE.BIN" "$root/guest.bin"
  $b=[IO.File]::ReadAllBytes("$root/guest.bin")
  if($b.Length -ne 54 -or $report -notmatch 'exit=0x00000000'){throw 'Ordinary WOW did not complete'}
  for($i=0;$i -lt 3;$i++) {
   $o=$i*18
   if([Text.Encoding]::ASCII.GetString($b,$o,4) -ne 'GEN1' -or
      [BitConverter]::ToUInt16($b,$o+4) -ne $i -or
      [BitConverter]::ToUInt16($b,$o+6) -ne 0){throw 'Ordinary WOW callback report failed'}
  }
 }
 'PASS '+$Case | Set-Content "$root/result.txt"
} finally {
 try {Stop-IsolatedPackageScope $scope;Remove-PhysicalRuntimeStage $runtime}
 finally {foreach($n in $saved.Keys){[Environment]::SetEnvironmentVariable($n,$saved[$n])}}
}
