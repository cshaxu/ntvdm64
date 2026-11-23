param([Parameter(Mandatory)][string]$BuildRoot,
      [ValidateSet('x86','x64')][string]$Architecture='x86')
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){
    throw 'Require a fresh build-only evidence directory'
}
$null=New-Item -ItemType Directory -Path $root
& "$repo/tools/build-fixtures.cmd" $Architecture console-graphics-publication *> "$root/build.log"
if($LASTEXITCODE){throw 'Graphics publication fixture build failed'}
$exe="$repo/build/.ninja/fixtures-$Architecture/console-graphics-publication.exe"
& $exe *> "$root/results.txt"
if($LASTEXITCODE){throw 'Graphics publication fixture failed'}
Get-Content "$root/results.txt"
