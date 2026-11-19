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
$vs = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
$lease = Join-Path $repo 'src/compat/guest_memory_lease.c'
$session = Join-Path $repo 'src/ntvdm-exe/session/session.c'
$access = Join-Path $out 'guest-memory-coordination.c'
& python (Join-Path $repo 'tools/build/extract-guest-memory-fixture.py') $access
if ($LASTEXITCODE) { throw 'C coordination extraction failed' }
$hostContext = Join-Path $repo 'src/compat/host_context.c'
foreach ($name in @('guest_memory_lease_boundary_test', 'guest_memory_lease_test', 'guest_memory_coordination_test')) {
    $dir = Join-Path $out $name
    New-Item -ItemType Directory -Force $dir | Out-Null
    $component = if ($name -eq 'guest_memory_lease_boundary_test') { 'compat' } elseif ($name -eq 'guest_memory_lease_test') { 'session' } else { 'mvdm-host' }
    $fixture = Join-Path $repo ('test/' + $component + '/' + $name + '.c')
    $sources = '"' + $fixture + '" "' + $lease + '"'

    if ($name -ne 'guest_memory_lease_boundary_test') { $sources += ' "' + (Join-Path $repo 'src/ntvdm-exe/softpc/mvdm_softpc_guest_memory.c') + '" "' + $session + '" "' + $hostContext + '" "' + $access + '" "' + (Join-Path $repo 'src/compat/guest_location.c') + '"' }
    $command = 'call "' + $vs + '" -arch=' + $Architecture + ' -host_arch=x64 >nul && cl /nologo /W4 /MT /TC /Gy /I"' +
        (Join-Path $repo 'src') + '" /I"' + (Join-Path $repo 'src/ntvdm-exe/softpc/include') + '" /Fo"' + $dir.Replace('\', '/') + '/" /Fe"' + $dir + '\fixture.exe" ' + $sources + ' /link /OPT:REF'
    & cmd.exe /d /s /c $command 2>&1 | Tee-Object (Join-Path $dir 'build.txt')
    if ($LASTEXITCODE) { throw "Build failed: $name" }
    & (Join-Path $dir 'fixture.exe') 2>&1 | Tee-Object (Join-Path $dir 'result.txt')
    $code = $LASTEXITCODE
    "exit=$code" | Add-Content (Join-Path $dir 'result.txt')
    if ($code) { throw "Lease test failed: $name" }
    Get-FileHash $fixture, $lease, $session, $hostContext, $access, (Join-Path $dir 'fixture.exe') |
        ConvertTo-Json | Set-Content (Join-Path $dir 'identity.json')
}
