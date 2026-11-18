# T483 S9 sequential audit

Baseline fcaae36df. Follow order.csv. Initial C/header footprint is 2,298 physical lines. Documentation is measured separately.

## 01 console_grid.c

90 lines, 4,279 bytes at entry and exit. Keep both operations. The NTVDM client uses the adapter on its native-handle path. NTCON uses it for frontend creation, mode changes and geometry requests. OpenNT windows/core/ntcon/server/output.c:3225 owns ResizeScreenBuffer over private row structures. The T412 S11 recorded control demonstrates native reflow when the window itself changes. Dropping either snapshot path would lose the cell-grid guarantee. The before and after buffers describe different geometries; the transfer helper checks clipping on both reads and writes. Cleanup preserves the error across buffer release. No demonstrated smaller equivalent implementation. Source and historical runtime evidence reviewed; no new runtime run or product change. Completed 1/40, 39 remaining; cumulative reduction 0 lines.

[Original comparison and runtime controls](../../docs/etc/evidence/m0-t412-s11-console-handoff.md).

## 02 opennt_support_rtl.c

279 to 243 physical lines (+17/-53), cumulative -36 C/header lines, 2,262 remaining. The x86 NTVDM link map placed RtlOemToUnicodeN in the local RTL object before and in ntdll after. The NT4 nlsxlat.c:575 function writes a partial result and reports the bytes written. The former binding instead calculated required bytes and returned before copying if short. A real x86 native import probe on this SBCS system tested capacities 0 through 4 for input AB, including guard cells, overflow and optional partial UTF-16 output. Non-ASCII DBCS behavior was not runtime tested.

State ownership: refresh and first access used to allocate two snapshots. Refresh now owns replacement and release, and first TLS initialization reuses a prepared snapshot. PEB process-parameter linkage is the stable TLS initialization marker. An isolated x86 probe measured two allocations before, one after, and preserved the prior snapshot on an injected allocation failure. The per-thread critical section exposed via FastPebLock was different from the lock taken by the RTL. The WOW hard-error path reads its owner; the WOW32 map imports the worker's NtCurrentPeb. A single once-initialized lock now backs both. A real acquire/inspect probe observed false before and true afterwards; the regression assertion also runs in environment_projection_fixture.c.

The two variable-length conversion helpers have different sizing units and output types. A broader switch to native strings must account for allocator/free pairing, the OEM private scratch descriptor and null/empty initialization. Crypto export lookup, historical current-directory/status bindings and surrogate TEB/PEB remain. No claim of global equivalence for these other providers.

Cached x86/x64 and all 11 published hashes passed for package 4ba45d7a-35a4-48ab-b8ae-e8459fedd138. Environment projection with lock ownership, x86 RTL and four redirector remote-name cases passed. Shared Win16 CF_TEXT transfer stopped at phase 5 on both candidate and previous release, with error 5 in the native trace; the candidate retry passed all 18 checkpoints. No change to the clipboard test or product clipboard code. Runtime evidence under build/t483-s9-rtl.
