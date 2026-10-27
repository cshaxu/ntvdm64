# S10 shard 2: selected WOW GDI font, text and metafile group

The frozen rows `wow32/wgfont.c`, `wow32/wgtext.c` and `wow32/wgmeta.c`
match both original source editions and are selected product inputs, but
remain pending until per-API behavioral obligations are checked. Identity
alone and selected build do not meet the full-restoration rule.

The retained x86 product link map SHA256
`37CE4B735DA1536D2DEE3492169D5A46785C04111E305A378E239B33D552B1F5`
corresponds to `wow32.dll` SHA256
`C97C512C6F6BD6F16A6FB68F340760F551CD2186B5262B1A6AE892FC8A06BC58`.
It lists `WG32CreateFont`/`WG32EnumFonts` from wgfont.obj,
`WG32ExtTextOut`/`WG32GetTextExtent`/`WG32TextOut` from wgtext.obj, and
`WG32CloseMetaFile`/`WG32EnumMetaFile` from wgmeta.obj. Original
`wgtbl2.h` maps each named entry to its GDI thunk table; map presence is
extraction evidence for those entrypoints, not runtime dispatch proof of the
whole file.

- `wgfont.c` bridges font creation and enumeration, including callback and
  compatibility-flag cases. `W32EnumFontFunc`/`W32EnumFontHandler` have
  callback-lifetime and enumeration-filter semantics; these need actual
  guest callback, errors and nested retirement witnesses. T456 S4's glyph
  rotation fixture targets a particular public glyph boundary and does not
  exercise every wgfont entry.
- `wgtext.c` owns source thunks for text output, extent, metrics and related
  GDI calls, requiring per-API guest buffer/encoding, count, clipping,
  copyback, failure and resource-lifetime checks. Current generic control
  text fixtures and successful drawing cannot by themselves prove all of
  these paths or all original compatibility flags.
- `wgmeta.c` owns classic metafile close/conversion, copy, play, and callback
  enumeration; the guest GDI local APIs for bits are explicitly distinct.
  T456 S3's public host fixture is explicitly not an extraction of wgmeta.c;
  S4 reports eight repetitions of that finite host boundary. It verifies
  host availability, not actual guest/host record and callback ownership for
  every record, failure or nested case.

For group classification, construct a per-ordinal matrix from wgtbl2.h and
separately mark selected/link identity, original-guest dispatch, output and
failure, alias/callback retirement. Preserve the finite T456 results as
historical host seam witnesses and retain all three files pending until the
remaining required dimensions are evidenced. No tests were rerun, no guest or
product source was changed, and no whole-file restored percentage is inferred
from a subset of table entries.
