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

## 17 include/conapi.h

204 to 204 lines. Four source-facing declarations also occur in console_client.h (CreateFileA/W, GetFileType and WriteConsoleA bindings), with one provider for each. Including conapi.h in that provider would redirect native calls back to the adapter. Including console_client.h in the mirror boundary would expose session, video protocol and graphics types. Splitting out just the prototypes would require another included header; no redundant provider or state was found.

## 18 include/console_grid.h

6 to 6 lines. NTCON frontend_session.c and console_frontend.c and worker-local console_client.c call the same resize function. Row preservation and cursor adjustment already have a single implementation in console_grid.c.

## 19 include/console_title.h

9 to 9 lines. The product Ninja generator applies the scoped title include to COMMAND. conapi.h shares the same two aliases. Folding it into conapi.h would widen COMMAND's native API redirections. Get and set use separate protocol operations, sharing the existing console client exchange.

## 20 include/environapi.h

13 to 8 lines. nt.h includes ntrtl.h with the local linkage decoration. Four prototypes repeat the original. The fifth, RtlQueryEnvironmentVariable_U, repeats the ABI but spells Name PUNICODE_STRING instead of the original declaration's PCUNICODE_STRING. Deleting the redundant declarations selects the original header. Keep the snapshot ingress used by mvdm_base_vdm_environment.c. P3 compared generated x86 code for the header-only alternative.

## 21 include/nt.h

331 to 323 lines. The pinned SDK winnt.h defines RtlMoveMemory and RtlZeroMemory; the original x86 ntrtl.h memory macros use the same memmove/memset. No intermediate production header undefines them. Remove both redundant fallbacks. W32THREAD, W32PROCESS and SECURITY_CLIENT_CONTEXT are embedded by value in userk.h. The private PEB/TEB carry fields populated in opennt_support_rtl.c; they cannot be aliased to the public host layouts.

## 22 include/nt_event_declarations.h

12 to 12 lines. One shared native query contract serves base_client, base_server, service_internal and nt.h. frontend_registry.c checks the returned event type at capability registration, retire/restore and worker pipe registration. No event policy or duplicate lifetime state in this header.

## 23 include/ntioapi.h

190 to 186 lines. Use the original source-facing names directly in the two bounded information-class enums. Three aliases and their private enumerator spellings can disappear. Keep enum types and values (3, 17, 4). The facade fixture uses the production spelling. The separate status-block shape is required because the legacy Information field is ULONG and modern native Information is pointer-sized; ntioapi_facade.c copies the fields explicitly.

## 24 include/ntrtlp.h

18 to 16 lines. Keep PPEB/PTEB bindings used by original environ.c/error.c. nt.h includes both ntrtl.h and nturtl.h inside the intended NTSYSAPI scope. Their second inclusion adds no surface.

## 25 include/opennt_user_multipass.h

4 to 4 lines. wow_user_client_view_layout.h selects the original layout, temporarily binds PTHREADINFO and re-enters user.h. The macro stack restores the caller's guard after inclusion.

## 26 include/opennt_usercli_multipass.h

4 to 4 lines. wow_edit_state_bindings.c re-enters only usercli.h. wow_control_extent_bindings.c re-enters both original headers. Separate guard scopes preserve both consumers without broadening the selected layout.
P8 delivery: 26/40 complete; C/header lines 2298 to 2238 (-60), README separately -9. Both cached release builds and 11-image staging passed. Package 4afc66ae-310c-4ca0-9cda-a10fdb2a980d was hash-verified at both release destinations. x86 environment projection and guest environment fixtures and both x86/x64 NT I/O facade fixtures returned zero. The latter exercises typed function bindings, layout and an invalid-handle query; it does not exercise successful directory enumeration. Refreshed the independent fixture generator include paths.

## 27 include/thread_start_compat.h

100 to 86. Drop two entry points that only supplied a NULL name to the same typed provider. The linked worker bridge loses 16 additional lines. Session tests retain all four denied-entry cases and successful callback kinds. The heartbeat test's unexpected-thread sentinel now names the same symbol as the production macro. No .def export or product caller of the old names was found. nt_rflop.c uses the separate parameterized void callback; keep that signature and preserve x86 stack cleanup.

## 28 include/vdmapi.h

6 to 6. NTSRV main explicitly includes this forwarding header. Original worker callers use vdmapi.h by name. All declarations come from the original private base header, with no copied implementation.

## 29 include/winbasep.h

8 to 8. Both original demsrch.c and base.h include this private historical spelling. The empty carrier is the sole repository file by that name; no store, declaration or operation to consolidate.

## 30 include/winconp.h

