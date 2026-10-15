[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$PackageRoot,
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$BuildRoot,
    [string]$Nasm='nasm.exe'
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
$source=Join-Path $PSScriptRoot 'win16_hook_probe.asm'
$process=$null;$runtime=$null;$scope=$null
$saved=@{}
foreach($name in @('TEMP','TMP','PATH','MVDM_OBSERVER_PRIVATE_DESKTOP','MVDM_WOW_DISPATCH_TRACE_PATH')){
    $saved[$name]=[Environment]::GetEnvironmentVariable($name)
}
try {
    $assembler=(Get-Command $Nasm -ErrorAction Stop).Source
    & $assembler -f bin -o "$root/CBTPROB.EXE" -l "$root/probe.lst" $source *> "$root/assemble.txt"
    if($LASTEXITCODE){throw 'Independent NE probe assembly failed'}
    $runtime=New-PhysicalRuntimeStage (Resolve-Path $PackageRoot).Path
    $scope=New-IsolatedPackageScope $runtime
    $tests=Join-Path $runtime 'tests'
    $null=New-Item -ItemType Directory $tests -Force
    Copy-Item "$root/CBTPROB.EXE" "$tests/CBTPROB.EXE"
    $inputs=@(@{File='probe-source';SHA256=(Get-FileHash $source).Hash},
        @{File='probe';SHA256=(Get-FileHash "$root/CBTPROB.EXE").Hash})
    foreach($file in @('system32/run16.exe','system32/ntvdm.exe','system32/WOW32.DLL',
            'system32/KRNL386.EXE','system32/USER.EXE')){
        $inputs+=@{File=$file;SHA256=(Get-FileHash (Join-Path $runtime $file)).Hash}
    }
    $inputs|ConvertTo-Json|Set-Content "$root/inputs.json"
    $env:TEMP=$runtime;$env:TMP=$runtime
    $env:PATH="$runtime\system32;"+$env:SystemRoot+'\System32'
    $env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
    $env:MVDM_WOW_DISPATCH_TRACE_PATH="$root/dispatch.txt"
    $start=[Diagnostics.ProcessStartInfo]::new((Resolve-Path $Observer).Path)
    $start.UseShellExecute=$false;$start.CreateNoWindow=$true
    foreach($arg in @("$runtime\system32\run16.exe",$tests,"$root\observer.txt",
            '--wait',"$tests\CBTPROB.EXE",'--observation-timeout-ms','30000')){
        $start.ArgumentList.Add($arg)
    }
    $process=[Diagnostics.Process]::Start($start)
    $null=$process.Handle
    @{pid=$process.Id;start=[DateTime]::UtcNow;deadlineSeconds=35;arguments=@($start.ArgumentList)}|
        ConvertTo-Json|Set-Content "$root/observer-process.json"
    if(!$process.WaitForExit(35000)){throw 'Owned observer exceeded actual deadline'}
    $process.Refresh()
    if(Test-Path "$tests/CBTPROB.BIN"){Copy-Item "$tests/CBTPROB.BIN" "$root/guest-report.bin"}
    $report=Get-Content "$root/observer.txt" -Raw
    if($process.ExitCode -or $report -notmatch 'result=exited' -or $report -notmatch 'exit=0x00000000'){
        throw 'Guest/launcher did not complete normally'
    }
    $bytes=[IO.File]::ReadAllBytes("$root/guest-report.bin")
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
    0|Set-Content "$root/result.exit"
} catch {
    $_|Out-String|Set-Content "$root/failure.txt"
    1|Set-Content "$root/result.exit"
    throw
} finally {
    try {
        if($process){if(!$process.HasExited){$process.Kill();$process.WaitForExit(5000)|Out-Null};$process.Dispose()}
        if($runtime -and (Test-Path "$runtime/tests/CBTPROB.BIN")){
            Copy-Item "$runtime/tests/CBTPROB.BIN" "$root/guest-report.bin" -Force
        }
    } finally {
        try {if($scope){Stop-IsolatedPackageScope $scope}}
        finally {
            try {if($runtime){Remove-PhysicalRuntimeStage $runtime}}
            finally {foreach($name in $saved.Keys){[Environment]::SetEnvironmentVariable($name,$saved[$name])}}
        }
    }
}
