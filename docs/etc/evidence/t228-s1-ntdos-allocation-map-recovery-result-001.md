# T228 S1 P16 — Reproducible NTDOS Allocation-Map Recovery Result

## Exact Build Result

The repository-local original NTDOS build completed in the disposable root
`build/M0-T228-S1/ntdos-allocation-map-rebuild-20260818a` using the retained
OpenNT source, BUILDIDX, NOSRVBLD, MASM, LINK16, RELOC and STRIPZ closure.
It produced `NTDOS.SYS` at 27,858 bytes with SHA-256
`957662320654ad5251c3a8b228a5dadec28aa65dddbcba38c3658a6e7f93bc84`, exactly
matching the immutable T228 NTDOS input.  The build manifest records all
source/tool identities and the 39 original modules.

The original link response emitted `NTDOS.MAP` (SHA-256
`fbac8088da0c935c8f8a4eaa685714d21cd51927a217d6ca47d355748a4e8baf`).
It establishes the final image segment layout:

| Linked range | Class |
| --- | --- |
| `00000h..07938h` | `DOSCODE` |
| `07940h..08f21h` | `DOSDATA` |

This is exact-image evidence, not a substitute NTDOS runtime input.

## Allocator Map Disposition

The original source still supplies the owner facts:

- `msinit.asm` writes `endmem`, establishes `arena_head`, and creates the
  initial terminal MCB from those values.
- `alloc.asm::$ALLOC` scans from `arena_head`, coalesces and selects free MCB
  blocks; it returns the largest available paragraph count when the `ffffh`
  allocation request cannot be satisfied.
- COMMAND `rdata.asm::EndInit`, already map-supported by P15, invokes this
  exact allocation protocol for both the resized environment and the
  largest-block transient placement.

However, the exact-image `NTDOS.MAP` has no public-symbol table: it contains
only the two linked segment ranges.  An evidence-only original LINK16
`/SEGMENTS` relink over the same objects emitted the same segment-only map;
it did not expose module contributions or private `$ALLOC`/`arena_head`
symbols.  That trial is rejected as an enrichment path, not adopted as a
build variant.

## Conclusion

P16 establishes NTDOS source-build identity and the shared final DOSCODE /
DOSDATA layout, but it does **not** provide an instruction or procedure offset
for the private allocator.  The repair boundary remains the complete,
map-supported COMMAND `EndInit` / original NTDOS MCB allocation-and-transient
placement package.  No map-based claim can name an NTDOS allocator instruction,
and no adapter, BOP, guest-image, or Bochs repair follows.

A further action must either observe a complete owner-domain state through an
already admitted generic mechanism, or repair a source-proven invariant with
whole-package regression.  It must not manufacture a private-symbol map,
heuristically disassemble NTDOS, or continue changing LINK16 output flags.