param(
    [Parameter(Mandatory)][string]$BuildRoot,
    [Parameter(Mandatory)][string]$CompilerWrapper
)

$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)) {
    throw 'Require a fresh build-only evidence directory'
}
$wrapper=(Resolve-Path $CompilerWrapper).Path
$null=New-Item -ItemType Directory -Path $root
$sources=@('src/ntcon-exe/lib/kvm-window/render.c','tests/observation/measure-window-render.c')
$objects=@()
foreach($source in $sources) {
    $object=Join-Path $root (([IO.Path]::GetFileNameWithoutExtension($source))+'.obj')
    & $wrapper cl.exe /nologo /MT /W4 /WX /O2 /std:c11 /D_CRT_SECURE_NO_WARNINGS /TC "/I$repo/src/ntcon-exe" "/I$repo/src" /c (Join-Path $repo $source) "/Fo$object" *>&1 |
        Set-Content (Join-Path $root ((Split-Path $object -Leaf)+'.log'))
    if($LASTEXITCODE){throw "Compilation failed: $source"}
    $objects+=$object
}
$exe=Join-Path $root 'window-render.exe'
& $wrapper link.exe /nologo "/out:$exe" @objects *>&1 | Set-Content "$root/link.log"
if($LASTEXITCODE){throw 'Link failed'}
& $exe *>&1 | Set-Content "$root/results.txt"
if($LASTEXITCODE){throw 'Window render measurement failed'}
Get-Content "$root/results.txt"
