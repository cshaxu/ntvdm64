# T420 S42 W1/W2 Lifecycle Binding Ledger

Owner-directed scope transfer, 2026-09-21: the
[handoff record](m0-t420-s42-wow32-successor-handoff.md) and
[WOW32 successor proposal](../../proposals/proposal-wow32-complete-runtime-recovery-001.md)
now own all remaining W1/W2 and broader WOW32 work. Statements below that
require a repair before S42 closure describe the superseded implementation
scope. The repair remains mandatory in the successor; S42 now closes only
research/code preservation and established DOS regression. Historical failures
and fixture-only results retain their original limitations.

## Question

Can selected original W1 task lifecycle and W2 class/window/message/dialog
owners share one worker-local USER synchronization domain without inventing a
second USER implementation or a second domain inside `WOW32.DLL`?

## Inputs

- Active packet: M0 T420 S42.
- Original WOW32 callers: `src/mvdm/wow32/wkman.c`, `wuclass.c`, `wudlg.c`,
  `wuser.c`, and `wkgthunk.c`.
- Recovered original owners: `src/opennt-host/windows/core/ntuser/kernel/`
  `taskman.c`, `queue.c`, and `exitwin.c`.
- Worker-local boundary: `src/ntvdm-exe/wow/wow_user_{runtime,session_binding}.c`
  and `src/ntvdm-exe/softpc/mvdm_standalone_worker.c`.

## Original Owner Ledger

| Original call | Original recipient | S42 binding status |
| --- | --- | --- |
| `WK32InitializeHungAppSupport` | `pfnRegisterUserHungAppHandlers` → `exitwin.c::xxxRegisterUserHungAppHandlers` | Recovered body and worker event-lifetime carrier are selected. |
| `W32Thread` | `pfnInitTask` → `queue.c::xxxInitTask` | Recovered body and one current-thread carrier are selected. |
| `WK32Yield`, `WK32DirectedYield`, `WK32WowWaitForMsgAndEvent` | `taskman.c::{xxxUserYield,xxxDirectedYield,xxxSleepTask}` | Recovered scheduler bodies use the one native queue bridge. |
| `W32DestroyTask`, `WU32FreeModule` | `pfnWOWCleanup` | S42 cleanup carrier is selected; task/window/class teardown remains a dedicated fixture/real-registration gate. |
| `WU32RegisterClass` | `pfnRegisterClassWOWA` | S42 class carrier is selected. |
| `WU32DialogBoxParam` | `pfnServerCreateDialog` | S42 dialog carrier is selected. |

The modern USER32 registration probe remains `STATUS_ACCESS_DENIED` with zero
output slots. Therefore no normal immutable Win16 launch reaches these calls
before S44 supplies the complete original all-or-nothing registrar. This is an
explicit reachability gate, not a success stub or an exclusion of original
bodies.

## Procedure

1. Generated a fresh x86 WOW32 provider graph at
   `build/M0-T420/S42/w1-w2-provider-r1`.
2. Its first link exposed the real boundary failure: W1/W2 code was selected
   into the DLL but four runtime references had no parent import surface.
3. Added the two worker-local binding units to `ntvdm.exe`, attaches one runtime
   after the existing session thread bind, detaches it before that bind is
   removed, and exports only `current`, `enter`, `leave`, and `set_context`.
4. Rebuilt affected parent objects and linked an incremental x86 parent at
   `build/M0-T420/S42/w1-w2-worker-incremental-r1`; the VDM_TIB map verifier
   passed.
5. Relinked the generated provider against that new parent import library.
6. Recompiled the session-binding fixture with the parent's ordinary-C ABI,
   linked it against the newly built worker runtime/session objects, and ran
   `build/M0-T420/S42/w1-w2-fixtures-r2/wow-user-session-binding-current.exe`.
7. Archived the fresh provider object set and used that archive, rather than
   any S40 object, for the current-source W1 task, W2 cross-task window/message
   and W2 dialog lifecycle fixtures.  The static archive lets the linker take
   only reachable original owners and requires no test-only provider export.

## Observations

- `dumpbin /imports wow32.dll` lists `ntvdm.exe` and exactly four runtime
  imports: `wow_user_runtime_current`, `enter`, `leave`, and `set_context`.
- `dumpbin /exports ntvdm.exe` lists exactly those four runtime operations.
- The parent map assigns all runtime/session-binding functions to one
  `wow-worker-bindings` archive, while `mvdm_standalone_worker` owns the sole
  `worker_wow_runtime` and `worker_wow_binding` storage.
- `Verify-VdmTibStorage.mjs` passed with no overlapping VDM_TIB symbol.
- The current-source worker binding fixture reported
  `WOW_USER_SESSION_BINDING errors=0 child=1 lifecycle=1`: a newly created
  worker-bound thread entered the one runtime, completed a callout
  leave/re-enter sequence, and detached before the session was disposed.
- The same current-provider archive produced these passing fixtures:
  `WOW_USER_TASK_LIFECYCLE errors=0`,
  `WOW_USER_TASK_WINDOW_LIFECYCLE errors=0`, and
  `WOW_USER_TASK_DIALOG_LIFECYCLE errors=0 init=1 terminal=1`.
  Together they cover original task registration/init/yield/wait/cleanup,
  synchronous and posted cross-task message receipt, native-window ownership
  retirement, and dialog init/terminal-message ownership.
- The original `wmdisp32.c::W32Win16WndProcEx` body was re-extracted only
  after its mirror hash matched the pinned OpenNT body, then linked with the
  same current provider archive.  It reported
  `WOW_ORIGINAL_WNDPROC errors=0 callbacks=3 idle_pairs=3`, covering callback
  release/re-entry, success/failure return routing and the ignore-input path.

## Artifact identities

