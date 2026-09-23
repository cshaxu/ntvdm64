# T422 S1 executable WOW32 closure graph

## Question

Can the 77 selected original WOW32 units, the 21/20 registration surface and
the direct PMODE32 readers be divided into finite implementation packets that
do not defer a completed package's required wiring to a later crash-driven
repair?

## Inputs and method

This graph reconciles the selected `src/mvdm/wow32/sources` manifest, all ten
original dispatch tables, the registration audit, the direct-data matrix, the
dynamic host-surface audit and the bootstrap/teardown order.  A dependency is
drawn only when a consumer needs a producer's complete data/lifetime contract;
shared use of a Win32 API alone does not create a packet dependency.

## Executable packet graph

```text
S1 design
  |
  +--> S2 USER client view / task / callback closure
  |       | \
  |       |  +--> S3 GDI -------------------+
  |       |                                  |
  |       +--> S4 resources / menus ---------+--> S5 clipboard / DDE
  |       |                                  |
  |       +--> S6 KERNEL / DOS / OEM --------+--> S7 remaining families / CORE
  |                                                    |
  +----------------------------------------------------+--> S8 WRITE/WINMINE/SOL
```

`S2` is deliberately the only initial producer of guest USER identity and
task lifetime.  No later packet may invent a second desktop, handle table,
thread scheduler or CCPU mapping state.  S3--S7 wire every entry they make
usable at the point it becomes usable; S8 integrates rather than backfills.

| Packet | Complete original selected surface | Registration/direct-data ownership | Preconditions | Required proof before packet closure |
| --- | --- | --- | --- | --- |
| S2 | `wcall16,wcall32,wmsg16,wmdisp32,wmsgbm,wmsgcb,wmsgem,wmsglb,wmsgsbm,wmtbl32,wuser31,wuman,wucaret,wuclass,wudlg,wumsg,wuser,wutext,wutmr,wuwind,wsubcls,wusercli`; USER portions of `wkman`. | IN-07/08/18/19/20; OUT-01/02/04/05/06/07/08/11/12/15/16/17; DATA-01--26 except menu materialization. | S1 B1--B5 order. | Valid first direct desktop read before `InitTask`; real task/window/class/dialog/message/callback creation, mutation, nested destruction and stale handle rejection; module versus task versus worker teardown. |
| S3 | `wgdi31,wgman,wgdi,wgfont,wgmeta,wgpal,wgtext,wreldc,wdib`. | Shared WND identity from S2; bitmap/DC carrier consumed by S4. | Complete S2 handles/borrows. | Guest drawing/text/font/palette/DIB/metafile result plus invalid input and repeated release; no GDI alias survives task/worker loss. |
| S4 | `wres16,wres32,wucursor,wumenu,wcuricon,waccel`; resource/menu portions of USER tables. | IN-12--17; OUT-09/10/13/14/18/19/20; DATA-13 menu graph. | S2 identities; S3 bitmap/DC acceptance for bitmap output. | Real resource lookup/load/free, accelerator/cursor/icon/bitmap, menu/submenu mutation and owner-draw callback/release; no host pointer enters the guest menu graph. |
| S5 | `wdde,wuclip`. | IN-11/21; OUT-03. | S2 message/task teardown; S4 resource formats where reached. | Guest clipboard/DDE transfer, rejection, cancellation/reentry and peer/module/task loss. |
| S6 | `walias,wstruc,wkmem,wkgthunk,wkman,wkfileio,wkernel,wdos,walloc16,wheap`. | IN-01--06/09/10; KERNEL dispatch plus OEM-WOW-DIR/DELETE. | S2 single task owner; existing worker memory/lease contract. | Loader/memory/file/environment/OEM success and rollback, alias lifetime, module/task release and real non-ASCII consumer paths. |
| S7 | `mapembed,wow32,wowtbl,wcntl32,wow32fax,wgprnset,wkbman,wsman,wshell,wucomm,wuhook,wulang,wowhooks,wmmedia,wmmedia1,wmmedia2,wmmstruc,wmmstru1,wmmstru2,wmmalias,wsdata,wsext,wsraw,wthman,wcommdlg,wspool,wole2,wparam`; all remaining keyboard/sound/shell/winsock/toolhelp/multimedia/common-dialog/print/OLE/hard-error and CORE rows. | Every remaining table row; common startup/rollback; original Shell registry bodies through the shared shadow tree. | S2--S6 complete relevant shared contracts. | Each family has a source/body/binding/real-or-explicit-negative/cleanup ledger. Dynamic tables resolve atomically or fail before call; no residual selected registration placeholder. |
| S8 | No deferred source subset: whole provider and immutable apps. | Rechecks all 21 inputs/20 outputs and all direct/thunk/callback routes. | S2--S7 closure evidence on one matching build. | WRITE, WINMINE, SOL startup/use/exit/restart plus all established DOS regression routes and controlled abnormal cleanup. |

## Critical dependency decisions

