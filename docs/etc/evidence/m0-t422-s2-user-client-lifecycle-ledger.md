# T422 S2 USER client-view and lifecycle ledger

## Run traceability

Owner-directed prospective recording follows the
[S2 reproducible run evidence protocol](../operations/m0-t422-s2-run-evidence-protocol.md).
New verification runs require unique IDs, frozen source/artifact identities,
non-overwriting observations and separately stated facts/verdicts. Historical
E entries without complete replay inputs remain explicitly partial evidence;
this protocol does not retroactively certify them.

## Question

Can the selected original WOW32 USER/task/message package publish the
immutable USER16 client data it actually reads, enter and return through real
callbacks, and retire that data without adding a USER server or a second
worker-state owner?

## Baseline

S1 fixed the client/server numeric rule and the coupled ownership boundary.
This S starts at `52e330bdf`, with the selected original source and hashes in
the [coverage ledger](../operations/wow32-recovery-coverage-ledger.md) and
the S1 [audit closure](m0-t422-s1-wow32-audit-closure.md).  Guest media are
immutable.

## Initial production-path observation

The inherited worker page domain created guest SHAREDINFO, SERVERINFO, the
16-byte handle table and TEB, but had no desktop/WND/CLS backing.  S2 now
publishes that finite backing and binds class/window associations; the
remaining gap is real guest registration/query/mutation/callback execution,
not the absence of the client data objects themselves.  This is not evidence
that the original `wuwind.c`, `wcall16.c`, or `wkman.c` algorithms are wrong.

The required source-shaped representation is finite:

- `TEB.pDeskInfo` is client-linear `C`; `TEB.ulClientDelta` is nonzero `D`.
- DESKTOPINFO bounds, its desktop WND pointer, HANDLEENTRY `phead`, WND
  `head.pSelf`, and WND relationship links use server-form `S = C + D`.
- Guest consumers range-check `S`, subtract `D`, and then access only `C`
  through the existing CCPU page domain.  No host pointer is published.
- Native HWND/class associations stay in the existing worker-local bindings;
  their guest records carry fixed-width original-layout state only.

## Closure register

| ID | Original owner / production edge | Required S2 closure | Initial state |
| --- | --- | --- | --- |
| USER-VIEW-01 | `desktop.c`, `client.c`, direct USER16 reads | B1 desktop/client allocation, exact rebasing, atomic publish/withdraw and bounded address validation. | B1 implemented and x86 page-domain fixture passed; real USER16 direct-read acceptance remains. |
| USER-HANDLE-01 | `handtabl.c`, `wuwind.c` | Typed HANDLEENTRY/WND/CLS records, uniqueness and stale-reuse rejection, native association and retirement. | E56 wires guest backing retention to existing WW borrows. E57 verifies production nested native destruction, SEH unwind, failed detach/retry and allocation return with real CCPU memory. Real Win16-triggered nested destruction and whole-worker teardown remain open. |
| USER-DATA-01 | USER16 direct SERVERINFO/TEB readers | Cursor, clock, metrics/style/rectangle and host-originated refresh rules without a thunk-only refresh. | Cursor and clock producers are wired. E54 corrects screen-relative client rectangles and wires rectangle/style refresh before bound window callbacks, with real CCPU page-domain tests. General host-originated changes, metrics and guest direct-read acceptance remain open. |
| USER-CLASS-01 | `wuclass.c`, recovered client class slices | Original class registration/query/remove joins guest CLS publication and module cleanup. | Guest CLS allocation/publication/retirement is wired; original guest registration/query/remove and module-cleanup acceptance remain. |
| USER-WINDOW-01 | `wuwind.c`, `wmdisp32.c`, `wudlg.c` | Real guest create/query/change/destroy, callback-visible complete state and destruction inside callback. | E45--E50 prove diagnostic creation and normal destroy callbacks. E53 adds owner-confirmed WINMINE gameplay; E54 repeats creation/normal exit after geometry repair. Full query/mutation, reentrant destruction and sealed ordinary-profile acceptance remain pending. |
| USER-MESSAGE-01 | `wmsg*.c`, `wumsg.c` | Send/post/wait/cancel transport through original routes with receiver/task loss behavior. | E70 wires Get/Peek/Wait to original taskman; E71 proves cross-task native activation still waits without its needed handoff. E76 repairs the production receive callback data-lock boundary with red/green reentry and repeated diagnostic WINMINE lifecycle evidence. General synchronous-call, cancel and receiver-loss acceptance remains open. |
| USER-TASK-01 | `wkman.c`, `wuser.c` | Init/yield/wait/hung registration from a real task context and multiple-task rejection/recovery. | Pending |
| USER-TASK-EXIT-01 | `W32DestroyTask`, `WU32FreeModule` | Exactly one nonzero task cleanup before retirement; zero-task module cleanup cannot retire a live task. | E47 separates resource cleanup from thread retirement; E49/E50 prove diagnostic normal guest exit. E51 covers explicit detach rejection/retry. Original thread-window patch/destroy ordering and exceptional exit remain open. |
| USER-CALLBACK-01 | `wcall16.c`, `wcall32.c` | Real guest callback frame/return, reentry/cancellation and task-frame restoration. | E41 corrects procedure encoding; E44 restores scheduler lock entry. E56 wires nested CallbackWnd save/restore and delayed backing release, with CCPU primitive tests and diagnostic WINMINE normal exit. Guest nested destruction, cancellation and exceptional task-frame restoration remain open; the old 83B7 publication explanation is withdrawn. |
| IN-07/08/18/19/20; OUT-01/02/04--08/11/12/15--17 | `W32Init` / registration output | Wire every selected S2 slot at the owner boundary; prove each reached slot through its real guest owner. | OUT-17 is worker-local identity wired with publish/retire/reuse fixture; its real message-path invocation and the remaining slot acceptance are pending. |

## Implementation order

1. Add a bounded worker-local client-view object allocator/publisher and its
   withdrawal contract to the existing WOW page domain; it owns no native
   object or scheduler state.
2. Make existing class/window bindings publish and retire complete guest
   CLS/WND/HANDLEENTRY records around the original source calls.
3. Bind the actual original task-exit path to the exactly-once cleanup branch;
   do not compose two cleanup calls.
4. Prove the real guest paths (creation, direct read after mutation,
   callback/reentry, destruction/reuse, task/module/worker teardown), then
   execute fresh x86 and the established DOS regression suite.

## Withdrawn B1 hypothesis

The production page-domain initializer now creates a bounded original-layout
`DESKTOPINFO`/root `WND` pair before registration.  It publishes only after
the worker selects its own CR3: `pDeskInfo` receives the client address and
`ulClientDelta` the nonzero relocation; the root's `pSelf`, the desktop's
window pointer and HANDLEENTRY[1].`phead` retain server-form values.  The
existing `wow-page-domain-test.exe` crosses real CCPU paging and verifies
that guest rebasing resolves handle `00010001` without exposing a native
pointer.  It also caught and now guards the required ordering: client
publication before CR3 selection is rejected rather than silently accepted.

## B2 WND/HANDLE evidence

Each non-desktop published window now receives a separate worker guest `WND`
allocation. Its `head.h`, `head.pSelf`, rectangles and matching HANDLEENTRY
use the original fixed x86 layout; `phead` is the server-form guest address.
The native HWND remains only in the worker-local object association and is
consulted by the original `pfnGetFullUserHandle` output. Original-layout CLS
records are independently allocated in the same client/server address domain:
the atom, fnid, flags, WOW words, task, style, procedure and window-extra
fields are preserved; a native `HMODULE` is never copied into guest memory.
The x86 fixture proves publish, server-to-client rebase, original CLS
atom/fnid/flags/WOW/task/window-class field placement, retirement and reuse
of the handle-table entry, while the formal independent `wow32.dll` imports
the five page-domain publication/readiness operations from `ntvdm.exe`. It
does not claim direct USER16 mutation, callback reentry or a real Win16
workload yet.

Native-only fixtures remain diagnostic evidence, never S2 acceptance.

## E1 real WRITE bootstrap and registry edge

The first immutable `run16 system32\\WRITE.EXE` observation did reach
`UserRegisterWowHandlers`, `RegisterHung` and `InitTask`, then ended with
worker exit 1067 at the first `WK32WowIsKnownDLL` dispatch.  This was not a
guest failure and not a client-view pointer failure.  The selected original
retail `W32Init` had been changed to omit `WK32InitWowIsKnownDLL` with its
WOW registry open; `WK32WowIsKnownDLL` subsequently compared an input string
with uninitialized Control/Progman path globals.

The production repair restores the original initialization call and its
empty-value behavior.  It uses the existing per-worker `NTVDM.REG`, then
admitted system-snapshot, registry facade; it neither mutates the system
Registry nor exposes a native key to guest-facing code.  A missing WOW key
now supplies the original empty KnownDLL-list input while still constructing
the required path globals.  The provider imports the facade from the worker.
Only this completed `W32Init`/`WK32InitWowIsKnownDLL` reader family includes
the facade; the original Shell/OLE/FAX registry-writer families remain on
their later whole-family owner packets, rather than being partially redirected
by a global precompiled-header macro.

Fresh x86 worker and provider links passed.  On the deployed immutable
WRITE route, the worker remained alive past twelve seconds (the pre-repair
worker ended at roughly eight seconds), and the paired dispatch trace
continued beyond the prior `WK32WowIsKnownDLL` entry.  This proves only the
bootstrap edge; it does not close USER-WINDOW, USER-MESSAGE, USER-CALLBACK or
the real application acceptance rows above.

The same trace repeatedly reaches original `WG32CreateDIBitmap` and
`ThunkWMMsg16`, with the eventual original `W32DestroyTask` exit route.  The
observed `FFFF0002` return is the existing 16-bit GDI-handle representation,
not by itself proof that `CreateDIBitmap` failed.  It must therefore not be
misclassified as a GDI blocker or used to waive the S2 real USER lifecycle
requirements.

## E2 real WINMINE dispatch frontier

The deployed immutable `O:\winnt\WINMINE.EXE` route was observed for fifteen
seconds with the default-off registration and dispatch witnesses enabled, then
only the test-owned `run16`, `basesrv` and `ntvdm` processes were terminated.
The trace records a real nonzero WOW task (`039F`), successful registration,
Hung-handler registration and `InitTask`.  It also reaches real original
`WG32CreateDIBitmap` and `ThunkWMMsg16` bodies repeatedly.

The selected `wow32.dll` map resolves original `WU32CreateWindow` and
`WU32CreateWindowEx` to loaded addresses `71A66580` and `71A665A0` respectively.
Neither address occurs in the bounded dispatch trace, and neither the
`CreateWindowEx` nor `RegisterClass` output witness is recorded.  This is a
positive call-chain fact: the current immutable startup workload has not yet
entered the selected original window-creation thunk.  It is **not** evidence
that the registered creation carrier is disconnected, nor a basis for closing
USER-WINDOW-01.  The next S2 work must trace and complete the earlier real
USER startup/message/data edge that prevents the workload reaching this
original call, while preserving the original GDI result carrier and keeping
the GDI package's wider acceptance in its assigned later packet.

## E3 SERVERINFO direct-data producer

The pinned USER.EXE profile identifies direct reads of `SERVERINFO` metrics,
colours and cursor fields at `+170h`, `+2A0h`, `+304h` and `+308h`; the pinned
counts are 76 metrics and 25 colours.  The original `user.h` `SERVERINFO`
definition independently names the same arrays and fields.  The prior worker
carrier set only `cHandleEntries`, leaving every one of these pre-thunk reads
zero.

The worker now constructs the full finite scalar bootstrap snapshot before it
returns a SHAREDINFO address: `GetSystemMetrics(0..75)` and
`GetSysColor(0..24)` populate the original offsets, and `GetCursorPos`
populates the original point when an interactive cursor is available.  A
noninteractive host cursor-query failure retains the initialized `(0,0)` point
without invalidating a complete page domain; it is not an error in the
original USER layout.  This adds no guest pointer, server, scheduler or GDI
replacement.

The fresh x86 CCPU `wow-page-domain-test.exe` passes after asserting the
guest-linear `SM_CXSCREEN` and `COLOR_WINDOW` values at their pinned offsets;
the static client-view gate passes as well.  Dynamic settings/cursor refresh,
expected-version publication and real immutable direct-read acceptance remain
open S2 requirements.

## E4 expected-version task carrier

The original `queue.c::xxxInitTask` writes its `dwExpWinVer` input to both the
current `THREADINFO` and its `CLIENTINFO`.  The standalone task carrier already
supplied those native fields through the selected original body, but the
immutable USER16 PMODE32 profile also directly reads the current guest TEB at
`+50h`; the page-domain allocation had left that field permanently zero.

The worker now exports one bounded page-domain operation which writes the
already selected task value only while the owning worker thread has its active
CR3, and clears it on rejected task initialization and successful original WOW
cleanup.  It does not select a version, store a host pointer, or create a
second task owner.  The provider invokes it before the original init may return
to guest code.  The CCPU fixture proves a `030Ah` write and withdrawal at the
actual TEB offset; the static gate checks the production carrier.

The deployed fifteen-second immutable WINMINE observation
`m0-t422-s2-WINMINE-20260922-212314` still records successful registration,
Hung-handler setup and `InitTask` for task `039F`, with no worker crash.  It
does not reach `WU32CreateWindow[Ex]`, exactly as the prior frontier did.  This
therefore closes neither USER-WINDOW-01 nor the required direct-read acceptance;
it is a source-proven producer repair whose real-task non-regression is now
recorded.

## E5 original task exit reaches one worker-local retirement edge

Original `wkman.c::W32DestroyTask` calls `pfnWOWCleanup` before it removes its
WOW task record.  The standalone callback previously executed the recovered
original USER cleanup only.  That left the provider-local task, queue event and
message bridge alive after the original task had departed; a second, unused
provider cleanup entry attempted to repeat original cleanup later.

The registered callback now follows the original call's completion boundary:
it runs `wow_user_cleanup_bound` once with the original instance/selectors,
then retires only the standalone carrier via the recovered original
`DestroyTask` ordering, unbinds the runtime thread and releases its local
queue/message resources.  A failed original cleanup retains every carrier
reference.  If worker teardown has already withdrawn the page domain, the
best-effort TEB-field clear cannot retain a successfully cleaned task.  No
second USER cleanup is issued.  The former fixture-only direct-cleanup and
legacy cleanup entry points are removed, leaving this registered completion
edge as the only cleanup/retirement operation available to tests and product
code.

`verify-wow-task-lifecycle.ps1` rebuilt the selected production lifecycle
object and passed `WOW_USER_TASK_LIFECYCLE errors=0`; its no-PMODE32 parent
stub explicitly limits that fixture to native task retirement.  The dedicated
CCPU page-domain fixture remains the acceptance owner for the real TEB write.
The real `pfnWOWCleanup` event, real direct-read acceptance and the preceding
window/message frontier remain open S2 items.

## E6 WINMINE's current frontier is prior to USER output slots

The later deployed fifteen-second immutable WINMINE observation
`m0-t422-s2-WINMINE-20260922-213520` used the complete selected registration
output witness.  Registration, Hung-handler setup and `InitTask` succeed for
real task `039F`; none of `CreateWindowEx`, `RegisterClass`, `YieldTask`,
`WaitForMessage`, `DirectedYield` or `GetFullUserHandle` is called.  The
dispatch witness instead repeats original `WG32CreateDIBitmap` and
`ThunkWMMsg16` after that task becomes current.

This proves the immediate runtime frontier is a GDI/DIB path before the
selected USER creation/message output edge, not an unwired USER registration
slot.  The source-shaped GDI conversion family is S3's owner and cannot be
changed under S2 merely to force a USER observation.  It is an explicit S2 to
S3 dependency witness, not a S2 completion claim: S2 still owns real callback,
direct-read and normal-exit evidence once the GDI predecessor reaches them.

The original cause boundary is also exact: `wow32.c::W32DllInitialize` obtains
the NT GDI `GdiQueryTable` shared handle table, and `walias.c::GDI16` converts
the returned original NT handle encoding by index and `FullUnique`.  Modern
GDI handles do not establish that NT4 shared-table contract.  S3 must therefore
prove a bounded original-handle representation before it can regard a DIB
return such as `FFFF0002` as valid or invalid; S2 makes no GDI semantic change.

## E7 latest lifecycle carrier runtime non-regression

After the exact-once retirement correction, the selected x86 `wow32.dll` was
rebuilt and deployed into the disposable `O:\winnt` package.  An isolated,
immutable `WINMINE.EXE` launch ran for fifteen seconds with the registration
and dispatch witnesses enabled.  It recorded one successful
`UserRegisterWowHandlers`, `RegisterHung`, and `InitTask`, and the one worker
remained alive at collection time.  The run produced 616 dispatch records;
the test-owned `run16`, `basesrv`, and `ntvdm` processes were then terminated.

This is a regression guard for the changed task-exit carrier, not task-exit
acceptance: no `pfnWOWCleanup` event occurred during this bounded startup run,
and the workload again stopped before USER window/message output callbacks.
It confirms that replacing the provider-local retirement edge did not restore
the earlier bootstrap crash or create a new startup failure.

## E8 modern GDI shared-handle-table contract is unavailable

The selected original `walias.c` converts a guest GDI handle by taking its
14-bit index and reading `ENTRYWOW.FullUnique` from the pointer returned by
`GdiQueryTable`; it then constructs `index | (FullUnique << 16)`.  The x86
probe `verify-wow-gdi-handle-contract.ps1` creates a real compatible bitmap,
obtains the system export, and performs exactly that read under structured
exception handling.

On the current system, `GDI32.DLL` still exports `GdiQueryTable`, but it
returns `NULL`.  The created bitmap was `14053009`, whose original conversion
would publish guest `C024` and query table index `3009`; no readable table
entry exists.  Thus the old direct conversion contract is positively
unavailable, not merely undocumented.  S3 must replace this one unavailable
system-private table with a bounded worker-local original-handle carrier;
retaining the import or truncating a modern GDI handle cannot be correct.
The probe makes no product mutation and is retained as S3's baseline-negative
test rather than as a successful GDI acceptance test.

## E9 bounded replacement is linked into the real parent/provider pair

`verify-wow-gdi-alias.ps1` compiles the worker-local carrier and proves its
only public contract: zero maps to `NULL`; the same native handle keeps its
slot; a retired nonzero slot maps to `INVALID_HANDLE_VALUE` rather than a
screen-DC `NULL`; and the first subsequent native handle may reuse the same
14-bit slot exactly as the original table did.  It also proves the 16,383-slot
capacity failure (`ERROR_NOT_ENOUGH_MEMORY`).  The fixture deliberately does
not add a generation, native-type policy, or task ownership policy that a
Win16 alias cannot express.

The x86 formal parent now exports this worker-local ABI and links the three
carrier bodies directly into `ntvdm.exe`; its `Verify-VdmTibStorage` audit
passes.  The 77-body original WOW32 provider then links against that actual
parent import library.  The obsolete `GdiQueryTable` compatibility import is
absent from the provider graph.

The first observation was invalid as GDI evidence: the package loader selects
`O:\winnt\WOW32.DLL`, whereas that run had updated only the `system32` copy.
The corrected immutable WINMINE observation
`m0-t422-s2-gdi-carrier-20260922-2230` deployed the provider to the selected
root, reached successful registration and `InitTask`, and emitted 616
dispatch records without crashing.  Its generated-RVA witness maps to the
original `WG32GetStockObject`; successive real calls returned `0004`, `0008`,
through `0040`, proving that guest-visible GDI aliases are now allocated by
the carrier rather than by truncating modern handles.  The later frontier is
`WK32FileOpen` returning DOS error 2 and then `WU32SysErrorBox`, not the
retired GDI table.  This is producer-path and non-regression evidence only:
S2 remains open and S3 owns complete GDI/DIB acceptance.

## E10 current USER startup reaches an original network-profile default

The current `O:\winnt\SYSTEM.INI` contains only `386Enh`, `drivers` and
`mci` sections; it has no `[boot]` `network.drv` value. The selected immutable
USER16 `LW_InitNetInfo` body calls `GetPrivateProfileString("boot",
"NETWORK.DRV", "NETWORK.DRV", ..., "SYSTEM.INI")` and then attempts to
load the returned driver unless the resulting string is empty. Therefore the
observed `WK32FileOpen` sequence is source-explained: it is the original
default `NETWORK.DRV` searched through the current DOS/host search path, not a
GDI handle conversion, a missing WINMINE resource, or a changed guest binary.

The paired current parent/provider deployment was repeated on 2026-09-22
(`m0-t422-s2-gdi-live-20260922-233606`). Its hashes match the freshly linked
parent export/provider import pair, including the worker-local GDI alias
operations. Registration and `InitTask` again completed and the worker
remained alive for the fifteen-second observation. The trace resolves the
repeated `00022D30` target directly to original `WK32FileOpen` in the current
provider map; its `FFFF0002` result is the original DOS file-not-found return.
This rules out stale package deployment and the retired GDI shared-table
contract as explanations for this frontier.