| Artifact | SHA-256 |
| --- | --- |
| `w1-w2-worker-incremental-r1/ntvdm.exe` | `8FFB8DFCE2405C3E6F7348FEB383662651679B61A5275056565D1005E99F2DC5` |
| `w1-w2-provider-r1/wow32.dll` | `E517335A21B3B79F9C43501DC016AEC665E041DD0FF1B2EB9519DEDDD971987D` |
| `w1-w2-fixtures-r2/wow-user-session-binding-current.exe` | `23139BEE9BEF3A9EC5EDC1224AB1BFE9CDA36C4895A0692DBDB8D880D501EBC8` |
| `w1-w2-fixtures-r2/wow-user-task-lifecycle-current.exe` | `FA2E12CE6098EC6788238B7336E1C08ABEB977D06C21549FB649E4D0998BC1AF` |
| `w1-w2-fixtures-r2/wow-user-task-window-lifecycle-current.exe` | `95CE8049A61C2D9377FC2DEB36DA25E998C3B0682FE1F0E97C12B34AE283A2FD` |
| `w1-w2-fixtures-r2/wow-user-task-dialog-lifecycle-current.exe` | `E7C8817FD5C76B80E94A69E2B78CDA74EDB2D73B8B6485AFD5C880FF872CBDD0` |
| `w1-w2-fixtures-r2/wow-original-wndproc-current.exe` | `6317C39CB9364A66F58D732FCD8B654CF39DFC314072BA34807C3AFEA1CAA822` |

## Interpretation

This closes the discovered parent/DLL ABI hole: the DLL cannot silently create
a distinct USER runtime. It does **not** close S42. Remaining gates are fresh
full x86 composition and S44 complete registration before real Win16
reachability can be asserted. No guest input was changed.

## S42 object-publication repair gate

The current fixtures also exposed a distinct, in-scope production gap.  The
native `wow_user_object_table` is correctly used by the recovered original
cleanup bodies, but it is not yet the guest `SHAREDINFO.aheList` published by
S41.  The empty S41 projection deliberately allocated only five pages and
therefore cannot contain a 64K-entry `HANDLEENTRY` array.

This cannot be solved by writing a native pointer to the guest.  The original
`wow32/wuman.c::WU32NotifyWow(FUN_FINALUSERINIT)` writes native `gpsi` to the
USER16 client.  In NT4 that is valid because USER's shared section is visible
to both consumers.  Here WOW32 native memory and CCPU/SAS guest memory are
different address spaces.  S42 must retain the native companion for original
kernel-owner execution while adding a source-shaped, guest-linear projection
with the original `SHAREDINFO`/`SERVERINFO`/`HANDLEENTRY` layouts.  The
`FUN_FINALUSERINIT` guest write must use that projected linear address only;
WOW32's native `gpsi` must remain native.  Publication/retirement must update
both views atomically under the existing worker runtime, and worker teardown
must withdraw the whole mapping.  This is a required S42 repair, not a S44
registration workaround.

## S42 object-publication delivery

That repair is now implemented in the worker's existing S41 page-domain
owner; it does not add a USER implementation or alter immutable USER16.

- The client view reserves the pinned USER16 `SHAREDINFO` and `SERVERINFO`
  prefix, a complete `65536 * 16`-byte checked `HANDLEENTRY` table, and then
  the guest TEB.  `SERVERINFO.cHandleEntries` is `65536`; every initial free
  entry has the original `phead = index + 1`, null owner, `TYPE_FREE`, and
  uniqueness one.  Host pointers are never projected.
- `wow_user_window_publish` and `wow_user_window_retire` retain their native
  companion records for the recovered original USER owners, and synchronously
  publish or withdraw only `bType`, `bFlags`, and `wUniq` in the guest entry.
  Failed guest projection rolls native publication back.  The worker runtime's
  existing publication lock serializes those paired updates.
- The one changed original WOW32 statement in
  `wuman.c::WU32NotifyWow(FUN_FINALUSERINIT)` now writes the worker's
  guest-linear `SHAREDINFO` address.  Native `gpsi` remains native-only for
  WOW32's original 32-bit owners.
- These parent/DLL calls are explicit `__cdecl` ABI operations.  This preserves
  the provider's original `/Gz` internal ABI without generated stdcall aliases
  that would disguise stack-cleanup differences.

Fresh x86 evidence:

| Artifact | Result | SHA-256 |
| --- | --- | --- |
| `w1-w2-page-projection-r1/wow-page-domain-test.exe` | exits `0`; exercises source-shaped 64K table initialization, publish/retire, WOW GDT/TEB, protected-mode re-entry and session withdrawal | `4C307592B63B5FEAEBB14F8A9BCD1FEEA1BCF159B074D13E39BE28EF30B3E2F8` |
| `w1-w2-page-projection-r1/ntvdm.exe` | fresh parent link; VDM_TIB verifier passes | `35A15CCE78D9B85A1A6C07B332C017959E9F8133DD62B30D55A77CE8C3E97E49` |
| `w1-w2-provider-projection-r1/wow32.dll` | fresh original 77-body provider link; its import table names all three page-domain operations from `ntvdm.exe` | `E8449D37E16C3F0025C7AD2432F495EAC507818849321B50AE2CE9501223DF3D` |

The independently regenerated `w1-w2-object-publication-r1` fixture links
the current `wow_user_object_bindings.c` itself (its map contains
`wow_user_window_publish@16` and `wow_user_window_retire@12`) and exits zero.
It publishes native window identity `0x4567:0002` through the actual S42
owner, observes guest `aheList[2]` as type `TYPE_WINDOW`, flags zero and
uniqueness `0x4567`, then retires it and observes the original free entry.
This is the production publication line under the existing source-shaped
fixture domain, rather than a helper-only test.

