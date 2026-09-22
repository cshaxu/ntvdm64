param([string]$BuildRoot = 'build/M0-T420/S40/class-registration-r1')
$ErrorActionPreference = 'Stop'
$root = (Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$output = [IO.Path]::GetFullPath((Join-Path $root $BuildRoot))
if (!$output.StartsWith(($root + '\build\'), [StringComparison]::OrdinalIgnoreCase)) {
    throw 'Fixture output must be under repository build.'
}
$mirror = Join-Path $root 'src/mvdm/wow32/wmdisp32.c'
$original = 'O:/repos.external/OpenNT/base/mvdm/wow32/wmdisp32.c'
$expected = '4A9004C1F57AF28280D0176AFD72CAE76A7FBAE90FFB758426C287435E5912D4'
foreach ($path in @($mirror, $original)) {
    if ((Get-FileHash $path).Hash -ne $expected) { throw "Source changed: $path" }
}
$text = [IO.File]::ReadAllText($mirror)
$start = $text.IndexOf('LONG W32Win16WndProcEx(')
$end = $text.IndexOf('// The following functions are used to', $start)
if ($start -lt 0 -or $end -le $start) { throw 'Original function boundary changed.' }
New-Item -ItemType Directory -Force $output | Out-Null
[IO.File]::WriteAllText((Join-Path $output 'original-wndproc.inc'),
    $text.Substring($start, $end - $start), [Text.Encoding]::ASCII)
Write-Output 'WOW_WNDPROC_MIRROR_BYTE_EXACT_SELECTED_BODY'
