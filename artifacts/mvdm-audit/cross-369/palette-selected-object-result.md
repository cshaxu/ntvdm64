# Selected WOW palette object: controlled short-return reproduction

The test-only `palette-selected-object-fixture.c` linked the **current cached `wgpal.obj`**, not a source-body copy. The map binds `_WG32GetPaletteEntries@4` to `wgpal.obj`, native `GetPaletteEntries` to `gdi32.dll`, and `putpaletteentry16` to the fixture. The fixture supplies deterministic 16:16 guest-copy, palette-alias and seeded-allocation seams; it does not execute the original `wstruc.obj` copy routine. Object disassembly independently shows it forwards the original requested count to `putpaletteentry16`. This complements the prior exact-source-copy fixture in `shard-0/wow-palette-image-review.md`.

On this run the executable returned 0 and printed:

```text
short result=1 copied=4 first=4,5,6 tail=a5 alloc=1 free=1
invalid result=0 copied=4 first=a5 alloc=2 free=2
```

Thus the actual cached original thunk requests copy of four entries after a real host result of one or zero, and the controlled copy seam observes seeded untouched tail bytes. Allocation/free counters balance in both cases. This does not demonstrate a real guest mapping, selected original copy body, process teardown, or naturally occurring guest-visible failure. The link uses `/OPT:REF /FORCE:UNRESOLVED`; three unresolved symbols belong to other unused `wgpal.obj` functions, so this executable is test-only and cannot establish those functions' behavior.

| Input at capture | SHA-256 |
| --- | --- |
| `src/mvdm/wow32/wgpal.c` | `942BD3C12615416A532638DF4B53A08E963BD1E60DD0D081FD34765ACE0872C8` |
| `build/.ninja/msvc-x86-release/wow32/obj/wgpal.obj` | `194EFBDB7C9C632C434644CB4A2FC2D6CC29062C4884442A69E2A8DB74439D59` |
| `palette-selected-object-fixture.c` | `A3E6AFD3F203FFAF6D73E9019D747CA31B91AC5236A4DECC2ABF2EF3DDF396E9` |
| `build/t473-s10/palette-object-shard3-r1/fixture.exe` | `A0FA7854508A28489E97C9DE9B1E7B0311F6726EAA7C07EF63A676B921457FA9` |
| corresponding `fixture.map` | `BF17210DD5CE6645B0D64DC578883E8781EECE37276CBC677415FAA3835BF96B` |

Build in a fresh owned directory using x86 MSVC, `/MT /O2 /Gy`, this fixture and that cached object, `/link /OPT:REF /FORCE:UNRESOLVED /MAP:<output> gdi32.lib user32.lib`; run only the isolated executable and verify map ownership as above. Recheck hashes and guest-frame layout against the actual object before reusing. The output is a counterexample witness, not a corrected product. No product, guest, published image or shared ledger was changed.