The page-domain fixture uses an 8 MiB simulated VDM because the original
one-MiB USER handle table cannot coexist with CCPU page tables and mappings
in the former two-MiB header-only fixture.  This is a fixture capacity
precondition, not a guest-media or product policy change.

## Immediate registration rule

No recovered or already-implemented original registration slot may be held
back merely because S44 is the aggregate registrar packet.  Each S must wire
every slot that its current production path can validly supply and verify.
For this indivisible ABI, an unavailable source owner uses a type-correct,
non-null placeholder that reports `ERROR_CALL_NOT_IMPLEMENTED`, is explicitly
listed with its replacement owner, and never reports success.  S44 is
responsible for replacing those residual placeholders and auditing all 21
input/20 output slots.  The S42 `FUN_FINALUSERINIT` projection above follows
this rule.

## Whole-table boundary found during immediate-wiring audit

The original owner is `OpenNT/windows/core/ntuser/client/client.c::
UserRegisterWowHandlers`: it assigns all 21 WOW32-to-USER inputs, assigns all
20 USER-to-WOW32 outputs, and only then returns `&gSharedInfo`.  It has no
per-slot registration operation.  By owner authorization, the standalone
bridge now preserves this all-or-nothing shape with explicitly failing
placeholders for unavailable owners, rather than withholding the whole table.

The current mirror intentionally selects only two narrow bodies from that
3,750-line original client source (`DispatchClientMessage` and the ANSI
`RegisterClassWOWA` branch).  The full original registrar and the remaining
USER client environment are not yet selected; several non-W1/W2 output slots
also lack their source-correct binding.  The registered bridge therefore
immediately wires the available slots and assigns a typed failure placeholder
to every remaining slot, while the single
atomic whole-table operation remains an S44 residual integration item.  This
is a source-proven all-or-nothing boundary, not a deferral of an available
per-slot API.

### Output-slot owner map

| Original output | Current owner disposition |
| --- | --- |
| `_CreateWindowEx`, `DirectedYield`, `InternalCreateDialog`, `WowWaitForMsgAndEvent`, `YieldTask` | **wired now** to the selected S42 carriers. They reject absent worker/task context rather than inventing one. |
| `FreeDDEData`, `GetClassWOWWords` | **wired now** to the selected original `hdata.c` and `ntstubs.c` bodies. |
| cursor/icon, menu, ID-from-directory, bitmap, menu index, default-window bits, `FillWindow` | **wired now** to the admitted S43-capable original/public carriers; bitmap explicitly fails when its required bitmap context is absent. |
| `WOWFindWindow` | **wired now** to public `FindWindowA`; it preserves native failure rather than creating a private USER lookup table. |
| `InitTask`, `RegisterUserHungAppHandlers`, `WOWCleanup` | **wired now** to one worker-local S42 lifecycle. The original order is retained: hung-app registration creates the lifecycle's WOW process record; `InitTask` creates its task/thread; cleanup uses that task's selected original cleanup carrier. |
| `RegisterClassWOWA` | **wired now** to the selected original `client.c` registration body through the task-local class-client context created by `InitTask`. The unavailable CallProcData special-class resolver remains an explicit original failure branch, not a success substitute. |
| `GetFullUserHandle` | typed `ERROR_CALL_NOT_IMPLEMENTED` placeholder for S44's complete shared-handle owner. |

All 21 input callbacks are copied as one `PFNWOWHANDLERSIN` record, preserving
their exact original ABI for every selected/later USER owner.  The bridge does
not call an input callback merely to claim it was tested.

### S41 registration contribution now live

S41 owns no separate USER-to-WOW output callback.  Its completed contribution
is the exact storage and addressability contract used by this registration:
the bridge retains the complete 21-input `PFNWOWHANDLERSIN` record and returns
S41's guest-linear `SHAREDINFO` from
`mvdm_softpc_wow_page_domain_guest_shared_info()`.  That view already contains
the paired `SERVERINFO`, complete `aheList` projection and guest TEB.  Thus
S41 is on the production registration line now, rather than being retained as
a fixture-only prerequisite.

This map was generated by direct source lookup, not by inferred DLL exports.

### Local registrar build and reached-worker witness

The local registrar bridge is linked from
`wow_user_registration_bridge.obj`, not imported from modern `USER32.DLL`.
`tests/observation/verify-wow-output-composition.ps1` checks both the pinned
original 20-slot ABI and the local bridge's 20 non-null assignments; the
current provider map reports `localOutputSlotCount=20` and a local
`_UserRegisterWowHandlers@8` symbol.

For a real reached-worker observation, the fresh S42 `run16.exe`,
`basesrv.exe`, `ntvdm.exe`, `VDMREDIR.dll`, and `wow32.dll` were staged only
under `build/t420s42` and exposed through the temporary short test alias
`O:/t42`.  The alias is required by the unchanged original COMMAND 64-byte
configuration-string limit; all actual artifacts and logs remain under
`build`.  `O:/t42/run16.exe system32\\WRITE.EXE` returned `0`; the default-off
registration witness was enabled only for that run and recorded:

```text
27316 UserRegisterWowHandlers result=0090A000
```

`0090A000` is the S41 guest-linear `SHAREDINFO` return, not a host USER32
pointer.  The same isolated fresh artifact ran `MEM.EXE` successfully before
the WRITE observation.  This proves the real BOP/W32Init path reaches the
local atomic registrar and receives the S41 carrier.  It does not claim that
every output operation or immutable WRITE UI behavior is accepted; residual
typed placeholders and their original owners remain S42/S44 work.

After wiring the three completed lifecycle slots, the same isolated WRITE
observation records both successful registration stages:

```text
48492 UserRegisterWowHandlers result=0090A000
48492 RegisterHung result=00000001
```