The default-off file-open witness records the complete failed search and
returns original DOS error `2`; it then reaches the original `WU32SysErrorBox`
dispatch. This establishes a configuration/remaining network-family frontier,
not a justification to add a fake `NETWORK.DRV`, modify immutable guest media,
or make S2 silently suppress an original load failure. The selected profile
reader/writer and network-driver family must be reconciled with its whole
owner before a real window/callback acceptance run can be claimed. S2 retains
the proof as a dependency witness; it remains open.

## E11 original shared-data mutation has server-side producers

The pinned original USER server source makes the ownership boundary explicit:
`kernel/cursor.c::InternalSetCursorPos` commits `gptCursorAsync` to the shared
`gpsi->ptCursor` only after its original cursor clipping and input-desktop
selection; `kernel/sysmet.c` updates `gpsi->argbSystem` together with the
matching server brush; and `kernel/init.c` initializes the complete colour
array. The existing client-side WOW consumers read these shared fields directly
without a WOW32 thunk.

The current Console `MOUSE_EVENT_RECORD` is not that contract. It begins in
character-cell coordinates and is subsequently transformed through SoftPC's
mouse queue/INT 33 path. Writing it directly into `ptCursor`, or adding a
periodic `GetCursorPos` refresher, would invent an unproved coordinate,
ordering and synchronization policy and could race direct USER16 reads.

Consequently S2's bootstrap snapshot remains a correctly bounded initial
carrier, but it is not dynamic shared-data completion. The exact dynamic
producer/consumer boundary is now recorded: recover a finite source-shaped
publication route that accepts the already-transformed guest pointer state and
its desktop context, or prove an equivalent public producer with the same
coordinate, clipping, publication and withdrawal semantics. Importing the
whole USER server/input subsystem is prohibited by the stopping boundary.
This is an open S2 contract, not an excluded capability or a passing fixture.

The cursor sub-contract is now production-wired at that source-established
boundary: `nt_mouse.c::host_os_mouse_pointer` invokes the worker-local
publisher after `ScaleToWindowedVirtualCoordinates` and before
`AssembleCallMask`/guest mouse processing. The publisher rejects a non-active
or wrong-CR3 context and writes only the two pinned `SERVERINFO.ptCursor`
DWORDs. The fresh x86 `wow-page-domain-test.exe` passes both publication and
post-withdrawal rejection; the formal `ntvdm.exe` links the changed original
host object and passes `Verify-VdmTibStorage`. This closes only the cursor
producer seam. System-colour and metric change publication remain open.

## E12 original system-colour publication has a bounded worker carrier

The original USER server body `kernel/sysmet.c::SetSysColor` writes the exact
accepted RGB value to `gpsi->argbSystem[icol]` after the public colour request
has succeeded; its brush update remains server-local. The selected original
WOW32 `WU32SetSysColors` already owns the guest arrays and the public
`SetSysColors` invocation. `MVDM-HOST-DIV-275` therefore leaves that body and
its failure path intact, and only loops over the same accepted arrays after a
successful call. The worker page-domain carrier validates active ownership,
the current protected page directory and the original 25-slot `SERVERINFO`
range before it writes one fixed-width RGB entry. It does not retain a host
brush, alter system colour policy, or add a second USER server.

The x86 original WOW32 `wuser.c` translation unit compiles with the formal
provider flags. The changed worker page-domain and fixture translation units
also compile with the formal x86 flags; after re-archiving the existing fixture
binding inputs, the real `wow-page-domain-test.exe` links and reports
`WOW_PAGE_DOMAIN_LIFECYCLE_OK`. The fixture verifies a valid `COLOR_WINDOW`
publication, rejects index 25, preserves the previously published value after
rejection, and rejects a colour update after page-domain withdrawal.

The existing complete x86 object set was then used for the project-recorded
fallback: regenerate the formal parent/provider graphs, rebuild each changed
object and affected static library, then invoke their generated link commands
verbatim. `ntvdm.exe` and the 77-body `WOW32.DLL` both linked successfully.
`dumpbin` proves the parent exports, and the provider imports,
`mvdm_softpc_wow_page_domain_publish_system_color`. The normal Ninja launcher
was again blocked before spawning `cl.exe` by its recorded `.ninja_lock` host
fault; this is therefore a targeted formal composition check, not a claim that
the entire product was freshly rebuilt or deployed. Metric change producers
remain open.

## E13 original CallBack16 active-CCPU ordering

The previous isolated callback fixture called `CallBack16` before CCPU had
entered an outer simulation level. That was not a valid way to exercise the
selected original `host_simulate()` recursion: CCPU's `D6 FE` exit returns by
longjmp to the active simulation frame.

The replacement fixture enters the existing CCPU BIOS BOP table through a
guest `D6 05` instruction. Its test-only table entry invokes the selected
original `wcall16.c::CallBack16`; the nested simulation executes `D6 FE` and
returns to the original callback body, and the BOP then exits the outer CCPU
level by the same original mechanism. The x86 executable reports
`ORIGINAL_CALLBACK16_ACTIVE_CCPU_BOP_ORDER_OK`. It verifies that the bounded
caller/callback frame leases, CCPU recursion, original stack restoration and
callback-stack retention order work together while a real CCPU outer frame is
active.

This is deliberately not a fake WOW16 callback: `D6 FE` does not execute the
immutable `WOW16_From_CallBack16` trampoline and therefore supplies no
synthetic AX/DX return. Real guest callback/reentry/cancellation remains a
required USER-CALLBACK-01 acceptance item; this evidence closes only the
previously invalid host-side test gap.

## E14 complete application set reaches the same immutable profile frontier

The paired current `ntvdm.exe` / `WOW32.DLL` package was launched separately
through ordinary `run16.exe` for `WRITE.EXE` and `SOL.EXE` on 2026-09-22. Both
workers completed `UserRegisterWowHandlers`, `RegisterHung` and `InitTask`.
Each then made 56 calls to the map-proven original `WK32FileOpen@4`
(`WOW32.DLL` RVA `00022D30`), including 42 original `FFFF0002` file-not-found
returns, and dispatched the original `WU32SysErrorBox@4` afterward. The
records are `O:\winnt\logs\m0-t422-s2-profile-frontier-20260922-234356-*.log`.

Together with the earlier current WINMINE run, this proves the missing
`[boot] network.drv` profile/driver condition is a common USER16
initialization predecessor for all three selected final applications, not an
application-specific GDI, resource or task-lifecycle regression. It is
missing immutable package media/configuration, not a demonstrated original
guest-code defect: the source-policy disposition for an original guest bug
does not apply. The project must neither edit `SYSTEM.INI`, invent a fake
`NETWORK.DRV`, nor change the original caller's default/failure result. A
source-policy-compliant package/profile admission is required before real
guest USER create/message/callback completion can be asserted.

The pinned source inventory was also searched beyond the deployed package.
It contains three byte-identical copies of the more appropriate original
`WFWNET.DRV` (15,744 bytes, SHA-256
`4C321D43511F845EC1A95518507133D1CF4CF3993BAE422037DBA95CE6258CCA`): the
OpenNT and OpenNT-4.5 `mvdm/wow16/wfwnet` product paths and the
`opennt-src-2/binaries/nt` release artifact.  The original installation
inventory explicitly writes `network.drv=wfwnet.drv`.  Its full original
source is adjacent to the binary in `mvdm/wow16/wfwnet`; its makefile requires
the period 16-bit `cl16`, `link16` and `rc16` toolchain, which is not installed
on this host.  Therefore the byte-identified original binary, not a rebuilt
or renamed substitute, is the correct candidate for a controlled deployment
test.  `NETWARE.DRV` remains a separate NetWare-stack-specific candidate and
is not used for this test.

The selected immutable `user.asm::InitApp` calls `LW_InitNetInfo` for every
first USER16 application, immediately after its queue and task signal setup;
there is no application-specific bypass. `LW_InitNetInfo` itself explicitly
returns without loading a driver only when the profile entry is *present and
blank*. The currently absent entry therefore cannot be treated as equivalent
to the original no-network configuration. This proves why a different final
application cannot provide a legitimate S2 real-guest bypass.

The live `product-package` contract confirms the boundary: it derives exactly
one executable-relative `mvdm_system_root`, and its only configuration
validation is for `CONFIG.NT`, `AUTOEXEC.NT` and `NTIO.SYS`. Neither the
worker session nor the package layout has a separate profile root, a
`SYSTEM.INI` merge layer, or a guest-profile override. Consequently no
existing product facility can select a replacement profile without changing
the immutable package. Adding such a facility would be a material new
guest-media/profile policy and requires a separately admitted design; it is
not an implementation detail S2 may silently introduce.

## E14 controlled SYSTEM.INI A/B admission

On 2026-09-23 the owner explicitly authorized a narrow, reversible exception
to the guest-media rule solely to establish the original USER16 profile
branch.  `O:\winnt\SYSTEM.INI` was backed up to
`O:\winnt\logs\m0-t422-s2-systemini-20260923-064551.original.ini`
(SHA-256 `6F533CCC79227E38F18BFC63BFC961EF4D3EE0E2BF33DD097CCF3548A12B743B`)
and was given a semicolon-explained `[boot]` `network.drv=` entry.  The exact
original `WFWNET.DRV` binary was separately staged at `O:\winnt\WFWNET.DRV`;
it is deliberately not enabled in the blank-value run.

The first run correctly falsified an assumption in the product-package model:
the selected original `GetPrivateProfileString(..., "SYSTEM.INI")` reaches the
host Win32 profile resolver, which maps that relative system profile name to
the host `%SystemRoot%\SYSTEM.INI`, not the executable-relative `O:\winnt` file.
The real run `m0-t422-s2-no-driver-20260923-064628` consequently still records
the original `NETWORK.DRV` search and error 2.  A process-local `WINDIR`
override also leaves the system-profile lookup unchanged.  This is a precise
host integration boundary, not evidence that an empty `network.drv` value is
ignored by original USER16.

The actual host `%SystemRoot%\SYSTEM.INI` is protected by its ACL. The approved
write was attempted with the same backup-and-comment protocol and was denied;
this work does not take ownership or alter ACLs.  The no-driver branch and
the `WFWNET.DRV` enabled branch therefore remain open real-guest acceptance
tests pending a user/admin-performed edit of that one host profile file.  No
registry, guest binary or product source was changed by this experiment.
The no-effect `O:\winnt\SYSTEM.INI` change was then restored byte-for-byte to
the recorded SHA-256 baseline.  The repeatable
`tests/observation/invoke-wow-user-network-profile-a-b.ps1` script now owns
the two real-profile runs: it requires an administrator token, backs up the
actual host profile, accepts only `Blank` or hash-pinned original `Wfwnet`,
collects the normal runtime witnesses, and has an explicit restore operation.
An attempted UAC launch of that script from the noninteractive worker also
did not obtain an administrator token: host `%SystemRoot%\SYSTEM.INI` remained at
the original hash, no new profile-run logs were created, and no runtime
process remained.  This is recorded as an environmental authorization
boundary, not as a failed USER16 no-driver test.

## E15 current lifecycle fixture uses the selected worker objects

`verify-wow-task-lifecycle.ps1` had retained a stale assumption that a
provider-only build had already produced `wow-worker-bindings.lib`. It now
builds the five selected worker objects directly under the x86 tool
environment, verifies their actual paths, and links them with the same CCPU
fixture-support object and complete selected library set used by the
page-domain fixture. The link explicitly rejects the accidental `ntvdm.lib`
default import, so the fixture's local session cannot be rebound to a
separately loaded worker.

The repaired current run
`build/M0-T422/S2/task-lifecycle-r10/fixture.exe` reports
`WOW_USER_TASK_LIFECYCLE errors=0`. It proves the existing native task
registration/init/yield/stale-window failure/cleanup ordering fixture against
current production-selected objects. It remains native-only evidence and does
not close the required immutable Win16 guest lifecycle acceptance.

## E16 package-rooted SYSTEM.INI recovery

E14 correctly established the modern-host divergence: a bare `SYSTEM.INI`
argument can reach host `%SystemRoot%\SYSTEM.INI`, which is not the standalone
worker's selected system root and must not be edited.  The first proposed
repair was incomplete: source review and the controlled A/B proved that the
first USER16 startup read does **not** enter `WK32GetPrivateProfileString`.
The immutable original `KRNL386` body
`up.c::IGetPrivateProfileString -> SetPrivateProInfo` handles that read before
WOW32, and its original `ldboot.asm::get_windir` selects `WIN16DIR` from the
environment inherited by the VDM child.

The correct binding is therefore at the existing original parent boundary:
`run16` takes the already selected package root from its own installed sibling
directory and adds `WIN16DIR` only to the `BaseCreateVDMEnvironment` MULTI_SZ
which is passed to the new `ntvdm` child.  It never changes the launcher's
process environment, never changes host `SYSTEMROOT`, and contains no fixed
drive/path literal.  Host `SYSTEMROOT` remains the loader identity required by
modern Win32.  This preserves the original child-environment mechanism rather
than replacing the guest profile reader.

`MVDM-HOST-DIV-297` remains narrower: its exact case-insensitive bare
`SYSTEM.INI` rewrite applies only to the three original **WOW32 private-profile
thunks** when those thunks are actually called.  It is not evidence for, nor a
substitute for, USER16 bootstrap routing.  It preserves arbitrary caller paths
and changes neither USER.EXE nor host configuration.

The controlled A/B acceptance writes only the owner-authorized selected
package `SYSTEM.INI`, makes the selected `[boot] network.drv` line explicit
with a semicolon comment, and restores the byte-identical baseline after each
run.  Its two required branches are blank `network.drv=` and the hash-pinned
original `network.drv=wfwnet.drv` with the original WFWNET.DRV staged next to
the worker.  This replaces the retired elevated host-profile experiment.

## E17 host-directory call-site classification

The selected product contains eleven logical host-directory consumers.
They are intentionally not all redirected: directory identity belongs to the
caller, not to a global shim.

| Owner and call | Selected directory | Decision |
| --- | --- | --- |
| WOW32 provider initialization (`wow32.c`) | Worker package root and package `system32` | Replaced by `GetNtvdmWindowsDirectoryA` / `GetNtvdmSystemDirectoryA`. These are the directories published to the Win16 guest. |
| WOW private-profile thunks (`wkernel.c`) | Package `SYSTEM.INI` | Exact bare filename only, via `mvdm_softpc_system_find_file`; arbitrary caller paths remain host API paths. This row does not own the earlier KRNL386 USER16 bootstrap profile read. |
| COMMAND bootstrap, KEYB and redirection fallback (`cmd.c`, `cmdkeyb.c`, `cmdredir.c`) | Package `system32` or root | Replaced by the matching `GetNtvdm*DirectoryA` query. These callers locate VDM media and temporary fallback files, not host Windows components. |
| PIF default discovery (`nt_pif.c`) | Package root | Replaced by `GetNtvdmWindowsDirectoryA`; `_default.pif` is VDM product media. |
| OEM/Unicode MVDM thunk (`oemuni/process.c`) | Package root and `system32` | Replaced by wide `GetNtvdm*DirectoryW` forms before the existing OEM conversion. The original conversion and buffer contract remain the owner. |
| Full-screen EGA.CPI lookup (`nt_fulsc.c`) | Package `system32` | Replaced by `GetNtvdmSystemDirectoryA`; it is VDM display media, not the modern Console font directory. |
| WOW public USER facade font search | Actual host Windows fonts | Retained `GetWindowsDirectoryW`: it opens host GDI font resources for modern presentation and must not publish that path to the guest. |
| NetAPI/XACTSRV workstation service | Actual host system directory | Retained `GetSystemDirectory`: this is host LAN service state, outside the worker media package. |
| Fax, test-shell and other unselected/guest-only sources | Not in current worker product | No production replacement. Guest binaries remain immutable and unselected host utilities are not silently redirected. |

The two new query families are deliberately narrow worker bindings, not
replacement imports for `kernel32`: they make every product-media use explicit
in the mirror diff while leaving genuine host-service use visible and intact.

## E18 package-media completion for redirected directories

The directory audit exposed one actual missing original file: the original
`nt_fulsc.c::LoadCPIFont` lookup now correctly resolves
`O:\winnt\system32\EGA.CPI`.  The file was absent from that selected package
directory, so the original byte-identical `opennt-src-2/binaries/nt/ega.cpi`
was copied there without alteration: 117,433 bytes, SHA-256
`30F809477737650B3005EA1EFD3BBD44DE439891036DB000372F63A6C0B1FE7A`.

The current required DOS staging manifest and the selected WOW16 runtime
closure were then checked path-by-path.  All other currently selected members
already exist at their package-root or `system32` paths: NTIO, NTDOS, COMMAND,
HIMEM, DOSX, KRNL386, USER, GDI, WOWEXEC, COMMDLG, OLECLI and the selected
display/input drivers.  The separately selected original WFWNET.DRV also
remains staged next to the worker for the pending profile-enabled branch. No
guest file was rebuilt, patched, or replaced with a non-original artifact.

## E19 child environment proof and no-fixed-root invariant

The immutable deployed `KRNL386.EXE` was read without modification and
contains both `SYSTEMROOT=` and `WIN16DIR=` literals, matching the selected
original `ldboot.asm::get_windir` source.  This rules out an accidental
ordinary Win3.x kernel which lacks the OpenNT startup convention.

The `run16` binding now derives its package root from its own executable path,
builds a separate UTF-16 environment MULTI_SZ, replaces only `WIN16DIR=`, and
passes that block through the original `BaseCreateVDMEnvironment`.  It does
not call `SetEnvironmentVariable`, does not alter `SYSTEMROOT`, and contains
no deployed drive/path literal.  The resulting ANSI command record and the
Unicode environment supplied to the worker therefore share the same derived
value.  A one-run, subsequently removed diagnostic at the original
`GetWowKernelCmdLine` entry recorded `worker-win16dir 1`; source review of
the immediately following original `cmdCreateVDMEnvironment` proves that it
preserves `WIN16DIR` (it filters `WINDIR`, not `WIN16DIR`) while projecting the
worker environment to the guest PDB.

This closes the environment-delivery question.  E20 below separately closes
the later WOW32 private-profile mapping edge without editing a host profile or
embedding a package path.

## E20 SYSTEM.INI mapping boundary and worker-local shadow

A controlled host API probe passed the selected package profile by its normal
path, by an explicitly normalized path, and by an extended-length path.  All
three calls to `GetPrivateProfileString` returned the `NETWORK.DRV` default
rather than the explicitly blank package value.  Modern Windows therefore
maps the special basename independently of spelling; an absolute package path
is not a safe repair.  A removed one-run WOW32 observation independently
proved that the original caller reaches this boundary with the bare
`SYSTEM.INI` name and that the worker package root is already correctly
derived.

The adapter now resolves the immutable profile from the current worker
session, copies it to a dynamically named temporary file whose basename is
not `SYSTEM.INI`, and keeps that file delete-on-close for the session lifetime.
The original WOW32 thunks redirect only their exact bare system-profile call to
that shadow; all other paths retain the original host API.  The implementation
contains no deployed drive or package-root literal.  Final current-package
acceptance passed both the blank `network.drv=` and hash-pinned original
`network.drv=wfwnet.drv` branches, restored the selected package profile to
SHA-256 `6F533CCC79227E38F18BFC63BFC961EF4D3EE0E2BF33DD097CCF3548A12B743B`,
and found no newly leaked `nvm*.tmp` shadow after worker teardown.

## E21 original class query reaches real native window creation

The first package-profile run after E20 reached original
`WU32RegisterClass` and `W32CreateWindow`, but no `CreateWindowEx` output-slot
witness appeared.  This was initially misread as a failed registration.  The
registered atom was nonzero; the actual source-shaped chain was
`W32CreateWindow -> FindClass16 -> GetClassWOWWords`, and the latter returned
null before the output slot could run.

Opt-in, default-off witnesses established the exact state: the same
worker-local `wow_class_lookup_context` was used by registration and query;
after registration its private list contained `WOWExecClass`; the later query
had the same class name and live runtime class context, but an intervening
nested borrow scope inherited a null class field from its parent.  It thereby
masked the active runtime context.  This was a standalone binding bug, not a
guest class, module-handle, CCPU, or host-path failure.

`wow_user_borrow_enter` now inherits a parent class view only when it is
non-null; otherwise it binds the already-active worker runtime class view.
It does not allocate a second class list, publish a host pointer to guest
memory, or alter the original `FindClassAtom`, `GetClassPtr`, class ownership,
or module matching bodies.  The final immutable `WINMINE.EXE` witness recorded
the original matched class records (`WOWExecClass`, then `WOWFaxClass`) and
nonzero `CreateWindowEx` output results.  The controlled blank and original
`WFWNET.DRV` profile branches both passed:
`T422_S2_NETWORK_PROFILE_EMPTY_OK`,
`T422_S2_NETWORK_PROFILE_WFWNET_OK`, and
`T422_S2_WOW_USER_PACKAGE_NETWORK_PROFILE_OK`.  The selected package
`SYSTEM.INI` was again restored to SHA-256
`6F533CCC79227E38F18BFC63BFC961EF4D3EE0E2BF33DD097CCF3548A12B743B`.

This proves real class registration, original class query and native window
gateway entry.  It does not yet prove callback dispatch, guest direct-data
mutation, message delivery, destruction, or task/worker teardown; those
remain S2 acceptance obligations.

