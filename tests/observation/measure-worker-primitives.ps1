param(
    [Parameter(Mandatory)][string]$BuildRoot,
    [Parameter(Mandatory)][string]$CompilerWrapper,
    [switch]$HighResolutionPublisher
)

$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)) {
    throw 'Require a fresh build-only evidence directory'
}
$wrapper=(Resolve-Path $CompilerWrapper).Path
$null=New-Item -ItemType Directory -Path $root
$publisher=if($HighResolutionPublisher){'tests/observation/publication_high_resolution_measured.c'}else{'src/worker-base/publication.c'}
$sources=@($publisher,
    'src/ntvdm-exe/softpc/mvdm_softpc_mouse_input.c',
    'tests/observation/measure-worker-primitives.c')
$objects=@()
foreach($source in $sources) {
    $object=Join-Path $root (([IO.Path]::GetFileNameWithoutExtension($source))+'.obj')
    & $wrapper cl.exe /nologo /MT /W4 /WX /O2 /TC "/I$repo/src" /c (Join-Path $repo $source) "/Fo$object" *>&1 |
        Set-Content (Join-Path $root ((Split-Path $object -Leaf)+'.log'))
    if($LASTEXITCODE){throw "Compilation failed: $source"}
    $objects+=$object
}
$exe=Join-Path $root 'worker-primitives.exe'
& $wrapper link.exe /nologo "/out:$exe" @objects kernel32.lib *>&1 | Set-Content "$root/link.log"
if($LASTEXITCODE){throw 'Link failed'}
& $exe *>&1 | Set-Content "$root/results.txt"
if($LASTEXITCODE){throw 'Worker primitive measurement failed'}
Get-Content "$root/results.txt"
