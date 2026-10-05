[CmdletBinding()]
param(
    [string]$BuildCache = 'build/M0-T427/S2/r001',
    [Parameter(Mandatory = $true)][string]$OutputRoot
)
$ErrorActionPreference = 'Stop'
$root = (Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$cache = (Resolve-Path (Join-Path $root $BuildCache)).Path
$out = [IO.Path]::GetFullPath((Join-Path $root $OutputRoot))
$allowed = [IO.Path]::GetFullPath((Join-Path $root 'build')) + '\'
if (!$out.StartsWith($allowed, [StringComparison]::OrdinalIgnoreCase)) {
    throw 'Evidence must be below repository build/.'
}
if (Test-Path -LiteralPath $out) { throw 'Select a fresh, unsealed evidence root.' }
$exe = Join-Path $cache 'keyboard-controller-pic-test.exe'
$map = Get-Content -LiteralPath ($exe + '.map') -Raw
foreach ($entry in @(@('kbd_inb','original-softpc-keymouse:keyba.obj'),
    @('kbd_outb','original-softpc-keymouse:keyba.obj'),
    @('KbdEOIHook','original-softpc-keymouse:keyba.obj'),
    @('ica_intack','original-softpc-system:ica.obj'))) {
    if ($map -notmatch ('(?m)^.*_' + $entry[0] + '\s+.*' + [regex]::Escape($entry[1]) + '\s*$')) {
        throw "Actual production provider missing: $($entry[0])"
    }
}
$graph = Get-Content -LiteralPath (Join-Path $cache 'build.ninja') -Raw
if ($graph -notmatch '/DNTVDM\b' -or $graph -notmatch '/DCPU_40_STYLE\b' -or
    $graph -match '/DCPU_30_STYLE\b') { throw 'Wrong selected controller/CCPU profile.' }
New-Item -ItemType Directory -Path $out | Out-Null
$start = [Diagnostics.ProcessStartInfo]::new()
$start.FileName = $exe
$start.UseShellExecute = $false
$start.CreateNoWindow = $true
$start.RedirectStandardError = $true
$start.RedirectStandardOutput = $true
$process = [Diagnostics.Process]::new()
$process.StartInfo = $start
$clock = [Diagnostics.Stopwatch]::StartNew()
try {
    if (!$process.Start()) { throw 'Fixture did not start.' }
    $stdout = $process.StandardOutput.ReadToEndAsync()
    $stderr = $process.StandardError.ReadToEndAsync()
    if (!$process.WaitForExit(10000)) {
        $process.Kill() # Only the exact process created by this test.
        $process.WaitForExit()
        throw 'Controller/PIC fixture timeout; not a pass.'
    }
    $text = $stdout.Result + $stderr.Result
    $text | Set-Content -LiteralPath (Join-Path $out 'controller.txt') -Encoding UTF8
    if ($process.ExitCode -ne 0 -or $text -notmatch 'assertions=18 failures=0' -or
        $text -notmatch 'RETAINED_LIMIT C0' -or
        $text -notmatch 'PASS actual INTACK/EOI') { throw 'Controller/PIC assertions failed.' }
    [pscustomobject]@{
        profile = 'x86 /MT NTVDM CCPU40'; elapsed_ms = $clock.ElapsedMilliseconds
        executable_sha256 = (Get-FileHash -LiteralPath $exe).Hash
        keyba_sha256 = (Get-FileHash (Join-Path $root 'src/mvdm/softpc.new/base/keymouse/keyba.c')).Hash
        ica_sha256 = (Get-FileHash (Join-Path $root 'src/mvdm/softpc.new/base/system/ica.c')).Hash
        original_limit = 'C0 selected NTVDM stale response retained, not repaired'
        scope = 'Actual controller/PIC; controlled host callbacks, no BIOS ISR/timer acceptance'
    } | ConvertTo-Json | Set-Content -LiteralPath (Join-Path $out 'result.json') -Encoding UTF8
    Write-Output $text
} finally { $process.Dispose() }