## E22 original guest window callback returns successfully

The native window gateway is not accepted merely because public USER32 made a
window.  A default-off witness at the return from the unchanged original
`wmdisp32.c::W32Win16WndProcEx` records the original `CallBack16` Boolean,
the selected 16-bit procedure and its returned `lReturn`, before the original
body chooses its normal return or `DefWindowProc` fallback.  It does not
change thunking, callback frame ownership, error policy, or message order.

The repeated immutable `WINMINE.EXE` package run passed both controlled
network-profile branches and recorded successful (`success=1`) original
`CallBack16` returns for `WM_NCCREATE` (`0081`) and `WM_CREATE` (`0001`), with
nonzero 16-bit procedure addresses.  The observation test now requires those
two witnesses in addition to class registration/query/window creation; a
missing callback can no longer be reported as a passing window gateway.

This closes the creation-message callback reachability evidence only.  It
does not prove ordinary input/message-loop dispatch, explicit window
destruction, module cleanup, or worker-loss cleanup, which remain required
S2 lifecycle rows.

## E23 current formal worker task-retirement regression

The task-lifecycle verifier had no explicit post-link existence check, and its
success output was not reliable enough to distinguish an unlinked fixture from
a passing run.  It now rejects a missing `fixture.exe` before invocation and
has an explicit success marker.  The current formal graph required two
fixture-only archives which had not yet been materialized in this build root;
they were generated by their named Ninja targets, not copied from a historical
build.

The resulting current `build/M0-T422/S2/task-lifecycle/fixture.exe` was run
directly and reported `WOW_USER_TASK_LIFECYCLE errors=0` with exit code zero.
It exercises the selected production lifecycle object and the worker-local
task registration/init/yield/stale-window failure/cleanup ordering.  This is
still native fixture evidence, not a substitute for an immutable Win16
guest's ordinary window destruction or worker-loss cleanup acceptance.

## E24 full-handle consumer reachability is not yet present in selected starts

`registered_full_handle` was already instrumented through the default-off
registration witness.  Fresh eight-second immutable starts of `WINMINE.EXE`,
`WRITE.EXE` and `SOL.EXE` each recorded successful
`UserRegisterWowHandlers`, `RegisterHung` and `InitTask`, but none recorded
`GetFullUserHandle`.  Thus the selected normal startup workloads do not reach
one of the original `FULLHWND32` consumers yet.  This is a reachability fact,
not an identity failure: the worker-local publish/retire/reuse fixture
continues to prove the implemented lookup contract.

OUT-17 remains non-passing for real-guest acceptance until an original
message/DDE/dialog route that uses `FULLHWND32` is reached and observed.
No fallback HWND conversion, fake handle, or guest modification is admitted
to manufacture that coverage.

## E25 real ordinary-message callback, and the remaining live-window selection boundary

The existing controlled package-profile observation temporarily writes an
explicit empty `network.drv=` under `[boot]`, then restores the supplied
`SYSTEM.INI` byte-for-byte.  This is test configuration only: it is not a
guest-binary change and the worker receives its package root through its
derived `WIN16DIR`, not a fixed drive/path literal.  Without that explicit
empty value the original profile takes the earlier `NETWORK.DRV`/GDI path and
does not reach the USER creation work in this packet.

With the current x86 provider, the `CreateWindowEx` witness names two actual
same-worker USER32 windows.  A direct `IsWindow`/visibility/parent query
proved that `WOWExecClass` is a visible top-level window.  A class-name-based
top-level enumeration does not reliably return this registration form, so
future external stimulus must select a currently valid handle from the
worker's observed window generation, rather than assuming that the first
creation-trace handle remains live.

The new default-off native-entry witness then recorded the ordinary original
callback gateway beyond creation: `WM_WINDOWPOSCHANGING` (`0046`), `WM_MOVE`
(`0003`), `WM_SIZE` (`0005`), `WM_WINDOWPOSCHANGED` (`0047`),
`WM_NCUAHDRAWCAPTION` (`00AE`) and `WM_SETTEXT` (`000C`) all entered the native
gateway and returned `success=1` from unchanged
`W32Win16WndProcEx -> CallBack16`.  This is real immutable-WinMine message
evidence, not a fixture result.

A first synchronous `WM_CLOSE` attempt selected a now-stale creation handle
and correctly failed with `ERROR_INVALID_WINDOW_HANDLE`; it is neither a
callback failure nor a destruction pass.  The remaining S2 destruction test
must choose and revalidate a live current window, then assert callback,
`WM_NCDESTROY` detachment, task retirement and worker cleanup separately.

## E26 formal parent/provider composition is a runtime prerequisite

`WOW32.DLL` is a separately loaded provider, but it imports the worker-local
package, profile, shadow-registry, GDI-alias and page-domain bindings from
`ntvdm.exe`.  A provider built against an older `ntvdm.lib` may compile yet
cannot be treated as the current runtime: it does not describe the complete
worker import surface.  The current formal x86 build first links
`build/M0-T422/S2/formal-x86-r1/ntvdm.exe` and its generated import library,
then links `build/M0-T422/S2/wow32-provider-r8/wow32.dll` against that exact
library.  The latter link succeeded only after the parent had been rebuilt;
this is a reproducible composition dependency, not a new provider or a
runtime workaround.

The matching five product artifacts plus the provider were copied to an
explicit test package root.  The existing two-branch immutable WINMINE
observation passed again:
`T422_S2_NETWORK_PROFILE_EMPTY_OK`,
`T422_S2_NETWORK_PROFILE_WFWNET_OK`, and
`T422_S2_WOW_USER_PACKAGE_NETWORK_PROFILE_OK`.  A separate 20-second
controlled natural run repeatedly reached and returned from the original WOW
BOP at `021f:aebc` with `CR0=80000011` and `CR3=00900000`; it did not take
`TerminateVDM` or reproduce the prior `0xC000041D` worker exit.  The script
restored `SYSTEM.INI` byte-identically to
`6F533CCC79227E38F18BFC63BFC961EF4D3EE0E2BF33DD097CCF3548A12B743B`.

Neither the worker nor the provider embeds `O:\winnt`: the package root is
derived at runtime and the external root above is a test/deployment argument
only.  This removes the stale-parent/provider mismatch from the current
callback investigation, but it does not close destruction, full-handle
consumer reachability, task retirement or WRITE acceptance.

## E27 original 16-bit WNDPROC tag is required, but exposes the next callback gap

The native USER gateway keeps `wow_window_native_proc` installed as the actual
32-bit `GWL_WNDPROC`; that was independently observed as a stable native
address.  The logical procedure retained with a WOW class must nevertheless
carry the original `WNDPROC_WOW` protocol bit.  `wuclass.c` sets that bit when
registering a 16-bit class and applies the paired LDT-bit transformation for a
selector whose own high bit is set.  The standalone class-publication binding
had omitted this step for an untagged logical far procedure.  Consequently,
the unchanged original `DispatchClientMessage` took its native-WNDPROC branch
and attempted to call the guest far address as a host function.

The binding now reproduces the `wuclass.c` encoding before passing the target
to the unchanged original dispatcher.  A controlled immutable `WINMINE.EXE`
run then recorded `W32Win16WndProcEx` entry for `WM_GETMINMAXINFO` (`0024`) at
the restored encoded procedure `83B70B98`; this proves the native-host direct
call has been removed.  That callback did not return, so no real window was
created for destruction testing.  This is not a passing result: it identifies
the next required boundary as the original `WM_GETMINMAXINFO` thunk and/or
`CallBack16` guest-frame path, which must be repaired with source evidence.

The test killed only its own product processes and restored `SYSTEM.INI`
byte-for-byte to SHA-256
`6F533CCC79227E38F18BFC63BFC961EF4D3EE0E2BF33DD097CCF3548A12B743B`.
`O:\winnt` remains solely the external test root; no product source contains
that literal.

## E28 superseded: virtual-selector provider is not the `83B7` callback owner

**Superseded by E35.** The E27 `WM_GETMINMAXINFO` callback enters the original CCPU far-call route
with procedure `83B7:0B98`, then does not return.  The original WOW class
encoding is correct: `wuclass.c` uses `WNDPROC_WOW` bit 31 as a tag and moves
the LDT bit through bit 18 while the tagged procedure is stored; its callback
path restores `83B7:0B98`.  The failure is therefore not a truncation of the
procedure, a native `WNDPROC` call, or an arbitrary selector rewrite.

The CCPU witness observes selector `83B7`, but its current descriptor source
and shadow do not contain the virtual-selector record required for that far
call.  The selected original provider for this record is also explicit:
`base/bios/virtual.c::virtual_device_trap` handles BOP `B1`, and its original
`VxD_Device_Init` case calls `sas_init_pm_selectors(BX,CX)`.  That source-
shaped call reaches `ccpusas4.c::c_SasRegisterVirtualSelectors`, which writes
the paired code/data entries to the active CCPU LDT.  Default-off observation
of the failing immutable WINMINE run produced no BOP `B1` hit at all.  Hence
the required original provider was never invoked; no host-side replacement
has been added.

The controlled destruction observation now deletes its B1 trace before every
run and reports the fresh count in its failure.  The fresh formal-x86 run on
2026-09-23 ended with `B1 VxD controls observed: 0`; its `finally` branch
terminated only the test product processes and restored `SYSTEM.INI` by its
pre-run SHA-256.  This makes the absence a current-run fact rather than an
inference from a potentially stale log.

The only checked-in nearby binary, MS-DOS 6 `WINA20.386`, is not a substitute.
Its original `mscode.asm` loads it only for Windows versions below 3.10 and
explicitly skips that VxD for Windows 3.1; it is an A20/paging compatibility
driver, not SoftPC's `INSIGNIA.386`.  No `INSIGNIA.386` or `spcmswd.drv`
artifact or source was found in the pinned OpenNT, OpenNT-4.5,
opennt-src-2, MS-DOS, or sibling SoftPC trees.  It would be incorrect to copy
or rename `WINA20.386` into the selected immutable package merely to obtain a
test result.

This historical investigation did not establish the current S2 blocker:
recovering the original
`INSIGNIA.386`/matching `spcmswd.drv` guest-provider path with provenance, or
prove a distinct, already selected original caller that initializes the same
two CCPU virtual selectors before the first WOW callback.  A direct
standalone call to `sas_init_pm_selectors`, a fabricated guest driver, or a
new selector state machine would bypass the original ownership boundary and
is not admitted.  The package root remains executable-relative; `O:\winnt`
is only the external deployment and observation root.

## E29 superseded: B1 contract reconstruction is unrelated to `83B7`

**Superseded by E35.** The original source narrows E28 further.  `virtual_device_trap` does not
allocate either selector: in `VxD_Device_Init` it accepts an already allocated
data selector in `BX` and an already allocated code selector in `CX`, records
the external driver's version from `DX`, and invokes the original
`sas_init_pm_selectors(BX, CX)`.  The unchanged CCPU40 body then writes only
the two source-defined descriptors: a flat writable data entry and a
base-`F0000` code entry used by `biosDoInst`.  That code entry is *not* the
observed `83B7:0B98` callback target: E35 establishes that target as ordinary
WOW module code.  The contrary wording in this superseded entry is retained
only as a record of the rejected hypothesis.

The original CCPU comment is explicit about this ownership: the protected-mode
code/data pair is made available by the Insignia host Windows driver, and the
implementation expected initialization/termination to move to the Insignia
VxD.  This explains the contrast with an x86 NT V86 worker: its paired
Windows/Insignia driver supplied the allocated pair and sent BOP `B1`; CCPU40
does not receive either event merely by emulating the same guest instructions.

The only adjacent compatibility helper,
`set_virtual_selectors_from_mswdvr()`, is not an alternate host bootstrap.  It
is an original old-driver ABI entry: when `insignia_386_version < 1`, it takes
the already allocated pair from guest `CX`/`DX` and calls the same original
registration body.  A complete source-tree search finds its definition but no
selected caller.  With neither `spcmswd.drv` nor `INSIGNIA.386` present, there
is no valid guest register source to feed it.

The current failing run provides a complementary execution witness.  The
physical LDT store at `83B7` contains `83B8`, the next-free selector link,
rather than either descriptor word; the regular `BOP 53:00` publisher reaches
only ordinary aligned selectors (maximum `1820`) and never `83B*`.  Thus the
missing record was neither published and lost by CCPU nor rejected by the
original DPMI alignment check.  It was never supplied by the absent VxD
provider.

This formerly proposed VxD/BOP replacement is rejected by E35.  It must not
be used for `83B7`; the open S2 work is instead to identify the original WOW
module loader/DPMI publication edge that makes this ordinary LDT selector
executable in the CCPU carrier.

## E30 superseded: FastBOP is the prerequisite of the WOW_x86 selector path

The original WOW boot path invokes DPMI BOP `GetFastBopAddress`, then uses its
nonzero result as the gate for `WOWGetFastAddress`,
`WOWGetFastCbRetAddress`, and (under PMODE32) `WowGetFlatAddressArray`.
`WOWFastBopInit` takes its `NoFastWow` branch before all three WOW calls when
the BOP result is zero.

The formal CPU40 composition currently selects the original RISC fallback
`dpmi32/dpmimscr.c::DpmiGetFastBopEntry`, whose body returns `BX=DX=ES=0`.
The fresh controlled WINMINE trace records those exact registers, and the
worker-owned `WowGetFlatAddressArray` witness has no hit.  Consequently the
retail `WOW_x86` `DPMIProc/WowSetSelector` fast path never receives the flat
selector-base table and never emits its original `INT 2Ah`,
`EAX=EBP=F0F0F0F1`, `NtSetLdtEntries`-shaped publication for `83B7`.

This does **not** revive BOP B1 or `INSIGNIA.386`: B1 remains a separate BIOS
virtual-device selector-pair mechanism and is absent from the same trace.
The pending repair is to supply the original x86 FastBOP carrier semantics to
CPU40, then retain the existing bounded INT 2Ah LDT binding.  It must neither
invent a selector nor publish `83B7` directly.

## E31 superseded: B1 selector-pair experiment is not a `83B7` repair

**Superseded by E35.** E29's remaining implementation assumption was tested without turning it into
product behavior.  A default-off, test-only observer accepted the already
observed dynamic pair `83AF`/`83B7` only when supplied by the test environment,
then invoked the unchanged original `c_SasRegisterVirtualSelectors`.  It did
not select a slot, scan the LDT, alter guest media, or persist a descriptor.

The fresh x86 run proves only the narrow effect: the callback's `83B7:0B98`
far call changed from an invalid/free LDT entry to the original CCPU code
descriptor (`base=F0000`, `limit=FFFFFFFF`, `AR=8F9F`), then repeatedly
entered the existing protected-mode debug exception rather than producing a
live USER window.  An earlier observation incorrectly read physical ROM
`F000:0B98` and treated zero bytes there as proof of the CCPU instruction
stream.  That inference is invalid: the callback has paging enabled, so the
descriptor's `F0000 + 0B98` is a *linear* address and must be translated
through the active CR3 page tables before inspecting its backing page.

The selected immutable `bios1.rom` being zero at physical offset `0B98` is
therefore not evidence that an external provider supplied a second code
trampoline.  It only shows that the unpaged physical-ROM shortcut is not a
valid observer for this protected-mode callback.  The default-off CCPU
linear-to-physical witness is now placed after descriptor validation and will
record the actual mapped backing page once a controlled, valid selector pair
is supplied.

Consequently the currently proved missing contract is narrower: the external
provider must arrange for an allocated data/code LDT pair and invoke the
unchanged original `VxD_Device_Init`/`sas_init_pm_selectors` registration
path.  Whether a separate injected gateway is also required remains unproved
until the paging-aware witness runs.  A product fallback still must not invent
a fixed selector, overwrite guest media, or route the callback through an
unrelated BOP; it must derive allocation, publication and teardown from the
existing DPMI/CCPU ownership.

## E32 superseded: BOP 98 / external-INSIGNIA investigation is not the callback owner

**Superseded by E35.** The later runtime class-publication and callback traces
prove that the target is the ordinary WOW module far pointer `83B7:0B98`, not
`F000:0B98`; this investigation must not be used to explain the WINMINE
failure or to justify an INSIGNIA provider.

The numerical resemblance between the earlier-assumed callback offset `F000:0B98` and SoftPC
BOP `98` is not evidence that they are the same mechanism.  The selected
original `bios.c` makes BOPs `98`--`9B` conditional on `MSWDVR` and names their
handlers `ms_windows`, `msw_mouse`, `msw_copy` and `msw_keybd`.  The matching
original headers describe the omitted component as the Microsoft Windows GDI
to dedicated-display-driver interface.  Neither the selected build nor any
pinned OpenNT source tree contains those handler bodies.

The original `wcall16.c::CallBack16` instead preserves the ordinary slow
protocol: it writes a `CBVDMFRAME`, changes only `SS:SP`, calls
`host_simulate()`, and waits for the 16-bit return path.  The selected original
`wow16cal.asm::WOW16DoneBoot` returns from that path through
`BOP_UNSIMULATE`.  It does not name BOP `98`, `ms_windows`, or an Insignia
display callback.  Consequently S2 must not enable MSWDVR, map BOP `98`, or
invent an `ms_windows` body as a fix for the observed virtual selector.

What the source does prove is narrower: the CCPU40 `BOP B1`
`virtual_device_trap(VxD_Device_Init)` entry explicitly belongs to
`INSIGNIA.386`, receives its two allocated selectors in `BX`/`CX`, and hands
them to the unchanged descriptor constructor.  The published SoftWindows
administrator reference independently lists `INSIGNIA.386` and
`SPCMSWD.DRV` as separate Windows 3.11 driver binaries, consistent with that
two-component external-provider boundary.  Exhaustive searches of the pinned
OpenNT/OpenNT-4.5/OpenNT-src-2 trees, the current SoftPC comparison tree and
available local assets find neither binary nor its source body.  The reference
is discovery evidence only; no third-party binary was downloaded, imported,
linked, packaged or executed.

The remaining source-recovery target is therefore the actual
`INSIGNIA.386`-side virtual-selector/trampoline protocol, not BOP `98`.

## E33 superseded: external-provider provenance does not block the WOW callback path

**Superseded by E35.** The search result itself remains useful provenance for
optional SoftWindows components, but the absence of `INSIGNIA.386` is not a
blocker for publishing WINMINE's ordinary module selector.

The final source-and-artifact search found no recoverable `INSIGNIA.386`,
`SPCMSWD.DRV`, or implementation source in the pinned OpenNT, OpenNT-4.5,
OpenNT-src-2, local SoftPC comparison tree, selected guest package, or the
available local research assets.  It also found no source statement that maps
the `F000:0B98` callback target to the optional `BOP 98` display service.

An independent contemporaneous SoftWindows administrator reference identifies
`INSIGNIA.386` as a separately supplied Windows 3.11/95 driver.  That confirms
the source-tree observation that the BOP `B1` provider is not a body silently
omitted from the selected OpenNT translation units; it does **not** disclose
the driver's selector allocation, injected guest-code, callback-frame or
return protocol.  No binary was downloaded, imported, linked, packaged or
used at runtime.

Consequently S2 has reached its declared stop condition: the original external
data/protocol required for a real `CallBack16` round trip cannot be verified.
The only source-first way to remove this stop is to admit a provenance-recorded
original provider artifact or a source-level specification of its complete BOP
`B1` and callback gateway contract.  Supplying an invented selector pair,
writing a ROM trampoline, or routing the call through `BOP 98` would create a
new standalone guest driver and would not be an original-minimal-diff recovery.
Until a provenance-approved original binary/source or a fully specified
replacement contract is available, USER-CALLBACK-01 remains an open S2 row.

## E34 superseded: 83B7 is not established as a B1 virtual selector

**Withdrawn; see E35.** The text below records the reasoning that was later
falsified by the class-registration and callback traces. It is retained only
for audit history and must not drive implementation.

The prior E34 conclusion is withdrawn.  It confused an independently valid
BIOS virtual-selector mechanism with the ordinary WOW module selector seen by
the callback.  `c_SasRegisterVirtualSelectors` and
`virtual_device_trap(VxD_Device_Init)` remain source evidence for
`biosDoInst`'s base-`F0000` virtualisation path only; they are not a producer
for `83B7:0B98` and must not be introduced to repair WINMINE.  The existing
`BOP 53:03` FastBOP refusal and the KRNL386 `#UD` stack-repair witness are
likewise unrelated.  E35 is the controlling correction.

## E35 correction: `83B7` is an ordinary WOW module selector, not proved B1 state

E28--E34 overreached by treating the observed `83B7:0B98` callback target as
the code selector constructed by `virtual_device_trap(VxD_Device_Init)`. That
is not established.  The original tree offers a *candidate* normal-WOW
publication route, but the current trace has not tied it to `83B7`:
`wow16/kernel31/3protect.asm::DPMIProc` routes `04F2h`; its x86 fast path uses
the original NT `INT 2Ah` `NtSetLdtEntries` protocol, while
`dpmi/dxint31.asm::i31_WOW_SetDescriptor` calls `NWOWSetDescriptor`, which
submits `SetDescriptorTableEntries` through BOP 53:00 when FastBOP is absent.
Those source paths can publish an already allocated application selector into
the active process LDT, but source availability alone is not proof that they
published this selector in the failing run.

