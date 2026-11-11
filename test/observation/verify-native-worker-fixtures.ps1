[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$NativeBuild,
    [Parameter(Mandatory)][string]$ReleaseRoot,
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$BuildRoot
)
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase)){throw 'Evidence must be below build/'}
if(Test-Path $root){throw 'Use a fresh evidence directory'}
. "$PSScriptRoot\isolated_package_cleanup.ps1"
$fixture=Join-Path $root 'fixture'
$null=New-Item -ItemType Directory $fixture -Force
foreach($name in @('ntvwm-execution-lifetime-test.exe','ntvwm-presentation-test.exe')){Copy-Item (Join-Path $NativeBuild $name) $fixture}
# Execution deliberately reaches the production suspended-child Hook installer.
# These are actual dependencies, not optional inputs or provider substitutes.
foreach($name in @('nthook32.dll','nthook64.dll','run16.exe')){Copy-Item (Join-Path $ReleaseRoot $name) $fixture}
Get-ChildItem $fixture -File | ForEach-Object {
    [pscustomobject]@{File=$_.Name;SHA256=(Get-IsolatedFileSha256 $_.FullName)}
} | ConvertTo-Json | Set-Content "$root\inputs.json"
$cases=@(
    @{Name='presentation';Image='ntvwm-presentation-test.exe';Args=''},
    @{Name='input-return';Image='ntvwm-presentation-test.exe';Args=' --input-return'},
    @{Name='lifetime';Image='ntvwm-execution-lifetime-test.exe';Args=''}
)
foreach($case in $cases){
    $name=$case.Name
    $start=[Diagnostics.ProcessStartInfo]::new($Observer)
    $start.UseShellExecute=$false;$start.CreateNoWindow=$true
    $start.EnvironmentVariables['MVDM_OBSERVER_PRIVATE_DESKTOP']='1'
    $start.Arguments='"'+$fixture+'\'+$case.Image+'" "'+$fixture+'" "'+$root+'\'+$name+'.observer" "'+$root+'\'+$name+'.txt"'+$case.Args+' --observation-timeout-ms 60000'
    $process=[Diagnostics.Process]::Start($start)
    try {
        if(!$process.WaitForExit(70000)){throw "$name exceeded its bound"}
        $report=Get-Content "$root\$name.observer" -Raw
        if($process.ExitCode -or $report -notmatch 'result=exited' -or $report -notmatch 'exit=0x00000000'){
            throw "$name failed; inspect captured assertions and observer report"
        }
        "PASS $name"
    } finally {
        if(!$process.HasExited){$process.Kill();$process.WaitForExit(5000)|Out-Null}
        $process.Dispose()
    }
}
'PASS presentation, input return and execution lifetime' | Set-Content "$root\result.txt"
