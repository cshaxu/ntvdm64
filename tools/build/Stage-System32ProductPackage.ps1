[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$BaselineRoot,
    [Parameter(Mandatory)][string]$BuildCache,
    [Parameter(Mandatory)][string]$Wow32,
    [Parameter(Mandatory)][string]$OutputRoot,
    [string]$NativeHook='',
    [string]$NativeHook64='',
    [string]$NativeFrontend='',
    [string]$NativeMonitor='',
    [string]$NativeLauncher='',
    [string]$NativeService,
    [Parameter(Mandatory)][string]$NativeWorker,
    [guid]$PackageId = [guid]::NewGuid()
)
$ErrorActionPreference='Stop'
function Get-StageFileSha256([string]$LiteralPath) {
    $path=(Resolve-Path -LiteralPath $LiteralPath -ErrorAction Stop).Path
    $algorithm=[Security.Cryptography.SHA256]::Create()
    $stream=[IO.File]::OpenRead($path)
    try {
        return ([BitConverter]::ToString($algorithm.ComputeHash($stream))).Replace('-','')
    } finally {
        $stream.Dispose()
        $algorithm.Dispose()
    }
}
if($NativeHook64 -and !$NativeHook){throw 'Hook64 requires the matching Hook32 package input'}
$baseline=(Resolve-Path -LiteralPath $BaselineRoot).Path
$cache=(Resolve-Path -LiteralPath $BuildCache).Path
$wow=(Resolve-Path -LiteralPath $Wow32).Path
$output=[IO.Path]::GetFullPath($OutputRoot)
if($PackageId -eq [guid]::Empty){throw 'Package identity must not be empty'}
$build=(Resolve-Path -LiteralPath "$PSScriptRoot/../../build").Path+'\'
if(!$output.StartsWith($build,[StringComparison]::OrdinalIgnoreCase) -or
   (Test-Path -LiteralPath $output)){throw 'Require a fresh repository-build staging root'}
