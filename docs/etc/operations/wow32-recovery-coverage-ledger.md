# WOW32 recovery coverage ledger

## Role and baseline

T422 coverage ledger for the admitted
[WOW32 proposal](../../proposals/proposal-wow32-complete-runtime-recovery-001.md),
updated from the owner-requested 2026-09-22 planning seed. S numbers below
refer only to that successor proposal, not historical T420 S numbers. The
[S1 execution checklist](wow32-recovery-s1-checklist.md) is the live gate
record; this ledger owns stable coverage IDs and downstream handoff rows.

Seed inspection: HEAD a87d6dd9437db6da77bc6db7c356780d8284cca6 plus the
owner-approved proposal additions. This is a documentation inventory, not
a fresh binary verification. Source/build facts and historical runtime
observations have different evidence levels.

## Receiver allocation after S2 handoff

The owner-approved 2026-09-24 proposal sequence supersedes old S2--S8
assignments. Stable IDs and evidence remain unchanged. The capability register
below uses new receivers. Detailed source/table/slot seed rows further below
retain their original S1 allocation as historical provenance, NOT admission or
current ownership: resolve them through this map and the current proposal.

| Original assignment / family | Current receiver |
| --- | --- |
| Old S2 message/callback/task execution, C01--C03/C09 | S3 |
| Old S2 view/handle/data/class/window, C04--C07 | S4 |
| Old S2 dialog C08; old S7 input/hooks/timers | S8 |
| Old S3 GDI handles/DC/drawing/bitmap/DIB/palette | S9 |
| Old S3 fonts/text/metafiles | S10 |
| Old S4 resource lookup/load/lock/free; conversion/menus/accelerators | S7 foundation; S11 consumers |
| Old S5 clipboard; DDE | S12; S13 respectively |
| Old S6 modules/memory/aliases; files/environment/OEM | S5; S6 respectively |
| Old S7 Shell/Registry; Winsock; COMM; print; multimedia | S14; S15; S16; S17; S18 respectively |
| Old S7 ToolHelp/WOW debugger; common-dialog/OLE; hard errors | S19; S20; S21 respectively |
| Old S7 CORE/registration | Every implementing owner wires/cleans its slice; S22 reconciles the whole |
| Old S8 final acceptance | S22 |
| C10 teardown | S3 execution, S4 objects, S8 dialogs, and each later resource owner |
| C11/C12 | Every implementation S; S22 final aggregate |

No receiver can defer its own normal/failure teardown or available production
wiring. Split stable parent rows retain named slices, not a blanket pass.
The S3 evidence checklist selects concrete message/callback/task edges first;
other rows cannot silently become S3 scope merely by sharing a source file.

## Evidence index

| Key | Existing evidence and reusable scope |
| --- | --- |
| E40 | [USER profile](../evidence/m0-t420-s40-wow-user-profile.md): 26 direct-data mappings, 47 pinned instruction checks, private-word probes, original resource/task/class algorithms and native experiments. Read later corrections before reusing earlier conclusions. |
| E41 | [Worker domain](../evidence/m0-t420-s41-wow-registration-frontier.md): mapping/descriptor/thread foundation. |
| E42 | [W1/W2 binding record](../evidence/m0-t420-s42-w1-w2-lifecycle-binding-ledger.md): real registration reachability, desktop fault, rejected incomplete handle publication and missing production exit wiring. |
| EH | [S42 handoff](../evidence/m0-t420-s42-wow32-successor-handoff.md): fresh 77-body build, focused fixtures and 17 DOS routes on recorded hashes; explicitly no Win16 acceptance. |
| EW | [Prior detailed obligations](m0-t420-s40-wow32-closure-checklist.md): scope inventory; obsolete execution/registration instructions are superseded by the successor proposal. |

## Status and update contract

Every capability row has separate Source, Build, Wiring, Focused, Guest,
Teardown and Delivery states. Allowed values: pending, in-progress, pass,
fail, dependency-blocked, or not-applicable with source justification.
Historical evidence is labelled inherited, never silently promoted to pass.
Unavailable host facilities and approved original-guest limitations are
explicit non-pass dispositions. Record precise scope for every pass.

