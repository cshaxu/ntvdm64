param(
    [string]$RepositoryRoot = (Resolve-Path (Join-Path $PSScriptRoot '..\\..')).Path
)

$ErrorActionPreference = 'Stop'
$source = Join-Path $RepositoryRoot 'tests\\adapter-mvdm-host-out\\wow\\wow_gdi_alias_fixture.c'
$implementation = Join-Path $RepositoryRoot 'src\\ntvdm-exe\\wow\\wow_gdi_alias.c'
$output = Join-Path $env:TEMP 'wow-gdi-alias-fixture.exe'
$include = Join-Path $RepositoryRoot 'src'
$workerWowInclude = Join-Path $RepositoryRoot 'src\\ntvdm-exe\\wow\\include'

& cl.exe /nologo /TC /W4 /WX /I $include /I $workerWowInclude $source $implementation /Fe:$output | Out-Host
if ($LASTEXITCODE -ne 0) { throw "fixture compile failed: $LASTEXITCODE" }
& $output | Out-Host
if ($LASTEXITCODE -ne 0) { throw "fixture failed: $LASTEXITCODE" }
