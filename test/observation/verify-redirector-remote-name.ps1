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
# Reuse the actual selected translation unit's compiler context.
$ninja = Get-Content (Join-Path $root 'build/.ninja/msvc-x86-release/worker/build.ninja') -Raw
$match = [regex]::Match($ninja, '(?m)^build obj/redir/vrremote\.obj:.*\r?\n  cflags = (.*)$')
if (!$match.Success) { throw 'Missing actual vrremote compiler context; run cached product build first.' }
$flags = $match.Groups[1].Value.Trim().Replace('$:', ':')
$sources = @('src/mvdm-host/vdmredir/vrremote.c',
    'src/compat/opennt_support_rtl.c',
    'test/mvdm-host/vdmredir/remote_computer_name_test.c')
$commands = @('@echo off', ('call "' + $vs + '" -arch=x86 -host_arch=x64 >nul'),
    'if errorlevel 1 exit /b %errorlevel%')
$objects = @()
foreach ($source in $sources) {
    $object = Join-Path $out ([IO.Path]::GetFileNameWithoutExtension($source) + '.obj')
    $commands += 'cl /Gy ' + $flags + ' /Fo"' + $object + '" "' + (Join-Path $root $source) + '"'
    $commands += 'if errorlevel 1 exit /b %errorlevel%'
    $objects += '"' + $object + '"'
}
$exe = Join-Path $out 'remote-computer-name-test.exe'
$commands += 'link /nologo /OPT:REF /OUT:"' + $exe + '" ' + ($objects -join ' ') + ' kernel32.lib ntdll.lib advapi32.lib legacy_stdio_definitions.lib'
$commands += 'exit /b %errorlevel%'
$commands | Set-Content -LiteralPath $wrapper -Encoding ASCII
& $wrapper 2>&1 | Tee-Object -FilePath (Join-Path $out 'build.txt')
if ($LASTEXITCODE -ne 0) { throw 'Compile/link failed.' }
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
        scope='Actual VrRemoteApi and production RTL; controlled NetAPI and unavailable RAP transport, no network/guest execution'
    } | ConvertTo-Json | Set-Content -LiteralPath (Join-Path $out 'result.json') -Encoding UTF8
    Write-Output $text
    if ($process.ExitCode -ne 0 -or $text -notmatch 'cases=4 failures=0') {
        throw 'Remote name assertions failed.'
    }
} finally { $process.Dispose() }
