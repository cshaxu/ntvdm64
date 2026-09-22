param([string]$BuildRoot = 'build/M0-T420/S40/bitmap-binding-r1')
$ErrorActionPreference = 'Stop'
$target = [IO.Path]::GetFullPath($BuildRoot)
$allowed = [IO.Path]::GetFullPath('build') + [IO.Path]::DirectorySeparatorChar
if (-not $target.StartsWith($allowed, [StringComparison]::OrdinalIgnoreCase)) { throw 'Build root required' }
$null = New-Item -ItemType Directory -Force $target
# Synthetic 2x1 black/white BMP: 14-byte file header, 40-byte DIB,
# two palette entries and one DWORD scanline. Not historical guest media.
$stream = [IO.MemoryStream]::new()
$writer = [IO.BinaryWriter]::new($stream)
try {
    $writer.Write([uint16]0x4d42)
    $writer.Write([uint32]66)
    $writer.Write([uint32]0)
    $writer.Write([uint32]62)
    foreach ($value in @(40, 2, 1)) { $writer.Write([uint32]$value) }
    $writer.Write([uint16]1)
    $writer.Write([uint16]1)
    foreach ($value in @(0, 4, 0, 0, 0, 0, 0, 0xffffff, 0x80)) { $writer.Write([uint32]$value) }
    $writer.Flush()
    if ($stream.Length -ne 66) { throw 'Unexpected bitmap size' }
    [IO.File]::WriteAllBytes((Join-Path $target 'fixture-bitmap.bmp'), $stream.ToArray())
} finally { $writer.Dispose(); $stream.Dispose() }
