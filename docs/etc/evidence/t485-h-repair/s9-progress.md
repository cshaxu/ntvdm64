# S9 Ordered Core Repair Progress

## Scope and counting

S9 owns the 41 files assigned in files.csv, processed by canonical relative-path
order. The frozen 437-point inventory is classification history, not a byte-
sensitive current diff count. Actual counts include line endings and EOF,
mask only admitted #error replacement bodies, and retain #line differences.
S8 closure is 2c8325846; product baseline is S8 runtime14 / fc9492342.

## P1: dbgdll/brkpt.c

Question: do the two explicit WINAPI additions have individual ABI evidence,
or does the historical whole-TU /Gz failure wrongly justify both?

Fixed XP object: a2f6d7c93aa4f11efc51147566d5961c59a8ef89,
Source/XPSP1/NT/base/mvdm/dbgdll/brkpt.c.
Original SHA256: df0830ac23f1c6332126a1840275e81831f5e2b6d92ffed778627c86833a4b26.
The complete file differs only in CRLF vs LF and WINAPI on xxxDbgBPInt and
xxxDbgTraceInt. Restored original bytes: 1 point/540 added-plus-deleted lines
-> 0/0. The separate frozen IDs HXP-R-00001 and HXP-R-00002 are locally restored.
No exclusions or #line remain to reconcile; original line numbering is exact.

Call-chain evidence: dbg/dbg.c obtains both undecorated PE exports via
GetProcAddress and calls BOOL (WINAPI *)(VOID). Both entries have no stack
arguments. ntvdmd.def exports undecorated names; target.cmake retains the
existing default compiler convention. The historical h001-convention trial
used /Gz for the whole DLL and failed; that does not prove these two zero-
argument additions individually necessary. No whole-TU convention is changed.
Final-link symbol resolution and actual calls are required before delivery.

The existing ntvdm_export_live_vdd.c witness is extended to call both breakpoint
and trace exports and verify exception delivery plus register writeback for
each, restoring its EAX afterward. It still uses the real DLL in the real MEM
worker; no parallel test framework or synthetic product wrapper is added.
The diagnostic restoration script asserts normalized source differs only by
the two qualifiers before writing original bytes.

Local state: 1/41 audited; 40 remain; 1 locally restored, 0 delivered.
Known measured-file reduction: 1 point/540 lines; whole-S9 totals are not yet
measured. Cached full build and dual publication are running in
build/t485-s9/p1; tests, commit and push remain pending.
### P1 final package verification

Runtime1 cached full build and dual publication passed, all 26 image hashes
match. The final link map resolves _xxxDbgBPInt and _xxxDbgTraceInt at the same
code addresses as the prior stdcall zero-argument entries. Existing decorated
imports elsewhere and DLL export names are unchanged.
The updated live fixture was separately incrementally compiled, then loaded
by the real MEM worker. Both calls produced matching SAS-base exception
notifications, and each register writeback was verified. MEM exited normally;
query/allocation/failure/task-termination witnesses also passed. Actual
NESticle rendered its menu and Escape hide/restore passed on the same package.
No original guest binary was modified. This is targeted acceptance, not Full.
Both frozen points are final-verified restored. 1/41 reviewed and verified;
40 unreviewed; byte-sensitive measured scope 1/540 -> 0/0. Commit/push completes
P1 delivery. Next: src/mvdm-host/dbgdll/dbgdll.c.
P1 delivery receipt: 1e15cd498 pushed to origin/main. 1/41 delivered; 40 remain.

## P2: dbgdll/dbgdll.c and coupled original declaration

Entry: raw byte-sensitive 1 point/1356 lines, four frozen points. The file
has CRLF while pinned XP has LF. Three differences are parameterized stdcall
boundaries: DbgDllInitialize (3 arguments; /entry:DbgDllInitialize@12),
xxxDbgNotifyRemoteThreadAddress (2 arguments) and xxxDbgNotifyDebugged
(1 argument), called through WINAPI pointers in dbg.c. These are unlike the
zero-argument P1 functions; restoring default cdecl would violate stack cleanup.
The three qualifiers remain on existing lines so original diagnostic positions
are preserved without new #line statements. The resulting file has 3 points/6
lines relative to XP; production algorithms are otherwise original.

HXP-R-00005's old restore-#error recommendation is wrong: target.cmake selects
/Di386 for this DLL, so #error would stop the supported build. The original
four-line initialization branch is restored instead. Its getIntelRegistersPointer
provider is the existing original stubs.c implementation: PROD assert0 is empty
and it returns zero. px86 has declarations/storage but no reads in current src.
No new pointer emulation, algorithm, callback or condition is introduced.

The original API declaration in softpc.h was confined to the V86-monitor branch.
The same declaration is exposed after v86def.h in the existing common i386
conditional (+1 declaration line); existing #line 33 preserves subsequent
original mapping. This is a coupled header adjustment within the owner's small
mirror-increment allowance. Before-image retained in build/t485-s9/p2.

The initial restoration helper rejected the header's CRLF anchor after restoring
dbgdll.c; the insertion was completed preserving header CRLF before the worker
compile. It was not a product compilation failure. Cached full build/publication
is running. Final link, actual DLL initialization and notification tests are
required before declaring the original initialization safe and delivered.
P2 is local, not delivered; S9 delivered progress remains 1/41.
### P2 final verification

Runtime2 cached full build and dual publication passed (26 matching hashes).
The real MEM worker loaded NTVDMD.DLL with restored initialization; both
breakpoint/trace callbacks, register writeback, allocation/failure and termination
checks passed. NESticle rendered its menu and Escape hide/restore passed.
[Final byte-sensitive metrics](s9-p2-mirror-diff.json): dbgdll.c 1/1356 -> 3/6;
softpc.h 1/1629 -> 1/1630. Batch 2/2985 -> 4/1636, +2 points/-1349 lines.
The header has no #error; CRLF differences remain included. Only its coupled
declaration is validated here; its full S12 audit remains outstanding.
HXP-R-00005 is restored; HXP-R-00004/6/7 retain evidenced parameterized stdcall
boundaries. No wrapper, alias, new state or alternate algorithm is added.
S9 is 2/41 reviewed and final-verified; commit/push pending. Across both S9
primary files and the one coupled header, measured baseline 3/3525 -> 4/1636,
+1 point/-1889 lines. This is not the unmeasured whole-S9 census.
Next primary file: dbgdll/excptn.c. Full remains the S13 integrated checkpoint.