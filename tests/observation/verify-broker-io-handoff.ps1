param(
    [Parameter(Mandatory)][string]$RuntimeRoot,
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$ReportPrefix,
    [ValidateSet('native','nested-console','nested-window')][string]$Case='nested-window'
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
$identity=@($names | ForEach-Object {
    $path=Join-Path $binary $_
    if(!(Test-Path $path)){throw "Missing package file: $path"}
    [ordered]@{name=$_;sha256=(Get-FileHash $path -Algorithm SHA256).Hash}
})
$identity | ConvertTo-Json | Set-Content "$prefix.package.json" -Encoding UTF8
$environmentNames='MVDM_OBSERVER_PRIVATE_DESKTOP','MVDM_OBSERVER_WINDOW_INPUT','MVDM_OBSERVER_MILESTONE_INPUT',
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
    $env:MVDM_OBSERVER_MILESTONE_INPUT='1'
    Remove-Item Env:MVDM_OBSERVER_WINDOW_INPUT -ErrorAction SilentlyContinue
    if($Case -eq 'nested-window'){$env:MVDM_OBSERVER_WINDOW_INPUT='1'}
    $env:NTVDM_BOOTSTRAP_TRACE="$prefix.bootstrap.txt"
    $env:NTVWM_GEOMETRY_ERROR_LOG="$prefix.native-errors.txt"
    $input="exit /b 23`r"
    if($Case -ne 'native') {
        $input='"'+(Join-Path $launchBinary 'run16.exe')+'" "'+(Join-Path $launchBinary 'COMMAND.COM')+'"'+"`rver`rmem`rexit`recho S6-PARENT-RETURN`rexit /b 23`r"
    }
    & $observerLaunch (Join-Path $launchBinary 'run16.exe') $launchBinary "$prefix.observer" cmd.exe /d `
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
    "PASS broker I/O $Case : actual CMD/DOS input, parent return and direct exit=23"
} finally {
    try {Stop-IsolatedPackageScope $testScope}finally{
        if($mapped) {& subst.exe Z: /d; if($LASTEXITCODE){Write-Error 'Failed to remove owned Z: mapping'}}
        foreach($name in $environmentNames) {
            [Environment]::SetEnvironmentVariable($name,$saved[$name],'Process')
        }
    }
}