1. **Desktop before task:** S2 B1 happens before `WU32NotifyWow` returns.
   Therefore no S3--S7 packet may depend on a hypothetical later `InitTask`
   publication.
2. **Menus consume handles, not vice versa:** S4 owns `MENU/MENUITEM`, but
   S2 first supplies typed handle/WND/CLS publication and withdrawal.  This
   prevents a separate menu handle namespace.
3. **KERNEL shares task ownership:** S6 retains original `wkman` algorithmic
   bodies but does not acquire a second task/queue owner.  S2's production
   task context is a prerequisite for S6 task/module teardown evidence.
4. **CORE comes last by design:** `W32Init`, dispatch and DLL teardown touch
   all families.  S7 makes startup/rollback and dynamic resolution complete
   after every consumer contract exists; it cannot postpone a missing family
   to S8.
5. **No acceptance shortcut:** an original table row marked local or
   unimplemented remains its original selected behavior.  A reached row must
   nevertheless have either a proved original result or an explicit
   source-shaped unavailable result with a test; a link symbol is not proof.

## A08/A10 delivery matrix

| Audit deliverable | Result | Remaining implementation receiver |
| --- | --- | --- |
| 77 selected bodies | Every body is assigned to exactly one primary packet; shared `wkman` is split by original function/lifecycle role, not duplicated. | S2/S6/S7 as listed. |
| Original DLL resource and exports | The formal graph links the selected `wow32.rc`; all 27 original `wow32.def` exports appear in the generated x86 DEF. Its two additional entries are only decorated `GetCommHandle`/`GetCommShadowMSR` ABI aliases. | S7 rechecks load/unload behavior; no new WOW API is implied. |
| Ten tables / 1,786 rows | Static disposition is complete; runtime family receivers are the table families above. | S2-S7, rechecked S8. |
| 21 inputs / 20 outputs | Each slot has exactly one owner; the S2 `GetFullUserHandle` placeholder remains an explicit non-pass until the guest handle producer exists. | S2-S7, rechecked S8. |
| Direct readers | Desktop, handles, WND/CLS, time and metrics are S2; menu is S4; all final application readers repeat in S8. | S2/S4/S8. |
| Dynamic imports | 49 WSOCK32, 15 WINSPOOL, 78 WINMM and one OLETHK32 symbol are statically present; atomically safe runtime load/failure/teardown is S7. | S7. |
| Autonomous footprint | 21 DLL-local files / 2,977 lines plus 2 worker-local files / 180 lines are retained only as named finite bindings.  No binding is promoted to a mirror merely by this graph. | Function-level four-rung/reduction review remains A03/A10. |

## Indirect callback and reentry allocation

The active 77-file manifest has **72** original `CallBack16` call sites in
24 source bodies, plus the three USER native-to-guest dispatch gateways
(`W32Win16WndProcEx`/`W32DialogFunc`/`W32EditNextWord`).  This is a
source-level ownership count, not an assertion that any callback has run.
It closes the S1 question of which packet owns each callback family; every
receiver still has to test normal, failed, reentrant and teardown behavior.

| Receiver | Original active callers | `CallBack16` sites | Additional gateway | Required closure condition |
| --- | --- | ---: | --- | --- |
| S2 USER/task | `wcall16,wcall32,wkman,wmdisp32,wmsgem,wsubcls,wudlg,wumsg,wuser,wutmr,wuwind` | 34 | all three USER dispatch gateways | One bounded CCPU frame lease; nested return, cancellation, task/module distinction and worker loss. |
| S3 GDI | `wdib,wgdi,wgfont,wgmeta` | 13 | none | DC/object release after callback, invalid input and task/worker loss. |
| S4 resource/menu | `wres16,wcuricon` | 11 | none | Resource conversion and callback failure/release after typed guest identity exists. |
| S5 clipboard/DDE | `wdde` | 2 | none | Peer rejection, abandon and task-loss cleanup. |
| S6 KERNEL | `walias` | 1 | none | Alias/module release cannot retain a callback frame. |
| S7 CORE/remaining families | `wow32,wowhooks,wcommdlg,wsext,wshell` | 11 | none | Dynamic-family callback failure/load/unload and final worker teardown. |
| **Total** | **24 selected bodies** | **72** | **3** | S8 repeats reached application interactions only; it acquires no unowned callback family. |

## Packet test invariant

Every implementation packet performs a fresh formal x86 build, tests every
newly wired production route and its failure/reentry/cleanup path, then passes
the existing 17 direct/interactive/nested COMMAND, MEM and EDIT routes on the
same artifact set.  S8 adds immutable WRITE/WINMINE/SOL interaction evidence;
it cannot substitute those applications for any family-level proof.

## Interpretation

The graph resolves the prior “wire later” ambiguity: direct desktop data,
task life, classes, windows, callbacks and cleanup are one S2 closure; GDI,
resources/menus, DDE, KERNEL/DOS and remaining interface families are each
complete downstream packets.  It does not claim an implementation is already
functional.  A packet can be regrouped only if new source evidence exposes a
real cycle; the evidence must name the changed edge and retain the old graph.
