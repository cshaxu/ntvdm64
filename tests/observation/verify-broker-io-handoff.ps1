param(
    [Parameter(Mandatory)][string]$RuntimeRoot,
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$ReportPrefix,
    [ValidateSet('native','nested-console','nested-window')][string]$Case='nested-window',
    [ValidateSet('I386','AMD64')][string]$NativeMachine='I386'
)
$ErrorActionPreference='Stop'
$runtime=(Resolve-Path $RuntimeRoot).Path
$observerPath=(Resolve-Path $Observer).Path
$prefix=[IO.Path]::GetFullPath($ReportPrefix)
$repository=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$build=[IO.Path]::GetFullPath((Join-Path $repository 'build'))+'\'
if(!$prefix.StartsWith($build,[StringComparison]::OrdinalIgnoreCase)) {
    throw 'Probe reports must remain below the repository build directory'
}
if(!(Test-Path (Split-Path $prefix -Parent))) {throw 'Declare/create the build run root first'}
if(Test-Path "$prefix.observer") {throw 'Preserve existing run evidence; choose a fresh prefix'}
if(Test-Path 'Z:\') {throw 'Z: already exists; do not replace an existing mapping'}
if(Get-Process ntsrv -ErrorAction SilentlyContinue) {throw 'An existing broker must finish before this isolated probe'}
. "$PSScriptRoot/isolated_package_cleanup.ps1"
$testScope=New-IsolatedPackageScope $runtime
$binary=Get-PackageBinaryRoot $runtime
$binaryRelative=$binary.Substring($runtime.Length).TrimStart('\')
$names='run16.exe','ntsrv.exe','ntvdm.exe','ntvwm.exe','ntcon.exe','ntmon.exe','WOW32.DLL','VDMREDIR.DLL'
foreach($hook in 'nthook32.dll','nthook64.dll') {
    if(Test-Path (Join-Path $binary $hook)){$names+=,$hook}
}
$cmd=Join-Path $env:WINDIR $(if($NativeMachine -eq 'AMD64'){'System32\cmd.exe'}else{'SysWOW64\cmd.exe'})
$image=[IO.File]::ReadAllBytes($cmd)
$machine=[BitConverter]::ToUInt16($image,[BitConverter]::ToInt32($image,60)+4)
if($machine -ne $(if($NativeMachine -eq 'AMD64'){0x8664}else{0x14c})) {throw 'Native CMD machine mismatch'}
# run16 remains x86: Sysnative names the actual AMD64 image without WOW64 redirection.
if($NativeMachine -eq 'AMD64'){$cmd=Join-Path $env:WINDIR 'Sysnative\cmd.exe'}
$identity=@($names | ForEach-Object {
    $path=Join-Path $binary $_
    if(!(Test-Path $path)){throw "Missing package file: $path"}
    [ordered]@{name=$_;sha256=(Get-FileHash $path -Algorithm SHA256).Hash}
})
$identity | ConvertTo-Json | Set-Content "$prefix.package.json" -Encoding UTF8
$environmentNames='MVDM_OBSERVER_PRIVATE_DESKTOP','MVDM_OBSERVER_WINDOW_INPUT','MVDM_OBSERVER_MILESTONE_INPUT',
    'MVDM_OBSERVER_SHORT_HISTORY',
    'NTVDM_BOOTSTRAP_TRACE','NTVWM_GEOMETRY_ERROR_LOG'
$saved=@{}
foreach($name in $environmentNames) {$saved[$name]=[Environment]::GetEnvironmentVariable($name,'Process')}
$mapped=$false
try {
    & subst.exe Z: $runtime
    if($LASTEXITCODE){throw 'SUBST Z: failed'}
    $mapped=$true
    $launchBinary=if($binaryRelative){Join-Path 'Z:\' $binaryRelative}else{'Z:\'}
    $testScope.Paths+=@($testScope.Paths | ForEach-Object {Join-Path $launchBinary ([IO.Path]::GetFileName($_))})
    $observerLaunch=$observerPath
    if($observerPath.StartsWith($runtime+'\',[StringComparison]::OrdinalIgnoreCase)){
        $observerLaunch=Join-Path 'Z:\' $observerPath.Substring($runtime.Length+1)
    }
    $env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
    # Use the observer's checked disposable 80-column Console fixture, like
    # control regression. The host's default private-desktop geometry is not
    # a stable capability profile; its retained failures remain separate.
    $env:MVDM_OBSERVER_SHORT_HISTORY='1'
    $env:MVDM_OBSERVER_MILESTONE_INPUT='1'
    Remove-Item Env:MVDM_OBSERVER_WINDOW_INPUT -ErrorAction SilentlyContinue
    if($Case -eq 'nested-window'){$env:MVDM_OBSERVER_WINDOW_INPUT='1'}
    $env:NTVDM_BOOTSTRAP_TRACE="$prefix.bootstrap.txt"
    $env:NTVWM_GEOMETRY_ERROR_LOG="$prefix.native-errors.txt"
    $input="exit /b 23`r"
    if($Case -ne 'native') {
        # The fixture starts in the selected binary directory. Shorten only
        # the launcher spelling: preserve the guest's absolute path while
        # fitting the observer's current-row echo acknowledgement.
        $input='.\run16.exe '+(Join-Path $launchBinary 'COMMAND.COM')+"`rver`rmem`rexit`recho S6-PARENT-RETURN`rexit /b 23`r"
    }
    [pscustomobject]@{Runtime=$runtime;Binary=$binary;LaunchBinary=$launchBinary;
        Observer=$observerLaunch;NativeMachine=$NativeMachine;Case=$Case;Input=$input;
        ShortHistory=$env:MVDM_OBSERVER_SHORT_HISTORY} |
        ConvertTo-Json | Set-Content "$prefix.launch.json" -Encoding UTF8
    & $observerLaunch (Join-Path $launchBinary 'run16.exe') $launchBinary "$prefix.observer" $cmd /d `
        --observe-console-input-text $input --observe-console-line-delay-ms 1800 `
        --observation-timeout-ms 40000
    if(!(Test-Path "$prefix.observer")){throw 'Observer did not write a report'}
    $report=Get-Content "$prefix.observer" -Raw
    if($report -notmatch '(?m)^result=exited\r?$' -or
       $report -notmatch '(?m)^exit=0x00000017\r?$' -or
       $report -notmatch '(?m)^scripted-console-input=delivered\r?$') {
        throw "Direct completion/input failed; inspect $prefix.observer"
    }
    $screen=Get-Content "$prefix.observer.console.txt" -Raw
    if($screen -notmatch 'exit /b 23'){throw 'Missing actual CMD exit command'}
    if($Case -ne 'native') {
        # Assert each actual output at its execution boundary, not perpetual
        # retention of an old banner after the worker's page is relinquished.
        # This strengthens ordering without depending on unsupported history.
        $dos=Get-Content "$prefix.observer.line-01.console.txt" -Raw
        $mem=Get-Content "$prefix.observer.line-03.console.txt" -Raw
        if($dos -notmatch 'Microsoft\(R\) Windows NT DOS' -or
           $mem -notmatch 'bytes total conventional memory' -or
           $screen -notmatch '(?m)^\[\d+\] S6-PARENT-RETURN\r?$') {
            throw 'Missing real DOS/MEM output or executed parent-return marker'
        }
    }
    if($Case -eq 'nested-window') {
        $caf=Get-Content "$prefix.observer.caf.txt" -Raw
        if($caf -notmatch 'caf-visible-window=1' -or $caf -notmatch 'result=pass error=0') {
            throw 'Window/CAF was not actually verified'
        }
    }
    foreach($row in $identity) {
        if((Get-FileHash (Join-Path $binary $row.name)).Hash -ne $row.sha256) {
            throw "Package changed during observation: $($row.name)"
        }
    }
    "PASS broker I/O $Case $NativeMachine : actual CMD/DOS input, parent return and direct exit=23"
} finally {
    try {Stop-IsolatedPackageScope $testScope}finally{
        if($mapped) {& subst.exe Z: /d; if($LASTEXITCODE){Write-Error 'Failed to remove owned Z: mapping'}}
        foreach($name in $environmentNames) {
            # PowerShell can marshal null to an empty string here. Native
            # GetEnvironmentVariable(name,NULL,0) sees that as present (1),
            # accidentally enabling a later observer's boolean mode.
            if($null -eq $saved[$name]){
                Remove-Item -LiteralPath ('Env:'+$name) -ErrorAction SilentlyContinue
            }else{[Environment]::SetEnvironmentVariable($name,$saved[$name],'Process')}
        }
    }
}
