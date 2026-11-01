[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$PackageRoot,
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$LogRoot
)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$build=(Resolve-Path "$repo/build").Path+'\'
$log=[IO.Path]::GetFullPath($LogRoot)
if(!$log.StartsWith($build,[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $log)) {
    throw 'Require a fresh repository-build evidence directory'
}
$observerPath=(Resolve-Path -LiteralPath $Observer).Path
if(!$observerPath.StartsWith($build,[StringComparison]::OrdinalIgnoreCase)) {
    throw 'Require a build-owned observer'
}
. "$PSScriptRoot/isolated_package_cleanup.ps1"
$stage=New-PhysicalRuntimeStage $PackageRoot -FreshPackageIdentity
$null=New-Item -ItemType Directory -Path $log
$results=@();$passed=$false
try {
    $env:TEST_RUNTIME_ROOT=$stage
    $env:MVDM_CAF_THREE_PROBE='1'
    $env:MVDM_TEST_WINDOW_CLOSE_CONTROL='1'
    $env:MVDM_TEST_BOOT_WAIT_MS='6000'
    $env:MVDM_CAF_REAL_RECORDS='1'
    $env:MVDM_CAF_RELEASE_DELAY='20'
    $env:MVDM_CAF_DELAY_SECOND_ONLY='1'
    $env:PATH="$stage\system32;$env:SystemRoot\system32"
    foreach($name in @('COMMAND.COM','cmd.exe')) {
        $env:MVDM_TEST_INITIAL_COMMAND=$name
        $raw=Join-Path $log "$name.raw"
        $stdout=Join-Path $log "$name.stdout"
        $stderr=Join-Path $log "$name.stderr"
        $process=Start-Process -FilePath $observerPath -ArgumentList @('80','30',('"'+$raw+'"'),'--s38-window-reentry') `
            -RedirectStandardOutput $stdout -RedirectStandardError $stderr -WindowStyle Hidden -PassThru
        if(!$process.WaitForExit(120000)) {
            $process.Kill();$process.WaitForExit()
            throw "Observer diagnostic deadline exceeded: $name"
        }
        $results+=@{name=$name;pid=$process.Id;exit=$process.ExitCode;raw=$raw;stdout=$stdout;stderr=$stderr}
        $process.Dispose()
    }
    $passed=@($results|Where-Object exit -ne 0).Count -eq 0
} finally {
    [ordered]@{stage=$stage;observer=$observerPath;observerSha256=(Get-FileHash $observerPath).Hash;
        control='Window close between two Console CAF requests; not a Window CAF test';cases=$results} |
        ConvertTo-Json -Depth 5 | Set-Content -Encoding utf8 (Join-Path $log 'result.json')
    if($passed){Remove-PhysicalRuntimeStage $stage}
}
if(!$passed){throw 'Console-to-Window source handoff control failed; stage retained'}
'PASS real DOS and native CUI input after repeated source entry (Window-close control)'
