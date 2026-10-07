param([Parameter(Mandatory)][string]$PublicationProof,
      [Parameter(Mandatory)][string]$ReportPrefix)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$prefix=[IO.Path]::GetFullPath($ReportPrefix)
if(!$prefix.StartsWith('O:\winnt\logs2\',[StringComparison]::OrdinalIgnoreCase)) {
    throw 'Deployed logs must use the owner-approved Logs2 directory'
}
$observer=(Resolve-Path "$repo/build/M0-T433/S5/r012-observer/console-startup-observer.exe").Path
. "$PSScriptRoot/isolated_package_cleanup.ps1"
$paths=@('run16.exe','ntsrv.exe','ntcon.exe','ntvdm.exe','ntvwm.exe'|ForEach-Object {Join-Path 'O:\winnt\system32' $_})
$saved=$env:MVDM_OBSERVER_PRIVATE_DESKTOP;$env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
try {
    foreach($case in @(
      @{name='dos';args=@('command.com','/c','ver');output='MS-DOS Version 5.00.500'},
      @{name='native32';args=@('C:\Windows\SysWOW64\cmd.exe','/c','echo','T435-MOUSE-N32');output='T435-MOUSE-N32'},
      @{name='native64';args=@('C:\Windows\System32\cmd.exe','/c','echo','T435-MOUSE-N64');output='T435-MOUSE-N64'})) {
        $report=$prefix+'-'+$case.name+'.txt'
        if(Test-Path $report){throw 'Fresh deployed evidence required'}
        & $observer O:\winnt\system32\run16.exe O:\winnt\system32 $report @($case.args) --observation-timeout-ms 10000
        if($LASTEXITCODE){throw 'Deployed observer failed'}
        $result=Get-Content $report -Raw;$screen=Get-Content "$report.console.txt" -Raw
        if($result -notmatch '(?m)^result=exited\r?$' -or $result -notmatch '(?m)^exit=0x00000000\r?$' -or !$screen.Contains($case.output)) {
            throw "Deployed smoke failed: $($case.name)"
        }
        "PASS deployed $($case.name) actual output and exit0"
        Stop-IdentityCheckedProcesses @(Get-CimInstance Win32_Process|Where-Object {$_.ExecutablePath -in $paths}) $paths
    }
    foreach($row in @(Get-Content $PublicationProof -Raw|ConvertFrom-Json)) {
        if((Get-FileHash (Join-Path 'O:\winnt\system32' $row.Name)).Hash -ne $row.Sha256){throw 'Deployed hash mismatch'}
    }
    'PASS deployed ten-image identity'
} finally {
    $env:MVDM_OBSERVER_PRIVATE_DESKTOP=$saved
    Stop-IdentityCheckedProcesses @(Get-CimInstance Win32_Process|Where-Object {$_.ExecutablePath -in $paths}) $paths
}