This matches the observed shape: the physical source-LDT word at `83B7`
contains the next-free link `83B8`, which is the allocation-stage value, while
the CCPU LDT shadow has no executable descriptor. It does *not* establish a
base-`F0000` BIOS virtualisation segment. Such a B1-created segment is for
`ccpusas4.c::biosDoInst`; it cannot substitute for a guest module procedure
merely because both mechanisms carry a code selector.

The controlled WINMINE trace records five `04F1h` allocations and one
`04F2h`, with that observed `04F2h` operating on `BX=0047` (the original
BIOS-data selector), not `83B7`.  It records neither the original `INT 2Ah`
publication signature nor a `53:00` publication for `83B7`.  Therefore the
already-admitted `DpmiSetWowLdtEntry` carrier is currently unexercised, but it
would be equally unsupported to declare `04F2/NWOWSetDescriptor` the missing
`83B7` repair before locating the source caller which should publish that
descriptor.  The next investigation is that allocation-to-publication edge;
it must not create an INSIGNIA provider or hard-code a selector. E28--E34
remain historical observations; their B1-producer claim is superseded by this
entry. No guest media was changed.

## E36 return-edge sampling: no evidence yet connecting `04F1` to `83B7`

The controlled WINMINE observation was rebuilt with a default-off IRET16
return-edge witness, enabled only for `04F1h`, `04F2h`, and `04F3h`.  The
fresh input trace still records five `04F1h` requests and one `04F2h` request
with input `BX=0047`.  The return-edge witness reached two `04F1h` returns and
one `04F2h` return, all with CF clear.  Its observed post-return `BX` values
(`4374`, `0000`, and `4340`) are continuation-register values, not a selector
publication record, and none identifies `83B7`.

This rules out neither an earlier allocation-stage divergence nor a later
caller-side branch: the source DPMI interrupt frame does not preserve a
service-specific result contract for this generic observer.  It does rule out
using the captured values as a justification for a B1 provider, an
INSIGNIA-derived selector, or a hard-coded `83B7` update.  The next probe must
capture the source allocation's returned selector and its immediate caller
continuation together, before any production repair is considered.  The
observation altered no guest media and its test harness restored `SYSTEM.INI`
in `finally`.

## E37 correction: FastBOP is the unentered publication prerequisite

E35's separation from B1 remains valid, but its statement that the FastBOP
refusal was unrelated to `83B7` is withdrawn.  The original
`wow16/kernel31/wow16cal.asm::WOWFastBopInit` first invokes DPMI BOP 53:03;
when BX:DX or ES is zero it branches to `NoFastWow` before invoking either
`WOWGetFastCbRetAddress` or `WowGetFlatAddressArray`.  In PMODE32, the latter
call supplies `FlatAddressArray`, which is the precondition checked by
`3protect.asm::WowSetSelector` before it emits the original special INT 2Ah
`NtSetLdtEntries` request.

The controlled WINMINE run records `DpmiGetFastBopEntry` index 03 returning
`BX=DX=ES=0`, and records no hit in the worker's otherwise observable
`WowGetFlatAddressArray` carrier.  Separately, the compiled WOW32 object has
no reference to `FastWOWCallbackRet`: the CPU40 build has selected
`FASTBOPPING=0`.  Thus the original selector publication route is not merely
unproven; its necessary entry gate is demonstrably disabled.

The original native-x86 implementation is
`v86/monitor/i386/fastpm.asm::GetFastBopEntryAddress`, which returns a
host-code far pointer to `FastLeavePm`; `wow32/i386/fastwow.asm` similarly
uses NT kernel VDM/monitor context.  Those raw host pointers cannot be handed
to CCPU.  The admissible standalone recovery is therefore a worker-local,
guest-resident FastBOP trampoline that preserves the original far-call and
return-byte BOP protocol, plus a distinct callback-return carrier matching
the original slow `BOP_UNSIMULATE` completion.  It must be source-verified,
released with the worker, and exercised through the existing original
INT-2Ah/LDT binding.  It must not enable B1, use a host code selector, modify
guest media, or directly fabricate `83B7`.

## E38 FastBOP protocol correction and failed runtime observation

E37's proposed guest-resident trampoline is superseded by continued source
review, not accepted as a completed repair. The original `BOP.INC` FBOP
far-call return points at an inline BOP-number byte. In contrast,
`wow16cal.asm::WOW16FastVector` and its `FastWOWCbRet` call return directly
to continuation instructions, with no inline service number. Treating these
three endpoints identically is incorrect. The current experimental carrier
distinguishes generic inline dispatch, WOW dispatch and callback completion.
Also, a zero callback endpoint takes `NoFastCb`, skipping PMODE32's
`WowGetFlatAddressArray`; a nonzero generic endpoint alone is insufficient.

This source dependency does not establish that INSIGNIA.386 creates the
observed application selector, nor does an absent observed B1 call establish
that the full service is unnecessary. E35's ordinary-module-selector
distinction remains in force. No selector-specific repair is permitted.

The 45-second real-window destruction observation did not produce a live
window or the expected DPMI/WOW logs. A subsequent directly launched test
worker displayed an illegal-instruction error at `0000:0078` (bytes
`C7 EF 00 F0 60`). This is a failed startup observation, not successful
FastBOP, LDT publication or callback acceptance. The test processes were
terminated; the controlled harness restored SYSTEM.INI. A full dependency
rebuild of the formal x86 graph was then completed (516 steps, exit zero,
VdmTib storage check passed). Provider rebuild and renewed runtime diagnosis
must precede any acceptance claim. The experimental selector reservation,
entry/return contracts and worker lifetime remain subject to verification.

The provider's formal 127-step rebuild also completed. Renewed window testing
still failed. An attempted r7 comparison was invalid: its retained worker
artifact is zero bytes. The destruction harness now validates MZ, PE offset,
PE signature and x86 machine before deployment; the zero-byte artifact is
rejected before any runtime mutation (negative test passed). The valid r8
worker/provider comparison also produced no live window within its 12-second
observation. That result alone neither identifies the same fault nor proves
a regression introduced by FastBOP. Following the comparison, all six current
formal EXE/DLL artifacts were restored to the runtime with matching SHA-256.
S2 remains open; no runtime or product acceptance is claimed.

## E39 FastBOP real-mode gate recovered; guest publication now reached

A test-only reduced host environment let DOSX reach 53:03. This does not
prove the earlier ordinary-environment failure is the known COMMAND defect;
production environment truncation remains forbidden. The endpoint witness
then recorded a far call with CCPU PE=0/VM=0. Original `inc/BOP.INC::FBOP`
gates that call on `FIXED_NTVDMSTATE_LINEAR` (0714h), `RM_BIT_MASK` (1000h).
CPU40 lacked the publication present in original
`dpmi32/i386/dpmi386.c::switch_to_real_mode/switch_to_protected_mode`, so the
guest selected a protected-only endpoint while in real mode.

DIV-310 now performs the original RM_BIT read-modify-write through existing
SAS before publishing the endpoint and after the four DPMI PE transitions.
The native assembly cannot compose: it dereferences host address 0714h and
uses native monitor context. The bounded binding retains the same guest word
and preserves all other bits; it does not patch guest media or relax CALLF
mode checks. RI/VIF or the complete native monitor are not claimed.

The x86 `/MT` `tests/mvdm-host/dpmi/fast_bop_mode_fixture.c`, linked against
the actual adapter object, passed (`FAST_BOP_MODE_BOUNDARY_OK`). Mocked CPU/SAS
checks cover disabled no-write behavior, null/missing-LDT failure, three
endpoint identities, unknown-target refusal and real/protected/real state
publication without changing unrelated bits. Products remain under
`build/M0-T422/S2/`; this is boundary evidence, not guest acceptance.

The controlled WINMINE run now records generic 53/FD/56 dispatch in PE=1,
`flat-address-array` at 00A1F000h and actual INT 2Ah publications for 020Bh,
0213h, 021Bh, 0233h and 023Bh, among others. Logs are
`O:\winnt\logs\t422-s2-wow-destroy-{cpu,dpmi-bop,wow-bop}.log`.
This proves progress beyond E38, not correctness of 83B7 or callback closure.
No accepted live window appeared. Complete endpoint lifecycle, normal-host-
environment testing and the full DOS regression remain open. Temporary
per-call mode/byte/IP witnesses were removed after isolating the mode gate;
the formal worker rebuilt successfully afterward.

## E40 Native cache and guest address domains

The test-only x86 exception observer identified the first C0000005 as a write
to zero at WOW32 RVA 00002E75, W32PatchCodeWithLpfnw32, for guest 021F:B7AD.
The memcpy crash reported by Event Log was secondary. Original WowSetSelector
in wow16/kernel31/3protect.asm writes FlatAddressArray before INT 2Ah. Our
copied guest array did not update WOW32's native cache; INT 2Ah changed only
the CPU LDT. The initial copy also incorrectly contained native pointers.

DIV-310 now reuses original GetDescriptorMapping and IntelBase conversion on
successful publication. The existing page-domain binding derives guest bases
from LDT descriptors, and ordinary 53:00 updates that view before native
conversion. Native x86 shared-address publication cannot directly compose
across CCPU's distinct guest/native domains. No new alias manager or guest
patch is introduced. Rejected-entry guest-prewrite ordering and complete
selector retirement/reuse remain unaccepted, not silently treated as equal.

Formal x86 /MT linking and VdmTib checking pass. The descriptor fixture exits
zero for successful cache publication, invalid-entry rejection, original DIB
alias retention and invalidation. Its generated target now uses the existing
worker fixture library group because the isolated DPMI link lacks the cache
owner's dependencies. WOW_PAGE_DOMAIN_LIFECYCLE_OK covers descriptor-derived
12345000 instead of an unrelated DEADBEEF native value, untouched zero entries,
normalized updates and out-of-range refusal. These are focused tests only.

The non-debug 35-second WINMINE run reaches WindowEntry message 0024 and
callback 83B7:0B98 without the immediate 1067 exit. It then reaches guest
GP 0D/83B5 with an absent descriptor and repeated WOW dispatch. Absence alone
does not distinguish legitimate demand loading from a lifecycle defect.
No accepted window appears. A separate 30-second debugger observation sees
no native AV, but also no window and different progress. Logs remain under
O:\winnt\logs, named t422-s2-wow-destroy-* and t422-s2-flat-cache-dispatch.log.
S2 is still active; full DOS regression and product acceptance remain pending.

## E41 Duplicate WNDPROC encoding, not a missing module selector

Original wuclass.c::WU32RegisterClass encodes WNDPROC_WOW and the virtual
selector-high bit before RegisterClassWOWA; the original client wrapper
copies it unchanged. Our publish_native adapter encoded it again: guest
03B7:0B98 became 83B70B98, then incorrectly 83B30B98. Original
W32Win16WndProcEx decoded the latter as 83B7:0B98. That selector was not the
requested module procedure. This supersedes the hypothesis that missing
83B7 publication or INSIGNIA.386 causes this callback failure.

The repair deletes duplicate adapter encoding. No mirror change, fabricated
descriptor, selector special case or guest edit is needed. The old class
fixture asserted the wrong second encoding; that expectation is corrected
and low/high-selector round-trip cases added, still pending fixture execution.

Formal x86 WOW32 DLL rebuilding passes. In the controlled non-debug WINMINE
run (worker 54104), raw 83B70B98 decodes to 03B70B98. Original callbacks
complete messages 0024, 0081, 0083, 0001 and subsequent text messages, and a
native window exists. Destruction verification still fails for missing
successful 0002/0082 callback evidence. A usable/rendered WINMINE and complete
window lifecycle are not yet proved. Next: verify the selected HWND/worker
at close, execute the expanded class fixture and complete real destruction.

A repeat records HWND 00060B9C owned by worker 24896, visible, with title
O:\winnt\WINMINE.EXE:. WM_CLOSE returns zero; the HWND disappears but both
worker and launcher remain alive, and no guest destroy callback was logged.
The harness now records window class/hung state as well. Ghost-window takeover
or a missing message-pump edge is a hypothesis, not yet a proven diagnosis.

## E42 Correct application gate; native handle reuse blocked by stale entry

The expanded class-client fixture now builds against selected production
provider/worker objects and exits zero. Its explicit page-domain publication
mock is compiled only for Case=class-client; the test covers original class
policy and low/high selector encoding, not guest page publication. Existing
page-domain and real guest checks remain separate. The harness accepts both
the default lifecycle case and this class case with independent build roots.

Window identity disproves the earlier destruction interpretation: the selected
window is WOWExecClass, not WINMINE, and IsHungAppWindow is false. The test
now selects only the Minesweeper / pinned Chinese guest's sweep-game class
(Chinese name recorded in the class-publication log), excluding WOWEXEC and
WOWFax helpers. E41 proves real WOWEXEC callbacks, not WINMINE creation.
The corrected gate fails, as required, instead of closing the helper window.

A default-off adapter creation witness records the exact failure. Worker
42560 creates WOWExec HWND 00060B30 and WOWFax 00100B18 successfully. WINMINE
then receives HWND 00070B30 (same low index, new generation). Its CBT scope
is claimed but publication returns bindingError=183 / ERROR_ALREADY_EXISTS;
creation returns NULL and only the new window's 0082 callback completes.
The original helper's matching retirement is absent from the observed native
dispatch log. This is evidence of stale local lifetime state, not permission
to overwrite a live entry or to claim original guest failure. Next work must
identify the actual native/task destruction path and restore its retirement
binding, with generation reuse and surviving borrowed references covered.

The real guest subsequently shows its creation-error path; one run later
reaches an unhandled guest exception 0A at 00C7:08C3. Neither is passed.
SYSTEM.INI is restored by the harness. An identical running dtmgr binary is
now skipped by deployment after hash comparison, avoiding needless overwrite
or termination of the user's manager. S2 and the complete WOW32 goal remain
open; no complete application or destruction acceptance is claimed.

## E43 Native owner-thread exit precedes handle-generation reuse

On 2026-09-24, a non-debugger repeat (worker 50756) created WOWExec HWND
004B0A44 and WOWFax HWND 002A0B64 on native thread 29112. Read-only process
thread sampling at 09:43:30 observed thread 29112; at 09:43:32 that thread
was absent and application thread 19616 was present. The creation witness
then recorded WINMINE creation on thread 19616 failing with bindingError=183.
Together with E42, this establishes that the previous owner thread actually
exited; absence of a guest destroy callback alone was not sufficient evidence.
Logs: O:\winnt\logs\t422-s2-wow-destroy-window.log and the class-publication
log from the same run. The thread snapshots are in the execution transcript.

Original OpenNT windows/core/ntuser/kernel/queue.c::xxxDestroyThreadInfo
patches thread window procedures before DestroyThreadsObjects. Native thread
cleanup therefore has a distinct lifetime boundary; application WM_NCDESTROY
delivery cannot be the sole retirement mechanism for our projected metadata.
Current wow_window_words_binding.c retires its table entry from that callback;
thread_start_compat.c::opennt_exit_thread only unbinds the worker session.
These observations locate a missing cleanup edge but do not yet explain why
the WOWEXEC owner thread exited. Investigate that exit before adding cleanup:
a task-order failure or unexpected host_simulate return must not be concealed
by deleting stale metadata. Do not overwrite occupied entries on creation.

The debugger-attached repeat did not reach class creation before its deadline
and supplies no contrary evidence. Both normal repeats failed the corrected
real-application gate with the guest creation-error dialog (1000). No usable
WINMINE, destruction acceptance, or S2 closure is claimed. Test processes were
cleaned up by the harness and SYSTEM.INI restored; guest binaries were unchanged.

## E44 Registration bypassed the existing USER lock owner

Read-only retained thread handles (without a debugger) show WOWExec thread
60600 in worker 28584 exiting with code 5023 / ERROR_INVALID_STATE, before
test teardown. A default-off failure witness then identifies release-lock;
worker 53288 reports binding=00FC800C, held=0, owner=0. This is an adapter
failure, not normal original task termination or a missing INSIGNIA driver.

The production registration bridge called xxxUserYield, xxxDirectedYield and
xxxSleepTask directly, bypassing existing wow_user_task_lifecycle wrappers.
Original taskman.c requires CheckCritIn on entry and later calls LeaveCrit.
The three registered entries now call the existing lifecycle wrappers, which
acquire/release the single worker USER lock around that original body. No
mirror code, scheduler algorithm, lock checks or guest media were changed.
The registration-surface check now rejects these direct bypasses; it passes.
Fresh x86 WOW32.DLL compilation also passes.

Real immutable WINMINE repeat (worker 44652): WOWExec HWND 00390A78 and WOWFax
00180B3A remain separate; WINMINE HWND 001B0B3E completes WM_CREATE with zero
and WindowCreate returns the HWND with error=0 / bindingError=0. Its WM_TIMER
callbacks continue to return. There is no TaskFailure in this run. This
confirms removal of the observed creation blocker, not full application
acceptance. The harness still times out at its class-name identity gate;
record the actual Unicode native class names before changing that gate.
Rendering, interaction, destruction and DOS regression acceptance remain open.

## E45 Real visible WINMINE creation and callback destruction

Native class-name observation identified the pinned CP936 name bytes C9 A8 C0
D7 rendered through the host ANSI registration as U+00C9 U+00A8 U+00C0 U+00D7.
The test now matches that exact byte sequence decoded with GetACP, in addition
to the English and Chinese names. It still rejects WOWExecClass/WOWFaxClass.
This fixes test identity only; localized text compatibility is not accepted.

The first successful destruction test closed a not-yet-visible window. A
stronger visibility gate correctly rejected the harness's SW_HIDE launch:
original wkman.c propagates STARTF_USESHOWWINDOW to the guest. The interactive
verification launch now uses Normal and waits for IsWindowVisible, without
forcing ShowWindow on the application or changing production startup policy.

Worker 37092, HWND 00140BF0: visible=True, hung=False, native class/title match
the pinned ANSI name; WM_CLOSE makes that HWND disappear. Both the native
dispatch log for that HWND and original W32WndProc guest callbacks report
WM_DESTROY (0002) and WM_NCDESTROY (0082), success=1, proc=179F047E. The
strengthened test prints T422_S2_WOW_USER_REAL_DESTRUCTION_OK. Worker and
launcher remain alive immediately after window destruction; their final task
cleanup is not inferred from this result. Harness cleanup restores SYSTEM.INI.

Artifacts SHA-256: WOW32.DLL
8D4C8341F51044C79F61D7A5F2D4758FE938E5C914FED25179ED691F28B743A1;
ntvdm.exe 9D8E7420F39118A4245DDFF4799E79600BE965662704DD0B416ECB01934DD837.
Evidence is O:\winnt\logs\t422-s2-wow-close-state.log and the same-run
destroy-window / destroy-callback logs. Board rendering, gameplay input,
localized display, complete task/resource retirement and all DOS regressions
remain separate gates. S2 and full WOW32 remain open.

## E46 Separate resource cleanup from native thread retirement

Diagnostic runs now receive unique log directories and retain deployed hashes,
observer hash, original profile hash, result and per-file checksums. They are
explicitly INCOMPLETE as reproducible acceptance: frozen build/source inputs
and rendered-content evidence are still absent. Previous fixed-name E43--E45
observations are partial evidence, not retroactively sealed baselines.

Run t422-s2-20260924T170219377Z-a05fbe33-window-lifecycle reproduces visible
window destruction. registration.log records WOWCleanup success followed by
WOWCleanup failure. basesrv-command.log records no exit-enter; afterGrace in
the close-state log records launcherExited=False after five seconds. These
facts do not prove the broker rejected an exit; the exit call was not observed.

Source audit identifies a separate definite error: wuser.c::ModuleUnload
calls pfnWOWCleanup with the task ID when fTaskExit is true, then with task ID
zero for module cleanup. Current wow_user_task_lifecycle_exit unconditionally
calls retire_task, which deletes the TDB/thread context after the first call
and rejects the zero-task call. Original kernel/cleanup.c::_WOWCleanup has
both task and module branches and does not call DestroyTask. Original
kernel/queue.c::xxxDestroyThreadInfo calls DestroyTask at native thread exit.
Therefore the adapter must preserve the thread context through module cleanup
and retire it at the distinct thread-exit boundary. The existing lifecycle
fixture asserts immediate retirement and must be corrected with the product;
its earlier zero-error result does not prove original lifecycle equivalence.
This audit does not yet prove all causes of the missing parent completion.

The prior run t422-s2-20260924T170039867Z-559d0907-window-lifecycle is retained
separately. The earlier d535d96f run failed before launch because reduced PATH
omitted git; the harness now accepts an explicit GitExecutable. No guest
binary was modified. S2 remains open.

## E47 Resource calls retain task context; thread-unbind owns retirement

Production pfnWOWCleanup now invokes the original cleanup body with the
current task context for both nonzero-task and zero-task module calls. It no
longer frees the TDB, class context, queue or thread carrier. The existing
worker runtime unbind boundary calls an attached provider retirement callback;
no second task table or scheduler was introduced. Initialization publishes the
callback only after successful owner creation, and disposal withdraws it.

