[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$RuntimeRoot,
    [Parameter(Mandatory)][string]$ObserverPath,
    [Parameter(Mandatory)][string]$OutputRoot,
    [switch]$AllModes
)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
. "$PSScriptRoot/../observation/isolated_package_cleanup.ps1"
$package=(Resolve-Path -LiteralPath $RuntimeRoot).Path
$observer=(Resolve-Path -LiteralPath $ObserverPath).Path
$out=[IO.Path]::GetFullPath((Join-Path $repo $OutputRoot))
if(!$out.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase) -or
    (Test-Path -LiteralPath $out)){throw 'A fresh build output directory is required'}
$runtime=$null
$scope=$null
$process=$null
try {
    & "$PSScriptRoot/../../tools/build-fixtures.cmd" x86 guest-profiles
    if($LASTEXITCODE){throw 'Video7 probe build failed'}
    $profile=if($AllModes){'all'}else{'default'}
    & "$repo/tools/prepare-test-inputs.cmd" $out "$repo/build/.ninja/fixtures-x86/guest-probes/profiles/VIDEO7-$profile.COM" V7FLAGS.COM
    if($LASTEXITCODE){throw 'Video7 input preparation failed'}
    $runtime=New-PhysicalRuntimeStage $package -TestFiles @("$out/V7FLAGS.COM")
    $scope=New-IsolatedPackageScope $runtime
    $binary=Get-PackageBinaryRoot $runtime
    $probe=Join-Path $runtime 'tests/V7FLAGS.COM'
    $command=Join-Path $binary 'COMMAND.COM'
    Save-IsolatedPackageManifest $runtime (Join-Path $out 'manifest.json')
    $start=[Diagnostics.ProcessStartInfo]::new($observer)
    $start.UseShellExecute=$false;$start.WindowStyle='Hidden'; $start.WorkingDirectory=$runtime
    $start.Environment['MVDM_OBSERVER_PRIVATE_DESKTOP']='1'
    $start.Environment['MVDM_OBSERVER_OWN_JOB']='1'
    $start.Environment['PATH']="$(Split-Path $command);$env:SystemRoot\System32"
    $start.Environment['TEMP']=$env:TEMP
    $start.Environment['TMP']=$env:TMP
    $log=Join-Path $out 'observer.txt'
    foreach($arg in @((Join-Path $binary 'run16.exe'),$runtime,$log,'--wait',$command,'/c',$probe,'--observation-timeout-ms','90000')){$start.ArgumentList.Add($arg)}
    $process=[Diagnostics.Process]::Start($start)
    @{pid=$process.Id;command=$start.FileName;arguments=@($start.ArgumentList);started=[DateTime]::UtcNow.ToString('o')}|ConvertTo-Json|Set-Content (Join-Path $out 'process.json')
    if(!$process.WaitForExit(100000)){$process.Kill($true);$process.WaitForExit();throw 'Owned BIOS diagnostic deadline'}
    $text=[IO.File]::ReadAllText($log+'.console.txt')
    $expected=if($AllModes){'VIDEO7_MODE_FLAGS_PASS modes=32 preserve=1 clear=1 invalid=2'}else{'VIDEO7_MODE_FLAGS_PASS modes=2 preserve=1 clear=1 invalid=2'}
    $record=Get-Content -LiteralPath $log -Raw
    $passed=$text.Contains($expected) -and !$text.Contains('VIDEO7_MODE_FLAGS_FAIL') -and
        $process.ExitCode -eq 0 -and $record -match '(?m)^result=exited\r?$' -and
        $record -match '(?m)^exit=0x00000000\r?$'
    @{passed=$passed;observerExitCode=$process.ExitCode;workerSha256=(Get-FileHash (Join-Path $binary 'ntvdm.exe')).Hash;probeSha256=(Get-FileHash $probe).Hash;computerUse=$false;inputInjected=$false}|ConvertTo-Json|Set-Content (Join-Path $out 'result.json')
    $text
    if(!$passed){throw 'Video7 BIOS mode flags failed'}
} finally {
    if($process){if(!$process.HasExited){$process.Kill($true);$process.WaitForExit()};$process.Dispose()}
    try {if($scope){Stop-IsolatedPackageScope $scope}}
    finally {if($runtime){Remove-PhysicalRuntimeStage $runtime}}
}
