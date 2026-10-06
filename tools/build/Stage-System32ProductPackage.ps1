[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$BaselineRoot,
    [Parameter(Mandatory)][string]$BuildCache,
    [Parameter(Mandatory)][string]$Wow32,
    [Parameter(Mandatory)][string]$OutputRoot,
    [string]$NativeHook='',
    [string]$NativeHook64='',
    [string]$NativeFrontend='',
    [Parameter(Mandatory)][string]$NativeWorker
)
$ErrorActionPreference='Stop'
if($NativeHook64 -and !$NativeHook){throw 'Hook64 requires the matching Hook32 package input'}
$baseline=(Resolve-Path -LiteralPath $BaselineRoot).Path
$cache=(Resolve-Path -LiteralPath $BuildCache).Path
$wow=(Resolve-Path -LiteralPath $Wow32).Path
$output=[IO.Path]::GetFullPath($OutputRoot)
$build=(Resolve-Path -LiteralPath "$PSScriptRoot/../../build").Path+'\'
if(!$output.StartsWith($build,[StringComparison]::OrdinalIgnoreCase) -or
   (Test-Path -LiteralPath $output)){throw 'Require a fresh repository-build staging root'}
Copy-Item -LiteralPath $baseline -Destination $output -Recurse
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
$manifest=foreach($name in $names){
    $source=if($name -eq 'WOW32.DLL'){$wow}elseif($name -eq 'nthook32.dll'){$NativeHook}elseif($name -eq 'nthook64.dll'){$NativeHook64}elseif($name -eq 'ntvwm.exe' -and $NativeWorker){$NativeWorker}elseif($name -eq 'ntcon.exe' -and $NativeFrontend){$NativeFrontend}else{Join-Path $cache $name}
    $bytes=[IO.File]::ReadAllBytes($source)
    $pe=[BitConverter]::ToInt32($bytes,60)
    $expectedMachine=if($name -eq 'nthook64.dll' -or ($name -eq 'ntvwm.exe' -and $NativeWorker) -or ($name -eq 'ntcon.exe' -and $NativeFrontend)){0x8664}else{0x14c}
    if([BitConverter]::ToUInt16($bytes,$pe+4) -ne $expectedMachine){throw "Wrong PE machine input: $name"}
    $destination=Join-Path $system $name
    Copy-Item -LiteralPath $source -Destination $destination -Force
    $hash=(Get-FileHash -LiteralPath $source).Hash
    if((Get-FileHash -LiteralPath $destination).Hash -ne $hash){throw 'Staging hash mismatch'}
    # Exact redundant product files only, in the already checked fresh tree.
    $flat=Join-Path $output $name
    if(Test-Path -LiteralPath $flat){Remove-Item -LiteralPath $flat}
    [pscustomobject]@{Name=$name;RelativePath=('system32\'+$name);Sha256=$hash;Machine=$expectedMachine}
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
}
'PASS staged coherent system32 host package; explicitly selected native images verified; baseline media/configuration retained'