Retirement rejects any enrolled window still referring to that thread instead
of freeing a referenced carrier. The abnormal native-window destruction edge
remains open; this ERROR_BUSY guard is memory-safety containment, not complete
abnormal-cleanup support. Final retirement currently performs the finite
cleanup preflight before DestroyTask, preserving failure/retry safety; exact
original queue.c ordering still needs review before lifecycle acceptance.

The corrected fixture checks successful task cleanup leaves context alive,
zero-task module cleanup still succeeds, and explicit runtime unbind retires
the task. It passed with zero errors. x86 worker and WOW32.DLL builds pass.
The additional rejection assertion also passes: unbinding with an enrolled
window returns ERROR_BUSY and preserves current context; removing that test
enrollment allows subsequent resource cleanup and thread retirement. The
rerun reports WOW_USER_TASK_LIFECYCLE errors=0.

Diagnostic run t422-s2-20260924T170732947Z-043c2063-window-lifecycle retains
distinct logs and hashes. Worker 16360: both successive WOWCleanup calls now
return 1 (previously 1 then 0), and visible WINMINE destruction passes. There
is no TaskFailure log, but afterGrace still records launcherExited=False and
the BaseSrv trace has no exit-enter. Thus this change fixes premature context
retirement, not the entire guest-exit/parent-completion chain. No complete
resource, application, DOS-regression or S2 acceptance is claimed.

## E48 Post-destruction guest exception, not an observed broker rejection

Diagnostic run t422-s2-20260924T171050623Z-ac84ef55-window-lifecycle enables
the existing W32 dispatch witness and waits 15 seconds after close. Its last
completed application thunk is WU32SignalProc (provider RVA 00035D60), after
the two successful resource cleanup calls. There is no observed WK32KillTask
or BaseSrv exit-enter. The DPMI log instead reaches BOP 53:18.

The CPU witness records fault vector=0a error=1068 at CS:IP 00C7:08C3,
linear/physical 001110C3, code starting CF (IRET). Saved flags are 7297,
including NT=1; saved SS:SP is 017F:0FC6. Original ccpu386/iret.c takes its
nested-task return branch when NT=1 and reads the TSS backlink. This explains
why that branch is relevant but does not prove where NT became set, nor that
the TSS/CCPU implementation is defective. The earlier deliberate 0FFF faults
at 021F:C223 and 021F:20B1 are separately handled and must not be conflated
with this terminal exception. Next trace the fault return and flags/frame
provenance without clearing NT speculatively or modifying the guest.

The test formerly emitted its narrow destruction success even when this
post-close guest failure occurred. It now rejects the combination of fault
0A and unhandled DPMI BOP 18, retaining window-destruction evidence separately.
The already-recorded run is not rewritten; this note corrects its interpretation.
Normal task exit, parent completion and S2 remain unaccepted.

## E49 PM handoff omitted original NT-flag sanitization

The original dpmi/486/dosx.map identifies 08C3 as DOSXiret. A bounded,
default-off NT-transition witness (two-line decode observation hook, adapter
logging only) records the application thread acquiring NT in real mode on
return FE2E:5443 -> D1BE:052B. It remained set through the local 53:01 entry
and reached protected-mode IRET. Failed diagnostic run:
t422-s2-20260924T171519682Z-73827284-window-lifecycle.

Original OpenNT base/ntos/vdm/x86/vdmmisc.asm::VdmSwapContexts applies
EFLAGS_USER_SANITIZE on the non-V86 incoming context; base/ntos/inc/x86.h
defines that mask as 003E0DD7, excluding NT. The standalone CPU40 replacement
in modesw.c restored selectors and stack but omitted that NT removal. It now
clears EFLAGS_NT_MASK at the same 53:01 PM handoff, retaining CCPU's existing
IF/IOPL behavior. This is one boundary statement, not an IRET change, a
blanket per-instruction flags mask, or guest modification; DIV-222 records it.

Successful diagnostic run:
t422-s2-20260924T171749239Z-ac62960d-window-lifecycle.
The application thread records D1BE:10FD flags=7002 -> 00CF:1101 flags=3002.
WINMINE becomes visible and completes destroy callbacks; BaseSrv trace records
exit-enter 1, exit-rpc 0, exit-ok 0, followed by run16-exit 0. afterGrace
records launcherExited=True, launcherExitCode=0. The prior terminal 0A/BOP18
is absent. This corrects E48's open parent-completion result on these artifacts.
The harness now requires successful launcher completion, not just HWND loss.

Manifest SHA-256:
2889CC6BD21A939B799B1C850257DA264028A77E41D53A2AC91E6A01F7D31E11.
Worker SHA-256:
1590F79FB14A9A6BF3C42BB07EF6AF3C2DDB9A3544C40E67844346111F450127.
WOW32 SHA-256:
625A8BDDDD95A1CA8B3D72A876E79BCF9C641E21CE85D645B3CF79F5222D1241.
The six deployed binaries and matching worker/provider maps are retained under
build/M0-T422/S2/t422-s2-20260924T171749239Z-ac62960d-window-lifecycle/.
Dirty input snapshots and full build provenance remain incomplete; this is
causal diagnostic evidence, not a sealed whole-product acceptance. Full flags
boundary fixtures, rendering/input, exceptional teardown and DOS regressions
are still required. S2 and the full WOW32 objective remain open.

## E50 PM handoff boundary and DOS regression follow-up

The repeat diagnostic run
`t422-s2-20260924T171930016Z-f0701599-window-lifecycle` records a visible,
non-hung WINMINE window, successful destruction and launcher exit zero. Its
result explicitly remains incomplete evidence: no sealed dirty-source/build
snapshot or rendered-content verification. It corroborates E49, not whole-S2
acceptance.

`tests/observation/verify-pm-entry-contract.ps1` extracts the complete current
`DpmiCpu40SwitchToProtectedMode` function without editing its body and compiles
it under MSVC x86 /MT with observed CPU/descriptor dependencies. The fixture
passes all 65,536 low-FLAGS combinations, plus invalid-IDT and absent-GDT-shadow
cases: 65,538 cases, zero failures. It checks NT removal before PE, preservation
of every other low flag (including IF/IOPL), original frame restoration,
descriptor/mode publication ordering, and refusal to continue a transition
without an IDT. This is a mocked boundary unit test, not execution of the CCPU
instruction engine or a replacement for guest integration. The first build
had a duplicate SDK LDT_ENTRY declaration; removing the redundant test typedef
produced the passing result. No production change was needed.

Production modesw.c SHA-256:
4BBFF6CB6021A4839B6616169529BCCA76510F98B82CA9665F03637E4654ED22.
Fixture source SHA-256:
3EBACD796DC72B0FD035B049B66A7B8137D352F1A8AA28E5F418B5011313967D.
Fixture executable SHA-256:
CD38E78B5728967392F403C287BA2A09142F8B306CB57B2304807CDCC0D8D7B1.
Artifacts remain in `build/M0-T422/S2/pm-entry-contract/`.

The existing `tools/audit/Verify-CommandExitStatus.ps1` then completed all
17 default Console-text and exit-status gated cases with the deployed E49
worker/provider hashes unchanged. Invocation used
`-Observer O:/winnt/tests/console-startup-observer.exe`,
`-GuestFixturePath O:/repos.hobby/ntvdm64/build/M0-T422/S2/G7.COM` and
`-LogPrefix t422-s2-dos-20260924P052351-ab89e3fe`. No minimal-environment
override was applied to this DOS regression invocation. The report is
`O:/winnt/logs/t422-s2-dos-20260924P052351-ab89e3fe-summary.json`:
17 records, each Actual equal to Expected. The original interactive COMMAND
exit value one remains expected, not rewritten as zero. Generated COM/CMD
inputs are test-only; original guest media were not edited.

These results close E49's pending bounded FLAGS test and current-artifact DOS
regression, not its remaining rendered-content/input, exceptional retirement
or formal evidence-provenance requirements. S2 remains active.

## E51 explicit detach failure and original abnormal-thread ordering

Production inspection finds that `wow_user_session_detach` called a void
unbind hook which discarded `wow_user_runtime_unbind` failure, then removed
the registration and reported success. The actual runtime deliberately refuses
retirement while task/window references remain. Explicit detach now propagates
that failure before unregistering anything, preserving the provider error,
current binding and hook for cleanup followed by retry. This is an existing
worker-binding error contract repair, not a new guest exit policy or mirror
change.

`verify-wow-user-session-binding.ps1` compiles the actual runtime, session
binding, session and lease sources with MSVC x86 /MT. The fixture injects a
retirement refusal (ERROR_BUSY), asserts unchanged registration/session/TLS,
then permits retirement and verifies successful retry and hook removal.
The pre-existing child-thread bind/unbind and callout-lock checks still run.
Result: `WOW_USER_SESSION_BINDING errors=0 child=1 lifecycle=1`.
This is host/mock boundary evidence only. Artifacts are under
`build/M0-T422/S2/user-session-binding/`.

The remaining abnormal-exit gap is source-defined, not speculative:
original `windows/core/ntuser/kernel/queue.c::xxxDestroyThreadInfo` invokes
`PatchThreadWindows` (line 1815), `DestroyThreadsObjects` (1903), then
`DestroyTask` (1989). `createw.c::PatchThreadWindows` selects the thread's
non-destroying windows, replaces guest callbacks with the appropriate server
procedure and marks server-side dispatch. `handtabl.c::DestroyThreadsObjects`
releases DC/thread locks, skips process-owned objects (with menu-reference
handling), then destroys eligible thread-owned objects. Private server locks,
DC cache and HMDestroyUnlockedObject cannot simply be called on native HWNDs;
the selected original slice and its finite public-host binding still need
closure. The current ERROR_BUSY guard is containment, not this restoration.
In particular the generic session's void thread-unbind hooks still cannot
propagate this refusal: explicit detach repair does not close that separate
thread-exit edge. Do not free THREADINFO or clear TLS to conceal it.

The production x86 rebuild was started after the focused pass; its result
was still pending at this entry. No new deployment or formal-build pass is
claimed by E51.

## E52 detach production verification and thread-cleanup boundary refinement

The E51 restricted Ninja invocation remained alive with no compiler children,
zero writes, unchanged 562,941 bytes read/37 reads and unchanged build outputs.
Windows wait-chain inspection returned one running thread and no cycle; it did
not establish the underlying cause. Only that exact Ninja PID/parent/command
was terminated. The same build graph under the previously working elevated
execution boundary then compiled the binding and linked ntvdm.exe successfully.
The post-link VdmTib check passed (owner mvdm_vdm_tib.obj, 4,208 bytes, no
overlapping symbol). This is a completed incremental x86 build, not a fresh
sealed-input build.

New worker SHA-256:
8930230C95BC71E7227619C32033EC055E814A108F0713B4C896AA5D4DB3B562.
Diagnostic guest run
`t422-s2-20260924T173429721Z-b2d32452-window-lifecycle` passes its actual
predicates on that worker: visible non-hung WINMINE, original destruction
callbacks, window gone, launcher eventually exits zero. SYSTEM.INI restoration
and reduced-environment/incomplete-provenance limitations remain explicit in
its run result. This is not board rendering, gameplay or exceptional teardown
acceptance.

The deeper original-source audit adds two constraints before implementing
thread retirement:

- `PatchThreadWindows` uses the **window's** last function ID, not merely its
  class. Original comments explicitly explain subclass/cross-class
  CallWindowProc cases. The current cleanup view has only a class fnid and
  procedure/server-side fields; blindly selecting a class default would not
  recover that contract. Shared-menu ownership reassignment is also a
  separate original branch, not an ordinary guest window destruction.
- `DestroyThreadsObjects` distinguishes thread and process ownership using
  `gabObjectCreateFlags`; menus, DC cache/borrowed locks, already-destroying
  objects and object references cannot be collapsed into one HWND loop.
  Existing live HWND/WW/CLS storage and reference retention should supply
  the finite binding; do not introduce a second object table or scheduler.

Original source identities (SHA-256): createw.c
DA4A0C0E0A85DBE88130F01CCCEECF9DA08A0FEBF3F841458D2A3CBD707A57D7;
handtabl.c F0B56314A93423599F06645D17104391F19A0CA07C3944853B065D8866493F7C;
queue.c 0400C034BB5A782E81777E9FD805C07DBCD18D60EC9F1A876CFA67444577E0F8;
taskman.c E37F4D724650D5758EE05C54E181771AA584AEAD15F5A1376B322ECB7016E64E.
All are under OpenNT/windows/core/ntuser/kernel. This is bounded source
research, not admission of the entire USER kernel. No original source was
imported or guest bytes changed in this follow-up.

The newly deployed E52 worker also completed all 17 established DOS routes:
`O:/winnt/logs/t422-s2-detach-20260924173503-962df21c-summary.json` contains
17 results with zero Actual/Expected mismatches. This reran the same E50
observer and guest-fixture invocation without the minimal-environment override,
changing only the fresh log prefix and selected worker. The explicit detach
repair therefore has focused failure/retry evidence, a completed production
link, repeated diagnostic WINMINE exit and current-artifact DOS regression.
It remains uncommitted and does not close the abnormal-thread cleanup gap.

## E53 user-observed WINMINE gameplay and visual receiver split

Visual diagnostic run `t422-s2-20260924T173926400Z-ce0d4b4c-visual`
displayed the actual minefield, digits, face, timer and flags. The owner then
confirmed playing it. Open cells/flags in the observations are owner input,
not evidence of unsolicited mouse actions. Computer-use was stopped by the
owner's Escape key and no further UI action was attempted that turn.
The launch observer subsequently exited normally (launcher 0, no timeout,
SYSTEM.INI restored), as recorded in its result.json. Its source/build snapshot
is not sealed, so it remains diagnostic evidence plus explicit owner testimony.

The owner reports small dimensions and black/white display. Cropping is not
yet proven: screenshot bounds alone cannot distinguish capture, native client
geometry and guest drawing. The owner separately accepts Chinese mojibake as
the host's encoding condition; it is not a blocker or authority to change
guest bytes/system settings.

Read-only NE resource inspection of O:/winnt/WINMINE.EXE finds RT_BITMAP
410/411: 16x256, 4/1 bpp; 420/421: 13x276, 4/1 bpp; 430/431: 24x120, 4/1 bpp.
Thus both color and monochrome original inputs exist. No conclusion about
runtime selection or color loss follows from their mere presence.

Diagnostic run `t422-s2-20260924T174431330Z-8caaed5e-window-lifecycle`
passed the existing window-close predicates but produced no BitmapLoad trace.
Source inspection explains the missing witness: registration calls
registered_load_bitmap -> WOWLoadBitmapA with an already-bound context,
whereas the attempted observer was in wow_bitmap_load. This does not prove
bitmaps were unused. The misplaced temporary observer is withdrawn instead
of leaving diagnostic code on an irrelevant path.

The proposal now explicitly assigns monochrome/DC/bitmap output to S3 and
resource selection/loading to S4, with a shared input-to-pixel witness and
negative/cleanup tests. Window/client geometry, metrics, direct-data updates,
callbacks and exceptional teardown remain S2. S2 cannot claim these passed
merely by handing off visual rendering; S8 still owns integrated three-app
acceptance. No S admission or closure is changed here.

## E54 live WND geometry and S2 remaining closure work

Original OpenNT `windows/core/ntuser/rtl/wow.c::_ClientToScreen` adds
`pwnd->rcClient.left/top`; `_GetClientRect` subtracts that same origin.
Therefore original WND.rcClient is screen-relative. The worker projection
incorrectly sampled the zero-origin public GetClientRect result. This is an
adapter defect, not a guest defect or a reason to change immutable USER.EXE.

The existing page-domain carrier now samples GetWindowInfo's screen-relative
window/client rectangles and native style/exstyle into the established fixed
guest offsets. The existing bound window dispatch refreshes that allocation
before calling the original DispatchClientMessage. Refresh rejects a mismatched
full handle, retired allocation, wrong worker or inactive paging domain.
It creates no second window store, native USER policy or mirror-source change.
This covers callback entrances, not every possible host-originated mutation;
general direct-read freshness and reentrant mutation remain unaccepted.

The current `wow-page-domain-test.exe` passes actual CCPU memory reads after
native hidden-window creation and move/resize, checks all eight rectangle
coordinates plus both styles, and rejects stale/reclaimed identities.
This is native-window plus real-CCPU fixture evidence, not a running USER16
direct-read test. Its SHA-256 is
CFB9788F2BDE570A675936D54CA35B4473116407423C8E9BBC773B870EF64032.
The regenerated MSVC x86 worker and provider graphs both build successfully.
Worker SHA-256:
D1B1CD6B3A9365B65FE2A21E2B27036D40B6D8B60691FB4C6A1E9503FB1C3FBF.
WOW32 SHA-256:
3D879E867E5B2142B9F7481A32DD94D51825F2C36AA3426C2201A1B6FB453FF6.

Diagnostic guest run `t422-s2-20260924T175412598Z-9fced9f8-window-lifecycle`
passes visible WINMINE destruction/callback and launcher-completion predicates;
SYSTEM.INI is restored. The observer finally force-stops any remaining scoped
processes, so this is not proof of complete autonomous broker/resource cleanup.
The reduced environment, unsealed source/build identity and lack of a new
content capture remain explicit limitations. It does not prove the screenshot's
small size or monochrome display had this single cause.

Remaining S2 work is grouped by closure contract, not a guessed percentage:

1. Complete live direct-data updates and real guest reads: geometry, styles,
   metrics, cached handle identity and host-originated changes.
2. Complete class/window query and mutation, nested callbacks and destruction
   during callback; prove source-shaped failure/retry and retained references.
3. Prove message send/post/wait/cancel and original task scheduling under
   multiple tasks, receiver loss and callback-frame restoration.
4. Complete exceptional task/thread/worker teardown, including original
   window-procedure patching and object-destruction ordering from E52.
5. Close every S2 registration/direct-data ledger edge with production and
   guest evidence, not merely a non-null slot or passing native fixture.
6. Seal replayable source/build/package identities, run fresh x86 and all
   established DOS regressions, review minimal diffs, then commit/push.

GDI color output and resource selection remain explicitly assigned to S3/S4;
their transfer does not excuse any of the above S2 work.

Follow-up checks on the E54 artifacts completed successfully:

- Original class/client fixture: `WOW_FIXTURE_OK case=class-client`, rebuilt
  against current provider and worker owners through
  `tests/observation/verify-wow-task-lifecycle.ps1`.
- All 17 established DOS routes pass; report
  `O:/winnt/logs/t422-s2-geometry-20260924175450-2bb2b2b5-summary.json`.
  The Verify-CommandExitStatus invocation retains E52's observer/G7 inputs
  and uses the fresh geometry-prefixed log namespace.
- Documentation governance and `git diff --check` pass. Existing Git
  LF-to-CRLF warnings are not semantic changes or a whitespace-error pass
  substitute; no bulk mirror formatting is performed here.

These checks do not seal the source snapshot, close S2, or imply that the
existing uncommitted worktree has been committed/pushed.

## E55 publication failure and original callback lifetime contract

The current page-domain publisher ignored GetWindowInfo failure. It now
rejects a failed non-null native window lookup before allocation/publication.
The fixture replaces its fabricated HWND with a real hidden native window,
and replaces the single-byte cleanup-object placeholder (dereferenced as a
larger structure by production code) with an actual wow_cleanup_window.
After DestroyWindow it checks failed republication leaves both native and
guest handle entries retired, with no apparent successful empty WND.
The zero-HWND low-level layout-only fixture remains explicit synthetic data,
not a production window or guest acceptance claim.

MSVC x86 worker and page-domain fixture build and the fixture reports
WOW_PAGE_DOMAIN_LIFECYCLE_OK. Worker SHA-256:
A8E01ACA08FBD463E4D9D61F2DF97DD60346738F74E7B515D8B935B7B4C492C9.
Fixture SHA-256:
FF16A86AD76791B65A9169B8B6C819786FEC6751CD0453406698C83F4B661DE0.
These supersede E54's mutable build outputs, not its historical test results.
No new all-17 DOS pass is claimed for this subsequent change.

The original sources clarify the next lifecycle requirement:

- `ntuser/client/wow.c::ValidateHwnd` bypasses general handle validation for
  CLIENTINFO.CallbackWnd, returning the current callback's borrowed PWND.
- `ntuser/kernel/ssend.c::LOCKPWND/UNLOCKPWND` thread-lock the object, save
  and publish that pair, then restore the previous pair before unlocking.
- `ntuser/kernel/handtabl.c::HMMarkObjectDestroy` sets HANDLEF_DESTROY but
  refuses physical destruction with nonzero cLockObj; HMFreeObject asserts
  zero locks.

Thus the fields previously called cached HWND/WND are a nested callback
borrow, not a general-purpose lookup cache. Current production only clears
those guest fields; it does not yet publish the original nested borrow.
Clearing them at retirement alone cannot restore that missing contract.
Moreover current native WW references survive callback destruction, whereas
the guest projection is freed immediately by retire_handle. The next coupled
repair must retain guest backing through the same existing window borrow,
separate unpublication from final backing release, and save/restore the
callback pair without inventing another object registry or scheduler.
Nested callback/destruction/failure tests and real guest reads must prove it.
This source-backed gap remains USER-HANDLE/USER-CALLBACK closure work, not a
claim that a reproduced WINMINE crash was caused by it.