The compact register below records inherited facts and remaining work. Before
closing a row, append its detailed record using the template below; a source
file, slot assignment or old fixture alone cannot close it. Stable IDs survive
regrouping. Reopening records counterevidence and invalidated result. No
successor capability is marked complete by this audit ledger.

## Capability and dependency register

| Stable ID | Owner | Original entry/owner and inherited evidence | Open contract and required test |
| --- | --- | --- | --- |
| CORE-INIT-01 | per-owner / S22 | wow32.c W32Init/W32Dispatch, wowtbl.c; EH build and E42 reached registrar | Initialization/rollback, dispatch coverage and DLL/worker teardown; dependencies on S2-S6 are explicit. |
| USER-VIEW-01 | S4 | USER GetDesktopWindow; original desktop/client layouts; E42 confirmed fault | Real desktop producer before USER bootstrap, pointer relocation, callback visibility and withdrawal; reproduce then eliminate the recorded fault. |
| USER-HANDLE-01 | S4 | Original handle validator, WND/CLS; E40/E42 | Full object/phead before typed publication, reuse/uniqueness/cache invalidation; stale/invalid identity and callback destruction tests. |
| USER-DATA-01 | S4 | SERVERINFO/CLIENTINFO/shared clock; E40/E41 | Field authority and update timing, host-originated changes without thunk entry, per-thread view and failure withdrawal. |
| USER-CLASS-01 | S4 | wuclass.c and recovered class/client owners; E40 native fixtures | Real guest register/query/change/unregister, private/public/system class scope, failed registration and module cleanup. |
| USER-WINDOW-01 | S4 | wuwind.c, wmdisp32.c and native creation bindings; E40 | Real guest create/query/mutate/destroy, first-callback state, subclass/reentrant destruction and external-object coverage. |
| USER-MESSAGE-01 | S3 | wumsg.c/wmsg16.c/message families; E40/E42 native tests | Actual guest synchronous/post/nested callback, reply/wait/cancel, destroyed receiver and task loss. |
| USER-DIALOG-01 | S8 | wudlg.c/W32InitDlg; E40 native initialization evidence | Real guest controls, initialization/procedure replacement, creation veto, cancellation and release. |
| USER-TASK-01 | S3 | wkman.c; recovered queue/taskman/exitwin slices; E42 | Registration/init/yield/wait through real task context and CCPU callback; multiple tasks and failure recovery. |
| USER-TASK-EXIT-01 | S3/S4/S8 by resource | W32DestroyTask, WU32FreeModule; E42 helper failure/retry pass | Connect thread/worker cleanup/dispose; module release must not destroy a live thread; prove teardown ordering. |
| USER-CALLBACK-01 | S3 | wcall16.c CallBack16 and wmdisp32.c; E40/E42 | Real frame lease, recursive CCPU entry/return, exception/reentry, task frame restoration and cancelled callback. |
| GDI-01 | S9/S10 by family | wgdi/wgfont/wgtext/wgmeta/wgpal/wdib families; E40 partial experiments | Drawing/DC/text/font/palette/metafile/DIB content, alias ownership, invalid input and native/guest release. |
| RESOURCE-01 | S7 foundation / S11 consumers | wres16/wres32/wcuricon/waccel and recovered clres/cldib; E40 | Real names/IDs/old formats, resource lease, conversion, callback timing and failed load cleanup; bitmap depends on GDI-01. |
| MENU-01 | S11 | wumenu.c and recovered original menu parser; E40 owner-draw counterexample/recovery | Original-layout menu graph and mutations, owner-draw data, callbacks, invalid handles and release; consumes S2 publication. |
| DDE-01 | S12 clipboard / S13 DDE | wdde.c/wuclip.c and original FreeDDEData; E40 | Real clipboard/DDE transfer, formats, ownership/reply, rejected/abandoned exchange and peer/task death. |
| KERNEL-01 | S5/S6 by family | wkman/wkmem/wkernel/wkfileio/wdos and alias/resource dependencies | Loader/memory/file/environment services, allocation and rollback, aliases and module/task release; uses S2 task owner. |
| OEM-01 | S6 | wdos.c/wkman.c; proposal OEM-WOW-DIR/DELETE obligations | Real non-ASCII paths, both deletion branches, font fallback and cleanup; S8 repeats integrated consumer acceptance. |
| OTHER-01 | S8/S14--S20 by family | Shell/Winsock/ToolHelp/keyboard/sound/multimedia/COMM/hooks/printing/commdlg/OLE families | S1 splits complete families into stable child rows with callers, dependency dispositions and real/negative/cleanup tests; no blanket pass. |
| ERROR-01 | S21 | Original hard-error callers and response/termination owners | Response/cancel/fatal exit; reconcile queued error-dialog owner, avoid duplicate implementation. |
| ACCEPT-01 | S22 | Immutable WRITE, WINMINE and SOL; EH DOS baseline only | Pin media then prove all proposal interaction scenarios, repeated/multiple tasks, abnormal exit and subsequent DOS usability. |