The observer then times out after 30 seconds rather than returning the former
immediate `W32Init FALSE` result.  Original `wow32.c::W32Init` calls
`WK32InitializeHungAppSupport` as its final required initialization step;
that function invokes `pfnRegisterUserHungAppHandlers` and returns success
only when this registration succeeds.  This is therefore a reached-branch
advance, not a WRITE acceptance claim: later WOWEXEC/task-dispatch work is
still S42/S45 scope.

The existing default-off BOP report was enabled for a separate run.  It
records the original BOP 51 before paging, after the S41 page-domain entry,
and repeated later protected-mode BOP entries with the same active CR3:

```text
wow-bop cr0=00000011 cr3=00000000 csip=01c7:aebc ...
wow-bop cr0=80000011 cr3=00900000 csip=01c7:aebc ...
...
wow-bop cr0=80000011 cr3=00900000 csip=01c7:aebc ...
```

`mvdm/softpc.new/host/src/nt_bop.c` calls
`mvdm_softpc_wow_page_domain_activate_wow_context()` only after `W32Init`
returns true, then invokes the original `W32Dispatch`.  The repeated records
therefore prove that the timeout is after successful W32Init/activation, in
the still-incomplete original WOWEXEC/task-creation dispatch path.  No
`InitTask` trace was observed in this bounded run; that absence is a frontier
observation, not a claim that its callback is unused.

The bounded dispatch witness independently resolves the first unpatched call
IDs through the original `wktbl2.h` table. Correction: the trace uses hex,
whereas the table's ordinal comments use decimal. The entries are
`010F=WOWDosWowInit` (271), `00E9=VirtualAlloc` (233),
`00F2=WOWFileOpen` (242), `00F3=WOWFileClose` (243), and
`00F9=WOWFileGetDateTime` (249). `WOWInitTask` is decimal 287, hex `011F`,
not hex `0287`. The earlier startup/debug names were a radix error, not a
runtime observation. Later `699AAAF0` is expected after original
`W32PatchCodeWithLpfnw32` replaces a 16-bit thunk's function ID with its
resolved host function pointer.  The default-off trace therefore establishes
that W32Dispatch is executing its original patch-and-invoke path after
successful W32Init; it does not manufacture task creation, and it keeps the
absence of `InitTask` correctly classified as a reached-work frontier.

## Fresh local-registrar rebuild and isolated reachability

The provider was regenerated after the S41-carrier and S42 lifecycle callback
assignments in `build/M0-T420/S42/w1-w2-registration-r2`.  This host's native
Ninja dispatch lock was bypassed by the established bounded fallback: native
Ninja expanded the exact 128 graph commands, and the graph-declared x86 MSVC
environment executed those commands serially.  This did not alter the graph or
select a different source set.  The resulting `wow32.dll` SHA-256 is
`DF30F77493F011AAA0ECEA400715F370CC2E07ECC9B981FE137B13C4F6E95062`; its
map SHA-256 is `C2356BDE1A0150B3DBD2BCADC7AA8A5D36A58FFCB90D9ED96F31545E7E91F82C`.
The composition verifier's required local-registration gate passes: the map
locates `_UserRegisterWowHandlers@8` in `wow_user_registration_bridge.obj`,
and verifies all 20 non-null output assignments.  It is intentionally a
composition witness, not an acceptance claim for every callback.

The new DLL was staged only in the short-path `build/t420s42` test package
through `O:/t42`; the package's prior DLL was retained there as
`WOW32.pre-s42-registration-r2.dll`.  `O:/t42/run16.exe MEM.EXE` completed
with exit zero and the normal conventional/XMS reports.  A separate bounded
`O:/t42/run16.exe system32/WRITE.EXE` observation timed out after 30 seconds,
but its opt-in witnesses establish the reached sequence:

```text
58664 UserRegisterWowHandlers result=0090A000
58664 RegisterHung result=00000001
57020 W32Dispatch call=010F thunk=71A3F8A0 task=0000
57020 W32Dispatch call=00E9 thunk=71A3AAF0 task=0000
57020 W32Dispatch call=71A3AAF0 thunk=71A3AAF0 task=0000
57020 W32Dispatch call=00F2 thunk=71A42820 task=0000
57020 W32Dispatch call=00F9 thunk=71A43180 task=0000
57020 W32Dispatch call=00F3 thunk=71A42E60 task=0000
```

The BOP witness for that same run includes the S41 CR3 `00900000` protected
mode entries.  Thus the fresh local registrar is actually used by the worker,
and the newly wired hung-app callback succeeds before the existing task-zero
frontier.  There is still no `InitTask` witness and no claim of WRITE UI
acceptance; those remain the S42/S45 work rather than being hidden by the MEM
regression pass.

The completed S41 class carrier is also now connected to the production
`pfnRegisterClassWOWA` slot.  On `InitTask`, the lifecycle creates a
task-local `wow_class_client_context` with its existing original class list,
the task version, and the exact `pfnGetExpWinVer`/`pfnWowWndProcEx` callbacks
that WOW32 supplied to `UserRegisterWowHandlers`; the original recovered
`client.c` registration body is then invoked by the slot.  This is not a
parallel class implementation.  The missing CallProcData resolver remains
NULL, so the original special CPD class branch still fails explicitly instead
of receiving a fabricated procedure.  The refreshed short-path package was
verified by `O:/t42/run16.exe MEM.EXE` with exit zero after this wiring.  The
existing class-client fixture separately covers normal class publication,
unregistration, callback preservation, and the explicitly unavailable CPD
branch.  A real WRITE task has not yet reached `InitTask`, so this is a
completed production connection plus direct fixture coverage, not a claim of
guest class-registration reachability.

