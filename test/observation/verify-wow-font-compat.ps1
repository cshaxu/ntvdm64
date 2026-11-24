param([Parameter(Mandatory)][string]$BuildRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$out=[IO.Path]::GetFullPath($BuildRoot)
if(!$out.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $out)){throw 'Fresh build-owned output required'}
New-Item -ItemType Directory $out|Out-Null
& "$repo/tools/build-fixtures.cmd" x86 wow-font-contracts *> "$out/build.log"
if($LASTEXITCODE){throw 'Font fixtures failed to build'}
foreach($kind in 'glyph','etm','etm-native','tracking','tracking-search'){
 & "$repo/build/.ninja/fixtures-x86/wow-font-$kind.exe" *> "$out/$kind.txt"
 if($LASTEXITCODE){throw "Font fixture failed: $kind"}
 Get-Content "$out/$kind.txt"
}