15 to 15. Correct the provider filename in the comment. Scope one rename around the WINBASEAPI keyboard layout import, then declare the actual worker provider and bind the COMMAND code page query. console_client.c owns both frontend exchanges and native detached fallbacks.

## 31 include/winuserp.h

125 to 125. The outer OR already restricts all nested declarations to task-order or object-flags mode. Neither selector changes inside this block. Reduce the PQ guard to its remaining cleanup condition.

## 32 include/wow32_provider_private.h

73 to 65. The directory-list mask has the same nine bits and the dialog flags the same four values in both compat headers. The shared flags header keeps the provider's original include site inside the GWL_WOWWORDS gate, and the resource consumer's independent fallback guards. MBToWCSEx has the same signature and a code page of zero at all four selected binding sites. Shared wow_multibyte.h keeps the original chartran.c owner. Two new headers cost 12 and 7 lines respectively and count in all totals; the class/bitmap binding headers outside compat lose another 3 lines combined.

## 33 include/wow_button_style_table.h

31 to 31. wow_button_resources.c indexes both columns by the original contiguous radio-mask to three-state range. Keep the 16-row slice and its original ordinal layout.

## 34 include/wow_char_conversion_abi.h

22 to 22. chartran.c uses its original LARGE_STRING and the bound code-page and paired allocation services. ntuser_rtl_compat.c backs allocation/free with LocalAlloc/LocalFree. The drawing precomp also declares that pair, but importing this entire header there would select an unrelated original USER layout.

## 35 include/wow_control_message_abi.h

25 to 25. wcntl32.c translates combo focus by subtracting the NT4 identifier, editsl.c emits it, and lb1.c dispatches listbox tracking. Preserve pinned cross-boundary constants independent of the contemporary SDK upper bound.

## 36 include/wow_control_resource_abi.h

85 to 66. Shares the flags and multibyte declaration reviewed with 32. Keep the two-byte packed dialog resources: clres.c advances by the concrete template/item sizes, and a modern in-memory object layout is a different contract.

## 37 include/wow_full_user_handle.h

38 to 38. GetFullUserHandle restores window uniqueness and passes the low handle for the historical DDE fallback. Single caller wow_user_full_handle performs one fallible metadata lookup and temporarily binds one copied row for the original signature. No shadow registry. wow_page_domain_fixture.c covers handle reuse and missing ownership.

## 38 include/wow_user_declarations.h

26 to 26. Actual users include wkman.c (callback ABI), wcall16.c (cdecl flag), wkgthunk.c (VDM pointer) and original draw.c (scroll flags). One declaration boundary, no alternate route.

## 39 include/xactsrv_wksta_minimal.h

52 to 45. Remove redundant windows.h after nt.h and the dead fallback for API_HANDLER_PARAMETERS_REFERENCE. Included original xstypes.h defines the macro unconditionally, including all four parameter references. Keep the reached handler types and remaining STATIC/IF_DEBUG bindings.

## 40 include/zwapi.h

20 to 20. Original environ.c allocates, queries and frees mutable environment regions. The wrappers are declarations only; native SIZE_T and ULONG_PTR preserve host width. No substitute VM allocator or second environment owner.

### P9 delivery measurements

The frozen 40-file list is complete. The final scope on disk is 14 C files (810 lines), 27 headers (1399 lines) and the README. Two new, reviewed shared declarations are included in these totals. The 39 baseline C/header files had 2298 lines; all current C/header files have 2209 (-89). README is separately -9. The last batch additionally removed 16 lines from worker-thread/bridge.c and 3 from the class/bitmap callers. These are physical source measurements, not image or performance claims.

Both cached product builds succeeded. Both release manifests and all 11 binary SHA256 values matched candidate 20cfd8c5-b279-43e4-b856-0317296d8bcb. The frozen inputs, logs and package are in build/t483-s9-thread. Thread-session test verified four rollback paths, five normal terminal releases and exactly-once cleanup. The original x86 DelayHeartBeat suspend/resume test passed after selecting the actual production video flags and binding its two unexpected alert-termination calls to explicit failure sentinels. The test does not exercise STATUS_ALERTED. Original redirector server enum passed 37 assertions. The private desktop Win16 tab-stop guest witness completed with observer exit 0 and bound the exact published WOW32 DLL (CA31B5A333A62514DD564FEB5D2E70EA8864638FE3738B37BB908330227FD275). Targeted verification per owner scope.
The supplemental x86 WOW control cohort built and ran 38 executables, each exit 0; build/t483-s9-thread/wow-run.log lists per-case outcomes. This is separate from the real Win16 guest witness.
