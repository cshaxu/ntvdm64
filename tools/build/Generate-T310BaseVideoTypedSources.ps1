[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)][string]$RepositoryRoot,
    [Parameter(Mandatory = $true)][string]$OutputDirectory
)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$videoRoot = Join-Path $root 'src/mvdm-host/softpc.new/base/video'
$output = [IO.Path]::GetFullPath($OutputDirectory)
[IO.Directory]::CreateDirectory($output) | Out-Null

foreach ($name in @('egawrtm0.c', 'egwrtm12.c')) {
    $sourcePath = Join-Path $videoRoot $name
    $source = Get-Content -Raw -LiteralPath $sourcePath
    $definitions = @{}
    # These two historical units declare table targets with `VOID fn()` then
    # define them later through IFNn.  Derive an equivalent early declaration
    # from that exact definition, preserving the original function/argument
    # spelling without changing the mirror source.
    foreach ($match in [regex]::Matches($source, '(?ms)^\s*(?:GLOBAL\s+)?VOID\s*(?:\r?\n\s*)?([A-Za-z_][A-Za-z0-9_]*)\s+IFN([0-9])\((.*?)\)')) {
        $definitions[$match.Groups[1].Value] = 'VOID ' + $match.Groups[1].Value + ' IPT' + $match.Groups[2].Value + '(' + ($match.Groups[3].Value -replace '\s+', ' ').Trim() + ');'
    }
    if ($definitions.Count -eq 0) { throw "No original IFN definitions discovered: $sourcePath" }
    $rewritten = [regex]::Replace($source, '(?m)^([ \t]*VOID[ \t]+)([A-Za-z_][A-Za-z0-9_]*)\s*\(\s*\)\s*;', {
        param($match)
        $symbol = $match.Groups[2].Value
        if ($definitions.ContainsKey($symbol)) { return $definitions[$symbol] }
        return $match.Value
    })
    $header = "/* Generated source-shaped declaration carrier.  Derived from exact original $name.`r`n * DIVERGENCE: table-target declarations receive the later original IFN signature`r`n * before their initializer; no algorithm, table order, or function body changes. */`r`n"
    [IO.File]::WriteAllText((Join-Path $output $name), $header + $rewritten, [Text.UTF8Encoding]::new($false))
}

$gfxPath = Join-Path $videoRoot 'gfx_updt.c'
$gfx = Get-Content -Raw -LiteralPath $gfxPath
$replacementMap = [ordered]@{
    '(boolean(*)())cga_mark_byte' = 't310_cga_mark_byte'
    '(boolean(*)())cga_mark_word' = 't310_cga_mark_word'
    '(boolean(*)())ega_mark_byte' = 't310_ega_mark_byte'
    '(boolean(*)())ega_mark_word' = 't310_ega_mark_word'
    '(boolean(*)())ega_mark_string' = 't310_ega_mark_string'
    '(boolean(*)())ega_mark_wfill' = 't310_ega_mark_wfill'
}
foreach ($pair in $replacementMap.GetEnumerator()) { $gfx = $gfx.Replace($pair.Key, $pair.Value) }
$gfx = $gfx.Replace('update_alg.mark_fill = (boolean(*)())cga_mark_string;', 'update_alg.mark_fill = t310_cga_mark_fill;')
$gfx = $gfx.Replace('update_alg.mark_wfill = (boolean(*)())cga_mark_string;', 'update_alg.mark_wfill = t310_cga_mark_wfill;')
$gfx = $gfx.Replace('update_alg.mark_string = (boolean(*)())cga_mark_string;', 'update_alg.mark_string = t310_cga_mark_string;')
$wrappers = @'
/* DIVERGENCE: original update_alg stores VOID marking procedures through
 * boolean callback slots.  All reached callers discard the boolean result.
 * These typed carriers preserve the original marking call and define that
 * historically unspecified result as FALSE. */
