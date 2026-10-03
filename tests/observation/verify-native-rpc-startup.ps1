[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$PackageRoot,
    [Parameter(Mandatory)][string]$ReportPath
)
$ErrorActionPreference='Stop'
$Observer=(Resolve-Path -LiteralPath $Observer).Path
$PackageRoot=(Resolve-Path -LiteralPath $PackageRoot).Path
$ReportPath=[IO.Path]::GetFullPath($ReportPath)
$build=(Resolve-Path (Join-Path $PSScriptRoot '../../build')).Path+'\'
if(!$ReportPath.StartsWith($build,[StringComparison]::OrdinalIgnoreCase)){
    throw 'Reports must remain under repository build/'
}
if(Test-Path -LiteralPath $ReportPath){throw 'Use a fresh report'}
$start=[Diagnostics.ProcessStartInfo]::new($Observer)
$start.UseShellExecute=$false
$start.WindowStyle=[Diagnostics.ProcessWindowStyle]::Hidden
$start.EnvironmentVariables['MVDM_OBSERVER_PRIVATE_DESKTOP']='1'
foreach($argument in @((Join-Path $PackageRoot 'run16.exe'),$PackageRoot,$ReportPath,
    $env:COMSPEC,'/d','/c','echo NATIVE-RPC-STARTED & exit /b 37',
    '--observation-timeout-ms','20000')){$start.ArgumentList.Add($argument)}
$process=[Diagnostics.Process]::Start($start)
try {
    if(!$process.WaitForExit(30000)){throw "Observer still live: $($process.Id)"}
    $report=Get-Content -LiteralPath $ReportPath -Raw
    $screen=Get-Content -LiteralPath ($ReportPath+'.console.txt') -Raw
    if($process.ExitCode -or $report -notmatch 'result=exited' -or
        $report -notmatch 'exit=0x00000025' -or $screen -notmatch 'NATIVE-RPC-STARTED'){
        throw "Native startup/output/direct-result failed: $ReportPath"
    }
    'PASS actual native startup, Console output and broker direct exit 37'
} finally {
    # An observation timeout is not authority to kill the target, worker or
    # service. The caller retains the reported live PID for investigation.
    $process.Dispose()
}
