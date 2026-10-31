param(
    [Parameter(Mandatory)][string]$RuntimeRoot,
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$FixtureRoot,
    [Parameter(Mandatory)][string]$ReportRoot,
    [switch]$MinimalEnvironment
)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
. "$PSScriptRoot/isolated_package_cleanup.ps1"
$out=[IO.Path]::GetFullPath($ReportRoot)
if(!$out.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $out)) {
    throw 'A fresh build-owned report directory is required'
}
New-Item -ItemType Directory -Path $out | Out-Null
$expected=@{
    'DOS4GW.EXE'='B8265123AC8A189637448618409EF3ECD2E9F3E1A47062C685A02240F688DEC1'
    'NESTICLE.EXE'='224E348E1FD5C11B16EF096FF5607C7C57C49D65FA7E4DBBC9B2A5C9C53E832B'
}
foreach($name in $expected.Keys) {
    if((Get-IsolatedFileSha256 (Join-Path $FixtureRoot $name)) -ne $expected[$name]) { throw "Fixture identity mismatch: $name" }
}
$stage=$null; $scope=$null
try {
    $stage=New-PhysicalRuntimeStage $RuntimeRoot -FreshPackageIdentity
    $scope=New-IsolatedPackageScope $stage
    foreach($name in $expected.Keys) {
        Copy-Item -LiteralPath (Join-Path $FixtureRoot $name) -Destination (Join-Path $stage $name)
        if((Get-IsolatedFileSha256 (Join-Path $stage $name)) -ne $expected[$name]) { throw "Fixture copy mismatch: $name" }
    }
    Copy-Item -LiteralPath (Join-Path $stage 'product-system32-manifest.json') -Destination (Join-Path $out 'runtime-manifest.json')
    $launcher=Join-Path (Get-PackageBinaryRoot $stage) 'run16.exe'
    $report=Join-Path $out 'nesticle.txt'
    $start=[Diagnostics.ProcessStartInfo]::new()
    $start.FileName=(Resolve-Path -LiteralPath $Observer).Path
    $start.UseShellExecute=$false
    $start.CreateNoWindow=$true
    if($MinimalEnvironment) {
        $start.Environment.Clear()
        foreach($name in @('SystemRoot','WINDIR','COMSPEC','TEMP','TMP')) {
            $value=[Environment]::GetEnvironmentVariable($name,'Process')
            if($value) { $start.Environment[$name]=$value }
        }
        $start.Environment['PATH']=(Get-PackageBinaryRoot $stage)+';'+$env:SystemRoot+'\System32'
    }
    $start.Environment['MVDM_OBSERVER_PRIVATE_DESKTOP']='1'
    $start.Environment['MVDM_OBSERVER_SHORT_HISTORY']='1'
    foreach($arg in @($launcher,$stage,$report,(Join-Path $stage 'DOS4GW.EXE'),'NESTICLE.EXE','--observation-timeout-ms','15000')) { $start.ArgumentList.Add($arg) }
    $process=[Diagnostics.Process]::Start($start)
    try { $process.WaitForExit(); $observerExit=$process.ExitCode } finally { $process.Dispose() }
    if(!(Test-Path -LiteralPath $report)) { throw "Observer failed without report (exit $observerExit)" }
    $result=Get-Content -LiteralPath $report -Raw
    $console=if(Test-Path "$report.console.txt") {Get-Content "$report.console.txt" -Raw} else {''}
    if($console -match '(?i)DOS/4GW (fatal )?error|exception 0Dh') { throw 'DOS4GW reported a fatal or protection exception' }
    $live=if(Test-Path "$report.timeout-live.txt") {Get-Content "$report.timeout-live.txt" -Raw} else {''}
    if($live -match '(?m)^window pid=\d+ visible=1 class=#32770\b') { throw 'A visible error or modal dialog remains on the test desktop' }
    if($result -notmatch '(?m)^result=timeout\r?$' -or $live -notmatch '(?m)^window pid=\d+ visible=1 class=LibKvmWindow\b') {
        throw "NESticle did not remain running with a visible graphics window (observer exit $observerExit)"
    }
    'PASS DOS4GW NESticle remains active with a visible graphics window; gameplay and rendered pixels are not asserted'
} finally {
    if($scope) { Stop-IsolatedPackageScope $scope }
    if($stage) { Remove-PhysicalRuntimeStage $stage }
    foreach($name in $expected.Keys) {
        if((Get-IsolatedFileSha256 (Join-Path $FixtureRoot $name)) -ne $expected[$name]) { throw "Original fixture changed: $name" }
    }
}
