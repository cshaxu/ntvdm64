param([string]$BuildRoot = 'build/M0-T430/S6/lease',
    [ValidateSet('x86','x64')][string]$Architecture = 'x86')
$ErrorActionPreference = 'Stop'
$repo = (Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$out = [IO.Path]::GetFullPath((Join-Path $repo $BuildRoot))
if (!$out.StartsWith((Join-Path $repo 'build/'), [StringComparison]::OrdinalIgnoreCase)) {
    throw 'Build root must be under build'
}
if (Test-Path $out) { throw 'Require a fresh evidence directory' }
New-Item -ItemType Directory -Force $out | Out-Null
& cmd.exe /d /c "`"$repo/tools/build-fixtures.cmd`" $Architecture guest-memory"
if($LASTEXITCODE){throw 'Native guest-memory fixture build failed'}
$cache=Join-Path $repo "build/.ninja/fixtures-$Architecture"
$lease=Join-Path $repo 'src/compat/guest_memory_lease.c'
$session=Join-Path $repo 'src/ntvdm-exe/session/session.c'
$access=Join-Path $cache 'guest-memory-coordination.c'
$hostContext=Join-Path $repo 'src/compat/host_context.c'
foreach ($name in @('guest_memory_lease_boundary_test', 'guest_memory_lease_test', 'guest_memory_coordination_test')) {
    $dir = Join-Path $out $name
    New-Item -ItemType Directory -Force $dir | Out-Null
    $component = if ($name -eq 'guest_memory_lease_boundary_test') { 'compat' } elseif ($name -eq 'guest_memory_lease_test') { 'session' } else { 'mvdm-host' }
    $fixture = Join-Path $repo ('test/' + $component + '/' + $name + '.c')
    Copy-Item (Join-Path $cache "$name.exe") (Join-Path $dir 'fixture.exe')
    & (Join-Path $dir 'fixture.exe') 2>&1 | Tee-Object (Join-Path $dir 'result.txt')
    $code = $LASTEXITCODE
    "exit=$code" | Add-Content (Join-Path $dir 'result.txt')
    if ($code) { throw "Lease test failed: $name" }
    Get-FileHash $fixture, $lease, $session, $hostContext, $access, (Join-Path $dir 'fixture.exe') |
        ConvertTo-Json | Set-Content (Join-Path $dir 'identity.json')
}
