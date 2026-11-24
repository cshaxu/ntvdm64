param(
 [Parameter(Mandatory)][string]$BuildRoot
)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$out=[IO.Path]::GetFullPath($BuildRoot)
if(!$out.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $out)){throw 'Use a fresh repository-build evidence directory'}
$cache=Join-Path $repo 'build/.ninja/msvc-x86-release/worker'
$null=New-Item -ItemType Directory $out
& "$repo/tools/build-fixtures.cmd" x86 redirector-server-enum *> "$out/build.log"
if($LASTEXITCODE){throw 'Native server enumeration build failed'}
Copy-Item "$cache/server-enum-test.exe" "$out/enum-test.exe"
Copy-Item "$cache/server-enum-test.exe.map" "$out/enum-test.map"
$map=Get-Content "$out/enum-test.map" -Raw
if($map -notmatch '_XsNetServerEnum2\s+.*apisrv-test.obj' -or $map -notmatch '_XsFillEnumBuffer\s+.*xssubs.obj'){throw 'Original handler/conversion owners missing'}
& "$out/enum-test.exe" | Tee-Object -FilePath "$out/result.txt"
if($LASTEXITCODE){throw 'Original server enumeration assertions failed'}
