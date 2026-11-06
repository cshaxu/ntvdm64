[CmdletBinding()]
param([Parameter(Mandatory)][string]$PackageRoot,
 [Parameter(Mandatory)][string]$Observer,
 [Parameter(Mandatory)][string]$LogonHelper,
 [Parameter(Mandatory)][string]$BuildRoot,
 [ValidateSet('shell','hook')][string]$Case='hook',
 [switch]$LeaveHookInstalled)
# Restores only the shell/hook contracts of the removed pre-db91004e5 runner.
# No Win16 Style modes or original guest build. Owned background invocation only.
$ErrorActionPreference='Stop'
if($LeaveHookInstalled -and $Case -ne 'hook'){throw 'LeaveHookInstalled requires hook case'}
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned root required'}
$null=New-Item -ItemType Directory $root
. "$PSScriptRoot/isolated_package_cleanup.ps1"
function Owned($name,$exe,$line,$seconds){
 $p=Start-Process $exe -ArgumentList $line -WindowStyle Hidden -WorkingDirectory $repo -PassThru -RedirectStandardOutput "$root/$name.out" -RedirectStandardError "$root/$name.err";$null=$p.Handle
 @{pid=$p.Id;start=$p.StartTime.ToUniversalTime().ToString('o');command=$line;timeoutSeconds=$seconds}|ConvertTo-Json|Set-Content "$root/$name.process.json"
 if(!$p.WaitForExit($seconds*1000)){taskkill.exe /PID $p.Id /T /F;throw "Owned deadline $name"}
 $p.WaitForExit();$code=$p.ExitCode;$p.Dispose();$code|Set-Content "$root/$name.exit";if($code){throw "$name failed"}
}
$runtime=$null;$scope=$null;$process=$null
try {
 Owned token whoami.exe '/groups /fo csv /nh' 15
 $label=[regex]::Match((Get-Content "$root/token.out" -Raw),'S-1-16-\d+').Value
 if($label -ne 'S-1-16-8192'){throw 'Require desktop-capable medium token'}
 @{pid=$PID;host=(Get-Process -Id $PID).Path;integrity=$label;script_sha256=(Get-FileHash $PSCommandPath).Hash}|ConvertTo-Json|Set-Content "$root/runner-context.json"
 $source="$PSScriptRoot/win16_${Case}_probe.asm"
 $probe=if($Case -eq 'shell'){'SHLPROBE'}else{'CBTPROB'}
 $assembler=(Get-Command nasm.exe).Source
 $defines=if($LeaveHookInstalled){'-DHOOK_LEAVE_INSTALLED '}else{''}
 Owned assemble $assembler ($defines+'-f bin "'+$source+'" -o "'+$root+'/'+$probe+'.EXE"') 30
 $runtime=New-PhysicalRuntimeStage (Resolve-Path $PackageRoot).Path -FreshPackageIdentity
 Save-IsolatedPackageManifest $runtime "$root/manifest.json"
 $scope=New-IsolatedPackageScope $runtime
 $tests=Join-Path $runtime 'tests';$null=New-Item -ItemType Directory $tests -Force
 Copy-Item "$root/$probe.EXE" "$tests/$probe.EXE"
 if($Case -eq 'shell'){
  Owned child-assemble $assembler ('-DCHILD -f bin "'+$source+'" -o "'+$root+'/SHLCHILD.EXE"') 30
  Copy-Item "$root/SHLCHILD.EXE" "$tests/SHLCHILD.EXE"
 }
 Get-FileHash $source,$PSCommandPath,$Observer,$LogonHelper,$assembler,"$root/$probe.EXE"|Export-Csv -NoTypeInformation "$root/inputs.csv"
 Get-ChildItem "$runtime/system32" -File|Get-FileHash|Export-Csv -NoTypeInformation "$root/runtime-inputs.csv"
 Add-Type -Path (Resolve-Path $LogonHelper).Path
 $identity=[Security.Principal.WindowsIdentity]::GetCurrent()
 try{$entries=[WowMessageLogonEnvironment]::Read($identity.Token)}finally{$identity.Dispose()}
 $info=[Diagnostics.ProcessStartInfo]::new((Resolve-Path $Observer).Path);$info.UseShellExecute=$false;$info.CreateNoWindow=$true
 $info.EnvironmentVariables.Clear()
 foreach($entry in $entries){$at=$entry.IndexOf('=',1);$info.EnvironmentVariables[$entry.Substring(0,$at)]=$entry.Substring($at+1)}
 $info.EnvironmentVariables['TEMP']=$runtime;$info.EnvironmentVariables['TMP']=$runtime
 $info.EnvironmentVariables['MVDM_OBSERVER_PRIVATE_DESKTOP']='1'
 $info.EnvironmentVariables['MVDM_WOW_CALLBACK_FRAME_TRACE_PATH']="$root/callbacks.txt"
 $info.EnvironmentVariables['MVDM_WOW_WINDOW_TRACE_PATH']="$root/windows.txt"
 $info.EnvironmentVariables.Keys|ForEach-Object {[pscustomobject]@{name=$_;characters=$_.Length+$info.EnvironmentVariables[$_].Length+2}}|Export-Csv -NoTypeInformation "$root/environment-shape.csv"
 foreach($arg in @("$runtime/system32/run16.exe",$tests,"$root/observer.txt",'--wait',"$tests/$probe.EXE",'--observation-timeout-ms','45000')){$info.ArgumentList.Add($arg)}
 $process=[Diagnostics.Process]::Start($info);$null=$process.Handle
 @{pid=$process.Id;start=$process.StartTime.ToUniversalTime().ToString('o');arguments=@($info.ArgumentList);timeoutSeconds=55}|ConvertTo-Json|Set-Content "$root/observer.process.json"
 if(!$process.WaitForExit(55000)){throw 'Owned guest observer deadline'}
 $process.WaitForExit();$process.ExitCode|Set-Content "$root/observer.exit"
 $report=Get-Content "$root/observer.txt" -Raw
 if($process.ExitCode -or $report -notmatch 'result=exited' -or $report -notmatch 'exit=0x00000000'){throw 'Guest/launcher did not complete normally'}
 Copy-Item "$tests/$probe.BIN" "$root/guest-report.bin"
 $bytes=[IO.File]::ReadAllBytes("$root/guest-report.bin")
 if($Case -eq 'shell'){
        if($bytes.Length -ne 36){throw 'Expected three12-byte Shell records'}
        $records=@()
        for($i=0;$i -lt 3;$i++){
            $o=12*$i
            if([Text.Encoding]::ASCII.GetString($bytes,$o,4) -ne 'SHL1'){throw 'Bad Shell record'}
            $records += [pscustomobject]@{phase=[BitConverter]::ToUInt16($bytes,$o+4);
                errors=[BitConverter]::ToUInt16($bytes,$o+6);success=[BitConverter]::ToUInt16($bytes,$o+8);
                failure=[BitConverter]::ToUInt16($bytes,$o+10)}
        }
        $records | ConvertTo-Json | Set-Content "$root/guest-report.json"
        if(($records.phase -join ',') -ne '0,1,2' -or @($records|Where-Object errors -ne 0).Count -or
                $records[-1].success -le 32 -or $records[-1].failure -gt 32){throw 'Shell result/continuation failed'}
        if(!(Test-Path "$tests/SHLCHILD.BIN")){throw 'Shell child did not run'}
        Copy-Item "$tests/SHLCHILD.BIN" "$root/child-report.bin"
        $child=[IO.File]::ReadAllBytes("$root/child-report.bin")
        if($child.Length -ne 12 -or [Text.Encoding]::ASCII.GetString($child,0,4) -ne 'SHL1' -or
                [BitConverter]::ToUInt16($child,4) -ne 3){throw 'Shell child did not finish report'}
        $callbackTrace=Get-Content "$root/callbacks.txt" -Raw
        $entry=[regex]::Match($callbackTrace,'(?m)^(\d+) before ret=0037 .*task=([0-9A-Fa-f]+) ')
        if(!$entry.Success){throw 'No original guest RET_WINEXEC callback'}
        $returnPattern='(?m)^'+$entry.Groups[1].Value+' after ret=0037 .*task='+$entry.Groups[2].Value+' '
        if($callbackTrace -notmatch $returnPattern){throw 'Shell callback did not return on its original task/thread'}
        'PASS actual guest ShellExecute success/failure, child execution and parent continuation'|Set-Content "$root/result.txt"
 }else{
    if($bytes.Length -ne 600){throw "Expected four22-byte phase records and512-byte class history; got $($bytes.Length)"}
    @(0..15|ForEach-Object {[Text.Encoding]::ASCII.GetString($bytes,88+32*$_,32).Split([char]0)[0]})|
        ConvertTo-Json|Set-Content "$root/observed-classes.json"
    $records=@()
    $names=@('phase','errors','created','queried','nested','vetoed','destroyed','unhooked','freed')
    for($record=0;$record -lt 4;$record++){
        $offset=22*$record
        if([Text.Encoding]::ASCII.GetString($bytes,$offset,4) -ne 'CBT1'){throw 'Bad guest record signature'}
        $row=[ordered]@{}
        for($i=0;$i -lt $names.Count;$i++){$row[$names[$i]]=[BitConverter]::ToUInt16($bytes,$offset+4+2*$i)}
        $records+=[pscustomobject]$row
    }
    $records|ConvertTo-Json|Set-Content "$root/guest-report.json"
    if(($records.phase -join ',') -ne '0,1,2,4' -or @($records|Where-Object errors -ne 0).Count){throw 'Guest phase/failure mismatch'}
    $last=$records[-1]
    if($last.created -ne 4 -or $last.queried -ne 4 -or $last.nested -ne 1 -or
            $last.vetoed -ne 1 -or $last.destroyed -ne 3 -or $last.unhooked -ne [int](!$LeaveHookInstalled) -or $last.freed -ne [int](!$LeaveHookInstalled)){
        throw 'Guest callback/nesting/veto/recovery/cleanup assertions failed'
    }
    if($LeaveHookInstalled){'PASS real Win16 CBT left installed until task exit; normal launcher completion (slot state requires separate owner proof)'|Set-Content "$root/result.txt"}
    else{'PASS real Win16 CBT query/nested creation/veto/recovery/unhook and launcher completion'|Set-Content "$root/result.txt"}

 }
 '0'|Set-Content "$root/exit.txt"
} catch { $_|Out-String|Set-Content "$root/failure.txt";'1'|Set-Content "$root/exit.txt";throw }
finally {
 if($process){if(!$process.HasExited){taskkill.exe /PID $process.Id /T /F | Out-Null};$process.Dispose()}
 try {if($scope){Stop-IsolatedPackageScope $scope}}finally {
  # Retain failed physical stages for diagnosis; helper validates successful deletion.
  if($runtime -and (Test-Path "$root/exit.txt") -and (Get-Content "$root/exit.txt") -eq '0'){Remove-PhysicalRuntimeStage $runtime}
 }
}
