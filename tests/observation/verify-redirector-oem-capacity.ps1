[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$OutputRoot,
    [switch]$OriginalProviders, [string]$BuildCache='build/M0-T427/S2/r001')
$ErrorActionPreference = 'Stop'
$root = (Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$out = [IO.Path]::GetFullPath((Join-Path $root $OutputRoot))
if (!$out.StartsWith(([IO.Path]::GetFullPath((Join-Path $root 'build')) + '\'),
    [StringComparison]::OrdinalIgnoreCase)) { throw 'Output must stay in build/.' }
if (Test-Path -LiteralPath $out) { throw 'Use fresh evidence directory.' }
New-Item -ItemType Directory -Path $out | Out-Null
$vs = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
$wrapper = Join-Path $out 'compiler.cmd'
@('@echo off', ('call "' + $vs + '" -arch=x86 -host_arch=x64 >nul'),
    'if errorlevel 1 exit /b %errorlevel%', '%*') |
    Set-Content -LiteralPath $wrapper -Encoding ASCII
$sources = @('src/vdmredir-dll/source/mvdm_redirector_guest_copy.c',
    'src/ntvdm-exe/softpc/mvdm_guest_location.c', 'src/ntvdm-exe/session/session.c',
    'src/ntvdm-exe/session/guest_memory_lease.c',
    'tests/mvdm-host/vdmredir/redirector_oem_capacity_test.c')
if ($OriginalProviders) {
    $sources[-1] = 'tests/mvdm-host/vdmredir/redirector_netapi_provider_test.c'
    $sources += 'src/ntvdm-exe/redir/mvdm_redirector_async.c'
}
$inputs = $sources | ForEach-Object { Join-Path $root $_ }
$objects = $sources | ForEach-Object {
    Join-Path $out ([IO.Path]::GetFileNameWithoutExtension($_) + '.obj')
}
& $wrapper cl /nologo /TC /c /MT /W4 /DNTVDM /DCPU_40_STYLE /DCCPU /DWIN_32 /DVDMREDIR_DLL "/I$root/src" "/I$root/src/vdmredir-dll/include" "/I$root/src/ntvdm-exe/redir/include" "/I$root/src/ntvdm-exe/softpc/include" "/FI$root/tests/mvdm-host/vdmredir/redirector_oem_fixture_binding.h" "/Fo$out/" @inputs 2>&1 |
    Tee-Object -FilePath (Join-Path $out 'build.txt')
if ($LASTEXITCODE -ne 0) { throw 'Compile failed.' }
if ($OriginalProviders) {
    $graph = Get-Content -LiteralPath (Join-Path $root "$BuildCache/build.ninja") -Raw
    $match = [regex]::Match($graph,
        '(?m)^build obj/redir/vrnetapi.obj:.*\r?\n\s+cflags = (.+)$')
    if (!$match.Success) { throw 'Missing actual production VrNetApi compile contract.' }
    $flags = $match.Groups[1].Value.Replace('$:', ':')
    if ($flags -notmatch '/DCPU_40_STYLE\b' -or $flags -match '/DCPU_30_STYLE\b') {
        throw 'Wrong selected CPU profile.'
    }
    $object = Join-Path $out 'vrnetapi.obj'
    $response = Join-Path $out 'original-provider.rsp'
    ($flags + ' /Gy /FI"' + $root + '/tests/mvdm-host/vdmredir/redirector_netapi_fixture_binding.h" /Fo"' + $object + '" "' + $root + '/src/mvdm/vdmredir/vrnetapi.c"') |
        Set-Content -LiteralPath $response -Encoding ASCII
    & $wrapper cl "@$response" 2>&1 |
        Tee-Object -FilePath (Join-Path $out 'build.txt') -Append
    if ($LASTEXITCODE -ne 0) { throw 'Original provider compile failed.' }
    $objects += $object
    # Original completion body resolves to the in-process production adapter,
    # rather than importing an executable. No completion algorithm is copied.
    $pipeObject = Join-Path $out 'vrnmpipe.obj'
    $response = Join-Path $out 'original-pipe-dependency.rsp'
    ($flags + ' /DMVDM_REDIRECTOR_WORKER_EXPORTS /Gy /Fo"' + $pipeObject + '" "' + $root + '/src/mvdm/vdmredir/vrnmpipe.c"') |
        Set-Content -LiteralPath $response -Encoding ASCII
    & $wrapper cl "@$response" 2>&1 |
        Tee-Object -FilePath (Join-Path $out 'build.txt') -Append
    if ($LASTEXITCODE -ne 0) { throw 'Original pipe dependency compile failed.' }
    $objects += $pipeObject
}
$exe = Join-Path $out 'redirector-oem-capacity-test.exe'
$libraries = @()
$extraFlags = @()
if ($OriginalProviders) {
    $generator = Get-Content (Join-Path $root 'tools/build/New-T310OriginalSoftpcNinja.ps1') -Raw
    $fixture = [regex]::Match($generator, '(?m)^\$fixtureHostLibraries = ''([^'']+)''')
    if (!$fixture.Success) { throw 'Missing selected fixture closure.' }
    $dependencies = @('obj/tests/ccpu_bounded_execution_fixture_seams.obj',
        'obj/tests/ccpu_host_fixture_seams.obj') + $fixture.Groups[1].Value.Split(' ') +
        @('original-mvdm-redir.lib','original-opennt-netlib.lib',
        'original-opennt-netapi-api.lib','original-opennt-xactsrv.lib',
        'common-root.lib','common-rpc.lib','common-transport.lib',
        'common-codec.lib','common-console.lib')
    $libraries = $dependencies | ForEach-Object {
        $path = Join-Path (Join-Path $root $BuildCache) $_
        if (!(Test-Path $path)) { throw "Missing fixture dependency: $_" }
        $path
    }
    # Same alias as the production ntvdm export definition; fixture overrides
    # keep their existing controlled host/register seams, never unresolved forcing.
    $extraFlags = @('/force:multiple','/alternatename:_call_ica_hw_interrupt=_ica_hw_interrupt')
}
& $wrapper link /nologo /OPT:REF @extraFlags "/OUT:$exe" "/MAP:$exe.map" @objects @libraries kernel32.lib ntdll.lib netapi32.lib user32.lib gdi32.lib advapi32.lib rpcrt4.lib shell32.lib legacy_stdio_definitions.lib 2>&1 |
    Tee-Object -FilePath (Join-Path $out 'build.txt') -Append
if ($LASTEXITCODE -ne 0) { throw 'Link failed.' }
if ($OriginalProviders) {
    $map = Get-Content -LiteralPath "$exe.map" -Raw
    foreach ($symbol in @('VrGetUserName','VrGetCDNames')) {
        if ($map -notmatch "(?m)^.*_$symbol\s+.*\bvrnetapi\.obj\s*$") {
            throw "Original entrypoint not bound to this run's compiled unit: $symbol"
        }
    }
}
$info = [Diagnostics.ProcessStartInfo]::new()
$info.FileName = $exe; $info.UseShellExecute = $false; $info.CreateNoWindow = $true
$info.RedirectStandardOutput = $true; $info.RedirectStandardError = $true
$p = [Diagnostics.Process]::new(); $p.StartInfo = $info
try {
    if (!$p.Start()) { throw 'Fixture did not start.' }
    $stdout = $p.StandardOutput.ReadToEndAsync(); $stderr = $p.StandardError.ReadToEndAsync()
    if (!$p.WaitForExit(10000)) { $p.Kill(); $p.WaitForExit(); throw 'Timeout, not a pass.' }
    $text = $stdout.Result + $stderr.Result
    $text | Set-Content -LiteralPath (Join-Path $out 'result.txt') -Encoding UTF8
    [pscustomobject]@{exit_code=$p.ExitCode;original_providers=[bool]$OriginalProviders
        executable_sha256=(Get-FileHash $exe).Hash
        source_hashes=@($inputs | ForEach-Object {
            [pscustomobject]@{path=$_;sha256=(Get-FileHash $_).Hash}
        })
    } | ConvertTo-Json -Depth 4 | Set-Content (Join-Path $out 'result.json')
    Write-Output $text
    $count = if ($OriginalProviders) { 10 } else { 15 }
    if ($p.ExitCode -ne 0 -or $text -notmatch "assertions=$count failures=0") {
        throw 'OEM/CD assertions failed.'
    }
} finally { $p.Dispose() }
