# Part 8: original REGEDT16 source and debug-artifact roles

This report supplements the six owned `wow16/regedit` rows and exact source/ZIP identities in `guest-family-inventory-review.md`. It follows the original application into the project Registry owner without asserting that the current debug artifacts are original or shipped.

Original `wow16/regedit/makefile:12-13,32-46,52-69,105-122` names REGEDT16 and REGLOAD separately, sets `DSTDIR=debug` for non-retail debug builds, compiles the listed source objects, links `regedit.def`, and attaches the selected locale resource with `rc16`. `dbase.c:55-63` calls RegOpenKey/RegCreateKey/RegCloseKey; its action edit path calls create/set/delete/close at lines 98–140. `cutils1.c:93,332` calls query and enumeration. Those are application source calls, not an observed live run of the debug executable. The `lib/shell` recipe dependency leads to the original guest SHELL ordinal/thunk path documented in `shell-semantic-reconciliation.md`; existing project NTVDM Registry owner then governs host state and WOW32 handles.

| Owned path | Evidence and bounded role |
| --- | --- |
| `wow16/regedit/debug/REGEDIT.RES` | Current-only debug build artifact by ownership inventory; makefile has a matching debug output recipe, but provenance, compiler settings and binary identity are not proved. Do not call it original or a release input. |
| `wow16/regedit/debug/REGEDT16.EXE` | Current-only debug artifact; matching recipe exists but exact provenance and actual packaged/loaded selection not established. Do not remove without owner/provenance check. |
| `wow16/regedit/regedit.ico` | Original resource asset referenced by USA RC, not a separate Registry provider. Inclusion in a particular shipped binary remains unproven. |
| `wow16/regedit/sdkreged.def` | Original alternate module definition; selected main REGEDT16 link uses `regedit.def`; alternate build and consumers remain pending. |
| `wow16/regedit/sdkreged.h` | Original declarations consumed by original regedit build (`makefile:196,200`); not a live provider. |
| `wow16/regedit/sdkreged.ico` | Original resource asset; exact active localized consumer unresolved. Preserve until independently established. |

Failure/lifecycle: original dbase.c code closes owned keys after operations and handles errors through its own paths. This source observation does not prove every early-exit cleanup or actual UI interaction. `docs/etc/evidence/m0-t461-s2-binding-implementation.md` and `m0-t461-s3-registry-persistence-candidate.md` prove only their recorded scoped fixtures/candidates; neither verifies this exact REGEDT16 binary. S14 should use a deliberately selected immutable guest caller and isolated NTVDM Registry state to verify create/query/delete, failed write, key close, restart and UI task teardown; S19 bind observed binary and release identities. Do not mutate real user Registry or rebuild guest for this audit.

Replay `rg -n "REGEDIT|regedt16|sdkreged|regload" src/mvdm/wow16/regedit/makefile` and `rg -n "Reg(Open|Create|Close|Delete|Set|Query|Enum)" src/mvdm/wow16/regedit`; rerun part-8 guest inventory observer. No test was executed and no shared ledger, guest or product was changed.
