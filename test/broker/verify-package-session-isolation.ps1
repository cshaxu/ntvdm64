[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$PackageA,
    [Parameter(Mandatory)][string]$PackageB,
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$OutputRoot,
    [Parameter(Mandatory)][string]$TempRoot,
    [ValidateSet('dos','native')][string]$Kind='dos'
)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$build=[IO.Path]::GetFullPath((Join-Path $repo 'build'))+'\'
$output=[IO.Path]::GetFullPath($OutputRoot)
if(!$output.StartsWith($build,[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $output)) {
    throw 'Use a fresh repository-build evidence directory'
}
$packages=@((Resolve-Path $PackageA).Path,(Resolve-Path $PackageB).Path)
$ids=@($packages | ForEach-Object {
    $rows=@((Get-Content (Join-Path $_ 'product-system32-manifest.json') -Raw | ConvertFrom-Json))
    $values=@($rows | Select-Object -ExpandProperty PackageId -Unique)
    if($rows.Count -ne 10 -or $values.Count -ne 1){throw "Invalid package manifest: $_"}
    $values[0]
})
if($ids[0] -eq $ids[1]){throw 'The packages must have different identities'}
$observerPath=(Resolve-Path $Observer).Path
$tempPath=(Resolve-Path $TempRoot).Path
$saved=@{}
foreach($entry in Get-ChildItem Env:) {$saved[$entry.Name]=$entry.Value}
New-Item -ItemType Directory -Path $output | Out-Null
$owned=@()
try {
    # DOS boot has a small environment contract. Avoid inheriting unrelated
    # tooling variables from the test runner, and restore them on exit.
    $keep=@('SystemRoot','WINDIR','COMSPEC','PATHEXT','SYSTEMDRIVE','NUMBER_OF_PROCESSORS',
        'PROCESSOR_ARCHITECTURE','PROCESSOR_IDENTIFIER','OS')
    foreach($entry in Get-ChildItem Env:) {
        if($entry.Name -notin $keep) {Remove-Item -LiteralPath ('Env:'+$entry.Name)}
    }
    $env:PATH=Join-Path $env:SystemRoot 'System32'
    $env:TEMP=$tempPath; $env:TMP=$tempPath
    $env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'; $env:MVDM_OBSERVER_SHORT_HISTORY='1'
    for($i=0;$i -lt 2;$i++) {
        $root=$packages[$i]
        $target=if($Kind -eq 'dos'){Join-Path $root 'system32/COMMAND.COM'}else{Join-Path $env:SystemRoot 'System32/cmd.exe'}
        $report=Join-Path $output "session-$i.report.txt"
        $args=@((Join-Path $root 'system32/run16.exe'),$root,$report,$target,'/c','exit','--observation-timeout-ms','20000')
        $p=Start-Process -FilePath $observerPath -ArgumentList $args -WindowStyle Hidden -PassThru `
            -RedirectStandardOutput (Join-Path $output "session-$i.stdout") `
            -RedirectStandardError (Join-Path $output "session-$i.stderr")
        # Retain a process handle before the child can exit; otherwise the
        # Windows PowerShell Process object may no longer expose ExitCode.
        $null=$p.Handle
        $owned+=,$p
    }
    foreach($p in $owned) {
        if(!$p.WaitForExit(40000)){throw "Observer $($p.Id) timed out"}
        if($p.ExitCode -ne 0){throw "Observer $($p.Id) failed with exit $($p.ExitCode)"}
    }
    for($i=0;$i -lt 2;$i++) {
        $report=Get-Content (Join-Path $output "session-$i.report.txt") -Raw
        if($report -notmatch '(?m)^result=exited\s*$' -or $report -notmatch '(?m)^exit=0x00000000\s*$') {
            throw "Package $i did not complete the $Kind command successfully"
        }
    }
    "PASS two distinct packages completed concurrent $Kind sessions"
} finally {
    foreach($p in $owned) {try {if(!$p.HasExited){$p.Kill();[void]$p.WaitForExit(5000)}} finally {$p.Dispose()}}
    foreach($entry in Get-ChildItem Env:) {Remove-Item -LiteralPath ('Env:'+$entry.Name)}
    foreach($key in $saved.Keys) {[Environment]::SetEnvironmentVariable($key,$saved[$key],'Process')}
}
