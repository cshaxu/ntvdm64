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
& "$root/tools/build-fixtures.cmd" x86 redirector-remote-name *> "$out/build.txt"
if($LASTEXITCODE){throw 'Native remote-name build failed'}
$exe=Join-Path $out 'remote-computer-name-test.exe'
Copy-Item "$root/build/.ninja/msvc-x86-release/worker/remote-computer-name-test.exe" $exe
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
