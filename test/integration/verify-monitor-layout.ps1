[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$BuildRoot,
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$ReportPath,
    [Parameter(Mandatory)][string]$MsvcWrapper,
    [string]$Ninja='ninja'
)
$ErrorActionPreference='Stop'
$repo=Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$build=(Resolve-Path -LiteralPath $BuildRoot).Path
$observerPath=(Resolve-Path -LiteralPath $Observer).Path
$wrapper=(Resolve-Path -LiteralPath $MsvcWrapper).Path
$report=[IO.Path]::GetFullPath($ReportPath)
$prefix=(Join-Path $repo 'build')+'\'
if(!$build.StartsWith($prefix,[StringComparison]::OrdinalIgnoreCase) -or
   !$report.StartsWith($prefix,[StringComparison]::OrdinalIgnoreCase)) {
    throw 'Build and evidence must remain under repository build/'
}
if(!(Test-Path -LiteralPath (Join-Path $build 'build.ninja'))) {
    throw 'Generate the formal x86 Ninja graph before running this fixture'
}
if(Test-Path -LiteralPath $report){throw 'Use a fresh evidence path'}
# Reuse the selected graph and common/RPC dependencies, not an ad-hoc recipe.
& $wrapper $Ninja -C $build monitor-layout-test.exe
if($LASTEXITCODE){throw 'Formal x86 fixture build failed'}
$oldPrivate=$env:MVDM_OBSERVER_PRIVATE_DESKTOP
try {
    $env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
    & $observerPath (Join-Path $build 'monitor-layout-test.exe') $build $report --observation-timeout-ms 10000
    if($LASTEXITCODE){throw 'Console observer failed'}
    $record=Get-Content -LiteralPath $report -Raw
    $screen=Get-Content -LiteralPath ($report+'.console.txt') -Raw
    if($record -notmatch '(?m)^result=exited\r?$' -or
        $record -notmatch '(?m)^exit=0x00000000\r?$' -or $screen -notmatch 'PASS') {
        throw "Layout/dispatch fixture failed: $report"
    }
    $screen
    'PASS formal x86 monitor renderer/refresh/input fixture'
} finally {$env:MVDM_OBSERVER_PRIVATE_DESKTOP=$oldPrivate}