## Source coverage seed

Exact manifest: [sources](../../../src/mvdm/wow32/sources), SHA-256
D24767FF50C4C7017E6FCF5F550E120DB0F580F7C07C77CBE7E99E4432F52B36.
The active SOURCES block contains 77 C files plus wow32.rc; disabled !IF 0
tables are not active inputs. EH reports those 77 compiled, not all accepted.
The primary reviewers below are S1's complete source-family ownership
assignment; shared files require function-level ownership in the receiver's
closure ledger. A row cannot exclude a function because its primary reviewer
differs from its caller's S.  “Inherited build; per-entry audit pending” means
the receiver must audit runtime behavior before implementation/closure; it is
not an unassigned S1 edge or permission to defer it to S8.

| ID | File under src/mvdm/wow32 | Primary reviewer | State |
| --- | --- | --- | --- |
| SRC-001 | mapembed.c | S7 / CORE | Inherited build; per-entry audit pending |
| SRC-002 | wow32.c | S7 / CORE | Inherited build; per-entry audit pending |
| SRC-003 | wowtbl.c | S7 / CORE | Inherited build; per-entry audit pending |
| SRC-004 | wcntl32.c | S7 / CORE | Inherited build; per-entry audit pending |
| SRC-005 | wow32fax.c | S7 / CORE | Inherited build; per-entry audit pending |
| SRC-006 | wcall16.c | S2 / USER | Inherited build; per-entry audit pending |
| SRC-007 | wcall32.c | S2 / USER | Inherited build; per-entry audit pending |
| SRC-008 | wmsg16.c | S2 / USER | Inherited build; per-entry audit pending |
| SRC-009 | wmdisp32.c | S2 / USER | Inherited build; per-entry audit pending |
| SRC-010 | wmsgbm.c | S2 / USER | Inherited build; per-entry audit pending |
| SRC-011 | wmsgcb.c | S2 / USER | Inherited build; per-entry audit pending |
| SRC-012 | wmsgem.c | S2 / USER | Inherited build; per-entry audit pending |
| SRC-013 | wmsglb.c | S2 / USER | Inherited build; per-entry audit pending |
| SRC-014 | wmsgsbm.c | S2 / USER | Inherited build; per-entry audit pending |
| SRC-015 | wmtbl32.c | S2 / USER | Inherited build; per-entry audit pending |
| SRC-016 | wuser31.c | S2 / USER | Inherited build; per-entry audit pending |
| SRC-017 | wuman.c | S2 / USER | Inherited build; per-entry audit pending |
| SRC-018 | wucaret.c | S2 / USER | Inherited build; per-entry audit pending |
| SRC-019 | wuclass.c | S2 / USER | Inherited build; per-entry audit pending |
| SRC-020 | wudlg.c | S2 / USER | Inherited build; per-entry audit pending |
| SRC-021 | wumsg.c | S2 / USER | Inherited build; per-entry audit pending |
| SRC-022 | wuser.c | S2 / USER | Inherited build; per-entry audit pending |
| SRC-023 | wutext.c | S2 / USER | Inherited build; per-entry audit pending |
| SRC-024 | wutmr.c | S2 / USER | Inherited build; per-entry audit pending |
| SRC-025 | wuwind.c | S2 / USER | Inherited build; per-entry audit pending |
| SRC-026 | wsubcls.c | S2 / USER | Inherited build; per-entry audit pending |
| SRC-027 | wusercli.c | S2 / USER | Inherited build; per-entry audit pending |
| SRC-028 | wgdi31.c | S3 / GDI | Inherited build; per-entry audit pending |
| SRC-029 | wgman.c | S3 / GDI | Inherited build; per-entry audit pending |
| SRC-030 | wgdi.c | S3 / GDI | Inherited build; per-entry audit pending |
| SRC-031 | wgfont.c | S3 / GDI | Inherited build; per-entry audit pending |
| SRC-032 | wgmeta.c | S3 / GDI | Inherited build; per-entry audit pending |
| SRC-033 | wgpal.c | S3 / GDI | Inherited build; per-entry audit pending |
| SRC-034 | wgtext.c | S3 / GDI | Inherited build; per-entry audit pending |
| SRC-035 | wreldc.c | S3 / GDI | Inherited build; per-entry audit pending |
| SRC-036 | wdib.c | S3 / GDI | Inherited build; per-entry audit pending |
| SRC-037 | wres16.c | S4 / RESOURCE | Inherited build; per-entry audit pending |
| SRC-038 | wres32.c | S4 / RESOURCE | Inherited build; per-entry audit pending |
| SRC-039 | wucursor.c | S4 / RESOURCE | Inherited build; per-entry audit pending |
| SRC-040 | wumenu.c | S4 / RESOURCE | Inherited build; per-entry audit pending |
| SRC-041 | wcuricon.c | S4 / RESOURCE | Inherited build; per-entry audit pending |
| SRC-042 | waccel.c | S4 / RESOURCE | Inherited build; per-entry audit pending |
| SRC-043 | wdde.c | S5 / DDE | Inherited build; per-entry audit pending |
| SRC-044 | wuclip.c | S5 / DDE | Inherited build; per-entry audit pending |
| SRC-045 | walias.c | S6 / KERNEL | Inherited build; per-entry audit pending |
| SRC-046 | wstruc.c | S6 / KERNEL | Inherited build; per-entry audit pending |
| SRC-047 | wkmem.c | S6 / KERNEL | Inherited build; per-entry audit pending |
| SRC-048 | wkgthunk.c | S6 / KERNEL | Inherited build; per-entry audit pending |
| SRC-049 | wkman.c | S6 / KERNEL | Inherited build; per-entry audit pending |
| SRC-050 | wkfileio.c | S6 / KERNEL | Inherited build; per-entry audit pending |
| SRC-051 | wkernel.c | S6 / KERNEL | Inherited build; per-entry audit pending |
| SRC-052 | wdos.c | S6 / KERNEL | Inherited build; per-entry audit pending |
| SRC-053 | walloc16.c | S6 / KERNEL | Inherited build; per-entry audit pending |
| SRC-054 | wheap.c | S6 / KERNEL | Inherited build; per-entry audit pending |
| SRC-055 | wgprnset.c | S7 / OTHER | Inherited build; per-entry audit pending |
| SRC-056 | wkbman.c | S7 / OTHER | Inherited build; per-entry audit pending |
| SRC-057 | wsman.c | S7 / OTHER | Inherited build; per-entry audit pending |
| SRC-058 | wshell.c | S7 / OTHER | Inherited build; per-entry audit pending |
| SRC-059 | wucomm.c | S7 / OTHER | Inherited build; per-entry audit pending |
| SRC-060 | wuhook.c | S7 / OTHER | Inherited build; per-entry audit pending |
| SRC-061 | wulang.c | S7 / OTHER | Inherited build; per-entry audit pending |
| SRC-062 | wowhooks.c | S7 / OTHER | Inherited build; per-entry audit pending |
| SRC-063 | wmmedia.c | S7 / OTHER | Inherited build; per-entry audit pending |
| SRC-064 | wmmedia1.c | S7 / OTHER | Inherited build; per-entry audit pending |
| SRC-065 | wmmedia2.c | S7 / OTHER | Inherited build; per-entry audit pending |
| SRC-066 | wmmstruc.c | S7 / OTHER | Inherited build; per-entry audit pending |
| SRC-067 | wmmstru1.c | S7 / OTHER | Inherited build; per-entry audit pending |
| SRC-068 | wmmstru2.c | S7 / OTHER | Inherited build; per-entry audit pending |
| SRC-069 | wmmalias.c | S7 / OTHER | Inherited build; per-entry audit pending |
| SRC-070 | wsdata.c | S7 / OTHER | Inherited build; per-entry audit pending |
| SRC-071 | wsext.c | S7 / OTHER | Inherited build; per-entry audit pending |
| SRC-072 | wsraw.c | S7 / OTHER | Inherited build; per-entry audit pending |
| SRC-073 | wthman.c | S7 / OTHER | Inherited build; per-entry audit pending |
| SRC-074 | wcommdlg.c | S7 / OTHER | Inherited build; per-entry audit pending |
| SRC-075 | wspool.c | S7 / OTHER | Inherited build; per-entry audit pending |
| SRC-076 | wole2.c | S7 / OTHER | Inherited build; per-entry audit pending |
| SRC-077 | wparam.c | S7 / OTHER | Inherited build; per-entry audit pending |

