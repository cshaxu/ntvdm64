param([Parameter(Mandatory)][string]$BuildRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$out=[IO.Path]::GetFullPath($BuildRoot)
if(!$out.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $out)){throw 'Fresh build-owned output required'}
New-Item -ItemType Directory $out|Out-Null
& "$repo/tools/build-fixtures.cmd" x86 wow-metafile-host *> "$out/build.log"
if($LASTEXITCODE){throw 'Metafile fixture failed to build'}
$result=& "$repo/build/.ninja/fixtures-x86/wow-metafile-host.exe" "$out/roundtrip.wmf" 2>&1
$code=$LASTEXITCODE
$result|Set-Content "$out/result.txt"
if($code -or ($result -join "`n") -notmatch 'WOW_METAFILE_PUBLIC_HOST_OK'){throw 'Public metafile contract failed'}
$result
