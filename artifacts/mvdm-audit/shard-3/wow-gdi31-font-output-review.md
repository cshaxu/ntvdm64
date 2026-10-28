# S10 shard 3: selected GDI 3.1 font-query output boundaries

Read-only continuation of `wow-gdi-print-audit.md` for selected original `wgdi31.c`, SHA-256 `38D1D570FD1625AFFDF99EB1CE072E78B26A532ABFD5FC9DD32C10B2C46D06E6`. `wgtbl2.h` registers the font query thunks; cached WOW32 map associates them with `wgdi31.obj`. `wow-generic-thunk-published-binding.md` records cached/release/installed code-section identity, so cached map section attribution applies to the observed published code, without proving execution. No new build, guest execution or fault injection.

Original thunks retain guest argument/structure conversion while host GDI owns real HDC, font and query result. `WG32GetGlyphOutline` delegates through registered `ADAPTER-WIN32-053`: `GetGlyphOutlineWow` uses a one-shot font selection to reproduce the old ignore-rotation policy, restores the original selection and deletes the temporary font. The focused rotated-font fixture and scope are in `m0-t456-s2-glyph-outline-rotation.md`; it does not establish all Win16 thunk output/error behavior.

Specific output and failure contracts needing validation:

1. `WG32GetFontData` allocates `f5` bytes and copies the full requested length back when buffer exists regardless of the API result. `WG32GetGlyphOutline` similarly copies requested `f5` bytes and publishes `Metrics` if its pointer is set without gating on the native return. Evaluate short returns and GDI error results against original observable behavior and initialized bytes before concluding correctness; native success/partial-output contracts must be established in controlled cases. No runtime defect is claimed.
2. `WG32GetKerningPairs` allocates `f2` entries and calls `putkerningpairs16` for `f2` entries regardless of native returned count; helper writes that many entries but passes one `KERNINGPAIR16` size to `FLUSHVDMPTR`. When the provider returns fewer entries or zero, the resulting bytes and mapping/flush semantics are not established here. Test exact returned count, initialization, cross-page/selector boundaries and actual flush provider. This is a concrete source pattern, not a reproduced corruption.
3. `WG32GetCurrentPositionEx` writes its stack POINT back after the native call even on false return. `WG32GetOutlineTextMetrics` writes the requested guest structure when its pointer is nonzero irrespective of the native result. Validate native failure and guest output preservation contract rather than assuming a stack value is initialized by the provider.
4. `WG32GetCharABCWidths` converts a range into a WORD count before host allocation; reversed/boundary ranges require semantic and bounds checks; do not infer an issue without controlled inputs.

`wgdi31.c` remains **pending** at whole-file scope. Printer success, glyph-rotation fixture, and dispatch/map presence are bounded evidence, not a font/output lifecycle pass. All findings above preserve original-source attribution; repair needs relevant failure/partial-output tests and registered divergence, not blind rewriting.

Replay:

```powershell
Get-FileHash src/mvdm/wow32/wgdi31.c -Algorithm SHA256
rg -n 'WG32Get(FontData|GlyphOutline|KerningPairs|OutlineTextMetrics|CurrentPositionEx|CharABCWidths)' src/mvdm/wow32/wgdi31.c src/mvdm/wow32/wgtbl2.h
rg -n 'putkerningpairs16|FLUSHVDMPTR' src/mvdm/wow32/wstruc.c
rg -n 'wgdi31.obj' build/.ninja/msvc-x86-release/wow32/wow32.dll.map
Get-Content docs/etc/evidence/m0-t456-s2-glyph-outline-rotation.md
```
