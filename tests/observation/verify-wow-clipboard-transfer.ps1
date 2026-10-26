[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$PackageRoot,
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$CompilerDriver,
    [Parameter(Mandatory)][string]$BuildRoot,
    [string]$FrozenInputsRoot,
    [switch]$SharedClipboard,
    [switch]$CallbackReturnOnly,
    [switch]$OwnerTaskExit,
    [string]$Nasm='nasm.exe'
)

# An isolated, independent Win16/native peer witness. It stages an exact
# package and test-only NE/DLL inputs, never edits guest media and refuses a
# fallback to the user's interactive Clipboard.
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'
if($CallbackReturnOnly -and $OwnerTaskExit){throw 'Select one Clipboard lifetime witness'}
if($FrozenInputsRoot -and $OwnerTaskExit){throw 'Task-exit witness requires its freshly assembled inputs'}
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$root=[IO.Path]::GetFullPath((Join-Path $repo $BuildRoot))
function ConvertTo-Win32CommandLineArgument([string]$Value) {
    # The retained product harness runs on Windows PowerShell/.NET Framework,
    # where ProcessStartInfo.ArgumentList is unavailable.  This test must use
    # the same explicit quoting contract as the other observation runners.
    if($Value -notmatch '[\s"]'){return $Value}
    return '"'+(($Value -replace '(\\*)"','$1$1\"') -replace '(\\+)$','$1$1')+'"'
}
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or
        (Test-Path -LiteralPath $root)){throw 'Require a fresh build-owned evidence root'}
