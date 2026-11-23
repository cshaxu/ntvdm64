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
& cmd.exe /d /c "`"$root/tools/build-fixtures.cmd`" x86 redirector-async" 2>&1 |
    Tee-Object (Join-Path $out 'build.txt')
if($LASTEXITCODE){throw 'Native redirector async fixture build failed'}
$exe=Join-Path $out 'redirector-async-publication-test.exe'
Copy-Item (Join-Path $root 'build/.ninja/fixtures-x86/redirector_async_publication_test.exe') $exe
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
    if ($process.ExitCode -ne 0 -or $text -notmatch 'assertions=27 failures=0') {
        throw 'Async publication assertions failed.'
    }
} finally { $process.Dispose() }
