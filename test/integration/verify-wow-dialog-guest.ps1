[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$PackageRoot,
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$BuildRoot,
    [switch]$CommonDialog,
    [string]$DebugObserver,
    [string]$Nasm='nasm.exe'
)
# Assemble only the independently authored probe.  The immutable guest image
# is copied into an exact, owned physical stage; this runner never alters it.
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or
        (Test-Path -LiteralPath $root)){throw 'Require fresh build-owned evidence root'}
$null=New-Item -ItemType Directory -Path $root
. "$PSScriptRoot/../observation/isolated_package_cleanup.ps1"
$runtime=$null;$scope=$null;$process=$null
$saved=@{}
foreach($name in @('PATH','TEMP','TMP','MVDM_OBSERVER_PRIVATE_DESKTOP','MVDM_WOW_WINDOW_TRACE_PATH')){
    $saved[$name]=[Environment]::GetEnvironmentVariable($name,'Process')
}
try {
    $package=(Resolve-Path -LiteralPath $PackageRoot).Path
    $observer=(Resolve-Path -LiteralPath $Observer).Path
    $probeName=if($CommonDialog){'CDLPROBE'}else{'DLGPROBE'}
    $sourceName=if($CommonDialog){'win16_common_dialog_probe.asm'}else{'win16_dialog_probe.asm'}
    $source=Join-Path $PSScriptRoot ('../observation/'+$sourceName)
    . "$PSScriptRoot/Invoke-Win16ProbeBuild.ps1"
    Invoke-Win16ProbeBuild -OutputRoot $root -Name $probeName
    Get-FileHash $source,"$root\$probeName.EXE",$observer,
        "$package\product-system32-manifest.json" |
        Export-Csv -NoTypeInformation "$root\inputs.csv"

    $runtime=New-PhysicalRuntimeStage $package
    $scope=New-IsolatedPackageScope $runtime
    # Older valid package snapshots predate the test-input directory.  The
    # probe is an owned disposable test input, never a guest replacement.
    $null=New-Item -ItemType Directory -Force -Path "$runtime\tests"
    Copy-Item "$root\$probeName.EXE" "$runtime\tests\$probeName.EXE"
    $env:TEMP=$runtime;$env:TMP=$runtime
    $env:PATH="$runtime\system32;"+$env:SystemRoot+'\System32'
    $env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
    $env:MVDM_WOW_WINDOW_TRACE_PATH="$root\lifetime.txt"

    $start=[Diagnostics.ProcessStartInfo]::new($observer)
    $start.UseShellExecute=$false;$start.CreateNoWindow=$true
    foreach($argument in @("$runtime\system32\run16.exe","$runtime\tests",
            "$root\observer.txt",'--wait',"$runtime\tests\$probeName.EXE",
            '--observation-timeout-ms','30000')){$start.ArgumentList.Add($argument)}
    if($DebugObserver){
        # Reuse the existing exception observer; diagnostic runs are not acceptance.
        $line=(@($start.ArgumentList)|ForEach-Object {'"'+$_+'"'}) -join ' '
        $observed=$start.FileName
        $start.FileName=(Resolve-Path $DebugObserver).Path
        $start.ArgumentList.Clear()
        foreach($argument in @('--exe',$observed,$line,"$root/exceptions.txt")){$start.ArgumentList.Add($argument)}
        Get-FileHash $DebugObserver,"$PSScriptRoot/../observation/wow_exception_observer.c"|
            Export-Csv -NoTypeInformation "$root/debugger-inputs.csv"
    }
    $deadline=if($DebugObserver){135}else{35}
    $process=[Diagnostics.Process]::Start($start);$null=$process.Handle
    @{pid=$process.Id;start=$process.StartTime.ToUniversalTime().ToString('o');
      arguments=@($start.ArgumentList);deadlineSeconds=$deadline}|ConvertTo-Json|
        Set-Content "$root\observer-process.json"
    if(!$process.WaitForExit($deadline*1000)){throw 'Owned dialog observer deadline'}
    $process.Refresh();$process.ExitCode|Set-Content "$root\observer-exit.txt"
    if($process.ExitCode){throw "Dialog observer exit $($process.ExitCode)"}
    $report=Get-Content "$root\observer.txt" -Raw
    if($report -notmatch 'result=exited' -or $report -notmatch 'exit=0x00000000'){
        throw 'Dialog guest/launcher did not complete normally'
    }
    $guest="$runtime\tests\$probeName.BIN"
    if(!(Test-Path -LiteralPath $guest)){throw 'Dialog probe did not emit its report'}
    Copy-Item $guest "$root\guest-report.bin"
    $bytes=[IO.File]::ReadAllBytes("$root\guest-report.bin")
    if($CommonDialog){
        if($bytes.Length -ne 120){throw 'Unexpected common-dialog report size'}
        $names=@('phase','errors','created','closed','hooks','leaving','task',
            'dialog','destroyResult','isWindow')
        $records=@()
        for($offset=0;$offset -lt $bytes.Length;$offset+=24){
            if([Text.Encoding]::ASCII.GetString($bytes,$offset,4) -ne 'CDL2'){
                throw 'Bad common-dialog report signature'
            }
            $row=[ordered]@{}
            for($i=0;$i -lt $names.Count;$i++){
                $row[$names[$i]]=[BitConverter]::ToUInt16($bytes,$offset+4+2*$i)
            }
            $records+=[pscustomobject]$row
        }
        $records|ConvertTo-Json|Set-Content "$root\guest-report.json"
        $last=$records[-1]
        if(($records.phase -join ',') -ne '0,1,1,1,2' -or
            @($records|Where-Object errors -ne 0).Count -or
            $last.created -ne 3 -or $last.closed -ne 2 -or $last.hooks -ne 3 -or
            $last.leaving -ne 1 -or !$last.task -or !$last.destroyResult -or
            $last.isWindow -ne 0 -or
            @($records|Where-Object task -ne $last.task).Count){
            throw 'Common-dialog creation/destroy/resource-exit contract incomplete'
        }
    } else {
    if($bytes.Length -ne 336){throw 'Unexpected dialog report size'}
    $names=@('phase','errors','initialized','terminated','text','shared','local',
        'modal','failed','killed','leaving','task','ds1','ds2','deferred',
        'tabFocus','defaultCommands','cancelCommands','editKeys')
    $records=@()
    for($offset=0;$offset -lt $bytes.Length;$offset+=42){
        if([Text.Encoding]::ASCII.GetString($bytes,$offset,4) -ne 'DLG1'){
            throw 'Bad dialog report signature'
        }
        $row=[ordered]@{}
        for($i=0;$i -lt $names.Count;$i++){
            $row[$names[$i]]=[BitConverter]::ToUInt16($bytes,$offset+4+2*$i)
        }
        $records+=[pscustomobject]$row
    }
    $records|ConvertTo-Json|Set-Content "$root\guest-report.json"
    $last=$records[-1]
    if(($records.phase -join ',') -ne '0,1,2,3,4,5,6,7' -or
            @($records|Where-Object errors -ne 0).Count -or
            $last.initialized -ne 6 -or $last.text -ne 6 -or
            $last.shared -ne 4 -or $last.local -ne 2 -or $last.modal -ne 2 -or
            $last.failed -ne 1 -or $last.killed -ne 1 -or $last.leaving -ne 1 -or
            $last.deferred -ne 2 -or $last.tabFocus -ne 2 -or
            $last.defaultCommands -ne 2 -or $last.cancelCommands -ne 2 -or
            $last.editKeys -ne 2 -or !$last.task -or !$last.ds1 -or
            $last.ds1 -ne $last.ds2 -or
            @($records|Where-Object task -ne $last.task).Count){
        throw 'Original dialog/control/cancel/EDIT/task contract incomplete'
    }
    }
    $trace=Get-Content "$root\lifetime.txt" -Raw
    $task8='{0:X8}' -f [int]$last.task
    if([regex]::Matches($trace,'(?m)^\d+ TaskRetirement .*task='+$task8+
            ' stage=complete status=0\r?$').Count -ne 1){
        throw 'Dialog task did not retire exactly once'
    }
    foreach($input in Import-Csv "$root\inputs.csv"){
        if((Get-FileHash -LiteralPath $input.Path).Hash -ne $input.Hash){
            throw "Input changed: $($input.Path)"
        }
    }
    $(if($CommonDialog){'WOW_COMMON_DIALOG_GUEST_OK created=3 destroyed=2 hooks=3 task_exit=1'}else{'WOW_DIALOG_GUEST_OK phases=8 init=6 modal=2 rollback=1 keyboard=2 retirement=1'})|
        Set-Content "$root\result.txt"
} catch {
    $_|Out-String|Set-Content "$root\failure.txt"
    throw
} finally {
    try {
        if($process){if(!$process.HasExited){$process.Kill();$process.WaitForExit(5000)|Out-Null};$process.Dispose()}
    } finally {
        try {if($scope){Stop-IsolatedPackageScope $scope}}
        finally {
            try {
                if($runtime){
                    $interim="$runtime/tests/$probeName.BIN"
                    if(Test-Path -LiteralPath $interim){Copy-Item $interim "$root/interim.bin" -Force}
                    Remove-PhysicalRuntimeStage $runtime
                }
            }
            finally {foreach($name in $saved.Keys){[Environment]::SetEnvironmentVariable($name,$saved[$name],'Process')}}
        }
    }
}