Diagnostic run `t422-s2-20260924T180057506Z-e06bcb6e-window-lifecycle`
also reports T422_S2_WOW_USER_REAL_DESTRUCTION_OK on the E55 worker and
unchanged E54 WOW32 DLL. It retains the same reduced-environment, source-seal,
content and observer-forced final cleanup limitations as E54. This is not an
exceptional-exit or nested-destruction pass.

## E56 coupled callback borrow and delayed guest backing release

The production window binding now keeps the retired guest allocation with
its existing reference-counted WW. Retirement unpublishes the handle and
transfers the allocation to that binding; its final release calls the existing
VdmFreeVirtualMemory owner. It does not add a registry, allocation algorithm
or reference-count owner. Unborrowed fixture callers retain immediate release.
Explicit detach now preserves its property/reference when retirement fails,
instead of ignoring the failure and freeing the native binding anyway.
Exceptional provider/worker teardown and allocator-failure recovery still need
their separate acceptance; this change does not prove those complete.

The same dispatch now publishes and restores the original CallbackWnd pair
inside its existing SEH finally/borrow scope. Source: ssend.c computes
`pwndClient = pwnd - pci->ulClientDelta`; LOCKPWND saves the outer pair and
UNLOCKPWND restores it before unlock. Immutable wow16/user/k/daytona/wowk.asm
ValidateHwnd reads the pair at TEB +6Ch/+70h and directly returns its client
pointer. The worker therefore publishes the client-linear allocation, not
the server-form HANDLEENTRY pointer or a native address. Restoration precedes
borrow release on both normal return and unwinding.

The extended real-CCPU page-domain fixture passes:

- Retire a window while retaining backing: handle lookup is withdrawn but
  its original guest WND contents remain readable.
- Republish the same native identity: a separate allocation is obtained and
  retiring it does not overwrite or release the retained outer backing.
- Save/publish a nested CallbackWnd pair, restore the outer retained client
  pointer, then restore the initial zero pair before final backing release.

This verifies the production page-domain primitives, not a guest-triggered
nested DestroyWindow callback or native WW last-reference failure injection.
The existing task-lifecycle fixture also passed with errors=0 before this
coupled edit, in `build/M0-T422/S2/task-lifecycle-e56`; it is a baseline,
not a new-version callback acceptance result.

Current MSVC x86 worker and WOW32 provider build successfully. The initially
missing active-domain export was caught by the DLL linker and added to the
existing parent export list; the final DLL link passes. SHA-256 identities:

- ntvdm.exe: F530A05E55D2A4C24E941A40786E9A87F05F5450FB7D58CEFC792CC191C27D9B
- wow32.dll: 50152BB79D0EA7C27384CD2C321AB92A080CD0180B0E9276A32AA7A79C1D95D0
- page-domain fixture: 68732F84CB7D79068FC69127959460E9A98C6A8F40DB4348ECD5ADDC8DD8C6F8

Diagnostic guest run `t422-s2-20260924T180954254Z-dd28e3e9-window-lifecycle`
reports T422_S2_WOW_USER_REAL_DESTRUCTION_OK. Its ordinary creation/callback/
normal-close regression passes, with the prior reduced-environment, source-seal,
rendered-content and final forced-cleanup limitations. Existing unrelated edits
to worker/environment files appeared during this turn and were preserved;
no claim of a frozen source snapshot or single-variable whole-build experiment
is made. No mirror-source edits or guest changes were made by this repair.

All 17 established DOS routes subsequently pass on the deployed E56 artifacts:
`O:/winnt/logs/t422-s2-borrow-20260924181024-a1c6767e-summary.json`.
The command uses the existing console-startup-observer and G7 fixture with a
fresh borrow-prefixed log namespace. Governance and whitespace checks also
pass. S2 remains open and these working-tree changes are not committed/pushed.

## E57 production window binding, nested destruction and SEH fixture

`tests/observation/verify-wow-task-lifecycle.ps1 -Case window-borrow` now
links the full existing page-domain fixture to the selected production provider
objects. Unlike class/task-only cases it omits the parent page-domain stubs.
The optional WOW_WINDOW_BORROW_FIXTURE body creates actual hidden native
windows, binds the existing production WW/object owner and enters the original
DispatchClientMessage through wow_window_dispatch_bound. Its callback is a
controlled native fixture, not a Win16 application or real CallBack16 body.

Covered assertions:

- DestroyWindow inside the outer callback recursively delivers WM_DESTROY
  and WM_NCDESTROY through wow_window_native_proc. The outer guest WND and
  native WW remain readable after native destruction.
- Both normal return and an explicit SEH exception restore the initial
  CallbackWnd pair. The original VdmQueryFreeVirtualMemory total returns to
  its pre-publication value after the last borrow, proving no allocation leak
  in these controlled single-window cases. The test does not attempt a
  double-free as an allocation query.
- With CCPU paging temporarily withdrawn, detach fails but retains the
  native property, existing reference and typed object entry. Restoring the
  page domain allows the subsequent destruction to finish normally.

Two unique build directories retain the test products and MAPs:
`build/M0-T422/S2/window-borrow-e57` and `window-borrow-e57-retry`.
The first passed normal/SEH destruction. The latter additionally includes the
failed-detach/retry case and reports WOW_WINDOW_BORROW_CCPU errors=0,
WOW_PAGE_DOMAIN_LIFECYCLE_OK and WOW_FIXTURE_OK case=window-borrow.
Its independent repeated execution exits 0 and retains raw output at
`O:/winnt/logs/t422-s2-window-borrow-20260924T181842647Z.log`.

Retry fixture SHA-256:
6022A8E6A08D6441BA38DBED2A6693363935B459527E975FBC287AD93075AE62.
Fixture source SHA-256:
6F1280ADC4265F8774E69FB632EAAC9E24EA60E35D4D6021DEB74AF7134006A2.
Runner SHA-256:
5ABF1BC22B44500F19CEAE399E86499AA940A7DA943ADA9B6251CD334A6FC3C2.
MAP inspection confirms window release from provider:wow_window_words_binding,
DispatchClientMessage from provider:client, VdmFreeVirtualMemory from original
nt_mem and retirement from the actual worker page-domain object. The fixture
link's existing /FORCE warning concerns duplicate printf from fprt/CRT; it
does not certify arbitrary duplicate owners. The audited functions above are
the selected real owners.

This turn changes tests/runner/evidence only, not the E56 product semantics.
The E56 17-route DOS result is retained, not represented as rerun here.
Real Win16-triggered nested destruction, task/thread cancellation and whole
worker teardown still require separate acceptance; the source/build snapshot
is not sealed. No S2 closure or commit/push is claimed.

## E58 thread-unbind failure and original previous-window-procedure routing

The generic session thread hook previously discarded the WOW runtime's failed
retirement result, then cleared TLS and decremented binding counts anyway.
Its private unbind callback now returns success/failure. Failed retirement
preserves the binding and counts, permitting the same caller to retry. The
session-binding fixture checks ERROR_BUSY, retained TLS/counts and successful
retry, followed by rebind and explicit detach. It passes with errors=0 in
`build/M0-T422/S2/user-session-binding-e58-retry`; executable SHA-256 is
63F4256A022CB06FFA70384C07B6BF857258921E050289AD29013A65CBD27D00.
Thread entry wrappers that ignore the generic return still require the
separate exceptional-thread-exit contract; this is not full teardown closure.

A subsequent diagnostic real-guest run failed instead of reaching WINMINE:
`t422-s2-20260924T182259312Z-76d627a0-window-lifecycle`. Debugger run
`t422-s2-20260924T182629245Z-93c341e2-window-lifecycle` captured first-chance
C0000005 execute at 83B7:1264 while WOWFax handled WM_ACTIVATEAPP, followed
by C000041D. Its stack maps to WowFaxWndProc, DispatchClientMessage and the
production window dispatcher. The debugger's own zero exit is not a guest
pass. The earlier 30-second debugger attempt timed out before useful WOW
dispatch; the observer bound is now 120 seconds. These unfrozen builds do
not prove the thread-hook edit caused the pre-existing routing defect.

Original wow32fax.c installs a native subclass, saves the tagged previous
Win16 procedure, and calls CallWindowProc on it. Our Get/SetWindowLong seam
preserved that tag, but the matching call still went to modern USER32, which
attempted native execution. Original ntuser/client/clmsg.c::CallWindowProcAorW
and usercli.h::CALLPROC_WOWCHECK instead dispatch WOW-tagged procedures through
the registered pfnWowWndProcEx, with NULL WW. The existing registration bridge
now provides this missing A/W API boundary, retaining native/CPD delegation
and the existing borrow scope. No WOWFax, other mirror body or guest binary
was changed for this repair. Source sweep also identifies wmdisp32 and wumsg
call sites; they share the same compile-time API redirection, not local hacks.

MSVC x86 worker/provider builds pass. Current executable SHA-256 identities:

- ntvdm.exe: 912C78C3B0136B82AB43281C3170F114529C9003C92CE540136FC4A6F5BF004E
- wow32.dll: 349272222601494AC3039D36541535D46596F5619B55FDAF2C249652F2F0678C

Real WINMINE creation/callback/normal-close diagnostic run
`t422-s2-20260924T183129430Z-422f3109-window-lifecycle` passes. All 17 established
DOS cases pass in `O:/winnt/logs/t422-s2-callproc-20260924183213-94b8d5ba-summary.json`.
This retains the previous source-seal, content and forced-final-cleanup limits.
Full A/W/CPD negative coverage is not inferred from WINMINE. An attempted
additional registration-based window fixture failed to link in
`build/M0-T422/S2/window-borrow-e58-callproc` because it pulled unrelated
registrar dependencies; that experimental fixture edit was removed rather
than supplying false-success stubs. The E57 fixture source hash is unchanged.
S2 remains open; current evidence and source changes are not yet committed.

The unchanged production-linked window fixture was then rebuilt against the
current worker/provider objects in `build/M0-T422/S2/window-borrow-e58-final`.
It passes WOW_WINDOW_BORROW_CCPU errors=0 normal=1 exception=1,
WOW_PAGE_DOMAIN_LIFECYCLE_OK and WOW_FIXTURE_OK case=window-borrow. Documentation
governance and `git diff --check` also pass. This covers the E57 assertions on
current objects, not the withdrawn registration/CallWindowProc branch test.

## E59 application completion versus worker retirement observation

The real-window observer no longer kills every process named run16, ntvdm or
basesrv. Selection requires its resolved runtime executable path; cleanup uses
the observed Process object. Failed observation/cleanup still restores the
original SYSTEM.INI bytes. Before forced cleanup, the test records PID,
start time, exact path and CPU time, both after launcher grace and in finally.
This makes resident-worker state inspectable instead of hiding it behind the
test's cleanup step. No production or guest code changes belong to this edit.

Diagnostic run `t422-s2-20260924T184219067Z-cc46cc7d-window-lifecycle` passes
real visible WINMINE destruction/callback and launcher-zero checks on the E58
products. `processes-after-launcher-grace.json` and
`processes-before-forced-cleanup.json` both retain worker PID 18012 and broker
PID 58996 from O:/winnt; the launcher has exited. These are observations, not
a worker-exit pass, leak proof or a requirement that shared WOW exit when an
application closes. Original wkman.c::W32DestroyTask signals an individual
task's ExitVDM when other tasks remain; shared WOWEXEC may remain. Reuse and
complete worker termination therefore need independent tests.

The actual native-thread termination chain was also checked against the build:
original wkman.c calls W32DestroyTask then host_ExitThread; original host
nt_thred.c calls ccpu386exitthread and ExitThread. Its selected x86 compile
uses OPENNT_HOST_CREATE_THREAD_ABI and the existing thread_start_compat.h,
so ExitThread enters opennt_exit_thread. This rules out an absent API binding
as the explanation for the remaining thread-cleanup gap. The unresolved part
is still the original PatchThreadWindows/DestroyThreadsObjects contract and
the wrapper's handling of failed retirement, not a missing thread-exit import.
No change to original CCPU thread state or speculative window-destruction loop
was made. S2 remains active and full WOW32 completion remains unproven.

## E60 real task retirement reveals incorrect idle-notification failure

A default-off TaskRetirement witness now records entry, live-window refusal,
failure or completed retirement, preserving GetLastError and validating trace
path length. It uses the existing window-trace destination, not a new state
owner. Diagnostic run `t422-s2-20260924T184623133Z-8cd3f79b-window-lifecycle`
proves WINMINE task 17BF retired on thread 47148, then another thread 31156
reported TaskFailure stage=wake-input-idle code=5023. The old observer still
reported its narrow window/launcher pass. That verdict is explicitly NOT
successful continued USER operation: the new evidence contradicts it.

The source cause is precise. Original ntuser/kernel/input.c::WakeInputIdle
(line 4904) clears TIF_FIRSTIDLE and, for shared WOW, signals only when
`pti->ptdb->pwti` exists. Original queue.c::xxxInitTask deliberately leaves
pwti NULL for shared idTask=-1 (WOWEXEC). The local wake_input_idle callback
instead treated absent pwti as fatal and called opennt_exit_thread. The
selected original taskman.c call is guarded by TIF_SHAREDWOW and the matching
WOWEXEC event. Missing waiter is therefore a legal reached state, not a
missing implementation or guest defect.

Recovery ladder: the complete original input.c owner depends on USER kernel
queues, W32GetCurrentProcess, private pseudo-events and cursor/priority policy;
it is not directly composable. Existing mirror input.c selects a different
finite grouping slice, not this entire server. The existing ADAPTER-WOW-039
host callback is the smallest same-call-shape boundary for this selected
shared-WOW taskman caller. Restore original optional-waiter branching and
FIRSTIDLE clearing there; do not import a new USER server or change original
task selection. No new mirror diff, guest edit, CCPU edit or external intrusion
is required. Source input.c SHA-256:
B7656BCA55E892A42BA825BE0324211450AD298C40717E1AE5A889D084467529.

The present-waiter branch retains the existing native-event binding; this
repair does not claim complete original OFF/ON/kernel-pseudo-event lifetime
or separate-WOW process idle notification. Those remain S2 interface audit
items, not silently covered by this shared-WOW regression. The focused fixture
now calls the actual production host callback without a waiter (must survive
and clear FIRSTIDLE) and with a real event (must signal and clear FIRSTIDLE).

Provider x86 compilation/link succeeds; current WOW32.DLL SHA-256:
9BDBC7A65BE988282B07AA0EE32308DBFEB696EA1E36398433894F72F175B101.
Worker remains the E58 artifact. New diagnostic run
`t422-s2-20260924T185017204Z-d7ae44e0-window-lifecycle` passes and records
task 17BF retirement complete on thread 59936, with no TaskFailure observed.
The observer now explicitly fails on any TaskFailure even when run16 exits
zero. It still does not prove unlimited future worker health or reuse.
The source/build snapshot remains unsealed and S2 remains active.

Focused `task-lifecycle-e60-idle` finishes with
WOW_USER_TASK_LIFECYCLE errors=0 and WOW_FIXTURE_OK case=task-lifecycle.
Its fixture SHA-256 is
9036F9F15DA86EFF5EE990BB6706B02A3A458CF666B2170ADC6332D414EE1DE5.
All 17 existing DOS routes also pass on this deployed provider in
`O:/winnt/logs/t422-s2-idle-20260924185057-9b625b09-summary.json`.
Documentation governance and whitespace checks pass. These results validate
this repair, not exceptional thread teardown, shared-WOW reuse, full idle
notification coverage or S2 closure. No commit/push is claimed here.

## E61 owner-requested interim source checkpoint

On 2026-09-24 the owner requested committing and pushing all current work,
then continuing S2 closure from a clean checkout. This checkpoint preserves
the accumulated source, diagnostic observers, fixtures and E01--E60 evidence;
it is not S2 acceptance and does not retroactively seal prior incremental
builds. The active packet remains M0 T422 S2. Exceptional cleanup, shared
worker reuse, remaining USER interfaces and sealed full-package verification
remain required. Retained instrumentation and mirror divergences remain
subject to the final minimal-diff review, not endorsed as final footprint.

Before staging, the existing MSVC x86 graphs successfully built/linked the
explicit run16.exe, basesrv.exe, ntvdm.exe, dtmgr.exe, VDMREDIR.dll and
wow32.dll targets. This was an incremental compile/link check; it supersedes
mutable build-output hashes but does not relabel E60 runtime results as runs
of these newly linked artifacts. E60 retains the last deployed runtime and
17-route regression evidence. No binary or guest medium is included in this
source checkpoint. The commit carrying this record is the source baseline;
push success and clean local/remote state must be checked after committing.

## E62 sequential shared-worker acceptance exposes absent registration transport

Checkpoint 806598b9b was pushed to origin/main with an empty worktree and zero
ahead/behind count. Subsequent work adds LaunchCount=1..3 to the existing real
WINMINE observer. Each cycle requires newly appended trace evidence, a visible
application window, original destruction callbacks and launcher zero. Reuse
additionally requires the same worker PID and creation time. No product or
guest code changes belong to this test addition.

Diagnostic two-launch run
`t422-s2-20260924T185830440Z-1c152175-window-lifecycle` fails: first application
in worker 40288 completes and task 17BF retires; second launcher 40284 remains
waiting without an application window. The test reaches its terminal failure
and cleans up only the runtime-path processes. It is not a reuse pass. Traces
contain no new application creation or WM_WOWEXECSTARTAPP window dispatch.

Source inspection identifies a definite missing transport edge:

- Original wow32/wkman.c::WK32RegisterShellWindowHandle calls RegisterWowExec
  for shared WOW. The original BaseClient vdm.c implementation submits
  BasepRegisterWowExec and intentionally has no return value.
- The local OpenNtBaseClientCallServer switch in base_rpc_client.c has no
  BasepRegisterWowExec branch; the default returns STATUS_UNSUCCESSFUL.
- The server-side dispatch table already selects BaseSrvRegisterWowExec.
  Original srvvdm.c registers HWND/PID/TID/process-sequence identity, then
  BaseSrvCheckWOW posts WM_WOWEXECSTARTAPP when queuing another application.
  Compiling both original endpoints did not connect their cross-process edge.

Next repair is the finite authenticated registration RPC, preserving original
BaseSrv registration/notification policy and worker identity validation, with
protocol-version and negative-input tests. Do not replace it with polling or
a second scheduler. This missing edge is established independently of whether
additional defects remain after it is connected. S2 remains open.

## E63 original WOWExec registration connected through authenticated transport

Recovery ladder: original wkman.c, BaseClient vdm.c and BaseSrv srvvdm.c
remain compiled and unchanged. Their private CSR transport cannot compose
on modern Windows; the existing BaseSrv RPC boundary now carries the exact
RegisterWowExec request. It retains authenticated process/generation binding,
requires a registered WOW worker and verifies native window ownership before
dispatching to the original server. No scheduler, polling loop or record
policy is added. The HWND is untrusted input, not process authority. Protocol
version advances from 3 to 4 so an old peer cannot enter the appended RPC.

The first registration transport attempt still failed. The provider import
map showed RegisterWowExec resolving to modern KERNEL32 rather than the
selected original BaseClient in ntvdm.exe. The formal graphs now export and
import that original worker function. A further diagnostic run
`t422-s2-20260924T190834053Z-260d6e49-window-lifecycle` recorded short HWND
1502, native owner PID zero and registration error 5. Original HWND32 is a
sign-extended WORD; the current native window API does not resolve that
identity. The existing WOW32 private-call seam now resolves it with the
existing typed USER handle table before calling original RegisterWowExec.
Invalid, absent and mismatched full handles are rejected; broker validation
is not relaxed. No guest or mirror file changes are required. This is the
smallest same-shaped binding rung; no external intrusion or new owner is used.

MSVC x86 incremental worker/provider links passed. The reservation fixture
passes, including null window, wrong generation, nonworker registration and
foreign-window rejection. Real diagnostic run
`t422-s2-20260924T191255135Z-ef065e41-window-lifecycle` uses the reduced
environment and LaunchCount=2. Its manifest SHA-256 is
`28057E7AB60F904FCB68BAAE3050BD20DD5E0AE44AE75ACA0F2D05DEEBA8EC58`.
The broker trace records full HWND 5768670, owner PID 34576 and RPC result 0.
Both launchers (30564 and 29920) complete with zero in worker 34576, whose
creation time is also checked. Each launch has fresh visible-window and
original callback/destruction evidence. SYSTEM.INI is restored byte-exactly.

This proves the previously failing sequential registration/reuse edge under
that diagnostic profile, not all shared-WOW lifecycle semantics. The harness
force-stops the exact runtime processes after recording their state; it does
not prove autonomous worker cleanup. There is no sealed source/build snapshot
or rendered-content verification, so this is explicitly not S2 acceptance.
Remaining full S2 criteria, including exceptional cleanup and ordinary-profile
verification, remain open.