A second bounded WRITE observation using that refreshed DLL repeats the same
post-registration dispatch frontier (`010F`, `00E9`, patched `71A3AAF0`,
`00F2`, `00F9`, `00F3`) and still records no `011F=WOWInitTask`.  This
separates the two facts: the production class slot is no longer a local
placeholder, while the immutable guest has not yet issued that slot's
operation.  The trace augmentation is observation-only: it records the
resolved thunk pointer alongside the pre-patch 16-bit call ID and does not
change `W32Dispatch`'s control flow.

### `WOWInitTask` ownership boundary

The original source rules out a host-side shortcut for the absent `011F` call.
`mvdm/wow16/kernel31/tasking.asm::StartWOWTask` switches to the newly prepared
Win16 task stack, constructs the module/task arguments, and jumps through
`WowInitTask`; `mvdm/wow16/kernel31/kthunks.asm` declares that thunk as the
internal `FUN_WOWINITTASK` BOP entry.  The original `wkman.c::WK32WOWInitTask`
then only receives that already-valid `VDMFRAME`, records `RET_TASKSTARTED`,
and creates/continues the W32 task thread.  It is therefore invalid for the
standalone host to synthesize `InitTask`: doing so would invent a TDB, stack,
module and caller return protocol.  The current bounded WRITE trace establishes
that execution has not yet reached `StartWOWTask`; locating why the immutable
guest has not entered that original KRNL386 tasking path is the next S42
frontier.

### Early-dispatch return observation

The follow-up bounded run proves that this is not a host thunk that fails to
return.  Each observed early dispatch had a paired return: `010F` returned
`00000000`, `00E9` returned `00A1D000`, patched `71A3AAF0` returned
`00A35000`, `00F2` returned `00000005`, `00F9` returned `5D29996B`, and `00F3`
returned `00000000`. This is a file-close return, not `DEBUGBREAK`.
No later dispatch, including `011F`, appears before the bounded observation
ends.  The remaining frontier is therefore after the BOP return in immutable
KRNL386/WOWEXEC control flow, not a deadlock or exception in the standalone
WOW32 callback carrier.

The BOP owner also recorded a post-`W32Dispatch` state for each of those six
returns: all retain protected+paged `CR0=80000011`, `CR3=00900000`, and
`CS:IP=01C7:AEBC`.  `AEBC` is the existing BOP-handler instruction position;
the original BOP engine advances/resumes guest execution after the handler
returns, so this scalar is not a post-resume guest EIP and cannot honestly be
used to claim a KRNL386 loop.  It proves only that the BOP owner has not
rewritten the CPU state while dispatching.  The next diagnostic must use the
existing bounded CPU opcode-window mechanism, rather than widening this
handler trace into an invented execution monitor.

### KRNL386 LDT initialization frontier

The bounded CPU40 witness was then enabled only for selector `01C7`, with a
512-instruction ceiling, during another isolated 30-second WRITE run.  It
records the opcode pointer already fetched by the original CCPU decode loop;
it neither fetches guest memory nor changes decode, register, event, or guest
memory state.  The `pa` field is the existing decode-page boundary and is not
used as an instruction address in this evidence.

All six pre-loop WOW dispatches again returned.  The guest then repeatedly
executed this exact three-instruction cycle:

```text
01C7:D7FA  op=83
01C7:D7FF  op=3B
01C7:D803  op=26
```

`retail3/kernel.map` maps `0001:D7C2` to original `LDT_INIT`; the observed
range is inside that original KRNL386 initialization routine.  Thus the
sample identifies execution within selector initialization, but does not
establish a stall there or order this sample after the six dispatches.
The former "spinning"/broken-descriptor conclusion is withdrawn. The
corrected dispatch identity below proves later startup execution. This
sample is not evidence for changing the guest or the CCPU descriptor model.

The immutable `O:/t42/system32/KRNL386.EXE` NE segment-1 bytes independently
confirm the nature of the cycle.  At `D7FA` the sequence starts with
`83 C6 08` (advance one eight-byte descriptor), then at `D7FF` compares that
cursor against its descriptor-table bound, and at `D803` begins the
ES-segment descriptor comparison.  The two subsequent conditional branches
return to `D7FA` when the candidate is not free.  This is original selector
allocation/search, not a Windows message loop and not an arbitrary CCPU
instruction loop.

The initial wording "spinning" was too strong and is superseded by a fresh,
default-off sparse-cursor observation.  In an isolated 20-second WRITE run,
the original cursor reached `SI=1000,2000,...,F000`, while `CX=FFEF`,
`ES=0137`, `ES.base=00116790`, and `ES.limit=0000FFEF` remained stable.  The
worker remained live. This proves cursor progress, not descriptor contents,
table equivalence or completion by itself. Separately, original `ldboot.asm`
calls `LDT_Init`, `AllocSelector_0x47`, `WOWFastBopInit`, INT31/0002, and then
`WOWDosWowInit`. The corrected trace therefore witnesses progress beyond
that initialization sequence. The r4 link map independently corroborates all
five thunk names by RVA (`1F8A0`, `1AAF0`, `22820`, `23180`, `22E60`).

The r12 256-instruction observation leaves the sampled LDT_Init address
interval and reaches `00C7:4414` (`dxint31.asm::PMIntr31`). Leaving that
interval may be a nested call, not LDT_Init's return. Its repeated
`4419..4427` instructions scan the original INT31 service table, advancing
BX by four; repetition alone is not an infinite-loop witness. The temporary
CCPU observation is removed rather than retained as a product diff. The
remaining runtime stop after the six returned thunks still needs attribution;
no WRITE or S42 acceptance is claimed.

### Actual startup exception and protected-return repair

