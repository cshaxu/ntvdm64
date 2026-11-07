# T479 S9 CHAIN2 display repair

## Scope and source

Repair the Win3.1 DOS window/fullscreen text transition contract without
changing original guest binaries, firmware or the archived S6 candidate.
SoftPC source is read-only comparison/import material, not a build dependency.

| Surface | Source and disposition |
| --- | --- |
| CHAIN2 plane addresses, strides and dirty marking | SoftPC9250732e..1ec0d551; [hunk inventory](m0-t479-s9-upstream-hunk-map.json), [generated statement mapping](m0-t479-s9-generated-rule-import.json), [display mapping](m0-t479-s9-display-import.json). |
| Internal generated continuations | SoftPCf1d394ca6952e3812c9ea49037570069e4406302; [48 mappings](m0-t479-s9-continuation-import.json). Real rule calls remain unchanged. |
| DWORD composition and odd mode1 WORD addresses | SoftPC8c27a3b680afa838fb81a6520dd44620a52b59db; offset+2 and low/high16 composition; next odd/even physical group8/10. |
| Local NTVDM Console paths | jazz_text_update and copyConsoleToRegen retain their original owners and use the same8-byte CHAIN2 cell layout. SoftPC does not select these paths. |
| Shared libraries |118 source/test/fixture files remain byte-identical to the current SoftPC tree; no new shared patch. |

Mirror exceptions DIV-545 and DIV-889 record the exact boundaries. Local GDP
pointer binding is retained; upstream slot ownership and blanket calloc are
not imported. Source line counts remain unchanged in the generated rules.
No second VRAM layout, guest-mode special case or new rendering engine.

## Evidence and limitations

Task-owned runs are under build/t479-s9-display. The actual-provider fixture
is tests/mvdm-host/cvidc_vector_binding_fixture.c, linked to production C-VID
providers through its existing fixture seams. Its existing /FORCE duplicate
seam linker warnings remain; it supplements, not replaces, product testing.

- Full014 and paired edit016 fail with candidate-only EDIT menu loss; S8
  passes the same fixture. jazz_text_update fixes EDIT, then edit020 reveals
  alternating-character loss after Console writeback. nt_fulsc correction
  makes edit021 pass the unchanged EDIT/MEM/EXIT assertions.
- rules026 fails mode1 offset101: actual00300010, expected40302010.
  rules029 passes after the two upstream constants are corrected. Subsequent
  review changes the upstream test's final offsetfffd tofff9 so all six seeded
  bytes and paired planes stay within the allocated256KiB. The final bounded
  oracle run, not rules029 alone, is required for delivery.
- rules017 used the wrong ascending RAM-source convention. rules018 corrects
  the fixture to the actual generated descending-source contract. Product
  logic was not changed to satisfy the invalid oracle.
- diagnostic019 never reached rendering: package identity rejection1306.
  Its force-full source experiment was restored before product qualification.
- r023 Full passes16/16. Real Win3.1 observations show both transition
  directions, input visible before Enter, echo output, DIR/VER/CLS and DOS
  EXIT. Its root observer expired before Windows exit, so it cannot prove
  the final exit code. r031 plus the final live run supersedes this candidate.

## Final qualification

Cached030 x86/x64 full builds pass. Full031 passes16/16 in323475ms;
[exact gates](m0-t479-s9-full-timings.json). Bounded rules033 passes.
Final package is44d63565-f8b2-47a4-b79f-42d4b8465197.

The final live run uses helper-owned stageO:/tmp/n766 and existing guest
O:/tmp/w479m. Readback of an open observer log initially failed sharing;
resume uses its exact parent/child process identity. Window discovery then
revealed the known COM1 prompt. The existing WindowInspector, restricted to
owned NTVDM25140, ignored only approved COM warnings. No guest restart or
product modification was used to bypass these runner failures.

resume038 completes both transitions, visible pre-Enter echo, echo output,
DIR/VER/CLS, DOS EXIT and normal Windows exit. Root Run1611692 reports
wait=0 exit=0 while observer16080 is live. Exact stage processes retire.
All six result frames were visually reviewed; fullscreen Console text retains
the same echo/input/output. Console OEM box bytes must be decoded as OEM,
not UTF-8 replacement glyphs. Original window screenshots retain raw pixels.
SYSTEM.INI, WIN.INI and both authored addon inputs match stage024 hashes.

[Retained commands, probe sources, frames and receipts](../../../artifacts/t479-s9-chain2/verification.zip)
and [input identities](../../../artifacts/t479-s9-chain2/manifest.json) preserve
reproduction details. Existing tracked window_surface_snapshot.c and the
actual-provider C fixture are the observation/algorithm implementations.
No Computer Use or global desktop input was used.

Publication040 completes: all eleven images, hook license and manifest are
identical at assets/release and O:/winnt/system32; [hash receipts](m0-t479-s9-publication.json).
The first publication attempt was correctly blocked by documentation governance
before any copy. The evidence index was added and an accidental UTF-8 decode
change repaired; audit035c passes, as does git diff --check.

Relative to S8 commitab413287f, production changes15 mirror C/header files:
352 added/348 removed lines, net+4. This is the incremental change, not the
aggregate delta against pristine OpenNT. Non-mirror production delta is zero.
The actual-provider fixture adds256/removes3 lines. Source mappings and
DIV-545/DIV-889 account for the retained mirror adaptations. Shared118-file
comparison remains exact. No original guest or addon bytes were changed.

Delivery is submitted as T479 S9 P1; owner acceptance remains pending and
T479 remains open. No S6 or Win95 deferred obligation is marked complete.

Tests do not prove every VGA register combination or all legacy generated
fill variants. The mode1 oracle uses default comparison masks, matching the
bounded upstream test; nondefault-mask behavior is not claimed here.
Windows95 and S6 asynchronous guest-task completion remain outside S9.