Post-change DOS regression completed all 17 cases with matching expected exit
results and the observer's text checks. Report:
`O:/winnt/logs/t422-s2-wowexec-20260924191328-dd9efb11-summary.json`.
This covers direct, interactive and nested COMMAND/MEM/EDIT routes and native
child stream/exit cases, not remaining WOW feature acceptance. Documentation
governance and whitespace checks pass. This incremental delivery preserves
S2 as active; it does not claim the fresh-build/full-scope closure gate.

## E64 exceptional retirement ownership and live-window negative test

Post-f97b7a074 source audit establishes the remaining retirement boundary:
original windows/core/ntuser/kernel/queue.c::xxxDestroyThreadInfo calls
createw.c::PatchThreadWindows before handtabl.c::DestroyThreadsObjects.
PatchThreadWindows skips destroyed/foreign windows, transfers the shared menu
window to its desktop owner, and selects the safe procedure using WND.fnid,
explicitly not CLS.fnid because subclassing can make them differ. Object
destruction first handles cached DCs and thread locks, skips process-owned
objects (while withdrawing menu notification references), and destroys only
unmarked objects belonging to this thread. These are not equivalent to
_WOWCleanup or a blanket DestroyWindow loop.

Current wow_cleanup_window has the procedure and thread owner but no per-WND
fnid. Class publication does retain CLS.fnid; that cannot substitute for the
missing field. The native window gateway dispatches the retained procedure
and detaches backing on WM_NCDESTROY. The current retirement hook refuses any
remaining window; opennt_exit_thread and all four normal thread-return
wrappers discard session_thread_unbind failure. Consequently the protection
against early free is not a complete exceptional-thread-exit contract. No
claim is made that ordinary WINMINE close currently exercises this failure.

The existing task-lifecycle fixture now additionally enrolls a genuinely live
native window before its stale-window case. It verifies ERROR_BUSY, retained
current binding/task/THREADINFO, a valid queue event, no leaked exclusive lock,
and no native window destruction. It then destroys the window and exercises
the existing stale-entry rejection and successful retirement retry. Build/run:
verify-wow-task-lifecycle.ps1, case task-lifecycle, current production objects,
MSVC x86, build/M0-T422/S2/live-window-retirement-20260924. Terminal result:
WOW_USER_TASK_LIFECYCLE errors=0; WOW_FIXTURE_OK case=task-lifecycle.
This is native fixture evidence with its existing test parent bindings, not
real-guest exceptional cleanup or a production repair.

Required recovery remains within S2: select the original preparation/object
retirement slice through the existing window/handle owner, preserve per-window
procedure identity and shared-owner exclusions, then verify native-thread exit
with live objects and cross-task survival. Full queue.c/createw.c/handtabl.c
translation units depend on the private USER server and kernel lock/DC owners;
that prevents direct whole-file composition. Their finite source slices and
same-shaped bindings must be evaluated before introducing replacement policy.
Do not bypass the current refusal, release TLS forcibly, use class fnid as a
guess, or call a naive destruction loop a recovered original lifecycle.

## E65 native thread death does not notify the window binding gateway

Original ntstubs.c::NtUserSetWindowFNID records the first actual control
procedure identity (or adds FNID_CLEANEDUP_BIT), confirming why CLS.fnid is
not an authoritative substitute. This does not yet require duplicating that
private native state: modern USER remains responsible for its own controls.
Before importing another cleanup slice, a host-only probe now tests that
public boundary independently of guest execution and project task state.

Source: tests/adapter-mvdm-host-out/wow/native_window_thread_exit_probe.c.
MSVC x86 /MT /W4 build under
build/M0-T422/S2/native-thread-exit-20260924; executable invoked with a 15-second
outer process timeout. Output:
O:/winnt/logs/t422-s2-native-thread-exit-20260924.stdout.log.
All six cases exit zero and leave no live HWND. For custom windows and
subclassed native BUTTON windows, both normal thread return and ExitThread
produce zero WM_DESTROY and zero WM_NCDESTROY callbacks. The two explicit
DestroyWindow controls each produce exactly one of each callback. This is
actual host behavior, not mocked USER or evidence of a WOW guest exit.

Consequences: native USER already destroys its native windows on thread death,
but wow_window_native_proc's WM_NCDESTROY detach cannot be relied on in that
case. Combined with the ignored unbind failure in E64, waiting for a callback
after exiting is not a valid private-binding cleanup design. The current
wow_user_session_binding also uses native TLS storage for current_binding,
so a post-death path cannot dereference the departed thread's TLS as though
it were retained owner state. No production workaround is introduced here.

Refinement of E64's next step: first preserve the native USER-owned cleanup
boundary and determine the finite original-shaped rundown of project WW,
guest backing and task/queue references around it. Do not import per-window
fnid or native control cleanup merely because the historical source used it;
those native mechanics may already be supplied by the host. A successful
binding must prove no calls into departed guest code, no surviving owner/TLS
reference, no loss of another task's objects, and exactly-once release. The
probe provides a regression baseline for selecting that boundary; S2 remains
open and no exceptional cleanup acceptance is claimed.

## E66 ordinary environment is still a real startup blocker

Revalidated unchanged f97b7a074 production artifacts using the existing real
WINMINE observer without removing inherited environment variables. Run
`t422-s2-20260924T192353798Z-4a7f307e-window-lifecycle` terminates as FAIL:
worker 15000 displays illegal instruction CS:05D8 IP:1F8E, bytes
63 61 6C 5C 4D, before a visible WINMINE window. No registration log exists.
The launcher remains waiting until scoped harness cleanup. This is not a
successful ordinary-profile launch, nor evidence that the later registration
repair regressed: those passing runs used a different environment.

Controlled diagnostic run
`t422-s2-20260924T192519856Z-a5066a80-window-lifecycle` uses the prior reduced
environment plus only NTVDM_ENV_SIZE_PROBE containing 4,500 ASCII Z bytes.
It also fails to create a window within 35 seconds (worker 46852 CPU 33.453s),
but does not show the same exception. This supports an environment-shape/size
investigation; it does not prove identical causality or a byte-count threshold.
Both failed harness sessions reached terminal status and restored SYSTEM.INI.

Source review finds a concrete boundary for the next witness:
cmdmisc.c::GetWowKernelCmdLine copies cmdVDMEnvBlk.cchEnv bytes to EnvSeg:0
without comparing CMDINFO.EnvSize, unlike cmdGetNextCmd's DOS branch.
COMMAND tcode.asm has the original carry/AX capacity retry protocol;
cmdenv.c::cmdGetInitEnvironment separately negotiates paragraphs in BX.
These source facts do not yet prove actual guest-buffer overflow or identify
why the initial and later environment contracts differ. Do not modify guest,
truncate production environment, or register an original-guest bug without
the allocation/copy evidence. The WOW host path remains S2-owned work.

The observer now records aggregate environment entry count, total UTF-16
character count, maximum entry length and synthetic-padding length immediately
before launching. No environment values/secrets are logged; aggregates are
not a sealed environment identity. Parser validation passes. Reduced baseline
run `t422-s2-20260924T192756835Z-4191d39c-window-lifecycle` passes again with
worker 53568 and launcher 13468: 39 entries, 3,511 total characters, maximum
entry 270, no padding. This includes observer trace-path variables. The
ordinary parent environment measurement (76 entries, 4,979 characters) did
not include those subsequently installed variables and must not be treated
as that failed child's exact block size. No production source changed in E66.

## E67 capacity witness identifies the already registered COMMAND INIT lifetime bug

Temporary default-off size/MCB probes were built in the existing command
binding with minimal hooks at initial environment negotiation and WOW copy.
Run `t422-s2-20260924T193059763Z-6f24d93b-window-lifecycle` fails before the
WOW copy hook is reached, excluding that later copy as the first cause.
Run `t422-s2-20260924T193238428Z-cd66bcb2-window-lifecycle` shows initial
negotiation growing EnvSeg 0683 from 256 to 7,072 bytes for a 7,061-byte copy.
Both the guest-reported capacity and actual DOS MCB agree. This is not a
host copy outside the allocated environment block.

Final direct witness run
`t422-s2-20260924T193517316Z-2e249406-window-lifecycle` records:

- PSP/MCB owner 05D8; environment segment 0683; MCB type 4D.
- Before negotiation, INIT EnvSiz at PSP:203C is 0010.
- After allocation, before copy: capacity/MCB 7,072, copy 7,061, EnvSiz 01BA.
- After the in-bounds copy: EnvSiz is 7767; the guest later faults at
  05D8:200F with environment bytes 63 61 6C 5C 4D.

The allocation occupies linear 06830--083CF. Both EnvSiz at 07DBC and the
observed instruction address 07D8F are within it. Original rdata.asm::EndInit
shrinks away INIT before allocating the environment, but subsequently reads
EnvSiz and can branch into INIT Alloc_error. Source/deployed COMMAND SHA-256
is 908A77AC617C2D741F0AA1B73F73973DCF29ADC91F092E5BCB02173C8C732C43,
identical to the previously approved
[S35 direct-copy evidence](m0-t420-s35-xms-capability-progress.md#direct-environment-copy-witness).
This identifies the same original guest lifetime defect, not a new WOW host
overflow. It remains the existing TODO, not a passing ordinary-startup row.

Important refinement to E66: the failing inherited-environment runs also add
the observer's trace-path variables (85 entries / 7,542 characters in the
capacity run). They are instrumented inherited-environment tests, not proof
that every uninstrumented normal user launch fails. The observer itself can
push the environment into the original defect. No production environment
truncation, forced DOS=HIGH, injected /E: or guest patch is authorized or used.

All temporary production/mirror hooks and the temporary observer trace switch
were removed after diagnosis. Replay patch and exact diagnostic EXE/MAP are
retained under build/M0-T422/S2/environment-witness-20260924. Patch SHA-256:
070CE5649BC797765227470F2271E768ABFAD9FEFE55E2240D8A2390EC5891F7;
diagnostic EXE SHA-256:
D7E084B4EE7CACDEFD2DD59FD80E1818B6E0436B1BBD42B0149054A331B8271C.
The patch applies to de9dd435b and includes the read-only pinned-map scalar
witness. Guest values are observed, never patched. MSVC x86 recompilation
after removal passed; production source returns to its pre-investigation
shape. These diagnostic runs lack a complete sealed build input snapshot and
do not close S2. Exceptional teardown and remaining USER acceptance continue.

After diagnostic removal, run
`t422-s2-20260924T193759394Z-0dd98e55-window-lifecycle` redeployed the rebuilt
worker and passed the reduced-environment visible-window/close/launcher check
(worker 17652, launcher 3080). This restores the previous diagnostic runtime
baseline, not ordinary uninstrumented acceptance. Only this evidence record
remains changed; no production source or guest change is retained.

## E68 owner-requested S2 checkpoint: sent-message boundary and overlap probe

The owner requests a commit/push and clean worktree before further S2 closure
work. This is an intermediate delivery, not S2 acceptance.

Original OpenNT windows/core/ntuser/kernel/userk.h::xxxReceiveMessages drains
only QS_SENDMESSAGE. The existing ADAPTER-WOW-050 bridge instead removed and
dispatched posted messages and consumed WM_QUIT. The retained smallest public
USER binding now peeks with PM_NOREMOVE | PM_QS_SENDMESSAGE; guest Get/PeekMessage
retains posted/quit ownership. The private SMS/PWND implementation remains an
unavailable USER-server boundary, not a reason to recreate that server.
No original source file or guest medium changes. The two production bridge
files change +8/-24 lines, removing the autonomous dispatch loop and quit state.

The x86 fixture in build/M0-T422/S2/message-receive-20260924 passes:
WOW_USER_MESSAGE_BRIDGE errors=0 sends=1 posts=1 quit=1. It proves synchronous
send delivery and preservation of a posted message and WM_QUIT for their
caller. The formal incremental wow32-provider-r10 DLL rebuild passes.
Real reduced-environment sequential run
`t422-s2-20260924T194828871Z-beb6d225-window-lifecycle` passes both WINMINE
launch/close/parent-completion rounds in worker 52084 (launchers 44004, 39436).
These retain the E67 environment and unsealed-build limitations; they are not
ordinary-profile or complete S2 acceptance.

The observer adds opt-in OverlapFirst with exactly two launches: keep the
first window live, launch/close the second, then verify/close the first.
Before the bridge repair, run
`t422-s2-20260924T194050471Z-27118538-window-lifecycle` timed out waiting for
the second window while the first remained live. After repair, run
`t422-s2-20260924T194440988Z-f7ec1999-window-lifecycle` ended INCOMPLETE with
a Substring start-index exception in trace slicing. Its first task was live
(worker 14364, launcher 23740, HWND 329052); cleanup restored the profile and
stopped scoped runtime processes. Neither run proves overlapping-task success,
and the latter cannot establish whether this repair changes that behavior.
The diagnostic observer's trace-offset handling needs repair before reusing
that evidence gate. Preserve this failed probe in the checkpoint rather than
silently relabel it as passing or discard the reproducer.

Next S2 work remains original cooperative Get/PeekMessage/wait integration,
overlapping-task verification, exceptional task/thread teardown and the other
owned USER mutation/cleanup rows. This checkpoint introduces no new scheduler
or parallel queue and does not transfer those obligations to a later S.

Checkpoint regression: all 17 established DOS routes pass, recorded at
O:/winnt/logs/t422-s2-checkpoint-20260924194905-b399f7b0-summary.json.
A repeat of the native fixture initially exposed a test assumption: an
unfiltered peek need not return WM_QUIT before unrelated host messages.
The assertion now explicitly requests WM_QUIT; after recompilation all five
repetitions pass. No production change was needed for that assertion repair.
Documentation governance and diff whitespace checks pass.

## E69 concurrent workload isolates the USER message-loop scheduling boundary

After checkpoint ee883d9ff, the observer reads each live trace through one
FileStream opened with ReadWrite sharing and one StreamReader snapshot.
It validates character offsets before slicing; a disappeared or shortened
trace fails rather than replaying previous-task evidence. Focused checks on
the retained trace pass full/suffix/EOF reads and reject an out-of-range
checkpoint. This avoids the earlier Get-Content slicing exception without
claiming its underlying transient cause proved. The observer now also writes
an explicit FAIL verdict when an assertion throws, instead of leaving the
initial INCOMPLETE verdict as the only result.

Reduced-environment real overlap run
`t422-s2-20260924T195306882Z-8dac6626-window-lifecycle` reaches a visible first
WINMINE (worker 12848, launcher 43900, HWND 005C084E). The second launcher
7540 reaches the broker, but no second application window appears within
35 seconds. The first worker continues running (43.4375 CPU seconds at the
failure snapshot). The harness reaches its normal no-window failure, not a
trace slicing exception; scoped cleanup restores SYSTEM.INI and stops the
test processes. This run predates the explicit catch/verdict improvement;
its terminal exception, not its initial INCOMPLETE result field, proves failure.

The exact formal DLL map resolves the repeated dispatch RVAs 00033410 and
000334D0 to WU32DispatchMessage and WU32GetMessage respectively. Registration
records only WOWEXEC and the first WINMINE InitTask. Broker evidence records
the second request without a subsequent worker GetNextVDMCommand request.
This narrows the live failure to service progress while the first task owns
the ordinary message loop; it does not yet prove a complete scheduling fix.

Source/build comparison establishes a concrete missing integration edge:

- Original mvdm/wow32/wumsg.c::WU32GetMessage, WU32PeekMessage and
  WU32WaitMessage call their original USER APIs and explicitly rely on them
  permitting a WOW task switch.
- The current formal DLL imports GetMessageA, PeekMessageA and WaitMessage
  directly from modern USER32. Its recovered taskman.c::xxxUserYield is
  reached by the registered YieldTask callback, not by those native imports.
- Original windows/core/ntuser/kernel/input.c::xxxInternalGetMessage yields
  before timer generation and at a no-message Peek exit unless PM_NOYIELD;
  a blocking Get goes through xxxSleepThread. Original queue.c::xxxSleepThread
  calls xxxSleepTask for a 16-bit thread rather than just a native wait.

Implementation must preserve that complete Get/Peek/Wait contract through
the existing source-owned scheduler: filter/remove/PM_NOYIELD, sent versus
posted/quit ownership, timer fairness, queue wake, idle transition, callback
reentry and guest-pointer invalidation. An unconditional yield at API entry
or a new scheduler is not an equivalent recovery. Native USER remains owner
of its queue and message classification. The precise finite facade and its
source-reuse boundary are the next S2 implementation step; no production
scheduler change or multi-task pass is claimed by this evidence delivery.

## E70 message API facade candidate (not acceptance)

E69 selects the existing ADAPTER-WOW-051 lifecycle owner for a same-signature
GetMessageA/PeekMessageA/WaitMessage facade, reached through the already
selected WOW private-access header. No mirror or guest bytes change.
Recovery ladder: input.c::xxxInternalGetMessage and queue.c::xxxSleepThread
cannot directly compose because their raw-input/post/paint/timer queues,
hooks, SMS and kernel thread state belong to the unavailable USER server.
The finite facade retains their pre-timer/no-message yield and task-sleep
ordering while native USER owns message filtering and queue storage; original
taskman.c still owns task selection, event accounting and context handoff.
No external-code intrusion or new scheduling algorithm is selected. The
facade is a candidate until queue filtering, PM_NOYIELD, WM_QUIT, new-input
waiting, timers, multiple tasks and teardown have all been verified.

The candidate separates the native early posted/input/paint scan from timer
generation, invokes original xxxUserYield before the latter, and blocks via
original xxxSleepTask rather than a native GetMessage wait. WaitMessage uses
native queue change bits and the same original task-sleep owner. Risks still
requiring proof include scheduler-only wakes, callback reentry and observation
of input that arrived before versus during a yield. No native message or
borrowed guest pointer is intentionally carried across that yield.

The formal x86 provider rebuild passes. Production-object lifecycle fixtures
pass in message-lifecycle-20260924, message-wait-20260924 and
message-filter-timer-20260924 below build/M0-T422/S2: posted-message filtering,
non-removing PM_NOYIELD peek, removal, WM_QUIT result/code, invalid output
parameter, delayed native input for WaitMessage, timer delivery and the prior
live-window retirement safety checks. A per-task native-message wake sequence
preserves the wait result across taskman's sent-message reception, which can
otherwise examine and clear the native change bits before WaitMessage returns.
It does not select tasks or store queued messages. Multi-task fairness and
nested sent-message behavior are not proved by these single-task fixtures.

Real reduced-environment run
`t422-s2-20260924T195955781Z-7ec76a60-window-lifecycle` reaches the second
application InitTask, unlike E69: worker 56116 now requests and receives the
second launch from BaseSrv. The two-window assertion still fails. Crucially,
the second application calls FindWindow, GetLastActivePopup and then
BringWindowToTop on the existing window. The recorded final dispatch RVA
00036F50 resolves to WU32BringWindowToTop in that build. Thus two WINMINE
instances are not a sound unconditional two-window acceptance assumption;
the observed path is consistent with single-instance activation. The second
launcher nevertheless remains waiting, and that activation path still needs
completion/return proof. Do not count the changed failure depth as a pass.

Original sendmsg.c explicitly couples cross-thread sends involving a 16-bit
participant to DirectedScheduleTask before reply waiting and on return. That
is the next bounded source owner to reconcile with native window activation;
the last dispatch alone is not a native-stack proof of a particular deadlock.
The concurrent test must distinguish second-instance activation/completion
from two independent live applications instead of requiring another WINMINE
HWND. No SendMessage reimplementation or asynchronous activation workaround
is admitted by this observation.

Real sequential run `t422-s2-20260924T200313140Z-73696d2c-window-lifecycle`
passes both WINMINE launch/close cycles in worker 58948 (launchers 52240 and
63124), after the new-input wake correction. All 17 DOS routes pass in
O:/winnt/logs/t422-s2-message-api-20260924200402-1cc791f8-summary.json.
These are incremental/reduced-environment diagnostic runs, not sealed S2
acceptance. Original WOW source call-site review also finds PeekMessage in
wkman's native-child wait, the Winsock blocking hook and the logoff loop;
they share the mapping, but their enclosing blocking APIs are not thereby
proved. S2's message/synchronous-call audit remains open. This delivery adds
97 non-mirror production/header lines and zero mirror diff or overlay lines;
it is necessary candidate wiring, not a claimed footprint reduction.

## E71 native wait-chain proof of cross-task synchronous activation blockage

The production baseline remains e5c0de5b3. Only observation code changes:
the lifecycle observer accepts an optional existing thread snapshot tool,
captures the exact runtime-path worker on failure, and distinguishes
ReactivateFirst from the older two-window OverlapFirst assertion. ReactivateFirst
requires the second launcher to finish successfully while the first window
and its launcher survive, followed by normal first-window destruction.
It does not require a second HWND or claim full visual activation from process
completion alone. The manifest records the optional tool path/hash.

Original two-window run
`t422-s2-20260924T200836316Z-913628cf-window-lifecycle` captures worker 8472:
thread 58472 is in WU32BringWindowToTop and native win32u offset 13AC;
thread 60880 is in WU32GetMessage -> wow_user_task_lifecycle_message ->
sleep_message_task -> xxxSleepTask -> wait_for_task_or_message; thread 12424
is in the WOWEXEC yield/sleep path. The formal map resolves those project
frames. The installed x86 win32u export table places NtUserSetWindowPos at
13A0 and NtUserMsgWaitForMultipleObjectsEx at 5FA0, identifying the native
frames without guessing from the last guest trace entry.