$null=New-Item -ItemType Directory -Path $root
. "$PSScriptRoot/isolated_package_cleanup.ps1"
$runtime=$null;$scope=$null;$process=$null
$saved=@{}
foreach($name in @('PATH','TEMP','TMP','MVDM_OBSERVER_PRIVATE_DESKTOP',
        'MVDM_TERMINATE_VDM_REPORT_PATH','MVDM_WOW_WINDOW_TRACE_PATH',
        'MVDM_WOW_CALLBACK_TRACE_PATH','MVDM_WOW_CALLBACK_FRAME_TRACE_PATH',
        'MVDM_WOW_CALLBACK_SCOPE_TRACE_PATH',
        'MVDM_WOW_CALLBACK_CPU_TRACE_PATH',
        'MVDM_WOW_DISPATCH_TRACE_PATH',
        'MVDM_WOW_CLIPBOARD_BRIDGE_TRACE_PATH')){
    $saved[$name]=[Environment]::GetEnvironmentVariable($name,'Process')
}
try {
    $package=(Resolve-Path -LiteralPath $PackageRoot).Path
    $observer=(Resolve-Path -LiteralPath $Observer).Path
    $driver=(Resolve-Path -LiteralPath $CompilerDriver).Path
    $packageImages=@(Get-Content "$package/product-system32-manifest.json" -Raw | ConvertFrom-Json | ForEach-Object {
        if($_.RelativePath -ine ('system32\'+$_.Name) -or [IO.Path]::GetFileName($_.Name) -ne $_.Name){
            throw 'Invalid Clipboard package image path'
        }
        $image=Join-Path $package $_.RelativePath
        if((Get-FileHash -LiteralPath $image).Hash -ne $_.Sha256){
            throw "Clipboard package image differs from manifest: $($_.Name)"
        }
        $image
    })
    $assembly="$PSScriptRoot/win16_clipboard_probe.asm"
    $native="$PSScriptRoot/wow_clipboard_native_bridge.c"
    if($FrozenInputsRoot){
        $frozen=(Resolve-Path -LiteralPath $FrozenInputsRoot).Path
        $manifest=Join-Path $frozen 'inputs.csv'
        if(!(Test-Path -LiteralPath $manifest)){throw 'Frozen Clipboard inputs require their sealed inputs.csv'}
        $inputs=Import-Csv $manifest
        foreach($source in @($assembly,$native)){
            $row=@($inputs|Where-Object { [IO.Path]::GetFullPath($_.Path) -eq [IO.Path]::GetFullPath($source) })
            if($row.Count -ne 1 -or (Get-FileHash -LiteralPath $source).Hash -ne $row[0].Hash){
                throw "Frozen Clipboard input does not match current immutable source: $source"
            }
        }
        foreach($name in @('CLIPPRB.EXE','GENHOST.DLL')){
            $source=Join-Path $frozen $name
            $row=@($inputs|Where-Object { [IO.Path]::GetFullPath($_.Path) -eq [IO.Path]::GetFullPath($source) })
            if(!(Test-Path -LiteralPath $source) -or $row.Count -ne 1 -or
                    (Get-FileHash -LiteralPath $source).Hash -ne $row[0].Hash){
                throw "Frozen Clipboard binary is absent or differs from its sealed manifest: $name"
            }
            Copy-Item -LiteralPath $source -Destination "$root\$name"
        }
        'PASS reused immutable checked Clipboard probe and native bridge' | Set-Content "$root\frozen-inputs.txt"
    } else {
        $assembler=(Get-Command $Nasm -ErrorAction Stop).Source
        $assemblyArgs=@('-f','bin','-o',"$root\CLIPPRB.EXE",'-l',"$root\clipprobe.lst")
        if($CallbackReturnOnly){$assemblyArgs+='-d';$assemblyArgs+='CALLBACK_RETURN_ONLY'}
        if($OwnerTaskExit){$assemblyArgs+='-d';$assemblyArgs+='OWNER_TASK_EXIT'}
        $assemblyArgs+=$assembly
        & $assembler @assemblyArgs `
            *> "$root\assemble.txt"
        if($LASTEXITCODE){throw 'Clipboard NE probe assembly failed'}
        Push-Location $root
        try {
            # The repository compiler driver is a batch file and deliberately
            # returns with ``exit /b``.  Invoke it through a child cmd.exe with
            # CALL so that the PowerShell witness continues to the runtime phase.
            $nativeCommand = 'call "{0}" cl.exe /nologo /LD /MT /O2 /W4 /WX /D_CRT_SECURE_NO_WARNINGS "{1}" "{2}" "{3}" /link "{4}" user32.lib /EXPORT:ClipboardBridge=_ClipboardBridge@12' -f `
                $driver, "/Fo$root\cliphost.obj", "/Fe$root\GENHOST.DLL", $native, "/IMPLIB:$root\GENHOST.lib"
            & $env:ComSpec /d /c $nativeCommand *> "$root\native-build.txt"
            if($LASTEXITCODE){throw "Clipboard native bridge build failed: $LASTEXITCODE"}
            if($OwnerTaskExit){
                $retirementCommand='call "{0}" cl.exe /nologo /MT /O2 /W4 /WX /D_CRT_SECURE_NO_WARNINGS /DCLIPBOARD_RETIREMENT_WITNESS "{1}" "{2}" "{3}" /link user32.lib' -f $driver,"/Fo$root\retirement.obj","/Fe$root\CLIPRET.EXE",$native
                & $env:ComSpec /d /c $retirementCommand *> "$root\retirement-build.txt"
                if($LASTEXITCODE){throw 'Clipboard retirement observer build failed'}
            }
        } finally {Pop-Location}
    }
    $fixtureInputs=@($PSCommandPath,$assembly,$native,"$root\CLIPPRB.EXE",
        "$root\GENHOST.DLL",$observer,$driver,"$package\product-system32-manifest.json") + $packageImages
    if($OwnerTaskExit){$fixtureInputs+="$root\CLIPRET.EXE"}
    Get-FileHash -LiteralPath $fixtureInputs |
        Select-Object Path,Hash | Export-Csv -NoTypeInformation "$root\inputs.csv"
    # This witness launches only the staged product binaries, the package's
    # system configuration and its freshly-created probe files.  Do not copy
    # unrelated root-level games, documents or historical assets into the
    # short physical stage: they are neither loader inputs nor part of the
    # Clipboard contract.
    $runtime=New-PhysicalRuntimeStage $package -TopLevelNames @('system32','system.ini')
    $scope=New-IsolatedPackageScope $runtime
    $null=New-Item -ItemType Directory -Force -Path "$runtime\tests"
    Copy-Item "$root\CLIPPRB.EXE","$root\GENHOST.DLL" -Destination "$runtime\tests"
    # Original LoadLibraryEx32W resolves its native peer through the worker
    # system directory.  Keep the same disposable DLL beside the guest input
    # for provenance and add the loader-visible copy only inside this owned
    # physical test stage.
    Copy-Item "$root\GENHOST.DLL" -Destination "$runtime\system32\GENHOST.DLL"
    # Preserve the original compact child environment used by this fixture.
    # The immutable peer is in the owned package system directory; neither a
    # copied desktop-token environment nor the runner's toolchain PATH is a
    # Clipboard behavior input.
    $env:TEMP=$runtime;$env:TMP=$runtime
    $env:PATH="$runtime\system32;"+$env:SystemRoot+'\System32'
    # The normal fixture is deliberately isolated from the interactive
    # Clipboard.  A separately authorized shared-desktop run is needed to
    # validate real delayed rendering, because a private desktop is denied
    # access to the public Clipboard by the host.
    if($SharedClipboard){
        Remove-Item Env:MVDM_OBSERVER_PRIVATE_DESKTOP -ErrorAction SilentlyContinue
    } else {
        $env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
    }
    $env:MVDM_TERMINATE_VDM_REPORT_PATH="$root\termination.txt"
    # Preserve the production adapter's opt-in retirement trace outside the
    # disposable runtime stage, so a failed task-loss witness remains
    # diagnosable after the stage is removed.
    $env:MVDM_WOW_WINDOW_TRACE_PATH="$root\window-trace.txt"
    $env:MVDM_WOW_CALLBACK_TRACE_PATH="$root\callback-trace.txt"
    $env:MVDM_WOW_CALLBACK_FRAME_TRACE_PATH="$root\callback-frame-trace.txt"
    $env:MVDM_WOW_CALLBACK_SCOPE_TRACE_PATH="$root\callback-scope-trace.txt"
    # Existing default-off CPU40 FastBOP witness.  It is diagnostic-only and
    # lets this bounded reentrant-callback fixture attribute every redirected
    # dispatch/return endpoint without changing the callback route.
    $env:MVDM_WOW_CALLBACK_CPU_TRACE_PATH="$root\callback-cpu-trace.txt"
    $env:MVDM_WOW_DISPATCH_TRACE_PATH="$root\dispatch-trace.txt"
    # Keep the native peer's public-Clipboard failure evidence beside the
    # original-task report.  The bridge remains observational: it never
    # changes Clipboard ownership or product behavior outside its declared
    # witness operations.
    $env:MVDM_WOW_CLIPBOARD_BRIDGE_TRACE_PATH="$root\native-bridge-trace.txt"
    $start=[Diagnostics.ProcessStartInfo]::new($observer)
    $start.UseShellExecute=$false;$start.CreateNoWindow=$true
    $observerArguments=@("$runtime\system32\run16.exe","$runtime\tests",
        "$root\observer.txt",'--wait',"$runtime\tests\CLIPPRB.EXE",
        '--observation-timeout-ms','10000')
    $start.Arguments=($observerArguments | ForEach-Object {
        ConvertTo-Win32CommandLineArgument ([string]$_)
    }) -join ' '
    $process=[Diagnostics.Process]::Start($start);$null=$process.Handle
    @{pid=$process.Id;arguments=$observerArguments;deadlineSeconds=15}|ConvertTo-Json|
        Set-Content "$root\observer-process.json"
    if(!$process.WaitForExit(15000)){throw 'Clipboard observer deadline'}
    $process.Refresh();$process.ExitCode|Set-Content "$root\observer-exit.txt"
    if($process.ExitCode){throw "Clipboard observer exit $($process.ExitCode)"}
    $observerReport=Get-Content "$root\observer.txt" -Raw
    # The original Win16 loader may preserve the package system directory as
    # its current directory. Preserve a partial report before asserting the
    # launcher result, so an owned failed stage retains its last guest phase.
    $guest=@(Get-ChildItem -LiteralPath $runtime -Filter CLIPPRB.BIN -File -Recurse)
    if($guest.Count -eq 1){Copy-Item $guest[0].FullName "$root\guest-report.bin"}
    if($observerReport -notmatch 'result=exited' -or $observerReport -notmatch 'exit=0x00000000'){
        throw 'Clipboard guest/launcher did not complete normally'
    }
    if($guest.Count -ne 1){throw 'Clipboard probe did not emit one report inside its owned stage'}
    $bytes=[IO.File]::ReadAllBytes("$root\guest-report.bin")
    $expectedPhases=if($OwnerTaskExit){'12,8,9,10,11,1,20'}elseif($CallbackReturnOnly){'12,8,9,10,11,1,13,14,15,16,17,18,19'}else{'12,8,9,10,11,1,2,3,4,5,6,7,13,14,15,16,17,18'}
    $expectedBytes=12 * ($expectedPhases -split ',').Count
    if($bytes.Length -ne $expectedBytes){throw "Unexpected Clipboard report size: $($bytes.Length)"}
    $rows=@()
    for($offset=0;$offset -lt $bytes.Length;$offset+=12){
        if([Text.Encoding]::ASCII.GetString($bytes,$offset,4) -ne 'CLP1'){
            throw 'Bad Clipboard report signature'
        }
        $rows += [pscustomobject]@{
            phase=[BitConverter]::ToUInt16($bytes,$offset+4)
            errors=[BitConverter]::ToUInt16($bytes,$offset+6)
            guestHandle=[BitConverter]::ToUInt16($bytes,$offset+8)
            nativeHandle=[BitConverter]::ToUInt16($bytes,$offset+10)
        }
    }
    $rows|ConvertTo-Json|Set-Content "$root\guest-report.json"
    $last=$rows[-1]
    # Five startup checkpoints precede the original Clipboard transfer
    # sequence.  Keep the phase-4 data and phase-5 rejection assertions
    # bound to their protocol records, rather than their old preamble-free
    # array offsets.
    $rejected=if($CallbackReturnOnly -or $OwnerTaskExit){$null}else{$rows[9]}
    $functionalFailure=if($OwnerTaskExit){
        @($rows|Where-Object errors -ne 0).Count -or !$last.nativeHandle
    }elseif($CallbackReturnOnly){
        @($rows|Where-Object errors -ne 0).Count -or !$last.guestHandle
    }else{
        @($rows|Where-Object errors -ne 0).Count -or !$rows[8].guestHandle -or !$rejected.guestHandle -or $rejected.nativeHandle -ne 0
    }
    if(($rows.phase -join ',') -ne $expectedPhases -or $functionalFailure){
        throw 'Original Clipboard guest/native transfer contract incomplete'
    }
    $windowTrace=Get-Content "$root\window-trace.txt" -Raw
    $callbackMessage='0306'
    if($OwnerTaskExit){
        if($windowTrace -match 'WindowDispatch .*message=0306 ' -or
           $windowTrace -notmatch 'TaskRetirement .*stage=complete status=0'){
            throw 'Task rundown called the departing Clipboard owner or did not finish'
        }
        $created=@([regex]::Matches($windowTrace,'WindowCreate .*result=([0-9A-Fa-f]{8}) .*claimed=1 ') |
            Where-Object {([Convert]::ToUInt32($_.Groups[1].Value,16) -band 65535) -eq $last.nativeHandle})
        if($created.Count -ne 1){throw 'Task-exit witness must create exactly one owned native window'}
        & "$root/CLIPRET.EXE" $created[0].Groups[1].Value *> "$root/retirement.txt"
        if($LASTEXITCODE){throw 'Native Clipboard owner/window retirement failed'}
    }elseif($windowTrace -notmatch ("(?m)^.*WindowDispatch .*message={0} .*result=447A0000 " -f $callbackMessage)) {
        throw 'Original callback did not return through the original guest procedure'
    }
    foreach($input in Import-Csv "$root\inputs.csv"){
        if((Get-FileHash -LiteralPath $input.Path).Hash -ne $input.Hash){
            throw "Input changed: $($input.Path)"
        }
    }
    $desktopMode=if($SharedClipboard){'shared'}else{'private'}
    $mode=if($OwnerTaskExit){'task-exit'}elseif($CallbackReturnOnly){'callback-return'}else{'transfer'}
    $transfers=if($CallbackReturnOnly -or $OwnerTaskExit){0}else{1}
    $renderAll=if($OwnerTaskExit){0}else{1}
    $taskExit=[int][bool]$OwnerTaskExit
    "WOW_CLIPBOARD_TRANSFER_OK mode=$mode checkpoints=$($rows.Count) guest_to_native=$transfers native_to_guest=$transfers rejected=$transfers deferred=1 replacement=$transfers owner_destroy_renderall=$renderAll task_exit=$taskExit desktop=$desktopMode" |
        Set-Content "$root\result.txt"
} catch {
    $_|Out-String|Set-Content "$root\failure.txt"
    throw
} finally {
    try {if($process){if(!$process.HasExited){$process.Kill();$process.WaitForExit(5000)|Out-Null};$process.Dispose()}}
    finally {
        try {if($scope){Stop-IsolatedPackageScope $scope}}
        finally {
            try {if($runtime){Remove-PhysicalRuntimeStage $runtime}}
            finally {foreach($name in $saved.Keys){[Environment]::SetEnvironmentVariable($name,$saved[$name],'Process')}}
        }
    }
}
