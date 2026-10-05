param([Parameter(Mandatory)][string]$BuildRoot,
      [Parameter(Mandatory)][string]$CompilerWrapper)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){
    throw 'Require a fresh build-only evidence directory'
}
$wrapper=(Resolve-Path $CompilerWrapper).Path
$null=New-Item -ItemType Directory -Path $root
$sources=@('src/worker-base/publication.c',
 'tests/component-integration/software_video_publisher_test.c')
$objects=@()
foreach($source in $sources){
 $object=Join-Path $root (([IO.Path]::GetFileNameWithoutExtension($source))+'.obj')
 & $wrapper cl.exe /nologo /MT /W4 /WX /O2 /TC "/I$repo/src" /c (Join-Path $repo $source) "/Fo$object" *> (Join-Path $root ((Split-Path $object -Leaf)+'.log'))
 if($LASTEXITCODE){throw "Compilation failed: $source"}
 $objects+=$object
}
$exe=Join-Path $root 'publisher-test.exe'
& $wrapper link.exe /nologo "/out:$exe" @objects kernel32.lib *> "$root/link.log"
if($LASTEXITCODE){throw 'Link failed'}
& $exe *> "$root/results.txt"
if($LASTEXITCODE){throw 'Production publisher fixture failed'}
Get-Content "$root/results.txt"
