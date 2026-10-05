[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$OutputRoot)
$ErrorActionPreference = 'Stop'
$root = (Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$out = [IO.Path]::GetFullPath((Join-Path $root $OutputRoot))
$allowed = [IO.Path]::GetFullPath((Join-Path $root 'build')) + '\'
if (!$out.StartsWith($allowed, [StringComparison]::OrdinalIgnoreCase)) {
    throw 'Fixture output must be under repository build/.'
}
if (Test-Path -LiteralPath $out) { throw 'Use a fresh evidence directory.' }
New-Item -ItemType Directory -Path $out | Out-Null
$vs = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
$wrapper = Join-Path $out 'compiler.cmd'
@('@echo off', ('call "' + $vs + '" -arch=x86 -host_arch=x64 >nul'),
    'if errorlevel 1 exit /b %errorlevel%', '%*') |
    Set-Content -LiteralPath $wrapper -Encoding ASCII
$sources = @('src/ntvdm-exe/redir/mvdm_redirector_async.c',
    'src/ntvdm-exe/softpc/mvdm_guest_location.c',
    'src/ntvdm-exe/session/session.c',
    'src/ntvdm-exe/session/guest_memory_lease.c',
    'tests/mvdm-host/vdmredir/redirector_async_publication_test.c')
$objects = @()
foreach ($source in $sources) {
    $object = Join-Path $out ([IO.Path]::GetFileNameWithoutExtension($source) + '.obj')
    & $wrapper cl /nologo /TC /c /MT /W4 /DWIN_32 /DVDMREDIR_DLL /DNTVDM /DCPU_40_STYLE /DCCPU "/I$root/src" "/I$root/src/ntvdm-exe/redir/include" "/I$root/src/ntvdm-exe/softpc/include" "/Fo$object" (Join-Path $root $source) 2>&1 |
        Tee-Object -FilePath (Join-Path $out 'build.txt') -Append
    if ($LASTEXITCODE -ne 0) { throw "Compile failed: $source" }
    $objects += $object
}
$exe = Join-Path $out 'redirector-async-publication-test.exe'
& $wrapper link /nologo "/OUT:$exe" "/MAP:$exe.map" @objects kernel32.lib 2>&1 |
    Tee-Object -FilePath (Join-Path $out 'build.txt') -Append
if ($LASTEXITCODE -ne 0) { throw 'Link failed.' }
$start = [Diagnostics.ProcessStartInfo]::new()
$start.FileName = $exe
$start.UseShellExecute = $false
$start.CreateNoWindow = $true
$start.RedirectStandardOutput = $true
$start.RedirectStandardError = $true
$process = [Diagnostics.Process]::new()
$process.StartInfo = $start
$clock = [Diagnostics.Stopwatch]::StartNew()
try {
    if (!$process.Start()) { throw 'Fixture did not start.' }
    $stdout = $process.StandardOutput.ReadToEndAsync()
    $stderr = $process.StandardError.ReadToEndAsync()
    if (!$process.WaitForExit(10000)) {
        $process.Kill(); $process.WaitForExit()
        throw 'Fixture timed out, not a pass.'
    }
    $text = $stdout.Result + $stderr.Result
    $text | Set-Content -LiteralPath (Join-Path $out 'result.txt') -Encoding UTF8
    [pscustomobject]@{exit_code=$process.ExitCode; elapsed_ms=$clock.ElapsedMilliseconds
        executable_sha256=(Get-FileHash -LiteralPath $exe).Hash
        scope='Production async adapter/location/session/lease; controlled memory, not real pipe callback'
    } | ConvertTo-Json | Set-Content -LiteralPath (Join-Path $out 'result.json') -Encoding UTF8
    Write-Output $text
    if ($process.ExitCode -ne 0 -or $text -notmatch 'assertions=24 failures=0') {
        throw 'Async publication assertions failed.'
    }
} finally { $process.Dispose() }