$manifestPath=Join-Path $output 'product-system32-manifest.json'
$manifestTemporary=Join-Path $output 'product-system32-manifest.json.pending'
$stageComplete=$false
try {
Copy-Item -LiteralPath $baseline -Destination $output -Recurse
# A copied baseline manifest describes different bytes and cannot certify this
# candidate. A failed partial copy is handled by the finally block below.
if(Test-Path -LiteralPath $manifestPath){Remove-Item -LiteralPath $manifestPath -Force}
if(Test-Path -LiteralPath $manifestTemporary){Remove-Item -LiteralPath $manifestTemporary -Force}
$system=Join-Path $output 'system32'
if(!(Test-Path -LiteralPath $system -PathType Container)){throw 'Baseline lacks original system32 media'}
$names=@('run16.exe','ntsrv.exe','ntcon.exe','ntvdm.exe','ntvwm.exe','ntmon.exe','WOW32.DLL','VDMREDIR.DLL')
if($NativeHook){$NativeHook=(Resolve-Path -LiteralPath $NativeHook).Path;$names+='nthook32.dll'}
if($NativeHook64){$NativeHook64=(Resolve-Path -LiteralPath $NativeHook64).Path;$names+='nthook64.dll'}
if($NativeWorker){
    $NativeWorker=(Resolve-Path -LiteralPath $NativeWorker).Path
    if(!$NativeHook64 -or !$NativeWorker.StartsWith($build,[StringComparison]::OrdinalIgnoreCase)){throw 'Native worker requires both Hooks and a build-owned input'}
}
if($NativeFrontend){
    $NativeFrontend=(Resolve-Path -LiteralPath $NativeFrontend).Path
    if(!$NativeWorker -or !$NativeFrontend.StartsWith($build,[StringComparison]::OrdinalIgnoreCase)){throw 'Native frontend requires the native worker and a build-owned input'}
}
if($NativeMonitor){
    $NativeMonitor=(Resolve-Path -LiteralPath $NativeMonitor).Path
    if(!$NativeFrontend -or !$NativeMonitor.StartsWith($build,[StringComparison]::OrdinalIgnoreCase)){throw 'Native monitor requires the native frontend and a build-owned input'}
}
if($NativeLauncher){
    $NativeLauncher=(Resolve-Path -LiteralPath $NativeLauncher).Path
    if(!$NativeMonitor -or !$NativeLauncher.StartsWith($build,[StringComparison]::OrdinalIgnoreCase)){throw 'Native launcher requires the native monitor and a build-owned input'}
}
if($NativeService){
    $NativeService=(Resolve-Path -LiteralPath $NativeService).Path
    if(!$NativeLauncher -or !$NativeService.StartsWith($build,[StringComparison]::OrdinalIgnoreCase)){throw 'Native service requires the native launcher and a build-owned input'}
}
$manifest=foreach($name in $names){
    $source=if($name -eq 'WOW32.DLL'){$wow}elseif($name -eq 'nthook32.dll'){$NativeHook}elseif($name -eq 'nthook64.dll'){$NativeHook64}elseif($name -eq 'ntvwm.exe' -and $NativeWorker){$NativeWorker}elseif($name -eq 'ntcon.exe' -and $NativeFrontend){$NativeFrontend}elseif($name -eq 'ntmon.exe' -and $NativeMonitor){$NativeMonitor}elseif($name -eq 'run16.exe' -and $NativeLauncher){$NativeLauncher}elseif($name -eq 'ntsrv.exe' -and $NativeService){$NativeService}else{Join-Path $cache $name}
    try {$bytes=[IO.File]::ReadAllBytes($source)}
    catch {throw "Cannot read package input '$source': $($_.Exception.Message)"}
    $pe=[BitConverter]::ToInt32($bytes,60)
    $expectedMachine=if($name -eq 'nthook64.dll' -or ($name -eq 'ntvwm.exe' -and $NativeWorker) -or ($name -eq 'ntcon.exe' -and $NativeFrontend) -or ($name -eq 'ntmon.exe' -and $NativeMonitor) -or ($name -eq 'run16.exe' -and $NativeLauncher) -or ($name -eq 'ntsrv.exe' -and $NativeService)){0x8664}else{0x14c}
    if([BitConverter]::ToUInt16($bytes,$pe+4) -ne $expectedMachine){throw "Wrong PE machine input: $name"}
    $destination=Join-Path $system $name
    Copy-Item -LiteralPath $source -Destination $destination -Force
    $sourceHash=Get-StageFileSha256 $source
    if((Get-StageFileSha256 $destination) -ne $sourceHash){throw 'Staging copy hash mismatch'}
    & "$PSScriptRoot/Set-PackageIdentity.ps1" -Image $destination -PackageId $PackageId
    $hash=Get-StageFileSha256 $destination
    # Exact redundant product files only, in the already checked fresh tree.
    $flat=Join-Path $output $name
    if(Test-Path -LiteralPath $flat){Remove-Item -LiteralPath $flat}
    [pscustomobject]@{Name=$name;RelativePath=('system32\'+$name);Sha256=$hash;Machine=$expectedMachine;PackageId=$PackageId.ToString();InputSha256=$sourceHash}
}
$flatRegistry=Join-Path $output 'NTVDM.REG'
$registry=Join-Path $system 'NTVDM.REG'
if(Test-Path -LiteralPath $flatRegistry){
    if(Test-Path -LiteralPath $registry){
        if((Get-StageFileSha256 $flatRegistry) -ne (Get-StageFileSha256 $registry)){throw 'Conflicting Registry overlays'}
        Remove-Item -LiteralPath $flatRegistry
    }else{Move-Item -LiteralPath $flatRegistry -Destination $registry}
}
if($NativeHook){
    Copy-Item -LiteralPath "$PSScriptRoot/../../src/nthook32-dll/detours/LICENSE.md" -Destination (Join-Path $system 'nthook32-LICENSE.txt')
}
# Publish the completion manifest only after every required staging operation.
$manifest|ConvertTo-Json|Set-Content -LiteralPath $manifestTemporary
Move-Item -LiteralPath $manifestTemporary -Destination $manifestPath
$stageComplete=$true
'PASS staged coherent system32 host package; explicitly selected native images verified; baseline media/configuration retained'
} finally {
    if(!$stageComplete){
        # Only these exact generated files in the validated fresh output are
        # removed; preserve all other partial output for diagnosis.
        if(Test-Path -LiteralPath $manifestPath){Remove-Item -LiteralPath $manifestPath -Force}
        if(Test-Path -LiteralPath $manifestTemporary){Remove-Item -LiteralPath $manifestTemporary -Force}
    }
}