The r13 isolated observation reads only windows owned by the launched product
processes and records their thread wait states. It finds a real error dialog:
`X#=0D, CS=01C7 IP=00004148`. All sampled execution-thread states are waits,
not a busy selector scan. Read-only NE decoding of the immutable KRNL386
segment at `4146` yields `6A 23 07` (`push 23h; pop es`). The retained map
and `3gcompac.asm::gmove` identify original `smov es,FLAT_SEL`, followed by
the original flat memory copy. This is not evidence of a guest defect.

Source-first disposition: retain original KRNL386/DOSX and CCPU segment-load
validation unchanged. The native user GDT lifetime is already the admitted
S41 boundary; the worker temporarily withdraws that view for DOSX real mode.
Its old binding forgot the selected WOW view and restored only paging on
return. The old fixture masked this by explicitly activating the view again.
Removing that artificial activation reproduces `LIFECYCLE_FAILED stage=reentry`.
Retain the selected context independently of temporary withdrawal, restore
it on domain entry/reentry, and invoke the same binding at the existing
`53:01` source-shaped CPU40 transition after original register restoration.
No instruction, descriptor permission, guest binary or original gmove body
is changed. Withdrawal remains idempotent and only replaces its own active
GDT view. The mirror seam is registered under the existing DIV-222 owner.

r14 proved a repair of the generic return alone insufficient: the mode log
also reaches `bop-fd-real -> 53:01-protected`. r15 includes that second
return path and passes the former `4148` exception, reaches nonzero guest
task `023F`, then stops inside `WU32NotifyWow` at the original message
`user.exe and user32.dll are mismatched`. This does not yet prove the
`InitTask` callback or WRITE acceptance. The existing pinned-binary profile
test identifies USER's build/layout value as `84000000` (WOW 0400, WOWDBG);
the local registration bridge incorrectly advertised `04000000`. Correcting
the bridge to the already selected checked shared-view layout retains the
original comparison; it must not bypass the check or modify USER.EXE.
The composition verifier now compares the bridge identity against the
read-only pinned-binary profile instead of checking only slot population.

Raw observations reside in `O:/winnt/logs/t420-s42-startup-wait-r13` through
`r15`. r13 restoration first met the terminating-process file lock, then
completed after process exit; subsequent observations explicitly wait for
exit before restoring. The original worker/DLL hashes are verified restored.
The disposable observer is not a product component.

The expanded CCPU40 page-domain fixture now passes with the explicit
`WOW_PAGE_DOMAIN_LIFECYCLE_OK` marker: pre-selection reentry retains DOSX,
selected reentry and entry restore WOW, source descriptors remain unchanged,
repeated withdrawal is idempotent, and teardown clears publication. Seed the
fixture GDT after its deliberate A20 rejection test: changing the A20 alias
otherwise discards the high-memory test sentinel before domain admission.
The marker uses CRT stderr because original `fprt.c` deliberately suppresses
`puts` in PROD; zero exit alone is not substituted for the marker.

The corrected worker (`B6E0839B178B0095C30E7BC270381E3224897F46418A9DB27316C7930A83C3BA`)
and provider (`60144142461DCF71251D8166A323D24CD7C813F5B02F57C515F85E21392D4A29`)
pass all 17 existing text-gated DOS routes, summary
`O:/winnt/logs/s42-reentry-regression-r1-summary.json`. The original formal
package was restored and hash-checked afterward. r16 observes the matching
USER registration complete (`034C` returns), followed by original
`WK32ExitKernel` (`0002`); the worker exits, so this is not WRITE acceptance.
W1/W2 lifecycle closure and the exit's original guest cause remain open.

### USER bootstrap client-view obligations still owned by S42

`WU32NotifyWow` publishes two guest-consumed pointers, not only `gpsi`.
Original `usercli.asm::TESTCALLSERVERCONDITION` and
`CLEARCALLSERVERCONDITION`, plus the retained `wowasmk.asm` keyboard/menu
clients, read/set/clear the original `CallCsrFlag` byte. The original static
address worked in native NT's shared linear address space; it is not a CCPU
guest address. The worker now supplies one initially zero writable byte in
the same finite client-view allocation and releases it with that allocation.
The original USER decision macros and host dispatch are unchanged. The
fixture proves zero initialization, writable translation, set/clear and
withdrawal; this is address binding, not a replacement CSR policy. r18 still
exits through `WK32ExitKernel`, so this necessary repair is not claimed as
the cause or cure of that exit.

The latest worker hash is
`AA0B68294AB32345D13497D2E8A34E7329DF36C7D7ADE595EFD266AA427AFB8E`;
provider hash is
`91BDF083EB48F14748EC2CFF691F27352D35ED12EC6C6BCD36678258B3143579`.
This pair passes all 17 text-gated DOS routes again in
`O:/winnt/logs/s42-reentry-regression-r2-summary.json` (17 rows, zero
expected/actual mismatches). The formal package is restored, not replaced
with this still-unaccepted WOW candidate. The enhanced page-domain fixture
also passes with its explicit lifecycle marker.

r17/r18 default-off frame traces put the last normal call at USER
`0337:486A` (`WU32NotifyWow` return), then `ExitKernelThunk` returning to
KRNL386 wrapper `021F:8347`. The frame identifies the wrapper, not its
earlier caller. Read-only USER NE decoding shows the next unconditional call
at `487C` targets `GetDesktopWindow` (`6054` in its pinned map). Its original
client implementation dereferences `TEB+5C -> DESKTOPINFO+8`, adjusting by
`CLIENTINFO.ulClientDelta`. The current worker view still leaves those
desktop/client fields zero. This is a concrete unimplemented S42 production
dependency, not a source-proven unused callback, not a guest defect, and not
work that can be transferred to graphics S43 or aggregate S45 acceptance.

