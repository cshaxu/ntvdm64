[CmdletBinding()]
param([Parameter(Mandatory)][string]$PackageA,[Parameter(Mandatory)][string]$PackageB,
      [Parameter(Mandatory)][string]$OutputRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
. "$repo/tests/observation/isolated_package_cleanup.ps1"
$root=[IO.Path]::GetFullPath($OutputRoot)
$build=[IO.Path]::GetFullPath((Join-Path $repo 'build'))+'\'
if(!$root.StartsWith($build,[StringComparison]::OrdinalIgnoreCase) -or (Test-Path -LiteralPath $root)) {
    throw 'Use a fresh build-owned evidence root'
}
$packages=@((Resolve-Path -LiteralPath $PackageA).Path,(Resolve-Path -LiteralPath $PackageB).Path)
$ids=@($packages | ForEach-Object {
    $rows=@((Get-Content -LiteralPath (Join-Path $_ 'product-system32-manifest.json') -Raw | ConvertFrom-Json))
    $values=@($rows | Select-Object -ExpandProperty PackageId -Unique)
    if($rows.Count -ne 10 -or $values.Count -ne 1){throw "Invalid package identity manifest: $_"}
    $values[0]
})
if($ids[0] -eq $ids[1]){throw 'The two packages must have different IDs'}
New-Item -ItemType Directory -Path $root | Out-Null
$owned=@()
function Launch([string]$exe,[string[]]$arguments,[string]$tag) {
    $options=@{FilePath=$exe;WindowStyle='Hidden';PassThru=$true;
        RedirectStandardOutput="$root/$tag.stdout";RedirectStandardError="$root/$tag.stderr"}
    if($arguments.Count){$options.ArgumentList=$arguments}
    $p=Start-Process @options
    $null=$p.Handle
    $script:owned+=,$p
    return $p
}
function WaitConnected($p,[string]$tag) {
    $deadline=[DateTime]::UtcNow.AddSeconds(15)
    do {
        if((Test-Path "$root/$tag.stdout") -and ((Get-Content "$root/$tag.stdout" -Raw) -match '(?m)^CONNECTED\s*$')){return}
        if($p.HasExited){throw "$tag exited before connection: $($p.ExitCode)"}
        Start-Sleep -Milliseconds 50
    } while([DateTime]::UtcNow -lt $deadline)
    throw "$tag did not connect within 15 seconds"
}
try {
    $services=@()
    for($i=0;$i -lt 2;$i++) {$services+=,(Launch "$($packages[$i])/system32/ntsrv.exe" @() "service-$i")}
    for($i=0;$i -lt 2;$i++) {
        $deadline=[DateTime]::UtcNow.AddSeconds(8)
        do {
            if((Test-Path "$root/service-$i.stdout") -and ((Get-Content "$root/service-$i.stdout" -Raw) -match 'basesrv: listening')){break}
            if($services[$i].HasExited){throw "Service $i exited before readiness: $($services[$i].ExitCode)"}
            Start-Sleep -Milliseconds 25
        } while([DateTime]::UtcNow -lt $deadline)
        if([DateTime]::UtcNow -ge $deadline){throw "Service $i did not report readiness"}
    }
    $clients=@()
    for($i=0;$i -lt 2;$i++) {$clients+=,(Launch "$($packages[$i])/system32/rpc-probe.exe" @('hold','20000') "client-$i")}
    for($i=0;$i -lt 2;$i++) {WaitConnected $clients[$i] "client-$i"}
    if($services[0].HasExited -or $services[1].HasExited){throw 'Both services must be alive after both clients connect'}
    $services[0].Kill();if(!$services[0].WaitForExit(5000)){throw 'Owned service A failed to exit'}
    if($services[1].HasExited -or $clients[1].HasExited){throw 'Stopping A disturbed B'}
    # The shared test guard must ignore a verified foreign UUID yet refuse
    # an already-owned endpoint; it must never stop either process.
    Assert-NoConflictingPackageBroker (Join-Path $packages[0] 'system32/ntsrv.exe')
    $sameIdentityRejected=$false
    try { Assert-NoConflictingPackageBroker (Join-Path $packages[1] 'system32/ntsrv.exe') }
    catch { $sameIdentityRejected=$_.Exception.Message -like 'Existing live broker *' }
    if(!$sameIdentityRejected){throw 'The package guard accepted its own live broker'}
    $again=Launch "$($packages[1])/system32/rpc-probe.exe" @('hold','100') 'client-b-again'
    if(!$again.WaitForExit(10000) -or $again.ExitCode -ne 0){throw 'Package B failed to reconnect after A stopped'}
    if(((Get-Content "$root/client-b-again.stdout" -Raw) -notmatch '(?m)^CONNECTED\s*$')){throw 'B did not produce a connect witness'}
    $failed=Launch "$($packages[0])/system32/rpc-probe.exe" @('hold','100') 'client-a-after-stop'
    if(!$failed.WaitForExit(10000)){throw 'A client did not finish after service A stopped'}
    if($failed.ExitCode -eq 0){throw 'Package A silently connected to B after A stopped'}
    "PASS two authenticated package clients connected to separate live NTSRV processes"
    "PASS stopping A leaves B connected and reconnectable; A cannot fall through to B"
    "package-a=$($ids[0]) package-b=$($ids[1])"
} finally {
    foreach($p in $owned) {try {if(!$p.HasExited){$p.Kill();[void]$p.WaitForExit(5000)}} finally {$p.Dispose()}}
}
