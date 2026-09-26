[CmdletBinding()]
param([string]$RepositoryRoot)
$ErrorActionPreference='Stop'
if(!$RepositoryRoot){$RepositoryRoot=Split-Path -Parent (Split-Path -Parent $PSScriptRoot)}
# Source guard only. This does not certify copied frames or Window rendering.
function ReadCode([string]$path) {
    $text=Get-Content -LiteralPath (Join-Path $RepositoryRoot $path) -Raw
    [regex]::Replace($text,'(?s)/\*.*?\*/|//[^\r\n]*',' ')
}
function Require([string]$text,[string]$pattern,[string]$label) {
    if($text -notmatch $pattern){throw "Video source contract changed: $label"}
    "PASS source: $label"
}
$header=ReadCode 'src/mvdm/softpc.new/host/inc/nt_graph.h'
Require $header '#define\s+MONO_BITS_PER_PIXEL\s+1\b' 'monochrome bitmap is one-bit'
foreach($name in @('CGA','EGA','VGA')) {
    Require $header ("#define\s+${name}_BITS_PER_PIXEL\s+8\b") "$name host bitmap is eight-bit indexed"
}
$graph=ReadCode 'src/mvdm/softpc.new/host/src/nt_graph.c'
Require $graph 'biHeight\s*=\s*-height\s*;' 'original DIB has top-down height'
Require $graph 'biCompression\s*=\s*BI_RGB\s*;' 'uncompressed original host DIB'
Require $graph '(?s)case\s+DIB_PAL_COLORS\s*:.*?tabSize\s*=\s*maxColours\s*\*\s*sizeof\(WORD\)' 'palette table entries are WORD indices, not RGB bytes'
Require $graph '(?s)CreateConsoleScreenBuffer\(.*?CONSOLE_GRAPHICS_BUFFER\s*,\s*&sc\.ConsoleBufInfo\)' 'original graphics buffer ABI remains selected'
Require $graph 'SetConsolePalette\(sc\.ScreenBufHandle,\s*sc\.ColPalette,\s*SYSPAL_STATIC\)' 'palette publication uses original buffer and palette'
$event=ReadCode 'src/mvdm/softpc.new/host/src/nt_event.c'
Require $event '(?s)for\s*\(dw\s*=\s*0;\s*dw\s*<\s*EGA_TICK_DELAY\s*&&\s*get_mode_change_required\(\);\s*\+\+dw\)\s*host_graphics_tick\(\);\s*\(\*update_alg\.calc_update\)\(\);' 'final handoff settles original pending mode before original paint with its original finite bound'
$vga=ReadCode 'src/mvdm/softpc.new/host/src/nt_vga.c'
Require $vga '(?s)GrabMutex\(sc\.ConsoleBufInfo\.hMutex\).*?RelMutex\(sc\.ConsoleBufInfo\.hMutex\).*?InvalidateConsoleDIBits\(sc\.ScreenBufHandle,\s*&rect\)' 'bitmap write completes before dirty-region publication'
Require $vga 'rect\.Right\s*=\s*rect\.Left\s*\+\s*\(width\s*<<\s*1\)\s*-\s*1' 'dirty rectangle has inclusive right edge'
Require $vga 'rect\.Bottom\s*=\s*rect\.Top\s*\+\s*\(height\s*<<\s*1\)\s*-\s*1' 'dirty rectangle has inclusive bottom edge'
$probe=ReadCode 'tests/observation/video_graphics_vram.asm'
Require $probe '(?is)mov\s+ax,\s*0013h.*?int\s+10h.*?mov\s+ax,\s*0003h.*?int\s+10h' 'existing guest witness returns from mode 13h to text'
'SOURCE ONLY: no graphics-buffer allocation, pixel transport, palette rendering or Window acceptance inferred.'
