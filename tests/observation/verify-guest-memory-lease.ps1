param([string]$BuildRoot = 'build/M0-T430/S6/lease')
$ErrorActionPreference = 'Stop'
$repo = (Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$out = [IO.Path]::GetFullPath((Join-Path $repo $BuildRoot))
if (!$out.StartsWith((Join-Path $repo 'build/'), [StringComparison]::OrdinalIgnoreCase)) {
    throw 'Build root must be under build'
}
if (Test-Path $out) { throw 'Require a fresh evidence directory' }
if (Test-Path $out) { throw 'Require a fresh evidence directory' }
New-Item -ItemType Directory -Force $out | Out-Null
$vs = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
$lease = Join-Path $repo 'src/ntvdm-exe/session/guest_memory_lease.c'
$session = Join-Path $repo 'src/ntvdm-exe/session/session.c'
foreach ($name in @('guest_memory_lease_boundary_test', 'guest_memory_lease_test')) {
    $dir = Join-Path $out $name
    New-Item -ItemType Directory -Force $dir | Out-Null
    $fixture = Join-Path $repo ('tests/session/' + $name + '.c')
    $sources = '"' + $fixture + '" "' + $lease + '"'
    if ($name -eq 'guest_memory_lease_test') { $sources += ' "' + $session + '"' }
    $command = 'call "' + $vs + '" -arch=x86 -host_arch=x64 >nul && cl /nologo /W4 /MT /TC /I"' +
        (Join-Path $repo 'src') + '" /Fo"' + $dir.Replace('\', '/') + '/" /Fe"' + $dir + '\fixture.exe" ' + $sources
    & cmd.exe /d /s /c $command 2>&1 | Tee-Object (Join-Path $dir 'build.txt')
    if ($LASTEXITCODE) { throw "Build failed: $name" }
    & (Join-Path $dir 'fixture.exe') 2>&1 | Tee-Object (Join-Path $dir 'result.txt')
    $code = $LASTEXITCODE
    "exit=$code" | Add-Content (Join-Path $dir 'result.txt')
    if ($code) { throw "Lease test failed: $name" }
    Get-FileHash $fixture, $lease, $session, (Join-Path $dir 'fixture.exe') |
        ConvertTo-Json | Set-Content (Join-Path $dir 'identity.json')
}
