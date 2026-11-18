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

## 03 README.md

233 to 224 documentation lines. Distinguish T423's historical run16 frontend account from current NTCON ownership. Retire ADAPTER-WIN32-027 following P5's actual NTVDM import map; consolidate overlapping introductions and document snapshot/lock ownership. Original register IDs and their order retained. No runtime change.

## 04 wow_class_client.c

31 lines unchanged. wow_class_client_bindings.c enters wow_original_register_class, linked beside RegisterClassExWOWA in wow-class-client.obj. The original WNDCLASSA to WNDCLASSEXA conversion supplies cbSize and hIconSm; original ntcftxt.h owns registration before publishing through the host binding. Inlining into the caller would move original code without removing behavior.

## 05 wow_client_dispatch.c

46 lines unchanged. Two wow_window_words_binding.c paths call the same DispatchClientMessage in wow-client-dispatch.obj. The tagged procedure needs both the procedure token and the window WOW words; the native procedure takes four ordinary window arguments. Both return the procedure result. No duplicate dispatch state found here.

## 06 wow_control_brush.c

32 lines unchanged. Host wow_control_brush validates and retains the control. The OpenNT body selects popup owner, parent, or the control itself. Original ThreadLock maps to retaining the exact recipient; the host scope releases references in finally across a reentrant color call. Removing either lifetime responsibility is not equivalent when recipient and control differ.

## 07 wow_control_fnid.c

21 lines unchanged. NtUserSetWindowFNID maps to wow_edit_set_window_fnid, linked from control-fnid.obj. The operation borrows HWND state, accepts an unset type or a cleanup bit, rejects conflicting assignments and allows repeated cleanup. Dialog initialization/finalization and class registration have different transition contracts; a general setter would obscure those rules. Preserve the small extracted body. Reviewed the existing fixture assertions and final product map; no new runtime test for unchanged C code.

Through position 7: 7/40 complete, 33 remaining. Production C/header footprint 2,262 lines, down 36. Documentation separately down 9 lines.
