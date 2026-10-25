[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$PackageRoot,
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$CompilerDriver,
    [Parameter(Mandatory)][string]$BuildRoot,
    [string]$Nasm='nasm.exe'
)

# An isolated, independent Win16/native peer witness. It stages an exact
# package and test-only NE/DLL inputs, never edits guest media and refuses a
# fallback to the user's interactive Clipboard.
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$root=[IO.Path]::GetFullPath((Join-Path $repo $BuildRoot))
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or
        (Test-Path -LiteralPath $root)){throw 'Require a fresh build-owned evidence root'}
$null=New-Item -ItemType Directory -Path $root
. "$PSScriptRoot/isolated_package_cleanup.ps1"
$runtime=$null;$scope=$null;$process=$null
$saved=@{}
foreach($name in @('PATH','TEMP','TMP','MVDM_OBSERVER_PRIVATE_DESKTOP',
        'MVDM_TERMINATE_VDM_REPORT_PATH')){
    $saved[$name]=[Environment]::GetEnvironmentVariable($name,'Process')
}
try {
    $package=(Resolve-Path -LiteralPath $PackageRoot).Path
    $observer=(Resolve-Path -LiteralPath $Observer).Path
    $driver=(Resolve-Path -LiteralPath $CompilerDriver).Path
    $assembly="$PSScriptRoot/win16_clipboard_probe.asm"
    $native="$PSScriptRoot/wow_clipboard_native_bridge.c"
    $assembler=(Get-Command $Nasm -ErrorAction Stop).Source
    & $assembler -f bin -o "$root\CLIPPRB.EXE" -l "$root\clipprobe.lst" $assembly `
        *> "$root\assemble.txt"
    if($LASTEXITCODE){throw 'Clipboard NE probe assembly failed'}
    Push-Location $root
    try {
        & $driver cl.exe /nologo /LD /MT /O2 /W4 /WX /D_CRT_SECURE_NO_WARNINGS `
            "/Fo$root\cliphost.obj" "/Fe$root\GENHOST.DLL" $native /link `
            "/IMPLIB:$root\GENHOST.lib" /EXPORT:ClipboardBridge=_ClipboardBridge@12 `
            *> "$root\native-build.txt"
        if($LASTEXITCODE){throw "Clipboard native bridge build failed: $LASTEXITCODE"}
    } finally {Pop-Location}
    Get-FileHash -LiteralPath @($assembly,$native,"$root\CLIPPRB.EXE",
        "$root\GENHOST.DLL",$observer,$driver,"$package\product-system32-manifest.json") |
        Select-Object Path,Hash | Export-Csv -NoTypeInformation "$root\inputs.csv"
    $runtime=New-PhysicalRuntimeStage $package
    $scope=New-IsolatedPackageScope $runtime
    $null=New-Item -ItemType Directory -Force -Path "$runtime\tests"
    Copy-Item "$root\CLIPPRB.EXE","$root\GENHOST.DLL" -Destination "$runtime\tests"
    # Original LoadLibraryEx32W resolves its native peer through the worker
    # system directory.  Keep the same disposable DLL beside the guest input
    # for provenance and add the loader-visible copy only inside this owned
    # physical test stage.
    Copy-Item "$root\GENHOST.DLL" -Destination "$runtime\system32\GENHOST.DLL"
    $env:TEMP=$runtime;$env:TMP=$runtime
    $env:PATH="$runtime\system32;"+$env:SystemRoot+'\System32'
    $env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
    $env:MVDM_TERMINATE_VDM_REPORT_PATH="$root\termination.txt"
    $start=[Diagnostics.ProcessStartInfo]::new($observer)
    $start.UseShellExecute=$false;$start.CreateNoWindow=$true
    foreach($argument in @("$runtime\system32\run16.exe","$runtime\tests",
            "$root\observer.txt",'--wait',"$runtime\tests\CLIPPRB.EXE",
            '--observation-timeout-ms','10000')){$start.ArgumentList.Add($argument)}
    $process=[Diagnostics.Process]::Start($start);$null=$process.Handle
    @{pid=$process.Id;arguments=@($start.ArgumentList);deadlineSeconds=15}|ConvertTo-Json|
        Set-Content "$root\observer-process.json"
    if(!$process.WaitForExit(15000)){throw 'Clipboard observer deadline'}
    $process.Refresh();$process.ExitCode|Set-Content "$root\observer-exit.txt"
    if($process.ExitCode){throw "Clipboard observer exit $($process.ExitCode)"}
    $observerReport=Get-Content "$root\observer.txt" -Raw
    if($observerReport -notmatch 'result=exited' -or $observerReport -notmatch 'exit=0x00000000'){
        throw 'Clipboard guest/launcher did not complete normally'
    }
    # The original Win16 loader may preserve the package system directory as
    # its current directory.  Locate the probe's uniquely named report inside
    # this owned stage instead of mistaking a current-directory policy for a
    # Clipboard failure.
    $guest=@(Get-ChildItem -LiteralPath $runtime -Filter CLIPPRB.BIN -File -Recurse)
    if($guest.Count -ne 1){throw 'Clipboard probe did not emit one report inside its owned stage'}
    Copy-Item $guest[0].FullName "$root\guest-report.bin"
    $bytes=[IO.File]::ReadAllBytes("$root\guest-report.bin")
    if($bytes.Length -ne 60){throw "Unexpected Clipboard report size: $($bytes.Length)"}
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
    $rejected=$rows[-1]
    if(($rows.phase -join ',') -ne '1,2,3,4,5' -or
            @($rows|Where-Object errors -ne 0).Count -or !$rows[3].guestHandle -or
            !$rejected.guestHandle -or $rejected.nativeHandle -ne 0){
        throw 'Original Clipboard guest/native transfer contract incomplete'
    }
    foreach($input in Import-Csv "$root\inputs.csv"){
        if((Get-FileHash -LiteralPath $input.Path).Hash -ne $input.Hash){
            throw "Input changed: $($input.Path)"
        }
    }
    'WOW_CLIPBOARD_TRANSFER_OK phases=5 guest_to_native=1 native_to_guest=1 rejected=1 private_desktop=1' |
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
