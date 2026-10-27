[CmdletBinding()]
param([Parameter(Mandatory)][string]$PackageRoot,
      [Parameter(Mandatory)][string]$OutputRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
. "$repo/tests/observation/isolated_package_cleanup.ps1"
$build=[IO.Path]::GetFullPath((Join-Path $repo 'build'))+'\'
$output=[IO.Path]::GetFullPath($OutputRoot)
if(!$output.StartsWith($build,[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $output)) {
    throw 'Use a fresh repository-build evidence directory'
}
New-Item -ItemType Directory -Path $output | Out-Null
$stages=@();$owned=@()
try {
    foreach($i in 0..1){$stages+=,(New-PhysicalRuntimeStage $PackageRoot)}
    $ids=@($stages | ForEach-Object {Get-IsolatedPackageIdentity (Join-Path $_ 'system32/ntsrv.exe')})
    if(!$ids[0] -or $ids[0] -ne $ids[1]){throw 'Ordinary package copies must retain the same identity'}
    $scope=New-IsolatedPackageScope $stages[0]
    $first=Start-Process (Join-Path $stages[0] 'system32/ntsrv.exe') -WindowStyle Hidden -PassThru `
        -RedirectStandardOutput (Join-Path $output 'first.stdout') -RedirectStandardError (Join-Path $output 'first.stderr')
    $null=$first.Handle;$owned+=,$first
    $deadline=[DateTime]::UtcNow.AddSeconds(10)
    do {
        if($first.HasExited){throw 'First broker exited before readiness'}
        Start-Sleep -Milliseconds 25
    } while([DateTime]::UtcNow -lt $deadline -and
        !((Get-Content (Join-Path $output 'first.stdout') -Raw -ErrorAction SilentlyContinue) -match 'basesrv: listening'))
    if([DateTime]::UtcNow -ge $deadline){throw 'First broker did not become ready'}
    $second=Start-Process (Join-Path $stages[1] 'system32/ntsrv.exe') -WindowStyle Hidden -PassThru `
        -RedirectStandardOutput (Join-Path $output 'second.stdout') -RedirectStandardError (Join-Path $output 'second.stderr')
    $null=$second.Handle;$owned+=,$second
    if(!$second.WaitForExit(10000) -or $second.ExitCode -eq 0){throw 'A second broker acquired the same package endpoint'}
    if($first.HasExited){throw 'Second broker disturbed the first broker'}
    'PASS ordinary package copies retain one UUID and cannot start competing brokers'
} finally {
    foreach($p in $owned){try{if(!$p.HasExited){$p.Kill();[void]$p.WaitForExit(5000)}}finally{$p.Dispose()}}
    foreach($stage in $stages){Remove-PhysicalRuntimeStage $stage}
}
