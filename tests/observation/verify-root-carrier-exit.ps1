[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$DirectObserver,
    [Parameter(Mandatory)][string]$PackageRoot,
    [Parameter(Mandatory)][string]$ReportPrefix,
    [string[]]$SelectedCases
)
$ErrorActionPreference='Stop'
$Observer=(Resolve-Path -LiteralPath $Observer).Path
$DirectObserver=(Resolve-Path -LiteralPath $DirectObserver).Path
$PackageRoot=(Resolve-Path -LiteralPath $PackageRoot).Path
. "$PSScriptRoot/isolated_package_cleanup.ps1"
$build=(Resolve-Path (Join-Path $PSScriptRoot '../../build')).Path+'\'
$ReportPrefix=[IO.Path]::GetFullPath($ReportPrefix)
if(!$ReportPrefix.StartsWith($build,[StringComparison]::OrdinalIgnoreCase)){throw 'Reports must remain under repository build/'}
$stage=$null;$scope=$null
try {
    $stage=New-PhysicalRuntimeStage $PackageRoot
    $scope=New-IsolatedPackageScope $stage
    $binary=Get-PackageBinaryRoot $stage
    # Each command is injected only after the preceding visible result.  This
    # is deliberately a root-carrier regression: a DOS/native transition can
    # replace the active input endpoint, so typeahead cannot prove completion.
    # Each chain crosses the DOS/native CUI boundary before exit.  A root
    # launcher that merely survives one child, but cannot retire after a
    # mixed chain, is not correct.
    $cases=@(
        @{Name='root-cmd-exit'; Image='cmd.exe'; Arguments=@('/d'); Steps=@(); Code='00000000'},
        @{Name='root-command-exit'; Image=(Join-Path $binary 'COMMAND.COM'); Arguments=@(); Steps=@(); Code='00000000'},
        @{Name='root-cmd-ver-mem-exit'; Image='cmd.exe'; Arguments=@('/d'); Steps=@(@('Microsoft Windows','ver'),@('bytes total conventional memory','mem')); Code='00000000'},
        @{Name='root-cmd-mem-ver-exit'; Image='cmd.exe'; Arguments=@('/d'); Steps=@(@('bytes total conventional memory','mem'),@('Microsoft Windows','ver')); Code='00000000'},
        @{Name='root-command-ver-mem-exit'; Image=(Join-Path $binary 'COMMAND.COM'); Arguments=@(); Steps=@(@('Microsoft Windows','ver'),@('bytes total conventional memory','mem')); Code='00000001'},
        @{Name='root-command-mem-ver-exit'; Image=(Join-Path $binary 'COMMAND.COM'); Arguments=@(); Steps=@(@('bytes total conventional memory','mem'),@('Microsoft Windows','ver')); Code='00000001'},
        @{Name='outer-cmd-cmd-ver-mem-exit'; Image='cmd.exe'; Arguments=@('/d'); Inner='cmd.exe'; Steps=@(@('Microsoft Windows','ver'),@('bytes total conventional memory','mem')); Code='00000017'},
        @{Name='outer-cmd-cmd-mem-ver-exit'; Image='cmd.exe'; Arguments=@('/d'); Inner='cmd.exe'; Steps=@(@('bytes total conventional memory','mem'),@('Microsoft Windows','ver')); Code='00000017'},
        @{Name='outer-cmd-command-ver-mem-exit'; Image='cmd.exe'; Arguments=@('/d'); Inner='COMMAND.COM'; Steps=@(@('Microsoft Windows','ver'),@('bytes total conventional memory','mem')); Code='00000017'},
        @{Name='outer-cmd-command-mem-ver-exit'; Image='cmd.exe'; Arguments=@('/d'); Inner='COMMAND.COM'; Steps=@(@('bytes total conventional memory','mem'),@('Microsoft Windows','ver')); Code='00000017'}
    )
    foreach($name in $SelectedCases){if($name -notin $cases.Name){throw "Unknown root-carrier case: $name"}}
    foreach($case in $cases) {
        if($SelectedCases -and $case.Name -notin $SelectedCases){continue}
        $report="$ReportPrefix-$($case.Name).txt"
        if(Test-Path -LiteralPath $report){throw 'Use a fresh report prefix'}
        $start=[Diagnostics.ProcessStartInfo]::new($(if($case.Inner){$Observer}else{$DirectObserver}))
        $start.UseShellExecute=$false
        $start.WindowStyle=[Diagnostics.ProcessWindowStyle]::Hidden
        # COMMAND's original real-mode startup consumes the launch environment.
        # Do not let a developer shell's unrelated diagnostics/tool paths
        # become fixture input; deployment-relevant lookup is supplied below.
        $start.Environment.Clear()
        $start.Environment['SystemRoot']=$env:SystemRoot
        $start.Environment['WINDIR']=$env:WINDIR
        $start.Environment['COMSPEC']=$env:COMSPEC
        $start.Environment['PATHEXT']=$env:PATHEXT
        $start.EnvironmentVariables[$(if($case.Inner){'MVDM_OBSERVER_PRIVATE_DESKTOP'}else{'MVDM_DIRECT_PRIVATE_DESKTOP'})]='1'
        # Match deployment's executable lookup without importing the test
        # runner's toolchain/Codex directories into COMMAND's guest block.
        $start.EnvironmentVariables['PATH']=$binary+';'+$env:SystemRoot+'\System32'
        $arguments=@((Join-Path $binary 'run16.exe'),$stage,$report,$case.Image)+$case.Arguments
        if($case.Inner) {
            $innerMarker=if($case.Inner -like '*COMMAND.COM'){'Microsoft(R) Windows NT DOS'}else{'Microsoft Windows'}
            $arguments+=@('--observe-console-input-escaped',('run16.exe '+$case.Inner+'\r'),
                '--observe-console-input-next-marker',$innerMarker,
                '--observe-console-input-next-escaped',($case.Steps[0][1]+'\r'),
                '--observe-console-input-next2-marker',$case.Steps[0][0],
                '--observe-console-input-next2-escaped',($case.Steps[1][1]+'\r'),
                '--observe-console-input-next3-marker',$case.Steps[1][0],
                '--observe-console-input-next3-escaped','exit\r',
                '--observe-console-input-next4-marker',$stage,
                '--observe-console-input-next4-escaped','exit /b 23\r')
        }
        $arguments+=@('--observation-timeout-ms','35000')
        # A root must start alone in its Console. The ordinary observer owns
        # a Console attachment and is valid only for the outer-CMD controls.
        if(!$case.Inner){$arguments=@($stage,$report,$case.Name)}
        foreach($argument in $arguments){$start.ArgumentList.Add($argument)}
        $process=[Diagnostics.Process]::Start($start)
        try {
            if(!$process.WaitForExit(60000)){throw "Root carrier case $($case.Name) did not exit"}
            $record=Get-Content -LiteralPath $report -Raw
            if(!$case.Inner) {
                $expected=[Convert]::ToUInt32($case.Code,16)
                if($process.ExitCode -or $record -notmatch ('(?m)^exit='+$expected+'\r?$') -or
                    $record -notmatch 'initial-ready=1 submitted=1' -or
                    $record -notmatch 'console-observed=1 observed=1' -or
                    $record -notmatch '(?m)^detached-before-exit-wait=1\r?$' -or
                    $record -notmatch '(?m)^frontend-exited=1\r?$') {
                    throw "Independent Console case $($case.Name) failed: $report"
                }
                for($step=0;$step -lt $case.Steps.Count;$step++) {
                    if(!$record.Contains("step=$step output="+$case.Steps[$step][0])) {
                        throw "Independent Console case $($case.Name) missing output $step"
                    }
                }
                "PASS independent Console $($case.Name): command outputs, launcher and frontend exit"
                continue
            }
            $screen=Get-Content -LiteralPath ($report+'.console.txt') -Raw
            if($process.ExitCode -or $record -notmatch '(?m)^result=exited\r?$' -or
                $record -notmatch ('(?m)^exit=0x'+$case.Code+'\r?$') -or
                $record -notmatch '(?m)^scripted-console-input=delivered\r?$' -or
            $screen -notmatch [regex]::Escape($case.Steps[0][0]) -or
            $screen -notmatch [regex]::Escape($case.Steps[1][0])) {
                throw "Root carrier case $($case.Name) failed: $report"
            }
            "PASS root carrier $($case.Name): mixed command completed and direct launcher exited"
        } finally {
            if(!$process.HasExited){$process.Kill();$process.WaitForExit()}
            $process.Dispose()
        }
    }
} finally {
    try {if($scope){Stop-IsolatedPackageScope $scope}} finally {if($stage){Remove-PhysicalRuntimeStage $stage}}
}