The next closure unit must bind the original client desktop/window/class
view and its publication/retirement to the already selected original object
owners. Native companion objects and HANDLEENTRY identity bits alone cannot
satisfy these direct guest reads. Audit original desktop/client publication
and relocation before implementation; do not fabricate a dummy desktop,
bypass GetDesktopWindow, change guest code, or import the whole USER server.
Retain exact consumer layout and extend lifecycle/error/teardown coverage.

### Confirmed USER bootstrap desktop-publication fault

r19/r20 capture original `DpmiDispatchFault` inputs before guest exception
handling. In r20, `0337:011F` raises page fault `0E`, error `00000004`,
CR2 `055A0016`. The pinned USER image/map identifies this exact instruction
as `_GetDesktopWindow@0`'s `mov eax,[eax]`, not an instruction-decoder defect.
The same observation records guest TEB `00A0C000`, `TEB+5C=0` (pDeskInfo),
`TEB+60=0` (client delta), and guest DWORD at address 8 = `055A0016`.
Original `__GetDesktopWindow@0` reads FS:18, then TEB+5C, desktop+8,
subtracts the client delta, and the wrapper dereferences the resulting WND.
Thus the unpublished desktop pointer causes a read from the mapped DOS IVT,
whose nonzero contents become a bogus WND address. Original ExitKernel
follows the fault. This establishes the concrete failure, not merely a
possibly missing structure inferred from source.

`wow16/user/init.c::LibMain` contains the selected first USER desktop query
after NotifyWow. Original `ntuser/kernel/desktop.c` associates the thread's
CLIENTINFO with its actual desktop view: client pDeskInfo is the original
desktop pointer minus the mapped view's delta, and that same delta is
published to CLIENTINFO. `GetDesktopView`, `MapDesktop` and
`_MapDesktopObject` establish ownership and relocation. Native NT MM/process
objects cannot be copied into CCPU; the finite binding must preserve that
contract around the actual desktop/window owners and the S41 mapping.
Do not replace the guest query, invent a dummy desktop, silently return zero,
or treat InitTask (which is later) as the only publication entry point.
All current/future task switches and failure/retirement paths must publish
or withdraw the appropriate view, not retain one stale worker-global TEB.

`tests/observation/verify-wow-user-profile.ps1` now pins the full original
desktop traversal and the faulting dereference. With `-BootstrapFaultReport
O:/winnt/logs/t420-s42-startup-wait-r20/fault.log`, it verifies the exact
null-desktop/IVT/fault-address relationship and emits
`CONFIRMED_MISSING_DESKTOP_PUBLICATION`. That is explicitly a failing-runtime
attribution witness, **not** a functional pass. Both guest hashes remain
unchanged; temporary fault hooks were removed and dpmiint.c again has zero
diff against HEAD. The installed package was restored and hash-verified.
S42 remains open until the original client object graph and lifecycle are
actually bound and the real path and regressions pass.

### Original shared-object schema recovered for the production binding

The selected `opennt-host/windows/core/ntuser/inc/user.h` now also retains
the original LARGE_UNICODE_STRING, HEAD/SHROBJHEAD/THROBJHEAD, WND and
DESKTOPINFO declarations under an opt-in compile boundary. Four extracted
spans are byte-for-byte checked against source user.h SHA-256
`ADCF0AC89C4779D9B7908610976F4A47D2385403EDC37496C8AB004B016CF6C1`.
The non-mirror `wow_user_client_view_layout.h` provides only forward types,
the retained original WW header, original Win4 hook count and compile-time
assertions against pinned USER16 field offsets. Modern WH_MAX must not enlarge
DESKTOPINFO. The x86 fixture proves WND=176 and DESKTOPINFO=100 bytes; header
assertions cover self, tree links, rectangles, class, menu/id, function id,
styles and desktop WND pointer. The formal worker graph exposes
`wow-user-client-view-layout-test.exe`. This is restored data schema and
layout proof, not runtime desktop publication or W1/W2 acceptance.

The review also confirms that current `publish_handle` sets bType/wUniq
without replacing the initial free-list value in phead. Existing page-domain
tests explicitly expect that value, so their green result does not prove
usable guest WND publication. The full repair must change publication and its
test together: publish a complete original-layout object and its valid
relocated phead before making the handle typed, preserve identity and owner,
invalidate before retiring/reusing storage, and clear per-thread cached
window/client pointers. Merely writing a desktop handle into a small ad-hoc
buffer would hide the first fault while leaving this second dependency wrong.

### Client desktop context transport

The existing S41 worker domain now exports set/clear operations for an
already-mapped original USER desktop view. They publish the original
CLIENTINFO pDeskInfo/client-delta pair into the pinned guest TEB and first
invalidate its cached HWND/PWND pair. No desktop object, handle identity,
allocation or window policy is synthesized by this transport. The owner
must retain the source-backed view and serialize CCPU entry; withdrawal must
precede releasing its backing. Calls reject absent/wrong session domains,
non-paged execution and a different active CR3 without modifying the view.
Explicit clear is idempotent; null is not accepted as a successful set.

The production x86 worker links/exports both operations (see
`build/M0-T420/S42/w1-w2-page-projection-r1/s42-client-context-build.log`
and ntvdm.exe.map). The page-domain fixture exercises original desktop/window
pointer subtraction using explicitly synthetic test-only data, cache
invalidation, rejection, withdrawal and post-disposal rejection. Its
`WOW_PAGE_DOMAIN_LIFECYCLE_OK` proves this finite transport, not a production
desktop producer. The actual W1/W2 owner still must supply the live original
object graph before USER LibMain; until wired, the confirmed startup failure
remains unfixed and S42 cannot close.

