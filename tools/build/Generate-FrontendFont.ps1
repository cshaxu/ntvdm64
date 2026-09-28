param([Parameter(Mandatory)][string]$OutputFile)
$ErrorActionPreference = 'Stop'
$repo = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$output = [IO.Path]::GetFullPath($OutputFile)
$prefix = (Join-Path $repo 'build') + [IO.Path]::DirectorySeparatorChar
if (!$output.StartsWith($prefix,[StringComparison]::OrdinalIgnoreCase)) { throw 'Font output must remain below build/' }
$rom = Join-Path $repo 'src/mvdm/softpc.new/roms/v7vga.rom'
if ((Get-FileHash -LiteralPath $rom -Algorithm SHA256).Hash -ne '970F105CD9E42EE56F07AAE695BAC89786D3455AB9D4C1EA9A1D1643B1E8F6F0') {
    throw 'Selected V7VGA font source changed'
}
$bytes = [IO.File]::ReadAllBytes($rom)
# Original sas.h EGA_CGMN_OFF; ega_vide.c loads 256 glyphs with height 14.
$lines = @('/* Generated from pinned V7VGA EGA_CGMN bitmap data, not executable firmware. */',
    'static const unsigned char frontend_native_font[256][14] = {')
for ($glyph=0; $glyph -lt 256; ++$glyph) {
    $offset = 0x2230 + $glyph*14
    $values = $bytes[$offset..($offset+13)] | ForEach-Object { '0x{0:x2}' -f $_ }
    $lines += '    {' + ($values -join ',') + '},'
}
$lines += '};'
$text = ($lines -join "`n") + "`n"
[void](New-Item -ItemType Directory -Force -Path ([IO.Path]::GetDirectoryName($output)))
if (!(Test-Path -LiteralPath $output) -or [IO.File]::ReadAllText($output) -cne $text) {
    [IO.File]::WriteAllText($output,$text,[Text.UTF8Encoding]::new($false))
}
