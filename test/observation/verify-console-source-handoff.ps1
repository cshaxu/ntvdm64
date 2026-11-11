[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$PackageRoot,
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$LogRoot,
    [switch]$WindowCloseControl
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
    if($WindowCloseControl) {
        $env:MVDM_TEST_WINDOW_CLOSE_CONTROL='1'
        Remove-Item Env:MVDM_TEST_WINDOW_ATTACHED_KEYS -ErrorAction SilentlyContinue
    } else {
        $env:MVDM_TEST_WINDOW_ATTACHED_KEYS='1'
        Remove-Item Env:MVDM_TEST_WINDOW_CLOSE_CONTROL -ErrorAction SilentlyContinue
    }
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
        $marker=if($WindowCloseControl) {'S36-PROBE window-caf <using-window-close-control>'}
            else {'S36-PROBE window-caf <attached-input>'}
        $observed=(Test-Path -LiteralPath $raw) -and ((Get-Content -LiteralPath $raw -Raw) -like "*$marker*")
        $results+=@{name=$name;pid=$process.Id;exit=$process.ExitCode;marker=$observed;raw=$raw;stdout=$stdout;stderr=$stderr}
        $process.Dispose()
    }
    $passed=($results.Count -eq 2) -and @($results|Where-Object { $_.exit -ne 0 -or !$_.marker }).Count -eq 0
} finally {
    $control=if($WindowCloseControl) {'Window close between two Console CAF requests; not a Window CAF test'}
        else {'Three CAF requests; Window chord uses attached private input queues'}
    [ordered]@{stage=$stage;observer=$observerPath;observerSha256=(Get-FileHash $observerPath).Hash;
        control=$control;cases=$results} |
        ConvertTo-Json -Depth 5 | Set-Content -Encoding utf8 (Join-Path $log 'result.json')
    if($stage){Remove-PhysicalRuntimeStage $stage}
}
if(!$passed){throw 'Console-to-Window source handoff control failed; see recorded result'}
'PASS real DOS and native CUI input after repeated source entry'
