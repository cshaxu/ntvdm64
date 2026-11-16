param(
    [Parameter(Mandatory)][string]$RuntimeRoot,
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$FixtureRoot,
    [Parameter(Mandatory)][string]$ReportRoot,
    [Parameter(Mandatory)][string]$PifPath,
    [string]$Python='python',
    [switch]$MinimalEnvironment=$true
)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
. "$PSScriptRoot/../observation/isolated_package_cleanup.ps1"
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
$pifInputs=@{}
$pifTool=Join-Path $repo 'assets/release/PIF.EXE'
if($PifPath) {
    $PifPath=(Resolve-Path -LiteralPath $PifPath).Path
    $FixtureRoot=(Resolve-Path -LiteralPath $FixtureRoot).Path
    $fields=@{}
    & $pifTool show $PifPath | ForEach-Object {
        $pair=$_ -split '=',2
        if($pair.Count -eq 2) { $fields[$pair[0]]=$pair[1] }
    }
    if($LASTEXITCODE) { throw 'Cannot read fixture PIF' }
    if($fields.PROGRAM -ine (Join-Path $FixtureRoot 'DOS4GW.EXE') -or
       $fields.DIRECTORY -ine $FixtureRoot -or $fields.ARGUMENTS -or $fields.'W386.ARGUMENTS') {
        throw 'PIF must select fixture DOS4GW with its fixture directory and no preset arguments'
    }
    foreach($path in @($PifPath,$fields.CONFIG,$fields.AUTOEXEC)) {
        $pifInputs[$path]=Get-IsolatedFileSha256 $path
    }
}
$stage=$null; $scope=$null
try {
    $stage=New-PhysicalRuntimeStage $RuntimeRoot
    $scope=New-IsolatedPackageScope $stage
    foreach($name in $expected.Keys) {
        Copy-Item -LiteralPath (Join-Path $FixtureRoot $name) -Destination (Join-Path $stage $name)
        if((Get-IsolatedFileSha256 (Join-Path $stage $name)) -ne $expected[$name]) { throw "Fixture copy mismatch: $name" }
    }
    $target=Join-Path $stage 'DOS4GW.EXE'
    if($PifPath) {
        $target=Join-Path $stage 'DOS4GW.PIF'
        Copy-Item -LiteralPath $PifPath -Destination $target
        foreach($field in @('CONFIG','AUTOEXEC')) {
            $destination=Join-Path $stage ($field+'.NT')
            $text=[Text.Encoding]::Latin1.GetString([IO.File]::ReadAllBytes($fields[$field]))
            $text=$text.Replace($FixtureRoot,$stage,[StringComparison]::OrdinalIgnoreCase)
            [IO.File]::WriteAllBytes($destination,[Text.Encoding]::Latin1.GetBytes($text))
        }
        & $pifTool replace-root $target --from $FixtureRoot --to $stage
        if($LASTEXITCODE) { throw 'Cannot relocate isolated PIF' }
        & $pifTool update $target --config (Join-Path $stage 'CONFIG.NT') --autoexec (Join-Path $stage 'AUTOEXEC.NT')
        if($LASTEXITCODE) { throw 'Cannot bind isolated PIF configuration' }
        & $pifTool show $target | Set-Content -LiteralPath (Join-Path $out 'staged-pif.txt')
        if($LASTEXITCODE) { throw 'Cannot verify isolated PIF' }
    }
    @{FixtureHashes=$expected;PifInputs=$pifInputs;PifPath=$PifPath;MinimalEnvironment=[bool]$MinimalEnvironment} |
        ConvertTo-Json -Depth 4 | Set-Content -LiteralPath (Join-Path $out 'inputs.json')
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
    $start.Environment['MVDM_OBSERVER_GRAPHICS_ESCAPE_WITNESS']='1'
    foreach($arg in @($launcher,$stage,$report,$target,'NESTICLE.EXE','--observation-timeout-ms','15000')) { $start.ArgumentList.Add($arg) }
    $process=[Diagnostics.Process]::Start($start)
    @{ObserverPid=$process.Id;Command=$start.FileName;Arguments=@($start.ArgumentList);DeadlineSeconds=45} |
        ConvertTo-Json -Depth 4 | Set-Content (Join-Path $out 'observer-process.json')
    try {
        if(!$process.WaitForExit(45000)) {
            & taskkill.exe /PID $process.Id /T /F | Out-Null
            throw 'DOS4GW observer exceeded its 45-second test deadline'
        }
        $observerExit=$process.ExitCode
    } finally { $process.Dispose() }
    if(!(Test-Path -LiteralPath $report)) { throw "Observer failed without report (exit $observerExit)" }
    $result=Get-Content -LiteralPath $report -Raw
    $console=if(Test-Path "$report.console.txt") {Get-Content "$report.console.txt" -Raw} else {''}
    if($console -match '(?i)DOS/4GW (fatal )?error|exception 0Dh') { throw 'DOS4GW reported a fatal or protection exception' }
    $live=if(Test-Path "$report.timeout-live.txt") {Get-Content "$report.timeout-live.txt" -Raw} else {''}
    if($live -match '(?m)^window pid=\d+ visible=1 class=#32770\b') { throw 'A visible error or modal dialog remains on the test desktop' }
    if($result -notmatch '(?m)^result=timeout\r?$' -or $live -notmatch '(?m)^window pid=\d+ visible=1 class=LibKvmWindow\b') {
        throw "NESticle did not remain running with a visible graphics window (observer exit $observerExit)"
    }
    if(!(Test-Path "$report.graphics-witness.txt") -or
       (Get-Content "$report.graphics-witness.txt" -Raw) -notmatch 'capture-and-input=pass') {
        throw 'Graphics capture or scoped Escape input failed'
    }
    & $Python "$PSScriptRoot/check-nesticle-frames.py" $report
    if($LASTEXITCODE){throw 'NESticle rendered menu/keyboard witness failed'}
    'PASS DOS4GW NESticle renders its menu and responds to Escape; ROM gameplay is outside this test'
} finally {
    if($scope) { Stop-IsolatedPackageScope $scope }
    if($stage) { Remove-PhysicalRuntimeStage $stage }
    foreach($name in $expected.Keys) {
        if((Get-IsolatedFileSha256 (Join-Path $FixtureRoot $name)) -ne $expected[$name]) { throw "Original fixture changed: $name" }
    }
    foreach($path in $pifInputs.Keys) {
        if((Get-IsolatedFileSha256 $path) -ne $pifInputs[$path]) { throw "Original PIF input changed: $path" }
    }
}