Also audit wow32.rc, DEF/exports, original headers and resources, generated
inputs, DLL entry/unload, dynamic lookups and selected non-MVDM imports.
S1 reconciles this source seed against the current formal graph after the
preceding DLL-boundary task; this list does not freeze old component paths.

## Dispatch coverage

[wowtbl.c](../../../src/mvdm/wow32/wowtbl.c) SHA-256
4C053570AF6E72B0159D0E1BC7C5E9859EF89D513781BBCD1E767DDAF7695960.

| Table | Family | Review owner |
| --- | --- | --- |
| wktbl2.h | KERNEL | S6; S2 task entries |
| wutbl2.h | USER | S2; S4 resources, S5 clipboard and S7 remaining entries |
| wgtbl2.h | GDI | S3; S7 print/spool integration |
| wkbdtbl2.h | Keyboard | S7 |
| wstbl2.h | Sound | S7 |
| wshtbl2.h | Shell | S7 |
| wwstbl2.h | Winsock | S7 |
| wthtbl2.h | ToolHelp | S7 |
| wmmtbl2.h | Multimedia | S7 |
| wcmdgtbl.h | Common dialogs | S7 |

S1 has completed the static disposition of all 1,786 selected rows: 182
`LOCALAPI`, 936 `UNIMPLEMENTEDAPI`, and 668 host thunks with selected original
C bodies. Runtime receiver ownership remains the packet split above. Record
original LOCALAPI/NOP/unimplemented entries by the original selected profile;
do not count them as new defects or silently use them to exclude a reached
capability.

