[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$BaselineRoot,
    [Parameter(Mandatory)][string]$BuildCache,
    [Parameter(Mandatory)][string]$Wow32,
    [Parameter(Mandatory)][string]$OutputRoot,
    [string]$NativeHook='',
    [string]$NativeWorker32='',
    [string]$NativeWorker64='',
    [string]$NativeHook64=''
)
$ErrorActionPreference='Stop'
$baseline=(Resolve-Path -LiteralPath $BaselineRoot).Path
$cache=(Resolve-Path -LiteralPath $BuildCache).Path
$wow=(Resolve-Path -LiteralPath $Wow32).Path
$output=[IO.Path]::GetFullPath($OutputRoot)
$build=(Resolve-Path -LiteralPath "$PSScriptRoot/../../build").Path+'\'
if(!$output.StartsWith($build,[StringComparison]::OrdinalIgnoreCase) -or
   (Test-Path -LiteralPath $output)){throw 'Require a fresh repository-build staging root'}
$dual=!!($NativeWorker32 -or $NativeWorker64 -or $NativeHook64)
if($dual -and (!$NativeWorker32 -or !$NativeWorker64 -or !$NativeHook -or !$NativeHook64)){
    throw 'Dual-width package requires both workers and both hooks'
}
$names=@('run16.exe','ntsrv.exe','ntcon.exe','ntvdm.exe','ntvwm.exe','ntmon.exe','WOW32.DLL','VDMREDIR.DLL')
if($dual){$names=@('run16.exe','ntsrv.exe','ntcon.exe','ntvdm.exe','ntvwm32.exe','ntvwm64.exe','ntmon.exe','WOW32.DLL','VDMREDIR.DLL','nthook64.dll')}
if($NativeHook){$NativeHook=(Resolve-Path -LiteralPath $NativeHook).Path;$names+='nthook32.dll'}
# Validate the complete source set before creating any candidate tree. PE
# checks here validate build artifacts, not runtime application discovery.
$inputs=foreach($name in $names){
    $source=switch($name){
        'WOW32.DLL' {$wow;break}
        'nthook32.dll' {$NativeHook;break}
        'nthook64.dll' {$NativeHook64;break}
        'ntvwm32.exe' {$NativeWorker32;break}
        'ntvwm64.exe' {$NativeWorker64;break}
        default {Join-Path $cache $name}
    }
    $source=(Resolve-Path -LiteralPath $source).Path
    $bytes=[IO.File]::ReadAllBytes($source)
    if($bytes.Length -lt 64 -or [BitConverter]::ToUInt16($bytes,0) -ne 0x5a4d){throw "Invalid PE input: $name"}
    $pe=[BitConverter]::ToInt32($bytes,60)
    if($pe -lt 64 -or $pe -gt $bytes.Length-24 -or [BitConverter]::ToUInt32($bytes,$pe) -ne 0x4550){throw "Invalid PE input: $name"}
    $machine=if($name -in @('ntvwm64.exe','nthook64.dll')){0x8664}else{0x14c}
    if([BitConverter]::ToUInt16($bytes,$pe+4) -ne $machine){throw "Wrong machine input: $name"}
    $dll=([BitConverter]::ToUInt16($bytes,$pe+22) -band 0x2000) -ne 0
    if($dll -ne $name.EndsWith('.dll',[StringComparison]::OrdinalIgnoreCase)){throw "Wrong image type: $name"}
    [pscustomobject]@{Name=$name;Source=$source;Machine=$machine;Sha256=(Get-FileHash -LiteralPath $source).Hash}
}
if(!(Test-Path -LiteralPath (Join-Path $baseline 'system32') -PathType Container)){throw 'Baseline lacks original system32 media'}
Copy-Item -LiteralPath $baseline -Destination $output -Recurse
$system=Join-Path $output 'system32'
$manifest=foreach($artifact in $inputs){
    $name=$artifact.Name;$source=$artifact.Source
    $destination=Join-Path $system $name
    Copy-Item -LiteralPath $source -Destination $destination -Force
    $hash=$artifact.Sha256
    if((Get-FileHash -LiteralPath $destination).Hash -ne $hash){throw 'Staging hash mismatch'}
    # Exact redundant product files only, in the already checked fresh tree.
    $flat=Join-Path $output $name
    if(Test-Path -LiteralPath $flat){Remove-Item -LiteralPath $flat}
    [pscustomobject]@{Name=$name;RelativePath=('system32\'+$name);Machine=$artifact.Machine;Sha256=$hash}
}
if($dual){
    # Remove only the obsolete product name inside the validated fresh tree.
    foreach($obsolete in @((Join-Path $system 'ntvwm.exe'),(Join-Path $output 'ntvwm.exe'))){
        if(Test-Path -LiteralPath $obsolete){Remove-Item -LiteralPath $obsolete}
    }
}
$flatRegistry=Join-Path $output 'NTVDM.REG'
$registry=Join-Path $system 'NTVDM.REG'
if(Test-Path -LiteralPath $flatRegistry){
    if(Test-Path -LiteralPath $registry){
        if((Get-FileHash $flatRegistry).Hash -ne (Get-FileHash $registry).Hash){throw 'Conflicting Registry overlays'}
        Remove-Item -LiteralPath $flatRegistry
    }else{Move-Item -LiteralPath $flatRegistry -Destination $registry}
}
$manifest|ConvertTo-Json|Set-Content -LiteralPath (Join-Path $output 'product-system32-manifest.json')
if($NativeHook){
    Copy-Item -LiteralPath "$PSScriptRoot/../../src/nthook32-dll/detours/LICENSE.md" -Destination (Join-Path $system 'nthook32-LICENSE.txt')
    if($dual){Copy-Item -LiteralPath "$PSScriptRoot/../../src/nthook32-dll/detours/LICENSE.md" -Destination (Join-Path $system 'nthook64-LICENSE.txt')}
}
if($dual){'PASS staged coherent eleven-image dual-width system32 package; baseline media/configuration retained'}
else{'PASS staged coherent x86 system32 host package; baseline media/configuration retained'}