The worker containing this transport has SHA-256
`1685A09BF5A33630628EB8FC82C393BA3E6C11FACE4D5B8FA1C8B862687A4299`.
With the unchanged r2 provider it passes the 17 text-gated DOS routes in
`O:/winnt/logs/s42-reentry-regression-r3-summary.json`; all Expected/Actual
values match. The final focused fixture additionally rejects a deliberately
different CR3 while preserving the previously published pair. Test-only
objects are not substituted into the real provider. The installed worker and
WOW32.DLL were restored and hash-verified after regression. No WRITE success,
production object-graph closure or clean-worktree claim follows from this.

### Incomplete handle publication no longer reports success

The native `wow_cleanup_window` is a finite cleanup view (child/next/class,
thread, procedure and server-procedure flag), not the original 176-byte WND.
`wow_window_words_binding` stores WW separately and uses that cleanup view as
its native handle companion. Neither pointer can simply become the guest
HANDLEENTRY phead. Doing so would expose an incompatible layout, even with
address translation. The full binding must retain one logical window owner,
its original field semantics and complete guest representation; it must not
reinterpret this small cleanup structure or introduce independent window
creation/destruction policy.

The legacy identity-only publisher now rejects active-guest publication
without touching the table; native-only fixture mode remains unchanged.
This removes the erroneous success that made free-list links 2/3 look like
live WND pointers. The actual `wow_user_window_publish` caller is checked
for ERROR_INVALID_STATE, rollback of its native entry and last-handle marker,
and an unchanged guest free entry. This is a temporary explicit missing
binding, not restored functionality, not a source-proven exclusion and not
an approved permanent limitation. It must be replaced by complete original
object publication before S42 closure. Earlier positive metadata-only
fixture results are superseded and must not be cited as WND acceptance.

The x86 worker rebuilt successfully, SHA-256
`2194C46438B78BAE6109E19BBDFD7E5BEC1B1F65D811E74E7A5F7A684845E564`,
and the updated page-domain fixture passes. Build output is
`build/M0-T420/S42/w1-w2-page-projection-r1/s42-incomplete-publication-build.log`.
The r3 17-route evidence above belongs to the preceding worker hash, not this
later safeguard image. This image was not deployed as the accepted package.

### Task cleanup failure and remaining exit binding

Current-source inspection finds no production callers of
`wow_user_task_lifecycle_cleanup` or `wow_user_task_lifecycle_dispose`.
`registered_cleanup` invokes the module/task WOWCleanup operation, which is
not the same as retiring the native thread/scheduler carrier. Original
WOW32 `W32DestroyTask` invokes WOWCleanup before host thread exit; original
USER `queue.c::xxxDestroyThreadInfo` owns the later message/thread/scheduler
destruction path, including DestroyTask. Do not wire scheduler destruction
directly into every WOWCleanup callback: `WU32FreeModule` also uses that
callback while the thread is still alive. Thread-exit and worker-disposal
binding remain explicit S42 obligations before adding backing lifetimes.

The existing helper also removed its task from the list and ignored failure
of native object cleanup before freeing the task. It now preserves the
enrolled task/context/event on failure, removes it only after cleanup
succeeds, and clears the temporary cleanup owner/releases the domain lock in
a finally block. The focused fixture creates and destroys a real hidden
native window, retains a deliberately stale test enrollment, verifies cleanup
failure and task survival, removes that test condition and retries successfully.
It reports `WOW_USER_TASK_LIFECYCLE errors=0` with production owner objects.

`tests/observation/verify-wow-task-lifecycle.ps1` records a reproducible runner:
pass the r4 provider root and page-projection-r1 worker root. It selects only
objects in the DLL link graph (not stale directory objects), rebuilds the
changed owner and fixture, and links the worker runtime/session plus original
RTL. The fixture entry uses ordinary C calling convention for session APIs;
provider owners retain their selected stdcall build. Output is under
`build/M0-T420/S42/task-cleanup-failure-r2`; fixture SHA-256
`A1EB2FB46206F45C3890CB48D42F53BBBA542ACE8C810DC53856185355A83381`.
The relinked candidate WOW32.DLL is
`9C5DFE53B30A263E7F61D1F01DFA17D8D1FE7638B549A4A26B17E713D182ECB2`.
This is helper failure/retry verification, not proof of production exit wiring,
guest object publication or WRITE acceptance. The two related native
window/dialog fixtures had stale two-argument lifecycle initialization calls;
they now use the current callbacks parameter. Earlier archived fixture passes
do not substitute for rebuilding/rerunning these current sources.

### Earlier provider import record

The S42 provider was rebuilt in
`build/M0-T420/S42/w1-w2-registration-r4` against the S41 page-projection
worker import library, rather than the older incremental worker library.  The
latter deliberately predates the three page-domain exports and therefore
cannot be used to judge the current composition.  The selected S41 parent
library exports exactly the worker-local ABI consumed by the provider:
`mvdm_softpc_wow_page_domain_guest_shared_info`,
`mvdm_softpc_wow_page_domain_publish_handle`, and
`mvdm_softpc_wow_page_domain_retire_handle`.

The fresh x86 provider linked successfully (SHA-256
`092239941389DEC0FD20A4DDAB3F0EE05ABC796438401EAAA1F60EBC0BCBC427`).
`verify-wow-output-composition.ps1 -RequireLocalRegistration` then proved the
local `_UserRegisterWowHandlers@8` symbol and all 20 non-null output-slot
assignments.  Its map also contains the selected original
`_wow_original_register_class@8` from `client.c`.  This is the required
cross-module proof that the completed S41 domain is available to the S42
registrar and object-publication path; it is intentionally not presented as
real-Win16 `InitTask` reachability. Its contemporary LDT-frontier hypothesis
was superseded by the ordinal correction and GDT/USER bootstrap observations
recorded above; it must not be reused as the current blocker.
