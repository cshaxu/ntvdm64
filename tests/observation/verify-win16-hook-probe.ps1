[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$PackageRoot,
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$BuildRoot,
    [string]$Nasm='nasm.exe',
    [ValidateSet('hook','timer','idle','common-dialog','generic','shell','print','winsock','controls','dialog')][string]$Case='hook',
    [ValidateSet('Off','Win16','All')][string]$Win16Style='Off',
    [ValidateRange(1,32)][int]$CommonDialogRounds=1,
    [string]$DiagnosticProbeSource,
    [string]$CommonDialogIdentityHost
)
# Invoke in an owned background process; this runner never activates a desktop.
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){
    throw 'Require fresh build-owned evidence root'
}
$null=New-Item -ItemType Directory $root
. "$PSScriptRoot/isolated_package_cleanup.ps1"
$source=Join-Path $PSScriptRoot ('win16_'+$Case.Replace('-','_')+'_probe.asm')
if($CommonDialogIdentityHost){
    $CommonDialogIdentityHost=(Resolve-Path -LiteralPath $CommonDialogIdentityHost).Path
    if($Case -ne 'common-dialog' -or $DiagnosticProbeSource -or
            !$CommonDialogIdentityHost.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase)){
        throw 'Identity host requires a normal authored common-dialog probe and a build-owned DLL'
    }
}
if($DiagnosticProbeSource){
    $source=(Resolve-Path -LiteralPath $DiagnosticProbeSource).Path
    if($Case -ne 'common-dialog' -or !$source.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase)){
        throw 'Diagnostic source must be an independently authored common-dialog probe copy under build'
    }
}
$probe=if($Case -eq 'timer'){'TMRPROBE'}elseif($Case -eq 'idle'){'IDLPROBE'}elseif($Case -eq 'common-dialog'){'CDLPROBE'}elseif($Case -eq 'generic'){'GENPROBE'}elseif($Case -eq 'shell'){'SHLPROBE'}elseif($Case -eq 'print'){'PRNPROBE'}else{'CBTPROB'}
$process=$null;$runtime=$null;$scope=$null
if($Case -eq 'winsock'){$probe='WSKPROBE'}
if($Case -eq 'controls'){$probe='CTLPROBE'}
if($Case -eq 'dialog'){$probe='DLGPROBE'}
$saved=@{}
foreach($name in @('TEMP','TMP','PATH','MVDM_OBSERVER_PRIVATE_DESKTOP','MVDM_WOW_DISPATCH_TRACE_PATH',
        'MVDM_WOW_CALLBACK_FRAME_TRACE_PATH','MVDM_WOW_WINDOW_TRACE_PATH',
        'MVDM_WOW_CALLBACK_TRACE_PATH')){
    $saved[$name]=[Environment]::GetEnvironmentVariable($name)
}
try {
    $assembler=(Get-Command $Nasm -ErrorAction Stop).Source
    $defines=@()
    if($Case -eq 'common-dialog'){$defines+="-DCOMMON_DIALOG_ROUNDS=$CommonDialogRounds"}
    if($CommonDialogIdentityHost){$defines+='-DCOMMON_DIALOG_IDENTITIES'}
    if($Case -eq 'print'){$defines+=('-DOUTPUT_DIR="'+$root+'"')}
    & $assembler @defines -f bin -o "$root/$probe.EXE" -l "$root/probe.lst" $source *> "$root/assemble.txt"
    if($LASTEXITCODE){throw 'Independent NE probe assembly failed'}
    $runtime=New-PhysicalRuntimeStage (Resolve-Path $PackageRoot).Path
    $scope=New-IsolatedPackageScope $runtime
    $tests=Join-Path $runtime 'tests'
    $null=New-Item -ItemType Directory $tests -Force
    if($CommonDialogIdentityHost){
        Copy-Item $CommonDialogIdentityHost "$tests/CDLHOST.DLL"
        Get-FileHash $CommonDialogIdentityHost,"$PSScriptRoot/wow_common_dialog_identity.c"|
            Export-Csv -NoTypeInformation "$root/identity-inputs.csv"
    }
    Copy-Item "$root/$probe.EXE" "$tests/$probe.EXE"
    if($Case -eq 'winsock'){
        $inputDll="$runtime/system32/WINSOCK.DLL"
        if((Get-FileHash $inputDll).Hash -ne '629E86C59C83F66D3623388B9ED6AE9D5A664979E588A9EB45B8A54441126FD9'){
            throw 'Require approved original WINSOCK input'
        }
        Get-FileHash $inputDll | Select-Object Path,Hash | ConvertTo-Json | Set-Content "$root/winsock-input.json"
        $env:MVDM_WOW_CALLBACK_FRAME_TRACE_PATH="$root/callbacks.txt"
    }
    if($Case -eq 'print'){$env:MVDM_WOW_CALLBACK_FRAME_TRACE_PATH="$root/callbacks.txt"}
    if($Case -eq 'shell'){
        & $assembler -DCHILD -f bin -o "$root/SHLCHILD.EXE" -l "$root/child.lst" $source *> "$root/child-assemble.txt"
        if($LASTEXITCODE){throw 'Independent Shell child assembly failed'}
        Copy-Item "$root/SHLCHILD.EXE" "$tests/SHLCHILD.EXE"
        Get-FileHash "$root/SHLCHILD.EXE","$runtime/system32/SHELL.DLL" |
            Select-Object Path,Hash | ConvertTo-Json | Set-Content "$root/shell-inputs.json"
        $env:MVDM_WOW_CALLBACK_FRAME_TRACE_PATH="$root/callbacks.txt"
    }
    if($Case -eq 'generic'){
        $nativeSource="$PSScriptRoot/wow_generic_callback_probe.c"
        $vs='C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
        & cmd.exe /d /s /c ('call "'+$vs+'" -arch=x86 -host_arch=x64 >nul && cl.exe /nologo /LD /MT /O2 /Fo"'+$root+'/generic.obj" "'+$nativeSource+'" /link /OUT:"'+$root+'/GENHOST.DLL" /IMPLIB:"'+$root+'/GENHOST.lib" /EXPORT:RunGenericProbe=_RunGenericProbe@12 /EXPORT:ResolveGenericSegment=_ResolveGenericSegment@4') *> "$root/native-build.txt"
        if($LASTEXITCODE){throw 'Generic native witness build failed'}
        Copy-Item "$root/GENHOST.DLL" "$tests/GENHOST.DLL"
        @($nativeSource,"$root/GENHOST.DLL") | Get-FileHash |
            Select-Object Path,Hash | ConvertTo-Json | Set-Content "$root/native-inputs.json"
        $env:MVDM_WOW_CALLBACK_FRAME_TRACE_PATH="$root/callbacks.txt"
    }
    $inputs=@(@{File='probe-source';SHA256=(Get-FileHash $source).Hash},
        @{File='probe';SHA256=(Get-FileHash "$root/$probe.EXE").Hash})
    foreach($file in @('system32/run16.exe','system32/ntvdm.exe','system32/WOW32.DLL',
            'system32/KRNL386.EXE','system32/USER.EXE','system32/GDI.EXE')){
        $inputs+=@{File=$file;SHA256=(Get-FileHash (Join-Path $runtime $file)).Hash}
    }
    $inputs|ConvertTo-Json|Set-Content "$root/inputs.json"
    if($Case -eq 'common-dialog'){
        Get-FileHash "$runtime/system32/COMMDLG.DLL" | Select-Object Path,Hash |
            ConvertTo-Json | Set-Content "$root/commdlg-input.json"
    }
    if($Case -in @('common-dialog','controls','dialog')){
        # Existing default-off traces share one chronological stream. No
        # probe-specific runtime behavior or replacement guest is selected.
        $env:MVDM_WOW_CALLBACK_FRAME_TRACE_PATH="$root/lifetime.txt"
        $env:MVDM_WOW_WINDOW_TRACE_PATH="$root/lifetime.txt"
    }
    $env:TEMP=$runtime;$env:TMP=$runtime
    if($Case -eq 'idle'){$env:MVDM_WOW_WINDOW_TRACE_PATH="$root/lifetime.txt"}
    $env:PATH="$runtime\system32;"+$env:SystemRoot+'\System32'
    $env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
    if($Case -eq 'hook'){$env:MVDM_WOW_CALLBACK_TRACE_PATH="$root/wndproc.txt"}
    $env:MVDM_WOW_DISPATCH_TRACE_PATH="$root/dispatch.txt"
    $start=[Diagnostics.ProcessStartInfo]::new((Resolve-Path $Observer).Path)
    $start.UseShellExecute=$false;$start.CreateNoWindow=$true
    $launchArguments=@('--wait')
    if($Win16Style -eq 'Win16'){$launchArguments+='--win16-style'}
    elseif($Win16Style -eq 'All'){$launchArguments+='--win16-style-all'}
    foreach($arg in (@("$runtime\system32\run16.exe",$tests,"$root\observer.txt")+
            $launchArguments+@("$tests\$probe.EXE",'--observation-timeout-ms','30000'))){
        $start.ArgumentList.Add($arg)
    }
    $process=[Diagnostics.Process]::Start($start)
    $null=$process.Handle
    @{pid=$process.Id;start=[DateTime]::UtcNow;deadlineSeconds=35;arguments=@($start.ArgumentList)}|
        ConvertTo-Json|Set-Content "$root/observer-process.json"
    if(!$process.WaitForExit(35000)){throw 'Owned observer exceeded actual deadline'}
    $process.Refresh()
    if(Test-Path "$tests/$probe.BIN"){Copy-Item "$tests/$probe.BIN" "$root/guest-report.bin"}
    $report=Get-Content "$root/observer.txt" -Raw
    if($Case -eq 'controls' -and (Test-Path "$root/guest-report.bin")){
        # Decode short failure reports too: the last guest phase and exact
        # assertion identify the boundary, not an observer timeout guess.
        $controlBytes=[IO.File]::ReadAllBytes("$root/guest-report.bin")
        if($controlBytes.Length % 38){throw 'Truncated control witness record'}
        $controlRecords=@()
        $controlNames=@('phase','errors','created','button','single','multi','list',
            'combos','menus','destroyed','leaving','task','wordCalls','wordNested','wordActions','scroll','dropdown')
        for($offset=0;$offset -lt $controlBytes.Length;$offset+=38){
            if([Text.Encoding]::ASCII.GetString($controlBytes,$offset,4) -ne 'CTL1'){
                throw 'Bad control witness signature'
            }
            $row=[ordered]@{}
            for($i=0;$i -lt $controlNames.Count;$i++){
                $row[$controlNames[$i]]=[BitConverter]::ToUInt16($controlBytes,$offset+4+2*$i)
            }
            $controlRecords+=[pscustomobject]$row
        }
        $controlRecords|ConvertTo-Json|Set-Content "$root/guest-report.json"
    }
    if($Case -eq 'dialog' -and (Test-Path "$root/guest-report.bin")){
        $dialogBytes=[IO.File]::ReadAllBytes("$root/guest-report.bin")
        if($dialogBytes.Length % 42){throw 'Incomplete dialog DS report'}
        $dialogRecords=@()
        $names=@('phase','errors','initialized','terminated','text','shared','local','modal','failed','killed','leaving','task','ds1','ds2','deferred','tabFocus','defaultCommands','cancelCommands','editKeys')
        for($offset=0;$offset -lt $dialogBytes.Length;$offset+=42){
            if([Text.Encoding]::ASCII.GetString($dialogBytes,$offset,4) -ne 'DLG1'){throw 'Bad dialog DS signature'}
            $row=[ordered]@{}
            for($i=0;$i -lt $names.Count;$i++){$row[$names[$i]]=[BitConverter]::ToUInt16($dialogBytes,$offset+4+2*$i)}
            $dialogRecords+=[pscustomobject]$row
        }
        $dialogRecords|ConvertTo-Json|Set-Content "$root/guest-report.json"
    }
    if($process.ExitCode -or $report -notmatch 'result=exited' -or $report -notmatch 'exit=0x00000000'){
        throw 'Guest/launcher did not complete normally'
    }
    $bytes=[IO.File]::ReadAllBytes("$root/guest-report.bin")
    if($Case -eq 'dialog'){
        if($bytes.Length -ne 336 -or ($dialogRecords.phase -join ',') -ne '0,1,2,3,4,5,6,7' -or
                @($dialogRecords|Where-Object errors -ne 0).Count){throw 'Original dialog guest phase/assertion failure'}
        $last=$dialogRecords[-1]
        if($last.initialized -ne 6 -or $last.text -ne 6 -or $last.shared -ne 4 -or
            $last.local -ne 2 -or $last.modal -ne 2 -or $last.failed -ne 1 -or
            $last.killed -ne 1 -or $last.leaving -ne 1 -or $last.deferred -ne 2 -or
            $last.tabFocus -ne 2 -or $last.defaultCommands -ne 2 -or $last.cancelCommands -ne 2 -or $last.editKeys -ne 2 -or !$last.task -or !$last.ds1 -or
            $last.ds1 -ne $last.ds2 -or @($dialogRecords|Where-Object task -ne $last.task).Count){
            throw 'Original resource dialog/EDIT DS/modal/rollback/destruction contract incomplete'
        }
        $trace=Get-Content "$root/lifetime.txt" -Raw
        $task4='{0:X4}' -f [int]$last.task;$task8='{0:X8}' -f [int]$last.task
        $complete=[regex]::Matches($trace,'(?m)^\d+ TaskRetirement .*task='+$task8+' stage=complete status=0\r?$')
        if($complete.Count -ne 1){throw 'Dialog probe task did not retire exactly once'}
        # Four valid shared dialogs plus the failed-child rollback allocate
        # through original GetEditDS/W32GlobalAlloc16; all must retire before
        # task completion, including the one deliberately left alive.
        foreach($ret in @('000C','000F')){
            $callbacks=[regex]::Matches($trace,'(?m)^\d+ after ret='+$ret+' .*task='+$task4+' .*\r?$')
            if($callbacks.Count -lt 5 -or @($callbacks|Where-Object Index -gt $complete[0].Index).Count){
                throw 'Original shared dialog segment allocation/release callbacks missing or late'
            }
        }
        "PASS real Win16 resource modeless/modal dialogs, shared/private EDIT DS, initialization parameter, text/CRLF, failed-child rollback, init destruction compatibility and task exit ($Win16Style)"|Set-Content "$root/result.txt"
    }elseif($Case -eq 'controls'){
        if($bytes.Length -ne 342 -or ($controlRecords.phase -join ',') -ne '0,1,2,3,4,5,6,7,8' -or
                @($controlRecords|Where-Object errors -ne 0).Count){throw 'Control guest phase/assertion failure'}
        $last=$controlRecords[-1]
        if($last.created -ne 11 -or $last.button -ne 1 -or $last.single -ne 1 -or
                $last.multi -ne 1 -or $last.list -ne 1 -or $last.combos -ne 3 -or
                $last.menus -ne 1 -or $last.destroyed -ne 9 -or $last.leaving -ne 2 -or
                $last.scroll -ne 1 -or $last.dropdown -ne 2 -or
                !$last.wordCalls -or $last.wordNested -ne $last.wordCalls -or ($last.wordActions -band 3) -ne 3 -or
                !$last.task -or @($controlRecords|Where-Object task -ne $last.task).Count){
            throw 'Actual Win16 control/normal-destroy/task-exit contract incomplete'
        }
        $trace=Get-Content "$root/lifetime.txt" -Raw
        $task4='{0:X4}' -f [int]$last.task
        $task8='{0:X8}' -f [int]$last.task
        $complete=[regex]::Matches($trace,'(?m)^\d+ TaskRetirement .*task='+$task8+' stage=complete status=0\r?$')
        if($complete.Count -ne 1){throw 'Control witness task did not retire exactly once'}
        $words=[regex]::Matches($trace,'(?im)^\d+ after ret=0036 .*task='+$task4+' .*\r?$')
        if($words.Count -ne $last.wordCalls -or @($words|Where-Object Index -gt $complete[0].Index).Count){
            throw 'Original W32EditNextWord callbacks disagree with guest execution or retirement'
        }
        # Actual original EDIT LOCAL*16 callbacks, not merely native controls
        # with a similar appearance. Original wow.h RET_LOCALALLOC/FREE=6/11.
        foreach($ret in @('0006','000B')){
            $callbacks=[regex]::Matches($trace,'(?im)^\d+ after ret='+$ret+' .*task='+$task4+' .*\r?$')
            if(!$callbacks.Count -or @($callbacks|Where-Object Index -gt $complete[0].Index).Count){
                throw 'Original EDIT LocalAlloc/Free callback absent or after task retirement'
            }
        }
        "PASS actual Win16 original controls, all3 combo forms, dropdown capture/release/persistence, list scrolling, EDIT replacement/CRLF, menu mutation, ordinary destruction and task exit ($Win16Style)"|
            Set-Content "$root/result.txt"
    }elseif($Case -eq 'winsock'){
        $size=28
        if($bytes.Length % $size){throw 'Truncated Winsock report'}
        $records=@();$names=@('phase','errors','startup','calls','cancelled','cancelError','selected','restored','closed','freed','cleaned','task')
        for($offset=0;$offset -lt $bytes.Length;$offset+=$size){
            if([Text.Encoding]::ASCII.GetString($bytes,$offset,4) -ne 'WSK1'){throw 'Bad Winsock signature'}
            $row=[ordered]@{}
            for($i=0;$i -lt $names.Count;$i++){$row[$names[$i]]=[BitConverter]::ToUInt16($bytes,$offset+4+2*$i)}
            $records+=[pscustomobject]$row
        }
        $records|ConvertTo-Json|Set-Content "$root/guest-report.json"
        $last=$records[-1]
        if(($records.phase -join ',') -ne '0,1,2' -or @($records|Where-Object errors -ne 0).Count -or
                $last.startup -ne 0 -or $records[1].calls -lt 1 -or $last.calls -le $records[1].calls -or
                $last.cancelled -ne 1 -or $last.cancelError -ne 10004 -or $last.selected -ne 0 -or
                $last.restored -ne 1 -or $last.closed -ne 1 -or $last.freed -ne 1 -or
                $last.cleaned -ne 1 -or !$last.task){throw 'Winsock callback/return/cleanup failed'}
        $trace=Get-Content "$root/callbacks.txt" -Raw
        $entries=[regex]::Matches($trace,'(?m)^(\d+) before ret=0032 .*task=([0-9A-Fa-f]+) ')
        if($entries.Count -ne $last.calls){throw 'Original blocking-hook callback count mismatch'}
        foreach($entry in $entries){
            if([Convert]::ToInt32($entry.Groups[2].Value,16) -ne $last.task -or
                $trace -notmatch ('(?m)^'+$entry.Groups[1].Value+' after ret=0032 .*task='+$entry.Groups[2].Value+' ')){
                throw 'Original blocking callback process/task return mismatch'
            }
        }
        'PASS actual Win16 Winsock blocking hook normal/cancel/task identity/restore/socket and task-buffer cleanup'|Set-Content "$root/result.txt"
    }elseif($Case -eq 'print'){
        if($bytes.Length -ne 80){throw 'Expected four20-byte print records'}
        $records=@();$names=@('phase','errors','normalCalls','cancelCalls','cancelled','deleted','freed','task')
        for($record=0;$record -lt 4;$record++){
            $offset=20*$record
            if([Text.Encoding]::ASCII.GetString($bytes,$offset,4) -ne 'PRN1'){throw 'Bad print callback signature'}
            $row=[ordered]@{}
            for($i=0;$i -lt $names.Count;$i++){$row[$names[$i]]=[BitConverter]::ToUInt16($bytes,$offset+4+2*$i)}
            $records+=[pscustomobject]$row
        }
        $records|ConvertTo-Json|Set-Content "$root/guest-report.json"
        $last=$records[-1]
        if(($records.phase -join ',') -ne '0,1,2,2' -or @($records|Where-Object errors -ne 0).Count -or
                $last.normalCalls -lt 1 -or $last.cancelCalls -lt 1 -or $last.cancelled -ne 1 -or
                $last.deleted -ne 2 -or $last.freed -ne 1 -or !$last.task){throw 'Print callback/return/cleanup failed'}
        $pdf=[IO.File]::ReadAllBytes("$root/normal.pdf")
        if($pdf.Length -lt 5 -or [Text.Encoding]::ASCII.GetString($pdf,0,5) -ne '%PDF-'){
            throw 'Normal guest print produced no PDF'
        }
        if(Test-Path "$root/cancel.pdf"){throw 'Cancelled guest print unexpectedly produced output'}
        $trace=Get-Content "$root/callbacks.txt" -Raw
        $entries=[regex]::Matches($trace,'(?m)^(\d+) before ret=0022 .*task=([0-9A-Fa-f]+) ')
        if($entries.Count -ne ($last.normalCalls+$last.cancelCalls)){throw 'Original abort callback count mismatch'}
        foreach($entry in $entries){
            if([Convert]::ToInt32($entry.Groups[2].Value,16) -ne $last.task -or
                $trace -notmatch ('(?m)^'+$entry.Groups[1].Value+' after ret=0022 .*task='+$entry.Groups[2].Value+' ')){
                throw 'Original abort callback did not return to its task/thread'
            }
        }
        'PASS real Win16 print abort callback, DC/task identity, normal/cancel and cleanup'|Set-Content "$root/result.txt"
    }elseif($Case -eq 'shell'){
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
    }elseif($Case -eq 'generic'){
        if($bytes.Length -ne 54){throw 'Expected three18-byte generic callback records'}
        $records=@();$names=@('phase','errors','pascal','cdecl','nested','freed','selector')
        for($record=0;$record -lt 3;$record++){
            $offset=18*$record
            if([Text.Encoding]::ASCII.GetString($bytes,$offset,4) -ne 'GEN1'){throw 'Bad generic callback signature'}
            $row=[ordered]@{}
            for($i=0;$i -lt $names.Count;$i++){$row[$names[$i]]=[BitConverter]::ToUInt16($bytes,$offset+4+2*$i)}
            $records+=[pscustomobject]$row
        }
        $records|ConvertTo-Json|Set-Content "$root/guest-report.json"
        $last=$records[-1]
        if(($records.phase -join ',') -ne '0,1,2' -or @($records|Where-Object errors -ne 0).Count -or
                $records[1].pascal -ne 3 -or $records[1].cdecl -ne 1 -or
                $records[1].nested -ne 1 -or $records[1].freed -ne 0 -or
                $last.pascal -ne 3 -or $last.cdecl -ne 1 -or $last.nested -ne 1 -or $last.freed -ne 3){
            throw 'Generic callback nesting/stack convention/return/cleanup assertions failed'
        }
        $trace=Get-Content "$root/callbacks.txt" -Raw
        $address=('{0:X4}0000' -f [int]$last.selector)
        if(!$last.selector -or $trace -notmatch ('before ret=0024 .*param='+$address)){
            throw 'No original nonpresent-segment resolver callback for the probe segment'
        }
        'PASS original generic callbacks/PASCAL/CDECL/nested/invalid arguments/stack return/nonpresent segment resolution/null resolution/cleanup'|Set-Content "$root/result.txt"
    }elseif($Case -eq 'idle'){
        if($bytes.Length -ne 48){throw 'Expected three16-byte idle-hook records'}
        $records=@();$names=@('phase','errors','idles','ticks','restored','task')
        for($record=0;$record -lt 3;$record++){
            $offset=16*$record
            if([Text.Encoding]::ASCII.GetString($bytes,$offset,4) -ne 'IDL1'){throw 'Bad idle-hook signature'}
            $row=[ordered]@{}
            for($i=0;$i -lt $names.Count;$i++){$row[$names[$i]]=[BitConverter]::ToUInt16($bytes,$offset+4+2*$i)}
            $records+=[pscustomobject]$row
        }
        $records|ConvertTo-Json|Set-Content "$root/guest-report.json"
        $last=$records[-1]
        if(($records.phase -join ',') -ne '0,1,2' -or @($records|Where-Object errors -ne 0).Count -or
                $records[1].idles -lt 1 -or $last.ticks -ne 2 -or $last.restored -ne 1 -or !$last.task){
            throw 'Idle callback/wait/vector restoration assertions failed'
        }
        $trace=Get-Content "$root/lifetime.txt" -Raw
        $task8='{0:X8}' -f [int]$last.task
        if($trace -notmatch ('task='+$task8+' stage=idle-dispatch status=0')){
            throw 'Guest idle callback was not observed through original taskman'
        }
        if([regex]::Matches($trace,'task='+$task8+' stage=complete status=0').Count -ne 1){
            throw 'Idle probe task did not retire exactly once'
        }
        'PASS real Win16 INT2F idle registration/taskman callback/vector restoration/retirement'|Set-Content "$root/result.txt"
    }elseif($Case -eq 'common-dialog'){
        $recordSize=if($CommonDialogIdentityHost){28}else{24}
        $signature=if($CommonDialogIdentityHost){'CDL3'}else{'CDL2'}
        if(!$bytes.Length -or $bytes.Length % $recordSize){throw 'Incomplete COMMDLG lifetime record'}
        $records=@();$names=@('phase','errors','created','closed','hooks','leaving','task','dialog','destroyAX','isWindowAX')
        if($CommonDialogIdentityHost){$names+=@('identityChecks','reused')}
        for($record=0;$record -lt ($bytes.Length / $recordSize);$record++){
            $offset=$recordSize*$record
            if([Text.Encoding]::ASCII.GetString($bytes,$offset,4) -ne $signature){throw 'Bad COMMDLG signature'}
            $row=[ordered]@{}
            for($i=0;$i -lt $names.Count;$i++){$row[$names[$i]]=[BitConverter]::ToUInt16($bytes,$offset+4+2*$i)}
            $records+=[pscustomobject]$row
        }
        $records|ConvertTo-Json|Set-Content "$root/guest-report.json"
        # Decode short failure reports before enforcing the successful shape.
        # 0101: guest DestroyWindow returned FALSE; 0102: IsWindow stayed TRUE.
        if($bytes.Length -ne $recordSize*($CommonDialogRounds+3)){throw 'Unexpected COMMDLG lifetime record count; see guest-report.json'}
        if($records[1].phase -ne 1 -or $records[1].created -ne 0 -or
                $records[1].closed -ne 0 -or $records[1].hooks -ne 0 -or
                $records[1].task -ne $records[0].task){throw 'COMMDLG owner creation checkpoint failed'}
        $last=$records[-1]
        if($records[0].phase -ne 0 -or $last.phase -ne 2 -or @($records|Where-Object errors -ne 0).Count -or
                $last.created -ne $CommonDialogRounds+1 -or $last.closed -ne $CommonDialogRounds -or
                $last.hooks -ne $CommonDialogRounds+1 -or $last.leaving -ne 1 -or !$last.task){throw 'COMMDLG normal/exit phase assertions failed'}
        for($round=1;$round -le $CommonDialogRounds;$round++){
            $row=$records[$round+1]
            if($row.phase -ne 1 -or $row.created -ne $round -or $row.closed -ne $round -or
                    $row.hooks -ne $round -or !$row.dialog -or !$row.destroyAX -or
                    (!$CommonDialogIdentityHost -and $row.isWindowAX -ne 0) -or
                    $row.task -ne $last.task){throw "COMMDLG destruction round $round failed"}
            if($CommonDialogIdentityHost -and ($row.identityChecks -ne $round -or $row.reused -gt $round)){
                throw "COMMDLG identity witness round $round incomplete"
            }
        }
        if($CommonDialogIdentityHost){
            Copy-Item "$tests/CDLID.BIN" "$root/native-identities.bin"
            $native=[IO.File]::ReadAllBytes("$root/native-identities.bin")
            if($native.Length -ne 128*$CommonDialogRounds){throw 'Missing paired native identities'}
            $observed=@();$reused=0
            for($round=1;$round -le $CommonDialogRounds;$round++){
                $pair=@()
                foreach($side in @(0,1)){
                    $offset=128*($round-1)+64*$side
                    $pair+=,@(for($word=0;$word -lt 16;$word++){[BitConverter]::ToUInt32($native,$offset+4*$word)})
                }
                $before=$pair[0];$after=$pair[1];$guest=$records[$round+1]
                $operation=if($guest.isWindowAX){3}else{2}
                if($before[0] -ne 0x31444943 -or $after[0] -ne 0x31444943 -or
                        $before[1] -ne 1 -or $after[1] -ne $operation -or
                        $before[2] -ne $guest.dialog -or $after[2] -ne $guest.dialog -or
                        !$before[3] -or $before[6] -ne 1 -or $before[7] -ne $before[10] -or
                        $before[8] -ne $before[9] -or $before[11] -ne 0x447a0001 -or
                        $after[4] -ne $before[3] -or $after[5] -ne 0 -or
                        $after[7] -ne $before[7] -or $after[8] -ne $before[8] -or
                        $after[11] -ne (0x447a0000 -bor $operation)){
                    throw "Native old-identity destruction failed in round $round"
                }
                if($operation -eq 3){
                    if(!$after[3] -or $after[3] -eq $before[3] -or $after[6] -ne 1 -or
                            ($after[3] -band 0xffff) -ne $guest.dialog){throw 'Guest TRUE lacks a proved new live generation'}
                    ++$reused
                }
                $observed+=@{round=$round;before=$before;after=$after;guestIsWindow=$guest.isWindowAX}
            }
            $observed|ConvertTo-Json -Depth 6|Set-Content "$root/native-identities.json"
            if($last.identityChecks -ne $CommonDialogRounds -or $last.reused -ne $reused){throw 'Native/guest identity accounting differs'}
        }
        $trace=Get-Content "$root/lifetime.txt" -Raw
        $task4='{0:X4}' -f [int]$last.task
        $task8='{0:X8}' -f [int]$last.task
        $complete=[regex]::Matches($trace,'(?m)^\d+ TaskRetirement .*task='+$task8+' stage=complete status=0\r?$')
        if($complete.Count -ne 1){throw 'Expected exactly one actual target task retirement'}
        foreach($ret in @('0014','0012')){
            $callbacks=[regex]::Matches($trace,'(?m)^\d+ after ret='+$ret+' .*task='+$task4+' .*\r?$')
            if($callbacks.Count -lt $CommonDialogRounds+1 -or @($callbacks|Where-Object Index -gt $complete[0].Index).Count){
                throw 'Original resource unlock/free callbacks absent or after task retirement'
            }
        }
        'PASS real Win16 modeless resource normal/exit cleanup callbacks before exact-once task retirement' |
            Set-Content "$root/result.txt"
    }elseif($Case -eq 'timer'){
        if($bytes.Length -ne 66){throw 'Expected three22-byte timer/font phase records'}
        $records=@();$names=@('phase','errors','ordinary','modal','killed','modal_result','freed','fonts','font_reads')
        for($record=0;$record -lt 3;$record++){
            $offset=22*$record
            if([Text.Encoding]::ASCII.GetString($bytes,$offset,4) -ne 'TMR1'){throw 'Bad timer record signature'}
            $row=[ordered]@{}
            for($i=0;$i -lt $names.Count;$i++){$row[$names[$i]]=[BitConverter]::ToUInt16($bytes,$offset+4+2*$i)}
            $records+=[pscustomobject]$row
        }
        $records|ConvertTo-Json|Set-Content "$root/guest-report.json"
        if(($records.phase -join ',') -ne '0,1,2' -or @($records|Where-Object errors -ne 0).Count){throw 'Timer phase/failure mismatch'}
        $last=$records[-1]
        if($records[1].ordinary -ne 1 -or $records[1].modal -ne 0 -or
                $last.ordinary -ne 1 -or $last.modal -ne 1 -or $last.killed -ne 2 -or
                $last.modal_result -ne 1 -or $last.freed -ne 1 -or
                $records[1].fonts -ne 1 -or $records[1].font_reads -ne 1 -or
                $last.fonts -ne 2 -or $last.font_reads -ne 2){throw 'Timer/font ordinary/modal/nested/cancellation/cleanup assertions failed'}
        'PASS real Win16 timer ordinary/modal dispatch, nested font callbacks and guest buffers, cancellation and release'|Set-Content "$root/result.txt"
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
            $last.vetoed -ne 1 -or $last.destroyed -ne 3 -or !$last.unhooked -or $last.freed -ne 1){
        throw 'Guest callback/nesting/veto/recovery/cleanup assertions failed'
    }
    'PASS real Win16 CBT query/nested creation/veto/recovery/unhook and launcher completion'|Set-Content "$root/result.txt"
    if($Case -eq 'hook'){
        $trace=Get-Content "$root/wndproc.txt" -Raw
        $entered=[regex]::Matches($trace,'(?m)^\d+ W32WndProc-enter message=([0-9A-F]{4}) raw=[0-9A-F]{8} proc=([0-9A-F]{8})\r?$')
        $returned=[regex]::Matches($trace,'(?m)^\d+ W32WndProc message=([0-9A-F]{4}) proc=([0-9A-F]{8}) success=[01] result=[0-9A-F]{8}\r?$')
        $pairs=@($returned|Where-Object {
            $return=$_
            @($entered|Where-Object {$_.Index -lt $return.Index -and
                $_.Groups[1].Value -eq $return.Groups[1].Value -and
                $_.Groups[2].Value -eq $return.Groups[2].Value}).Count
        })
        if(!$pairs.Count){throw 'Actual original window callback did not reach both owned diagnostics'}
        @{entries=$entered.Count;returns=$returned.Count;matchingWitnesses=$pairs.Count;
          limit='Matching actual invocation witnesses, not whole-process atomic trace pairing'}|
            ConvertTo-Json|Set-Content "$root/wndproc-witness.json"
    }
    }
    0|Set-Content "$root/result.exit"
} catch {
    $_|Out-String|Set-Content "$root/failure.txt"
    1|Set-Content "$root/result.exit"
    throw
} finally {
    try {
        if($process){if(!$process.HasExited){$process.Kill();$process.WaitForExit(5000)|Out-Null};$process.Dispose()}
        if($runtime -and (Test-Path "$runtime/tests/$probe.BIN")){
            Copy-Item "$runtime/tests/$probe.BIN" "$root/guest-report.bin" -Force
        }
    } finally {
        try {if($scope){Stop-IsolatedPackageScope $scope}}
        finally {
            try {if($runtime){Remove-PhysicalRuntimeStage $runtime}}
            finally {foreach($name in $saved.Keys){[Environment]::SetEnvironmentVariable($name,$saved[$name])}}
        }
    }
}
