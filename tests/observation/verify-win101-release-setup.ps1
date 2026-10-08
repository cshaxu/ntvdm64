param([Parameter(Mandatory)][string]$BuildRoot)

$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
& (Join-Path $repo 'tests/component-integration/win101_setup_cmd_test.ps1') `
    -ToolRoot (Join-Path $repo 'tools/win101-setup') -BuildRoot $BuildRoot
if($LASTEXITCODE -ne 0){exit $LASTEXITCODE}
'PASS released CMD Win1.01 setup payload verification'