The corrected single-instance run
`t422-s2-20260924T201110264Z-fc5fd7b0-window-lifecycle` also fails: the second
invocation does not complete within 20 seconds. This excludes the former
requirement for a second window as the sole cause of the observed failure.

The existing x86 worker_thread_snapshot observer now also uses public WCT
to record copied thread IDs, object type/status and cycle result. It installs
no hooks, changes no product state or guest media, and does not log object
names. Build command uses MSVC x86 /MT /W4 with user32, dbghelp and advapi32;
artifact: build/M0-T422/S2/activation-snapshot-20260924/worker-thread-snapshot.exe,
SHA-256 51247DA896C642585BF97317DF3E1049BD40450497635F70A806E6C49398EF3D.
Its inherited stack capture briefly suspends/resumes each target thread;
the WCT step itself is read-only. Build passes with the SDK wct.h anonymous
union warning, not a product source warning repair.

Run `t422-s2-20260924T201316071Z-816865f4-window-lifecycle` fails the same
second-invocation completion assertion. Worker 20628's snapshot reports:

- Sender thread 36604: NtUserSetWindowPos -> WU32BringWindowToTop.
- WCT: thread 36604 (blocked) -> object type 2 (WctSendMessageType,
  owned) -> thread 6320 in the same worker.
- Receiver 6320: WU32GetMessage -> the message facade -> sleep_message_task
  -> original xxxSleepTask -> native MsgWaitForMultipleObjectsEx.
- WCT cycle=0: Windows sees the native send dependency, not a complete cycle
  involving the project-owned logical scheduler. Do not label this a WCT-
  reported kernel deadlock.

The wait dependency is now direct evidence, not just a hypothesized final
API. Original sendmsg.c's DirectedScheduleTask send/reply pair supplies the
missing kind of coordination; current Get/Peek/Wait wiring alone is insufficient.
Next implementation must cover the synchronous USER call family, receiver
ownership, nested replies, failure and task retirement with the original
scheduler, not make BringWindowToTop asynchronous or blindly dispatch a
guest callback on an unscheduled thread. The source-audited finite boundary
and focused multi-thread proof remain required before claiming repair.
All runs complete scoped cleanup and restore SYSTEM.INI; no successful
concurrent activation is claimed. Observer syntax and diff checks pass.

## E72 owner-requested checkpoint before continuing S2 closure

This checkpoint preserves test-only changes; it changes no production,
mirror, overlay or guest file. The original-task-order fixture now models
same-worker nested send/reply ordering, explicitly distinguishing manual
scheduler handoff from actual native send/wait acceptance. Its host-operations
table was stale: the added wait slot displaced the find-thread callback.
A typed event-wait mock restores the table shape; the task-order runner
enables C4113/C4047 as errors for its compilation.

The existing x86 run root
`build/M0-T422/S2/same-worker-send-reply-r2-20260924` produced an executable
that returned zero without the final WOW_ORIGINAL_TASK_ORDER marker.
Therefore the former runner's WOW_FIXTURE_OK output is not a pass. The runner
now requires that case's successful completion marker as well as exit zero.
The early exit remains under investigation; neither these new assertions nor
the real synchronous activation path are accepted as passing here.

The owner requested a committed, pushed clean-worktree checkpoint before more
S2 work. S2 remains active with the E71 synchronous-call boundary, callback
ownership and failure/retirement verification still outstanding. This is
preservation of reviewed research/test work, not S2 closure or a new product
deployment; preceding production and DOS regression evidence is unchanged.

## E73 task-order evidence output restored and assertions verified

E72's missing output was not proof of early termination. The r2 link map
resolves `_printf` to original-softpc-host-fixture-roots:fprt.obj, not CRT
output. Original mvdm/softpc.new/host/src/fprt.c returns zero without writing
under PROD. This source and link evidence explains the missing marker without
changing the original diagnostic implementation. The task-order fixture now
formats bounded text and writes its native stdout handle directly, failing
with exit 97 if evidence cannot be emitted.

The current x86 /MT task-order rebuild in
`build/M0-T422/S2/same-worker-send-reply-r3-20260924`, using the existing
wow32-provider-r10 and formal-x86-r9 graph objects, completes with:
`WOW_ORIGINAL_TASK_ORDER errors=0 sequences=5 send_reply=4 same_worker_nested=4 locks=6 destruction=7 waits=4 directed=6 registration=6 init=8`.
The runner verifies both this marker and exit zero; its selected C4113/C4047
error gates pass. This proves the fixture's original task-order assertions,
including the separately modeled nested handoffs, not native send/reply
delivery or real guest concurrent activation. The E71 product defect remains
open. No production or guest file changes and no new deployment occur.

## E74 synchronous USER boundary: original ownership and selected callers

The checked product is wow32-provider-r10/wow32.dll, SHA-256
D946A531F870F7913FCF323FA8E21C1E4D4E48D5F4D1173FCE5E9D13B8377735.
MSVC dumpbin /imports reports 290 named USER32 imports. This is an import
count, not 290 missing bindings or 290 validated capabilities. In particular,
existing facade implementations themselves legitimately import native USER.

Review of pinned OpenNT windows/core/ntuser/kernel/sendmsg.c confirms:
send at lines 1660--1742 sets the receiver wake bit, calls DirectedScheduleTask,
then waits through xxxSleepThread for reply. Reply at 2245--2249 calls the
reverse DirectedScheduleTask and can sleep the receiver. The earlier explicit
ReplyMessage path at 311--313 has the same scheduling obligation. Kernel SMS
ownership also covers sender/receiver death, timeout and nested replies; its
cleanup must not be replaced with a successful local no-op.

Current original wumsg.c::WU32ReplyMessage explicitly warns that guest flat
pointers cannot be reused after ReplyMessage because another task may have run.
This is direct source evidence that merely forwarding native ReplyMessage
without restoring cooperative scheduling is insufficient. Likewise current
wow_window_dispatch_bound publishes callback state and calls the original
DispatchClientMessage, but has no scheduler-reentry boundary. Its borrow scope
and the runtime SRW lock are not substitutes for taskman's ptiScheduled.

The checked caller families requiring one coherent synchronous-call contract
are below; they are a concrete implementation worklist, not passed coverage.

| Family | Selected source/callers | Required distinction |
| --- | --- | --- |
| Explicit send/reply | wumsg.c SendMessage, SendDlgItemMessage via SendMessage, ReplyMessage; wcntl32.c and wmsgem.c nested control sends | Same-thread direct callback versus same-worker cross-thread transfer; nested send and early reply. |
| Activation/position | wuwind.c BringWindowToTop, SetWindowPos, MoveWindow, ShowWindow, SetActiveWindow; wuser.c SetFocus; wuser31.c SetWindowPlacement | Native operations can synchronously notify other windows; not an asynchronous activation replacement. |
| Text/control state | wuwind.c GetWindowText, GetWindowTextLength, SetWindowText, EnableWindow; native dialog-item APIs in the import table | Reads as well as writes can send messages; a setters-only wrapper list is incomplete. |
| Creation/destruction/modal | Existing create-window/dialog bindings, DestroyWindow, DialogBoxIndirectParamA/W, TrackPopupMenu, MessageBoxA imports | Reentrant callbacks and internal native message loops; one HWND target cannot describe every recipient. |
| Callback entry | wow_window_dispatch_bound and original wcall16.c::CallBack16 | Scheduler ownership must precede callback TEB/frame mutation and recursive CCPU entry; restore it on nested return. |
| Timed/external send | wkman.c::SendMessageTimeout heartbeat with SMTO_BLOCK and 1000 ms | Preserve timeout, blocking flags and external native ownership; do not turn it into an infinite cooperative wait. |

Recovery ladder: the original taskman bodies are already directly composed
and remain the task-order owner. Full sendmsg.c cannot be directly composed
as a native USER32 transport: it owns private kernel SMS, Q/THREADINFO, wake
bits, kernel waits and USER server objects at the prohibited server boundary.
The next usable rung is a finite same-shaped native-call/callback binding
around those original scheduling bodies, with native USER retaining message
delivery and reply/timeout/death ownership. No external-code intrusion or new
message queue/scheduler is justified. This records the rejected translation
unit's dependencies; it is not a claim that it was experimentally compiled.

Two tempting implementations are explicitly rejected by source review:
an unconditional yield before the API cannot deschedule the sender throughout
the native blocking call; and an unconditional HEVENT_REMOVEME bracket is not
equivalent to original sends. queue.c uses that sentinel for native multiwait,
whereas taskman.c retains ptiScheduled when nTaskLock is nonzero and its
return alone does not reacquire callback ownership. A correct binding must
account for that lock, same-thread callbacks, nested/early replies, timeout,
receiver death and task teardown before replacing the current production path.
The E73 ordering fixture proves none of those native integration cases.

This audit narrows the next implementation to the complete synchronous USER
boundary rather than a BringWindowToTop-only patch. The real E71 failure is
still open; there are zero production, mirror or guest changes in this entry.

## E75 real native synchronous-send positive and negative transport witness

The task-order fixture now combines the linked original taskman with two
native threads and a message-only USER32 window. There is no guest callback,
window publication or replacement queue in this witness. The test supplies
explicit source-shaped sender/receiver views and a stack-local message view;
the native message and its return value remain owned by USER32.

The positive case performs original DirectedScheduleTask, uses SleepTask's
remove sentinel to leave the native blocking caller, and resumes the receiver
through original SleepTask. During the real native window callback it asserts
both ptiScheduled and CSOwningThread identify the receiver. Reverse original
scheduling precedes native return, and sender resumption verifies ownership
and untouched interprocess send/receive lock counts. The exact result is 114
from input values 41 and 73, with one callback.

The negative control omits the sender handoff. Native SendMessageTimeoutW
returns zero with ERROR_TIMEOUT, the receiver receives no scheduler event,
and no callback executes. Both threads then close their test-only handles and
the message-only window/class. This is an expected negative, not a product
timeout being counted as functional success. The positive callback deliberately
uses the test-owned reverse handoff; production does not yet supply that edge.

Run: verify-wow-task-lifecycle.ps1 with Case task-order, provider
wow32-provider-r10, worker formal-x86-r9 and fresh run root
`build/M0-T422/S2/native-send-r2-20260924`. MSVC x86 /MT compilation and the
completion-marker gate pass: `WOW_ORIGINAL_TASK_ORDER errors=0`,
`same_worker_nested=4`, `native_send=2`, followed by WOW_FIXTURE_OK.
Fixture source SHA-256 at this run:
F728A3935CF124D202F906A5CFCAE68FD7C18E98AECF9ECB052D9DC6191B458F.
This is incremental native-fixture evidence, not a sealed product-guest run.

The witness makes the ordinary send handoff executable rather than only a
source inference. It does not validate early ReplyMessage, native nested
callbacks, timeout-after-delivery, receiver death, task locks, modal loops or
CCPU reentry. E74's complete boundary remains the implementation scope; E71
is still unfixed in production. No product, mirror, overlay or guest changes
and no runtime-package deployment are part of this test delivery.

## E76 USER-domain lock across native receive callbacks

The synchronous-call review found a separate prerequisite in the production
receive boundary. Original taskman.c calls its receive operation while inside
the USER domain. Current native PeekMessage can invoke the window callback
before returning, while the standalone runtime's nonrecursive SRW lock is
still held. A nested USER operation cannot enter that same domain.

Original owner evidence is windows/core/ntuser/kernel/ssend.c MAKECALL and
MAKECALLCAPTURE (lines 137--155): LeaveCrit, KeUserModeCallback, EnterCrit.
This releases the USER data lock, not the separate WOW execution ownership.
The full kernel callback transport is unavailable; its finite lock ordering
can be retained using the existing parent runtime enter/leave ABI around
native queue delivery. No new scheduler, TLS, callback queue, import or guest
change is needed. Mirror bodies remain unchanged. This is the same-shaped
adapter rung, not permission to recreate KeUserModeCallback.

The focused task-lifecycle test queues a real cross-thread native send,
explicitly invokes the production receive operation under its normal USER
lock, and checks that the callback has released that lock but retains the
original CSOwningThread. It reenters the actual lifecycle yield operation,
then verifies receive returns with the data lock restored. The red test skips
recursive entry when the lock is known held, reporting failure instead of
deliberately deadlocking the test process. This remains a native boundary
fixture, not real WOW16/CCPU acceptance.

The baseline run in build/M0-T422/S2/receive-callout-red-20260924 reports
FAIL at both callback lock-state checks and WOW_USER_TASK_LIFECYCLE errors=2.
After the production bridge change, receive-callout-green-20260924 reports
WOW_USER_TASK_LIFECYCLE errors=0 and WOW_FIXTURE_OK. The nested callback now
executes original xxxUserYield without losing task execution ownership; the
outer receive returns with its USER data lock held. Native Peek is bracketed
at each of the bridge's initialization/sync/receive sites, so queue-view
mutation occurs after reacquisition. SEH uses a finally clause for the same
reacquisition, but this test does not claim an injected exceptional callback.

MSVC x86 /MT rebuilds and links WOW32.DLL with only the already-known DEF
DESCRIPTION/duplicate-export warnings. DLL SHA-256:
514136B864C6F9BCB55B47637601C3A08E9224CDCF68E0807B7A6ED3262EFAC3.
Real diagnostic run t422-s2-20260924T204802502Z-6f783d35-window-lifecycle
passes two sequential WINMINE launch/close cycles in worker 55524 (launchers
33320 and 59812), with normal configuration restoration and scoped cleanup.
This incremental reduced-environment run is not sealed ordinary-profile S2
acceptance. Production delta is +22/-3 non-mirror lines (net +19), zero mirror/overlay
or guest changes; it restores missing callback lock ordering, not a scheduler.

Concurrent diagnostic retest
t422-s2-20260924T204849930Z-f48ac934-window-lifecycle still fails the second
invocation's completion criterion. Worker 16332's WCT again identifies
sender 36552 -> SendMessage object -> receiver 47540. Native frames remain
NtUserSetWindowPos (win32u +13AC) and MsgWaitForMultipleObjectsEx (+5FAC),
respectively; the final guest dispatch remains WU32BringWindowToTop (+36F50).
Thus this repair addresses callback data-lock reentry, not the missing
synchronous sender/receiver task handoff. The harness restores SYSTEM.INI
and performs scoped cleanup; no concurrent activation pass is claimed.

All 17 established DOS routes pass on the matching deployed package in
O:/winnt/logs/t422-s2-receive-lock-20260924204951-d36912d5-summary.json,
including direct/interactive/nested COMMAND and MEM, native streams/EOF,
guest exit codes and EDIT return. Deployed O:/winnt/WOW32.DLL matches the
formal DLL hash above. Diff and documentation-governance checks pass.
S2 remains active; this delivery is the production receive-lock repair only,
not closure of synchronous scheduling or the complete WOW32 capability.

## E77 Owner-requested unaccepted implementation checkpoint

On 2026-09-24 the owner requested a commit/push and clean worktree before
continuing S2. This checkpoint preserves the six pending provider/header/test
files: a stack-local native-call/callback scope, sixteen native USER call
bindings, two callback entry bindings and a focused scope fixture. The source
delta is +179/-1 lines, entirely outside the original mirrors and guest media.
It is saved work, not an accepted P or S2 closure.

The latest attempted x86 WOW32.DLL build in
build/M0-T422/S2/wow32-provider-r10 failed with C2011 duplicate definitions
from wowuserp.h in wow_window_words_binding.c after adding the lifecycle
header. No successful link, new fixture pass or runtime validation is claimed
for this candidate. The sixteen bindings are not evidence that all synchronous
USER families are covered. Scheduling order, nested and early replies,
timeouts, task locking, callback failure and cleanup remain to be verified.

No candidate binary is deployed. E76 remains the last tested deployed DLL;
its passing DOS regressions cannot be inherited by this unverified source.
Next work is to resolve the header composition, rebuild, test the actual
scope and original scheduler contract, and rerun real guest activation and
the established regressions. S2 remains active throughout this checkpoint.

## E78 Rejected blanket native-call scope

Removing the redundant wowuserp.h inclusion from window_words_binding fixed
the E77 C2011 failure; the x86 DLL linked. The new native-scope fixture in
build/M0-T422/S2/native-scope-r1-20260924 reported
WOW_USER_TASK_LIFECYCLE errors=0 and WOW_FIXTURE_OK, covering single-task
nested scopes, task-lock preservation and synthetic exceptional unwinding.
It did not prove cross-task synchronous delivery.

Real incremental reduced-environment run
t422-s2-20260924T210529405Z-29d8a709-window-lifecycle failed the second
single-instance invocation. The first WINMINE window appeared (worker 53896,
launcher 51608). The captured thread 58568 stack, resolved against this DLL's
link map, contains WU32SetWindowText -> wow_native_SetWindowTextA ->
wow_window_native_proc -> wow_window_dispatch_bound -> wow_user_call_enter ->
xxxDirectedYield -> xxxSleepTask -> wait_for_task_or_message. Thus the blanket
native-call remove/reentry scope can itself wait during a native callback;
the prior BringWindowToTop wait has not been demonstrated repaired. WCT
reports no cycle; this evidence does not identify every logical dependency.

This disproves sufficiency of the candidate and its single-task fixture.
The six candidate files are restored to the E76 source revision 5909d1359;
the complete rejected experiment remains recoverable in e977765a3. No mirror
or guest changes are needed for this withdrawal. E76's receive-data-lock fix
remains. The harness restored SYSTEM.INI and stopped its scoped processes.
Further work must prove original send/receive/reply scheduling relationships,
including callback ownership, rather than apply unconditional native-call
yielding. S2 and synchronous activation remain unaccepted.

The withdrawal rebuilt all 83 affected x86 graph steps successfully. Comparing
all provider sources and the lifecycle fixture against 5909d1359 produces no
diff. Restored diagnostic run
t422-s2-20260924T210829114Z-5bb97cf0-window-lifecycle passes two sequential
WINMINE launch/close cycles in worker 26540 (launchers 52824 and 8580), with
T422_S2_WOW_USER_REAL_DESTRUCTION_OK. The rebuilt and deployed DLLs both have
SHA-256 E12C1CA584F88F1507FE5AB09AC6C48C2F8586923F510143584C3147B47E2AA9;
this is a rebuild of E76 sources, not the old binary hash. No new complete
DOS regression or sealed acceptance run is claimed here. Diff and
documentation-governance checks pass.

## E79 Native same-thread and early-reply contract witnesses

The E78 candidate is not resumed. Pinned OpenNT
windows/core/ntuser/kernel/sendmsg.c::xxxSendMessageTimeout tests
pti != GETPTI(pwnd) before entering xxxInterSendMsgEx. Its same-thread branch
calls the window procedure without inter-thread SMS scheduling. In contrast,
the cross-thread path records psmsSent, calls DirectedScheduleTask and waits
for QS_SMSREPLY. _ReplyMessage (lines 227--318) marks the reply, wakes the
sender and performs reverse scheduling; receiver completion (lines 2218--2253)
does not repeat that reply when SMF_REPLY is already set. queue.c::xxxSleepThread
also distinguishes exclusive sends, pending replies and incoming sends.
The former unconditional remove/reentry wrapper erased these distinctions.

The task-order fixture adds a real native early ReplyMessage case. Using the
already linked original taskman and test-owned sender/receiver identities,
the callback replies with 114, waits for a test rendezvous proving the sender
resumed under its original execution ownership, then resumes the receiver
and finally returns 999. The sender must observe 114 while the receiver's
completion counter is still zero. This tests native transport plus explicit
test-owned scheduling edges, not production ReplyMessage wiring or a recovered
sendmsg.c body. The new rendezvous event never enters the product.

The first run, build/M0-T422/S2/native-early-reply-r1-20260924, passes the
x86 task-order fixture with errors=0, native_send=3 and WOW_FIXTURE_OK.
The same-thread extension calls native SendMessage on a locally owned window,
asserts InSendMessage and ReplyMessage are false, and verifies original
ptiScheduled, CSOwningThread and event counts are unchanged. This is the
required direct-call counterexample to blanket descheduling.

The combined run build/M0-T422/S2/native-send-contract-r1-20260924 passes
MSVC x86 compilation/linking and reports WOW_ORIGINAL_TASK_ORDER errors=0,
native_send=3, native_same_thread=1, followed by WOW_FIXTURE_OK. Fixture
SHA-256 is 5FE97FEFFC766FD2C55606C0A601537B72C340D0B09228D5A3ED43B40B711C8E.
Only test code and this evidence change; the E78-restored runtime package
is unchanged. No real guest improvement or S2 closure is claimed.

Remaining production obligations are unchanged: exact cross-task binding,
nested sends, early reply versus receiver completion, timeout/death cleanup,
implicit sends and modal callbacks. Public native delivery remains its owner;
the fixture's explicit message identity must not be misrepresented as an
available native SMS identity or introduced as a second product message queue.
