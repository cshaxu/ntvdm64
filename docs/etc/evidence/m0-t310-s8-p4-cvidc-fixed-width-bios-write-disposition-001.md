# M0 T310 S8 P4 C-video fixed-width BIOS-write disposition

## Scope

This review covers every remaining selected `C4244` record in the original
`softpc.new/base/cvidc/ev_glue.c` translation unit:

- `write_byte_ev_glue` at line 117: `IU32` to `IU8`;
- `write_word_ev_glue` at line 138: `IU32` to `IU16`;
- `write_bios_byte` at line 726: `ULONG` to `IU8`;
- `write_bios_word` at line 736: `ULONG` to `IU16`.

## Original contract

The first pair is the original C-video byte/word vector boundary: `eaOff` is
the 32-bit guest/video offset and `eaVal` is already the byte/word value. The
called `b_write` and `w_write` vector slots intentionally consume those fixed
guest widths. The latter pair is the existing `MVDM-HOST-DIV-062` outer
base-video callback binding: base/video supplies two `ULONG` values, and the
original C-video rules deliberately use their low byte or word.

MSVC keeps `IU32`, `ULONG`, `IU8`, and `IU16` fixed-width on both x86 and x64.
No native pointer, handle, `size_t`, function pointer, or mapping-manager
identity crosses any of the four conversions. The fresh formal graph emits
the same four C4244 forms on both targets.

## Disposition

These are original fixed-width guest/video scalar conversions, not an
x86/x64 ABI transition. They remain compiler-visible and source-unchanged.
Adding casts would only suppress a historical range diagnostic and would not
improve either target's contract. The governed classifier records all four as
`not-x86-x64-fixed-width-cvideo-video-scalar-contract`.
