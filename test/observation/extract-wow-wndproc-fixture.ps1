param([string]$BuildRoot = 'build/M0-T420/S40/class-registration-r1')
$ErrorActionPreference = 'Stop'
$root = (Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$output = if ([IO.Path]::IsPathRooted($BuildRoot)) {
    [IO.Path]::GetFullPath($BuildRoot)
} else {
    [IO.Path]::GetFullPath((Join-Path $root $BuildRoot))
}
if (!$output.StartsWith(($root + '\build\'), [StringComparison]::OrdinalIgnoreCase)) {
    throw 'Fixture output must be under repository build.'
}
$mirror = Join-Path $root 'src/mvdm-host/wow32/wmdisp32.c'
$original = 'O:/repos.external/OpenNT/base/mvdm/wow32/wmdisp32.c'
$expected = '4A9004C1F57AF28280D0176AFD72CAE76A7FBAE90FFB758426C287435E5912D4'
if ((Get-FileHash -LiteralPath $original).Hash -ne $expected) {
    throw "Pinned original source changed: $original"
}
$bytes = [IO.File]::ReadAllBytes($mirror)
$text = [Text.Encoding]::GetEncoding(28591).GetString($bytes)
$signature = 'LONG W32Win16WndProcEx('
$terminator = '// The following functions are used to'
$start = $text.IndexOf($signature, [StringComparison]::Ordinal)
if ($start -lt 0) { throw 'Current mirror function is missing.' }
$end = $text.IndexOf($terminator, $start, [StringComparison]::Ordinal)
if ($end -le $start -or
    $text.IndexOf($signature, $start + $signature.Length, [StringComparison]::Ordinal) -ge 0) {
    throw 'Current mirror function boundary is missing or ambiguous.'
}
New-Item -ItemType Directory -Force $output | Out-Null
$body = New-Object byte[] ($end - $start)
[Array]::Copy($bytes, $start, $body, 0, $body.Length)
[IO.File]::WriteAllBytes((Join-Path $output 'original-wndproc.inc'), $body)
@{ mirrorSha256 = (Get-FileHash -LiteralPath $mirror -Algorithm SHA256).Hash;
   originalSha256 = $expected; start = $start; length = $body.Length;
   bodySha256 = (Get-FileHash -LiteralPath (Join-Path $output 'original-wndproc.inc') -Algorithm SHA256).Hash } |
    ConvertTo-Json | Set-Content -LiteralPath (Join-Path $output 'wndproc-input.json') -Encoding UTF8
Write-Output 'WOW_WNDPROC_CURRENT_MIRROR_BYTE_EXACT_BODY'