static boolean t310_cga_mark_byte IFN1(int, address) { cga_mark_byte(address); return FALSE; }
static boolean t310_cga_mark_word IFN1(int, address) { cga_mark_word(address); return FALSE; }
static boolean t310_cga_mark_fill IFN2(int, first, int, last) { cga_mark_string(first, last); return FALSE; }
static boolean t310_cga_mark_wfill IFN3(int, first, int, last, int, colour) { UNUSED(colour); cga_mark_string(first, last); return FALSE; }
static boolean t310_cga_mark_string IFN2(int, first, int, last) { cga_mark_string(first, last); return FALSE; }
static boolean t310_ega_mark_byte IFN1(int, address) { ega_mark_byte(address); return FALSE; }
static boolean t310_ega_mark_word IFN1(int, address) { ega_mark_word(address); return FALSE; }
static boolean t310_ega_mark_fill IFN2(int, first, int, last) { ega_mark_string(first, last); return FALSE; }
static boolean t310_ega_mark_wfill IFN3(int, first, int, last, int, colour) { ega_mark_wfill(first, last, colour); return FALSE; }
static boolean t310_ega_mark_string IFN2(int, first, int, last) { ega_mark_string(first, last); return FALSE; }

'@
$marker = 'GLOBAL VOID' + [Environment]::NewLine + 'set_mark_funcs IFN0()'
if (!$gfx.Contains($marker)) { throw "Unable to find original set_mark_funcs boundary: $gfxPath" }
$gfx = $gfx.Replace($marker, $wrappers + $marker)
$gfxHeader = "/* Generated source-shaped callback carrier.  Derived from exact original gfx_updt.c.`r`n * DIVERGENCE: only the historically untyped marking callback casts are replaced.`r`n */`r`n"
[IO.File]::WriteAllText((Join-Path $output 'gfx_updt.c'), $gfxHeader + $gfx, [Text.UTF8Encoding]::new($false))

# ega_writ.c imports eight CHAIN2 BIOS writers which are defined by the two
# generated files above.  Project their exact original IFN2 definitions into
# the original import block before assignment to video.h's already-typed
# globals; this is a declaration-only carrier.
$writeDefinitions = @{}
foreach ($provider in @('egawrtm0.c', 'egwrtm12.c')) {
    $providerText = Get-Content -Raw -LiteralPath (Join-Path $videoRoot $provider)
    foreach ($match in [regex]::Matches($providerText, '(?ms)^\s*(?:GLOBAL\s+)?VOID\s*(?:\r?\n\s*)?([A-Za-z_][A-Za-z0-9_]*)\s+IFN([0-9])\((.*?)\)')) {
        $writeDefinitions[$match.Groups[1].Value] = 'IMPORT VOID ' + $match.Groups[1].Value + ' IPT' + $match.Groups[2].Value + '(' + ($match.Groups[3].Value -replace '\s+', ' ').Trim() + ');'
    }
}
$egaWritPath = Join-Path $videoRoot 'ega_writ.c'
$egaWrit = Get-Content -Raw -LiteralPath $egaWritPath
# Keep the source line boundary intact.  `\s*` after `^` can consume the
# preceding newline under the multiline expression, concatenating consecutive
# declarations in the generated declaration carrier.
$egaWrit = [regex]::Replace($egaWrit, '(?m)^[ \t]*IMPORT\s+VOID\s+([A-Za-z_][A-Za-z0-9_]*)\s*\(\s*\)\s*;', {
    param($match)
    $symbol = $match.Groups[1].Value
    if ($writeDefinitions.ContainsKey($symbol)) { return $writeDefinitions[$symbol] }
    return $match.Value
})
$egaWritHeader = "/* Generated source-shaped declaration carrier.  Derived from exact original ega_writ.c.`r`n * DIVERGENCE: CHAIN2 BIOS writer imports receive the exact original provider IFN signatures.`r`n */`r`n"
[IO.File]::WriteAllText((Join-Path $output 'ega_writ.c'), $egaWritHeader + $egaWrit, [Text.UTF8Encoding]::new($false))

$vgaPath = Join-Path $videoRoot 'vga_mode.c'
$vga = Get-Content -Raw -LiteralPath $vgaPath
$vga = $vga.Replace('extern void jazz_text_update();', 'extern void jazz_text_update IPT0();')
$vgaHeader = "/* Generated source-shaped declaration carrier.  Derived from exact original vga_mode.c.`r`n * DIVERGENCE: preserve the original zero-argument display-update ABI at its external declaration.`r`n */`r`n"
[IO.File]::WriteAllText((Join-Path $output 'vga_mode.c'), $vgaHeader + $vga, [Text.UTF8Encoding]::new($false))

Write-Output ('generated=' + (Join-Path $output 'egawrtm0.c') + ';' + (Join-Path $output 'egwrtm12.c') + ';' + (Join-Path $output 'gfx_updt.c') + ';' + (Join-Path $output 'ega_writ.c') + ';' + (Join-Path $output 'vga_mode.c'))