## Registration coverage

Input assignments are read from wow32.c::W32Init. Output assignments are read
from the current wow_user_registration_bridge.c. These are source observations,
not claims of real guest invocation. S7 reconciles the aggregate ABI; the
listed capability owner must implement and wire its slots as soon as usable.

| ID | Input callback | Original assigned body | Owner | State |
| --- | --- | --- | --- | --- |
| IN-01 | pfnLocalAlloc | W32LocalAlloc | S6 | Assignment present; invocation/lifetime pending |
| IN-02 | pfnLocalReAlloc | W32LocalReAlloc | S6 | Assignment present; invocation/lifetime pending |
| IN-03 | pfnLocalLock | W32LocalLock | S6 | Assignment present; invocation/lifetime pending |
| IN-04 | pfnLocalUnlock | W32LocalUnlock | S6 | Assignment present; invocation/lifetime pending |
| IN-05 | pfnLocalSize | W32LocalSize | S6 | Assignment present; invocation/lifetime pending |
| IN-06 | pfnLocalFree | W32LocalFree | S6 | Assignment present; invocation/lifetime pending |
| IN-07 | pfnGetExpWinVer | W32GetExpWinVer | S2 | Assignment present; invocation/lifetime pending |
| IN-08 | pfnInitDlgCb | W32InitDlg | S2 | Assignment present; invocation/lifetime pending |
| IN-09 | pfn16GlobalAlloc | W32GlobalAlloc16 | S6 | Assignment present; invocation/lifetime pending |
| IN-10 | pfn16GlobalFree | W32GlobalFree16 | S6 | Assignment present; invocation/lifetime pending |
| IN-11 | pfnEmptyCB | W32EmptyClipboard | S5 | Assignment present; invocation/lifetime pending |
| IN-12 | pfnFindResourceEx | W32FindResource | S4 | Assignment present; invocation/lifetime pending |
| IN-13 | pfnLoadResource | W32LoadResource | S4 | Assignment present; invocation/lifetime pending |
| IN-14 | pfnFreeResource | W32FreeResource | S4 | Assignment present; invocation/lifetime pending |
| IN-15 | pfnLockResource | W32LockResource | S4 | Assignment present; invocation/lifetime pending |
| IN-16 | pfnUnlockResource | W32UnlockResource | S4 | Assignment present; invocation/lifetime pending |
| IN-17 | pfnSizeofResource | W32SizeofResource | S4 | Assignment present; invocation/lifetime pending |
| IN-18 | pfnWowWndProcEx | W32Win16WndProcEx | S2 | Assignment present; invocation/lifetime pending |
| IN-19 | pfnWowEditNextWord | W32EditNextWord | S2 | Assignment present; invocation/lifetime pending |
| IN-20 | pfnWowSetFakeDialogClass | SetFakeDialogClass | S2 | Assignment present; invocation/lifetime pending |
| IN-21 | pfnWowCBStoreHandle | WU32ICBStoreHandle | S5 | Assignment present; invocation/lifetime pending |

