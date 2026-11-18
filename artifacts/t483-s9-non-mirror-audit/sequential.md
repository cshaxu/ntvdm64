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

## 08 wow_control_rect.c

25 to 25 lines. Source matches the four coordinate operations and return in original rtl/rect.c:46. lboxrare.c, editsl.c and combo.c call the same implementation. The WOW32 link map resolves CopyInflateRect to wow-control-rect.obj. SysWOW64 USER32 exports CopyRect and InflateRect individually; composing those would still need the existing ABI wrapper and two calls. Keep the original.

## 09 wow_control_state.c

23 to 23 lines. Button, combo, edit and dialog callers share the setter. SetWindowState skips a write when all requested bits are present; ClearWindowState skips it when none are present. Both use the same NtUserCallHwndParam path with different operations. A combined boolean-mode helper would still need both predicates and public entry points.

## 10 wow_default_window_bits.c

66 to 66 lines. wuman.c obtains the registered output callback. wow_private_user_get_def_window_proc_bits checks readiness, wide maximum and byte capacity, zeros the admitted span, and calls the original merge. Copying the ordinary bitmap then OR-ing the special bitmap respects the separate maxima. The apparent duplicate maximum is also needed for pre-write validation before narrowing to WORD. No second persisted bitmap or duplicate builder here.

## 11 wow_dialog_lookup.c

22 to 22 lines. Dialog, combo and listbox modules consume an exact PWND. The original searches direct children in sibling order, returning the first matching spmenu/id. The DLL links both this provider and public USER32 GetDlgItem. Substituting the latter would add handle-to-view lookup and lifetime requirements.

## 12 wow_dialog_memory.c

60 to 60 lines. Dialog initialization forwards its argument to the registered WOW callback. DS_LOCALEDIT allocation requests 256 shared movable zeroed bytes; failure unwind and destruction use the paired guest free callback on LOWORD of the handle. The bindings already use one registered table. Moving these calls into every creator/destroyer would duplicate the guest ABI policy.

## 13 wow_dialog_pointer.c

20 to 20 lines. The original changes the PDLG link, FNID and dialog flag together. wow_dialog_finalization_binding.c:131-149 handles three distinct states: completed, failed allocation before PDLG existed, and normal final destruction with an allocated PDLG. The explicit cleanup bit in the failed-allocation path owns no resources; it does not duplicate the ordinary destructor.

## 14 wow_input_membership.c

102 to 102 lines. AddAttachment handles queue-label replacement and reference counts. Recalc2 computes the transitive closure of explicit links and same-process Win16 peers. wow_input_group_commit seeds each component, maps labels to group IDs, clears all temporary queue pointers, then commits native links. Repeated passes are observable for transitive membership. The persistent link records are needed to retire actual native attachments. No replacement graph or new optimization without a measured need.

## 15 wow_object_ownership.c

29 to 29 lines. Single indexed gabObjectCreateFlags definition in WOW32, consumed by original cleanup.c:445. Entries retain type ordinals even for zero masks. A switch or sparse lookup would re-encode the table.

## 16 include/command_process_compat.h

35 to 30 lines. Keep declarations and the child-local standard-handle boundary. Replace the two argument-repeating macros with direct symbol aliases. cmdexec.c has only calls, the provider undefines both aliases before using native APIs, and the standalone test calls the explicitly named adapter. MSVC A/B compiles with the actual product worker flags selected the candidate header for cmdexec.obj and command_process_compat.obj; objdump -dr showed identical instructions and relocations. No original mirror edit. Local controls are in build/t483-s9-command.
P7 delivery: cached x86/x64 builds passed. Both release destinations matched the 11-image package 7f373e53-fb05-4984-9e6d-5fa80d009be9. Independent command capture/launch and nested-window broker I/O passed, including parent return and direct exit 23. Completed 16/40, remaining 24. C/header lines 2298 to 2257 (-41), README separately -9.
