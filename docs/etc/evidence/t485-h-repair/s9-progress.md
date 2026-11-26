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