| ID | Output callback | Owner | Current assignment | State |
| --- | --- | --- | --- | --- |
| OUT-01 | pfnCsCreateWindowEx | S2 | wow_user_create_window_ex | Assignment present; full guest/cleanup acceptance pending |
| OUT-02 | pfnDirectedYield | S2 | registered_directed_yield | Assignment present; full guest/cleanup acceptance pending |
| OUT-03 | pfnFreeDDEData | S5 | FreeDDEData | Assignment present; full guest/cleanup acceptance pending |
| OUT-04 | pfnGetClassWOWWords | S2 | GetClassWOWWords | Assignment present; full guest/cleanup acceptance pending |
| OUT-05 | pfnInitTask | S2 | registered_init_task | Assignment present; full guest/cleanup acceptance pending |
| OUT-06 | pfnRegisterClassWOWA | S2 | registered_register_class | Assignment present; full guest/cleanup acceptance pending |
| OUT-07 | pfnRegisterUserHungAppHandlers | S2 | registered_hung_handlers | Assignment present; full guest/cleanup acceptance pending |
| OUT-08 | pfnServerCreateDialog | S2 | wow_user_server_create_dialog | Assignment present; full guest/cleanup acceptance pending |
| OUT-09 | pfnServerLoadCreateCursorIcon | S4 | wow_private_user_load_create_cursor_icon | Assignment present; full guest/cleanup acceptance pending |
| OUT-10 | pfnServerLoadCreateMenu | S4 | wow_private_user_load_create_menu | Assignment present; full guest/cleanup acceptance pending |
| OUT-11 | pfnWOWCleanup | S2 | registered_cleanup | Assignment present; full guest/cleanup acceptance pending |
| OUT-12 | pfnWOWFindWindow | S2 | registered_find_window | Assignment present; full guest/cleanup acceptance pending |
| OUT-13 | pfnWOWGetIdFromDirectory | S4 | wow_private_user_get_id_from_directory | Assignment present; full guest/cleanup acceptance pending |
| OUT-14 | pfnWOWLoadBitmapA | S4 | registered_load_bitmap; depends on S3 | Assignment present; full guest/cleanup acceptance pending |
| OUT-15 | pfnWowWaitForMsgAndEvent | S2 | registered_wait_for_message | Assignment present; full guest/cleanup acceptance pending |
| OUT-16 | pfnYieldTask | S2 | registered_yield_task | Assignment present; full guest/cleanup acceptance pending |
| OUT-17 | pfnGetFullUserHandle | S2 | registered_full_handle -> worker-local HANDLEENTRY/native HWND association | Publish/retire/reuse fixture passes; real guest message-path invocation remains pending |
| OUT-18 | pfnGetMenuIndex | S4 | wow_private_user_get_menu_index | Assignment present; full guest/cleanup acceptance pending |
| OUT-19 | pfnWowGetDefWindowProcBits | S4 | wow_private_user_get_def_window_proc_bits | Assignment present; full guest/cleanup acceptance pending |
| OUT-20 | pfnFillWindow | S4 | registered_fill_window; depends on S3 | Assignment present; full guest/cleanup acceptance pending |

