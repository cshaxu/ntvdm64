[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$PackageRoot,
    [Parameter(Mandatory)][string]$ReportPrefix,
    [string[]]$SelectedCases
)
$ErrorActionPreference='Stop'
$Observer=(Resolve-Path -LiteralPath $Observer).Path
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
    $cases=@(
        @{Name='root-command-ver-exit'; Image=(Join-Path $binary 'COMMAND.COM'); Arguments=@(); Command='ver'; Marker='Microsoft Windows'; Exit='exit'; Code='00000000'},
        # The original DOS MEM utility returns 1 after successful display in
        # this guest; preserve that observed task result while proving the
        # root launcher itself does not remain attached.
        @{Name='root-command-mem-exit'; Image=(Join-Path $binary 'COMMAND.COM'); Arguments=@(); Command='mem'; Marker='bytes total conventional memory'; Exit='exit'; Code='00000001'},
        @{Name='root-cmd-ver-exit'; Image='cmd.exe'; Arguments=@('/d'); Command='ver'; Marker='Microsoft Windows'; Exit='exit'; Code='00000000'},
        @{Name='root-cmd-mem-exit'; Image='cmd.exe'; Arguments=@('/d'); Command='mem'; Marker='bytes total conventional memory'; Exit='exit'; Code='00000000'},
        @{Name='outer-cmd-command-ver-exit'; Image='cmd.exe'; Arguments=@('/d'); Inner='COMMAND.COM'; Command='ver'; Marker='Microsoft Windows'; Code='00000017'},
        @{Name='outer-cmd-command-mem-exit'; Image='cmd.exe'; Arguments=@('/d'); Inner='COMMAND.COM'; Command='mem'; Marker='bytes total conventional memory'; Code='00000017'},
        @{Name='outer-cmd-cmd-ver-exit'; Image='cmd.exe'; Arguments=@('/d'); Inner='cmd.exe'; Command='ver'; Marker='Microsoft Windows'; Code='00000017'},
        @{Name='outer-cmd-cmd-mem-exit'; Image='cmd.exe'; Arguments=@('/d'); Inner='cmd.exe'; Command='mem'; Marker='bytes total conventional memory'; Code='00000017'}
    )
    foreach($name in $SelectedCases){if($name -notin $cases.Name){throw "Unknown root-carrier case: $name"}}
    foreach($case in $cases) {
        if($SelectedCases -and $case.Name -notin $SelectedCases){continue}
        $report="$ReportPrefix-$($case.Name).txt"
        if(Test-Path -LiteralPath $report){throw 'Use a fresh report prefix'}
        $start=[Diagnostics.ProcessStartInfo]::new($Observer)
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
        $start.EnvironmentVariables['MVDM_OBSERVER_PRIVATE_DESKTOP']='1'
        # Match deployment's executable lookup without importing the test
        # runner's toolchain/Codex directories into COMMAND's guest block.
        $start.EnvironmentVariables['PATH']=$binary+';'+$env:SystemRoot+'\System32'
        $arguments=@((Join-Path $binary 'run16.exe'),$stage,$report,$case.Image)+$case.Arguments
        if($case.Inner) {
            $innerMarker=if($case.Inner -like '*COMMAND.COM'){'Microsoft(R) Windows NT DOS'}else{'Microsoft Windows'}
            $arguments+=@('--observe-console-input-escaped',('run16.exe '+$case.Inner+'\r'),
                '--observe-console-input-next-marker',
                $innerMarker,
                '--observe-console-input-next-escaped',($case.Command+'\r'),
                '--observe-console-input-next2-marker',$case.Marker,
                '--observe-console-input-next2-escaped','exit\r',
                '--observe-console-input-next3-marker',$stage,
                '--observe-console-input-next3-escaped','exit /b 23\r')
        } else {
            $arguments+=@('--observe-console-input-escaped',($case.Command+'\r'),
                '--observe-console-input-next-marker',$case.Marker,
                '--observe-console-input-next-escaped',($case.Exit+'\r'))
        }
        $arguments+=@('--observation-timeout-ms','35000')
        foreach($argument in $arguments){$start.ArgumentList.Add($argument)}
        $process=[Diagnostics.Process]::Start($start)
        try {
            if(!$process.WaitForExit(60000)){throw "Root carrier case $($case.Name) did not exit"}
            $record=Get-Content -LiteralPath $report -Raw
            $screen=Get-Content -LiteralPath ($report+'.console.txt') -Raw
            if($process.ExitCode -or $record -notmatch '(?m)^result=exited\r?$' -or
                $record -notmatch ('(?m)^exit=0x'+$case.Code+'\r?$') -or
                $record -notmatch '(?m)^scripted-console-input=delivered\r?$' -or
                $screen -notmatch [regex]::Escape($case.Marker)) {
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
