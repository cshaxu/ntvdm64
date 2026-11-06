# T478 S22 — font output count and retained original limits

Baseline: dfb396e21. Finding 51 is dispositioned per subcase, not declared a
whole-file font correctness proof. Only the confirmed kerning copy count is
changed (MVDM-HOST-DIV-877).

## Confirmed correction

The original GetKerningPairs thunk allocates the requested capacity and converts
that entire capacity after GDI returns. On this host, Arial has 120 pairs;
requesting 123 returns 120 and leaves the native tail untouched. The old thunk
nevertheless converts all 123 temporary entries. malloc_w does not zero the
allocation. The fix converts only ul returned entries when ul is nonzero,
then frees the allocation as before. Native return, allocation failure,
query-only operation, handles, original converter and resource ownership stay
unchanged. No clamp, retry, zero filling, parameter validation or new capability.

The [native API contract](https://learn.microsoft.com/en-us/windows/win32/api/wingdi/nf-wingdi-getkerningpairsa)
defines the returned number of entries. The fixture's untouched guest tail is
an assertion about this corrected conversion boundary, not a new guarantee
about every native GDI failure buffer.

## Evidence and limits

[Host fixture](../../../test/integration/wow_font_output_test.c) and
[extracting runner](../../../test/integration/verify-wow-font-output.ps1)
compile three exact thunk bodies and original argument declarations with x86
MSVC /W4 /WX. Guest pointer/handle translation and pair conversion are explicit
seams; GDI is real except the zero-return kerning provider control. The selected
rotation adapter is not exercised by this fixture: unrotated native glyph
calls are controls, and existing rotation evidence remains applicable.
Baseline: 20 checks, two failures (short kerning tail and zero-return copy).
Fixed: 20 checks, zero failures. Temporary allocations retire in both paths.
Font-data success/size query, glyph metrics/bitmap/size query and kerning size
query retain their observed outputs and returns.

The [independent Win16 witness](../../../test/integration/wow_kerning_guest.asm)
and [isolated runner](../../../test/integration/verify-wow-kerning-guest.ps1)
exercise real GDI16 ordinal 332 through WOW32, native GDI and the original
six-byte KERNINGPAIR16 converter. It creates a DC/font, requests 123 pairs,
checks the 120 result and the final three sentinel entries, restores selection,
deletes font/DC, writes a binary report and exits. The S20 package fails the
tail check; the S22 package passes and exits zero. No guest mirror/media changes.
This is a real guest buffer witness, unlike the host fixture's conversion seam.

The [receipt](../../../artifacts/mvdm-audit/shard-3/font-output-s22-results.json)
records source comparisons, focused results and package verification.
All six reviewed thunk bodies match both original families before this change;
afterward only GetKerningPairs differs. Current WOW32 map selects wgdi31.obj.

## Disposition of the other finding-51 candidates

| Candidate | Evidence and disposition |
| --- | --- |
| Kerning one-element mapping/flush length | Release pointer provider maps directly to guest memory; current CPU40 flush is the original no-op. Real guest multi-entry conversion succeeds. No copy-back truncation was established, so helper sizes are unchanged. This does not grant validity to arbitrary selectors or undersized caller buffers. |
| Font/glyph requested-length copy | Valid font data and glyph cases retain native outputs. Native GGO_METRICS unexpectedly returns 48 here and initializes the supplied buffer; bitmap returns 60 and likewise initializes its full supplied buffer. The initial assertion that metrics mode would leave the buffer untouched was disproved and removed. These observations do not establish every driver or failure case. No successful short-output corruption requiring another correction was proved. |
| Failed font/glyph/outline or current-position output | Original thunks may publish temporary output on native failure; this remains an explicit original limitation, not a newly guaranteed untouched-output or recovery contract. This S does not claim those buffers initialized on all failures, and does not add error tolerance based solely on their spelling. Native error returns remain the caller's boundary. Reopen if a supported caller consumes failed output or a concrete fault escapes that contract. |
| Outline success conversion | Original structure/string translation is retained, including its native/Win16 size adjustment. No new layout or partial-structure guarantee is introduced; whole-file correctness is not claimed. |
| ABC range arithmetic | Original WORD range and success-gated conversion remain. Reversed/full-wrap ranges and invalid selectors are not expanded into supported input. No supported-range failure was demonstrated here. |

The [glyph API documentation](https://learn.microsoft.com/en-us/windows/win32/api/wingdi/nf-wingdi-getglyphoutlinea)
is not used to override observed native return values or invent a different
rotation/provider policy. The earlier metrics-buffer hypothesis was a fixture
assumption, not a product defect; baseline/current controls now compare actual
native output instead.

## Delivery

Coherent x86/x64 build passed; all 16 Full gates pass. Ten released images
and manifest exactly match tested package 7fdf5ef1-17fb-46e2-ac6c-30aafd0d8d49
(version 0.0.478, protocol 67). DOS4GW/NESticle remains owner-excluded,
skipped rather than passed.
Only this worktree assets/release is authorized. No O:/winnt write or push.
S23/finding 55 follows after S22 delivery.