GetFullUserHandle belongs to S2's shared identity contract even though the
historical slot map deferred it to the old aggregate registrar packet.
S7 verifies its completed integration; it cannot postpone S2's required owner.

## Direct-data coverage

The following 26 mappings inherit E40's binary/source evidence. All runtime
contracts remain pending successor acceptance; GetDesktopWindow has the
specific failing E42 witness. Forty-seven instruction checks are maintained
by tests/observation/verify-wow-user-profile.ps1, not duplicated here as
manually copied opcodes. S1 records each check's mapping/field/owner and any
additional consumers discovered by complete-source review.

| ID | Original consumer | Data dependencies | Owner |
| --- | --- | --- | --- |
| DATA-01 | ClientToScreen | WND client rectangle | S2 |
| DATA-02 | GetClassName | WND/CLS/name | S2 |
| DATA-03 | GetClientRect | WND rectangle/version/metrics | S2 |
| DATA-04 | GetCursorPos | SERVERINFO cursor | S2 |
| DATA-05 | GetDesktopWindow | TEB/DESKTOPINFO/WND | S2 |
| DATA-06 | GetDlgItem | child/sibling/control ID | S2 |
| DATA-07 | GetMenu | WND menu/control ID | S4 |
| DATA-08 | GetMenuItemCount | MENU count | S4 |
| DATA-09 | GetMenuItemID | MENU items/ID/submenu | S4 |
| DATA-10 | GetMenuState | recursive MENU flags/state | S4 |
| DATA-11 | GetNextWindow | GetWindow graph | S2 |
| DATA-12 | GetParent | WND parent/owner/style | S2 |
| DATA-13 | GetSubMenu | MENU item/submenu | S4 |
| DATA-14 | GetSysColor | SERVERINFO colors | S2 |
| DATA-15 | GetSystemMetrics | SERVERINFO metrics/version | S2 |
| DATA-16 | GetTickCount | shared tick/multiplier; GetCurrentTime | S2 |
| DATA-17 | GetTopWindow | desktop/first child | S2 |
| DATA-18 | GetWindow | WND relationships/flags | S2 |
| DATA-19 | GetWindowRect | WND rectangle pointer/copy | S2 |
| DATA-20 | IsChild | WND ancestry | S2 |
| DATA-21 | IsIconic | WND minimized flag | S2 |
| DATA-22 | IsWindow | handle type/uniqueness/destruction | S2 |
| DATA-23 | IsWindowEnabled | WND disabled flag | S2 |
| DATA-24 | IsWindowVisible | WND ancestor visibility | S2 |
| DATA-25 | IsZoomed | WND maximized flag | S2 |
| DATA-26 | ScreenToClient | WND client rectangle | S2 |

S2 provides shared identity/thread/publication prerequisites for S4 menu rows.
Ordinary thunk tests and optimized direct-read tests remain separate.

## Detailed row template

Copy within the owning S evidence record and link the stable ID here; do not
duplicate chronology across records.

- ID / capability / responsible S / dependencies / disposition.
- Original source path, function range and pinned hash; current revision/diff.
- Production caller -> original owner -> finite binding -> return/cleanup.
- For shared data: consumer instructions, field/layout, authoritative producer,
  guest read/write direction, mapping, all mutation/retirement triggers,
  callback visibility, external-object coverage and borrowed-storage lifetime.
- Source / Build / Wiring / Focused / Guest / Teardown / Delivery:
  each pending until its own evidence is recorded.
- Tests: normal; invalid/failure/rollback; reentry/concurrency; task/module/
  thread/worker exit; direct-read versus thunk versus callback distinction.
- Evidence: exact command, source revision, artifact hashes, observed output,
  evidence anchor/log path and which property is not proved.
- Next action / concrete blocker / receiver acceptance; no unnamed later owner.
- Closure decision, commit/push; reopening counterevidence and invalidated claims.

## Maintenance and closure

Each implementation S links its one evidence checklist here at admission.
S2-S8 records are not created empty in advance. S1 owns inventory expansion,
function-level shared-file allocation and complete dependency disposition.
Per-row implementation evidence lives with its responsible S. S8 audits this
index for uncovered inputs and joins matching-artifact results; it does not
absorb unfinished wiring. Counts measure coverage, never a percentage of
working Win16 APIs. No completion checkbox is ticked solely by this seed.
