param([Parameter(Mandatory)][string]$BuildRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$out=[IO.Path]::GetFullPath($BuildRoot, $repo)
if(!$out.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $out)){throw 'Fresh build-owned output required'}
New-Item -ItemType Directory $out|Out-Null
& "$repo/tools/build-fixtures.cmd" x86 wow-session-contracts *> "$out/build.log"
if($LASTEXITCODE){throw 'Session fixtures failed to build'}
foreach($name in 'wow-session-binding','wow-runtime-independent'){
 & "$repo/build/.ninja/fixtures-x86/$name.exe" *> "$out/$name.txt"
 if($LASTEXITCODE){throw "Session fixture failed: $name"}
 Get-Content "$out/$name.txt"
}
