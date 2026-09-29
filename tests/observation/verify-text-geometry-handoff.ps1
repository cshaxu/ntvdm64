[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$PackageRoot,
    [Parameter(Mandatory)][string]$NativeProbe,
    [Parameter(Mandatory)][string]$GuestProbe,
    [Parameter(Mandatory)][string]$LogPrefix,
    [string]$LogRoot='O:\winnt\Logs2'
)
$ErrorActionPreference='Stop'
if($LogPrefix -notmatch '^[a-z0-9-]+$'){throw 'Invalid log prefix'}
$Observer=(Resolve-Path -LiteralPath $Observer).Path
$PackageRoot=(Resolve-Path -LiteralPath $PackageRoot).Path
$LogRoot=(Resolve-Path -LiteralPath $LogRoot).Path
$fixtures=(Resolve-Path -LiteralPath (Join-Path $PackageRoot 'tests')).Path
$buildRoot=(Resolve-Path -LiteralPath (Join-Path $PSScriptRoot '../../build')).Path+'\'
foreach($path in @($NativeProbe,$GuestProbe)){
    $resolved=(Resolve-Path -LiteralPath $path).Path
    if(!$resolved.StartsWith($buildRoot,[StringComparison]::OrdinalIgnoreCase)){throw 'Authored probes must be built under repository build'}
}
Copy-Item -LiteralPath $NativeProbe -Destination (Join-Path $fixtures 'GEOMH.EXE') -Force
Copy-Item -LiteralPath $GuestProbe -Destination (Join-Path $fixtures 'GRID.COM') -Force
$savedPrivate=$env:MVDM_OBSERVER_PRIVATE_DESKTOP
$savedColumns=$env:MVDM_OBSERVER_BUFFER_COLUMNS
try {
    $env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
    $env:MVDM_OBSERVER_BUFFER_COLUMNS='200'
    foreach($rows in @(22,25,28,43,50)){
        $report=Join-Path $LogRoot "$LogPrefix-$rows.txt"
        $fixture=Join-Path $LogRoot "$LogPrefix-$rows-fixture.txt"
        if((Test-Path -LiteralPath $report) -or (Test-Path -LiteralPath $fixture)){throw 'Use fresh evidence names'}
        & $Observer (Join-Path $PackageRoot 'run16.exe') $PackageRoot $report --observation-timeout-ms 60000 `
            (Join-Path $fixtures 'GEOMH.EXE') (Join-Path $PackageRoot 'run16.exe') $rows $fixture
        if($LASTEXITCODE -or !(Test-Path -LiteralPath $report) -or !(Test-Path -LiteralPath $fixture)){throw 'Missing or failed observation'}
        $observed=Get-Content -LiteralPath $report -Raw
        $checks=Get-Content -LiteralPath $fixture -Raw
        $screen=Get-Content -LiteralPath ($report+'.console.txt') -Raw
        if($observed -notmatch '(?m)^result=exited\r?$' -or
            $observed -notmatch '(?m)^exit=0x00000000\r?$' -or
            $checks -match '(?m)^FAIL ' -or
            ([regex]::Matches($checks,'(?m)^AFTER cycle=')).Count -ne 3 -or
            ([regex]::Matches($checks,'(?m)^CHILD .*result=0\r?$')).Count -ne 9 -or
            $checks -notmatch "(?m)^PASS rows=$rows cycles=3 real-DOS-MEM=yes real-native-child=yes history-preserved=yes\r?$" -or
            $screen -notmatch 'S13-GUEST-GRID-PASS' -or $screen -match 'S13-GUEST-GRID-FAIL' -or
            $screen -notmatch 'NATIVE-CHAIN-CONTINUED' -or $screen -notmatch 'bytes total conventional memory'){
            throw "Real geometry handoff failed: $rows"
        }
        # Small viewports need not retain every prior line in the final snapshot;
        # each of the nine actual child completions is independently asserted.
        Write-Output "PASS real BIOS/Console rows=$rows cycles=3 history preserved; host buffer=200 columns"
    }
} finally {
    foreach($entry in @(@('MVDM_OBSERVER_PRIVATE_DESKTOP',$savedPrivate),@('MVDM_OBSERVER_BUFFER_COLUMNS',$savedColumns))){
        if($null -eq $entry[1]){Remove-Item -LiteralPath ('Env:'+$entry[0]) -ErrorAction SilentlyContinue}
        else{Set-Item -LiteralPath ('Env:'+$entry[0]) -Value $entry[1]}
    }
}
