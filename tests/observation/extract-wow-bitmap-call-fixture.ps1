param([string]$BuildRoot = 'build/M0-T420/S40/bitmap-call-r1')
$ErrorActionPreference = 'Stop'
$root = (Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$output = [IO.Path]::GetFullPath((Join-Path $root $BuildRoot))
if (!$output.StartsWith(($root + '\build\'), [StringComparison]::OrdinalIgnoreCase)) {
    throw 'Fixture output must be under repository build.'
}
$source = Join-Path $root 'src/mvdm/wow32/wuser.c'
$text = [IO.File]::ReadAllText($source)
$start = $text.IndexOf('ULONG FASTCALL WU32LoadBitmap(PVDMFRAME pFrame)')
$finish = $text.IndexOf('ULONG FASTCALL WU32WOWGetIdFromDirectory', $start)
if ($start -lt 0 -or $finish -le $start) { throw 'Original bitmap thunk not found.' }
New-Item -ItemType Directory -Force $output | Out-Null
[IO.File]::WriteAllText((Join-Path $output 'bitmap-call.inc'),
    $text.Substring($start, $finish - $start), [Text.Encoding]::ASCII)
Get-FileHash $source -Algorithm SHA256
