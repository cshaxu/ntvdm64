# opennt-host

OPENNT-HOST-063 selects the unchanged gabObjectCreateFlags table from
`windows/core/ntuser/kernel/handtabl.c` (SHA-256
`F0B56314A93423599F06645D17104391F19A0CA07C3944853B065D8866493F7C`),
original TYPE declarations from `inc/user.h` and OCF declarations from the
existing `kernel/userk.h` subset. The cleanup owner now links this table instead
of accepting caller-defined object classification. Source verification is in
verify-wow-cleanup-bounds.ps1. The selected non-FE_IME profile has 16 types.
HMAllocObject/HMFreeObject are not selected: they generate their own handle
identity and own allocation, whereas this profile uses native USER objects.
This is not evidence that native object enrollment or every resource lifecycle
has been implemented; the original table defines ownership, not publication.

The same original `inc/user.h` subset now optionally exposes unchanged
LARGE_UNICODE_STRING, HEAD/SHROBJHEAD/THROBJHEAD, WND and DESKTOPINFO bodies
for the S42 guest client view (source SHA-256
`ADCF0AC89C4779D9B7908610976F4A47D2385403EDC37496C8AB004B016CF6C1`).
`wow_user_client_view_layout.h` supplies only the finite compile boundary and
x86 offset assertions against the pinned USER.EXE. Its hook count uses the
original Win4 WH_MIN=-1/WH_MAX=12, not the modern SDK extension. This restores
the data schema, not a private USER server or a claim of runtime publication.

OPENNT-HOST-062 recovers the complete WOWCleanup owner and its selector/window
helpers from `windows/core/ntuser/kernel/cleanup.c`, SHA-256
`DFB0FB8D81AFEB555B3631ADC5456F4CEB13142F2E954A6F2B22631C49142E5C`.
ClientDied and its kernel exception dependency are not selected. Explicit
context replaces ambient USER globals; the 13-entry procedure table is indexed
by element count instead of its original erroneous byte count. The WOW BOOL
ABI returns FALSE on allocation/probe/native class-release failure, preserving
the failed node instead of retrying forever, and TRUE on completed branches.
This adds failure visibility, not transactional rollback: prior cleanup actions
remain applied. Original module/task/thread selection and resource ownership
tests are retained. `verify-wow-cleanup-bounds.ps1` checks the complete body.
Formal compilation and the cleanup fixture do not establish production object
membership or 20-slot registration; those bindings remain required.

T417 S4 registers OPENNT-HOST-031: the existing standalone
BaseSrvDOSWorkerWaitPending query in `base/win32/server/srvvdm.c` is not
original OpenNT source. It is a minimal registered divergence in its existing
original BaseSrv owner: the body keeps the original DOS lock, record and event
as the sole state owners, and adds no task state machine or transport policy.
The former private fragment has been retired; strict mirror policy forbids
adding a new file for this small owner-local correction.

This is the original non-MVDM OpenNT host-service mirror. It contains only
complete source-audited packages used by a selected MVDM caller. It is not an
alternate MVDM provider, a compatibility layer, or a general collection of
host utilities.

## Package admission and current package

### S40 original read probe

OPENNT-HOST-061 imports unchanged ProbeForRead from base/ntos/ex/probe.c,
SHA-256 `0D1CAE414B609C624E6BDF04FA28EBD84FF38DBB9687A63D8BC9E840C2F2372B`.
It checks alignment, arithmetic overflow and user address range only; it does
not access pages. ADAPTER-WOW-046 supplies the current process address ceiling
and native RtlRaiseStatus, while retail assertion/page annotations are elided.
No kernel image or service dependency is introduced. Actual caller reads must
remain inside original SEH and use the existing guest-memory boundary. This
core is ready for WOWCleanup but that complete receiver is not installed yet.

### S40 original class registration wrapper

OPENNT-HOST-060 restores _UnregisterClass from the same pinned class.c and
routes existing unregister through it. Original private/public lookup, window
association check and menu pointer selection are preserved. Besides explicit
context, the native DestroyClass binding can fail and its failure is propagated
instead of reporting false success. ADAPTER-WOW-045 keeps outputs private until
success and retains native USER's final live-window/ownership check. Original
class-list selection is removed from the autonomous unregister implementation.

OPENNT-HOST-059 recovers _RegisterClassEx in class.c, separately compiled from
lookup. Original source SHA-256:
`82528443BC5F6C227E0E14FE8A9DBC928BA735F98CF3891744C20C8E1604939A`.
Only an explicit boundary context parameter is added. Original CPD lookup,
registration failure, worker/menu/WOW-word publication and conditional TDB
hTaskWow ownership remain intact. ADAPTER-WOW-044 requires actual class
registration and CPD resolution operations and supplies current thread state;
neither callback has a default success implementation. The existing owned
class entry now stores the source fields, replacing duplicate adapter words
and menu pointer storage. The class-client publish path now uses this core,
verified through native class registration. Worker callback-table publication,
production CPD ownership and WOWCleanup remain incomplete.

### S40 original input membership owner

OPENNT-HOST-058 additionally recovers RecalcThreadAttachment from the same
pinned input.c. Only current-thread parameter plumbing and its Recalc2 call
change. Original queue reuse, allocation ordering, failure break and reference
increments remain unchanged. ADAPTER-WOW-043 requires queue allocation for its
only AllocQueue(NULL,NULL) call; native input publication remains separate.

OPENNT-HOST-057 admits AddAttachment and Recalc2 from original
`windows/core/ntuser/kernel/input.c`, SHA-256
`B7656BCA55E892A42BA825BE0324211450AD298C40717E1AE5A889D084467529`.
AddAttachment is unchanged; Recalc2 adds only an explicit current-thread
parameter. ADAPTER-WOW-043 binds implicit context, desktop membership and
required queue release. The original fixed-point traversal owns explicit
edge and same-process 16-bit grouping. Native queue assignment/publication,
message redistribution and focus/capture remain unbound; these two compiled
bodies are not a claim that ReattachThreads or USER registration is complete.
The original release call may reenter; the owner must retain and serialize
all views. No invented input.c content substitutes for the omitted owners.

### S40 original WOW priority ordering owner

OPENNT-HOST-047 admits only unchanged InsertTask from
`windows/core/ntuser/kernel/taskman.c`, SHA-256
`E37F4D724650D5758EE05C54E181771AA584AEAD15F5A1376B322ECB7016E64E`.
The entire 744-line source has been reviewed. This selected function contains
the actual stable-priority remove/reinsert algorithm needed by original
xxxInitTask, sleep, yield and send/reply scheduling. Its only outward calls
are the original retail-elided CheckCritIn/UserAssert checks; it allocates
nothing, waits on nothing and does not access kernel objects. ADAPTER-WOW-039
provides only a finite list/priority view, not NT4 private structure layouts.
This is a source-owner recovery, not admission of the USER server or a
replacement scheduler. The caller must own serialization and task lifetime.
OPENNT-HOST-048 additionally retains exact WakeWowTask and DirectedScheduleTask
bodies, and CurrentTaskLock with only an explicit original-process argument
instead of PpiCurrent. ADAPTER-WOW-039 supplies the same-domain event counter,
process/thread/message field views and native event signal operation (the
original call discards KeSetEvent's result). Full event/message/thread/process integration cannot be
substituted with no-ops.
Formal compilation and finite ordering tests do not establish runtime use.

OPENNT-HOST-049 additionally recovers unchanged DestroyTask and its exact
CLOSE_PSEUDO_EVENT/ExitWowCritSect support from original kernel/userk.h
(SHA-256 `9260E0BA3CC89974E10B3804ED66D18B9864F4AE7D1175CCE6DDE8EE79D66E8C`).
The original body retains event-count subtraction, waiter/task unlinking,
execution-owner release and successor/WOWEXEC signaling. Binding macros
select the explicit process's shared domain, process-heap records and an
independently retained native idle-event handle. Other kernel objects and
the USER server are not imported. Destruction does not clear the original
thread's ptdb after freeing it; full thread teardown must not reuse that
record or call this destructor twice. Runtime cleanup hookup is still open.

OPENNT-HOST-050 recovers xxxSleepTask and xxxUserYield plus the exact original
EnterWowCritSect helper. Only the current-thread lookup becomes an explicit
parameter, and UserYield passes its original thread to SleepTask. ADAPTER-039
binds the reached non-timeout alertable WaitAny to native multi-event wait,
retains NT4 queue-mask values and delegates message/death/idle/outer-lock
operations to a required host contract. These are not optional success stubs.
The complete production owner must supply them before exposing the callbacks.
Bounded real-event wait tests do not prove USER messages or WOW task execution.

OPENNT-HOST-051 restores xxxDirectedYield from the same pinned taskman.c.
Only PtiCurrent becomes an explicit current-thread argument, also passed to
SleepTask. Original caller/target event posting, transient priority boost,
missing-target return and OldYield policy remain unchanged. ADAPTER-WOW-039
requires lookup by existing host thread ID under the shared USER lock, with
borrowed thread lifetime retained through the call; it creates no ID registry.
Six host cases include two native threads executing the original wait/yield
handoff. Actual WOW task registration and USER output-table hookup remain open.

OPENNT-HOST-052 recovers xxxRegisterUserHungAppHandlers as a true subset of
original kernel/exitwin.c, SHA-256
`45B92F7236FBA49D58E2C3D912126067C3843EC560C28CDD8A20167C62AAEF65`.
Only current-process lookup is an explicit parameter. Original zero-init,
event-reference failure release, callback/client-event recording and process
list publication remain intact. ADAPTER-WOW-040 retains a same-access native
event handle after checking EVENT_ALL_ACCESS and event type; no kernel object
pointer escapes. Full USER shutdown code is not imported. The original caller
registers once per WOW process; repeat registration is not made idempotent.
Native failure/reference tests pass, but production registration and ordered
process teardown remain open. The output ABI still requires a bound wrapper.

OPENNT-HOST-053 restores complete xxxInitTask, SetAppCompatFlags and
_ShowStartGlass from original kernel/queue.c, SHA-256
`0400C034BB5A782E81777E9FD805C07DBCD18D60EC9F1A876CFA67444577E0F8`.
Current thread/process are explicit, InitTask passes its process to the
start-glass helper, and the native-thread image-name fallback reads the
captured process view instead of ETHREAD/PEB. Name capture, startup policy,
TDB insertion, compatibility parsing and publication order remain original.
ADAPTER-WOW-041 exposes the finite original fields and required profile,
input attachment/journal and cursor operations with no production defaults.
USER Unicode literals are retained even in the ANSI provider build; numeric
compatibility parsing uses native NTDLL RtlUnicodeStringToInteger. No full
USER queue/server is imported. Six initialization fixtures use real heap/TDB
and process registration, but controlled profile/input/cursor observers;
they do not prove those native services or a real Win16 task initialized.

OPENNT-HOST-054 corrects one original host defect in SetAppCompatFlags:
FastGetProfileStringW consumes a WCHAR count but the caller passed the byte
size of WCHAR szHex[80]. Pass sizeof(szHex)/sizeof(szHex[0]) instead. Original
profile.c multiplies this count by sizeof(WCHAR), then copies the resulting
string, so the previous 160-character promise exceeded the 80-WCHAR array.
A guarded red test reproduces the wrong requested count without writing out
of bounds; corrected capacity and long-value tests pass. No guest is changed,
and this does not establish a cause for any historical WRITE failure.

OPENNT-HOST-055 retains byte-identical FastGetProfileStringW from original
kernel/profile.c, SHA-256
`8ECBB58954ECA0FECE4AB180BB56A3953B8B4408400D3C7AA01D50B62CAAA531`.
Only the unavailable include/registry/pool boundary is bound by ADAPTER-WOW-042.
The selected PMAP_COMPAT machine key uses read-only native access; other
sections, impersonation and full profile mapping are not imported. Default,
query-buffer allocation, truncation and returned-length policy stay original.
OPENNT-HOST-056 adds only original KEY_VALUE_PARTIAL_INFORMATION and the
complete KEY_VALUE_INFORMATION_CLASS enum from public/sdk/inc/ntregapi.h,
SHA-256 `0FB164317335D8B48C51122182922259C902C7F60AFB28111BAFA9FB75735A6A`.
The unrelated legacy nt.h macro incorrectly said Full=2; it is corrected to
Full=1 (Partial=2), and this reader uses the original enum. Missing-key paths
are native-tested without registry writes. Existing-value/truncation tests
use the actual reader with a test-only opening boundary for existing OS
metadata, not a changed product registry path. Production task hookup remains
unaccepted.

### S40 original ANSI class client composition

OPENNT-HOST-046 compiles the retained ntcftxt.h ANSI class operations in a
second composition of original client.c, alongside its unchanged original
RegisterClassWOWA wrapper (client.c SHA-256
`787EDA33785A87CE9E71202C97B5913E2857F7106FED5BB97103049B1D5E3472`).
ntsend.h now also carries original COPYLPSTRIDW and its three selected ANSI
aliases. The dispatch-only composition is unchanged; no new mirror path is
invented. Existing OPENNT-HOST-037 cleanup correction remains registered.
ADAPTER-WOW-038 provides explicit task/module-version inputs and scoped native
class publication; original capture, validation, menu ownership and old-version
return policy remain original. Formal output-table hookup and task/creation
lifecycle acceptance are still open, not satisfied by host registration tests.

### S40 original class-menu resource client

OPENNT-HOST-043 imports original CommonLoadMenu and LoadMenuA/W from
`windows/core/ntuser/client/clmenu.c` (SHA-256
`DEE4BDB0C79E58993B25AB65C37EB71CC4CFC9A551A9A0E07B5A2F67EB8461CD`).
OPENNT-HOST-044 restores WOWFindResourceExWCover in its existing clres.c owner
(original SHA-256 `C11265557B8406D60928032F3CAA0C106301B5847930C98E356E8C72372808F5`).
OPENNT-HOST-045 also restores MenuLoadWinTemplates, MenuLoadChicagoTemplates
and CreateMenuFromResource from the same source. Only the unavailable
TIF_16BIT read becomes an explicit invocation parameter carried through
recursion, and two cast-postincrement expressions become standard pointer
advancement. CommonLoadMenu's private WOW-only entry supplies TRUE. The
Chicago parser body is unchanged. Private USER object structures remain
excluded; ADAPTER-WOW-037 binds creation/insertion/destruction/help IDs to
native menu objects. No new parser or fake task/TEB state is introduced.
Original
W32 resource callbacks own guest/native discrimination and materialization.
These clients are required by original createw.c/ntcb.h automatic class-menu
loading; registering a packed WOW module with native USER does not supply
that callback. Host lifecycle tests do not close that creation integration.

### S40 original class-word query client

OPENNT-HOST-041 retains the unchanged GetClassWOWWords body from original
`windows/core/ntuser/client/ntstubs.c`, SHA-256
`7DF814A5496CEB8CEDAEE8B51D9F4D1966629F109F15EBF8622805EB6902EB01`.
OPENNT-HOST-042 retains only its seven original call/error/capture/cleanup
macros from `windows/core/ntuser/inc/ntsend.h`, SHA-256
`B708F1FEE46E93735C4C931D556E16DAA9679E6352D72620904D18AD21396ABF`.
Unrelated stubs and private USER precompiled/thread-connect dependencies are
cropped. The direct caller is WOW32 walias.c::FindClass16 through the pending
USER output table. ADAPTER-WOW-035 binds its captured-name lookup to existing
scope-retained class words and original GetClassPtr policy. Full private
kernel CLS/desktop-heap mapping cannot compose; the local view has zero
server/client address delta and never enters guest state. This recovers the
original client algorithm, not a USER server or shared guest graph.
Formal x86 linking and actual capture/TLS/native class lifetime tests pass;
service registration and real FindClass16 guest execution remain unaccepted.

### S40 original USER ANSI capture dependency

OPENNT-HOST-040 retains the complete unchanged `RtlCaptureAnsiString` body
from `windows/core/ntuser/client/rtlinit.c`, original SHA-256
`D8841BD2F13E5825A84F670EC7CC60923977BF80618D737CBAC2208F94C6523C`.
Original class registration and class query require this capture contract.
The remaining USER initialization and capture routines are excluded because
they require the private USER initialization environment, not this bounded
string operation. The retained body and line endings match the original.
ADAPTER-WOW-034 supplies its original IN_STRING declaration and finite heap,
conversion and existing worker TLS bindings; no new TEB or conversion policy
is introduced. The formal x86 provider links the original body. A fixture
using that object and the real worker TLS object passes null/empty, forced
allocation, the original static-buffer threshold, oversized rejection and
two-thread isolation. This is dependency evidence, not class-service or
real Win16 acceptance; allocation/conversion failure injection remains open.

### S40 finite original class metadata lookup

OPENNT-HOST-039 retains only `_InnerGetClassPtr` and `GetClassPtr` from
original `windows/core/ntuser/kernel/class.c`, SHA-256
`82528443BC5F6C227E0E14FE8A9DBC928BA735F98CF3891744C20C8E1604939A`.
Both function bodies, including comments and CRLF, are unchanged. Required
consumers are the pending WC bindings for WOW32 FindPWC/FindClass16 and
class registration/query/unregistration. Same-module public/private classes,
module-HIWORD matching, deferred-destroy filtering and system fallback must
retain this original owner rather than grow a replacement lookup algorithm.

The complete outgoing closure is list traversal and HIWORD comparisons.
`wow_class_lookup_bindings.h` (ADAPTER-WOW-028) supplies caller-owned native
metadata links and binds the original system/client-module roots to an
explicit context. This is not the kernel CLS/PROCESSINFO layout, a guest
projection or a runtime USER server. Full class.c cannot compose: its other
functions require desktop heaps, process/thread objects, kernel locks and
handle destruction. Those are excluded; native USER still owns registration
and windows. No kernel allocator, scheduler or USER service is imported.
This finite semantic slice follows the package-interface stopping boundary,
not directory-wide server recovery. No autonomous lookup implementation is
superseded because none has been admitted. The formal WOW32 graph selects
the original slice; metadata population, synchronization, WC lifetime and
real guest callers remain unconnected and unaccepted.

### S40 original USER window dispatch owner

OPENNT-HOST-038 retains client/client.c::DispatchClientMessage unchanged,
from original windows/core/ntuser/client/client.c SHA-256
`787EDA33785A87CE9E71202C97B5913E2857F7106FED5BB97103049B1D5E3472`.
Other USER client entry points and the private precompiled environment are
cropped. `wow_window_dispatch_bindings.h` supplies a synchronous HWND/WW/
callback view in place of kernel PWND and global callback lookup. The original
retail assertion remains non-evaluating. The original tagged-procedure branch
and callback arguments/return remain byte-exact; no guest pointer decoding or
message-thunk algorithm is duplicated. The function enters the formal WOW32
x86 link and focused nested-call test. Real window lifetime, callback routing
from the native window gateway and Win16 acceptance remain incomplete.

### S40 USER class client recovery in progress

OPENNT-HOST-036 retains the required class-client subset of original
`windows/core/ntuser/inc/ntcftxt.h`: StringDuplicate, InitClsMenuName,
RegisterClassExWOW and UnregisterClass. Source SHA-256 is
`E7F3D42CE8939BBE0B14EA34BC693107334C19F8B2D7862EC15B75A501D11DAB`.
Direct MVDM consumers are wuclass.c's RegisterClassWOWA output slot and
direct UnregisterClass call. The other text API families and private USER
environment are cropped. The finite outbound contracts are native allocation,
original string conversion/capture, expected-version/client metadata, GDI
validation, and NtUserRegisterClassExWOW/NtUserUnregisterClass resource
bindings. Full USER kernel/desktop/CSR imports are not admitted.
This recovered source is currently selected by the x86 class lifecycle
fixture only. Production callback dispatch, class/clone metadata and shared
graph integration remain open; no partial output table is installed.

OPENNT-HOST-037 adds exactly two LocalFree calls at InitClsMenuName's
capture-error exit. Original capture may fail after duplicating both client
menu strings, before a class owns them. The unchanged source retains two
allocations in the negative control; the corrected mirror retains zero.
Success, resource-ID and failed-kernel-registration rules remain original.
The original unregister client is tested on failure followed by successful
cleanup. See the S40 USER evidence for source/test identities and limitations.

### S40 finite USER bitmap owner

OPENNT-HOST-035 retains original `windows/core/ntuser/client/cldib.c`
scaling code, replacing only its precompiled-header dependency with the
consuming bitmap binding. Source SHA-256:
`E16FB36805F44767B45B597510A6C08D1DBC47271783E5D1F8F09273C0F782FB`.
It supplies clres.c's SmartStretchDIBits/ScaleDIB closure using public GDI
and local allocation; it is not a USER server import. Original ChangeDibColors
is also retained in clres.c, with original usercli.h stretch/color macros
bound to public GDI/GetSysColor. The bitmap owners and bounded DC environment
now enter the WOW32 link. Generic stretch/color-flag acceptance is
not implied by the fixed-argument WOW bitmap regression.


OPENNT-HOST-033 selects original `windows/core/ntuser/client/clres.c`
bitmap conversion functions and constants/old-header structure as a subset.
OpenNT and OpenNT-4.5 copies have identical SHA-256
`C11265557B8406D60928032F3CAA0C106301B5847930C98E356E8C72372808F5`.
Original WU32LoadBitmap reaches this owner through WOWLoadBitmapA.
The retained chain is CopyDibHdr/HowManyColors/TrulyMonochrome,
ConvertDIBBitmap/BitmapFromDIB/CreateScreenBitmap/Convert1BppToMonoBitmap.
Unrelated resource families and the USER precompiled shell are cropped;
native fixtures and WOW32 select the bounded DC/capability binding.
Resource-name loading and actual guest/output-table integration remain required.

The original WOWLoadBitmapA body is now retained unchanged as part of
OPENNT-HOST-033. Its MBToWCS macro reuses the already selected original
chartran.c MBToWCSEx(0, ...). Only its fixed zero-dimension/zero-flag LoadBmp
call maps to public LoadBitmapW; this does not supply a general replacement
for original LoadBmp. The finite binding uses LocalAlloc/LocalFree consistently
with the existing chartran allocator. Native data, named-resource and
missing-name/ID fixtures pass; real guest/output-slot acceptance remains open.

OPENNT-HOST-034 fixes the original old-icon/cursor header branch: capture
`upOldIcoCur->abBitmap` before redirecting `upbih` to local `Fake`, because
the macro aliases that variable. Previously returned bits pointed into the
callee's stack. Only the order of two assignments changes. The fixture
selects unchanged upstream as a failing negative control or this correction
to assert the input bits and converted header. This is a host correction,
not a guest change or evidence that current Win16 workloads use old formats.

### S40 finite USER DDE data owner

OPENNT-HOST-032 admits the original `windows/core/ntuser/client/hdata.c`
`FreeDDEData` body as a true subset. Direct callers are original
`mvdm/wow32/wumsg.c` through `pfnOut.pfnFreeDDEData`. Source SHA-256:
`3BEC21C4BB3CAEF0798208693A89012DF6005C6A330F0F670F974A1CE821FCA2`.
The full translation unit requires excluded USER DDEML instance, transaction
and handle machinery; those APIs are cropped with registered markers.
The retained function owns original release-flag and format-dependent nested
object cleanup, unchanged. Its outgoing closure is GlobalLock/Unlock/Size/Free,
DeleteObject, DeleteMetaFile and DeleteEnhMetaFile, all public native APIs.
`wow32-dll/include/wow_dde_data_bindings.h` retains the original DDE_DATA
layout and retail USER memory macros; it owns no replacement DDE algorithm.
This source is selected in the WOW32 provider build and native fixtures,
but not registered as a working WOW output slot. Evidence and remaining
real-Win16 acceptance are in the indexed
S40 USER profile audit. No full USER package or new server is imported.

This root is not limited to BaseSrv/BaseClient. Every separately admitted
non-MVDM OpenNT host package belongs below this root with its original
relative path preserved. A package is admitted only after the shared tracker
records its direct `mvdm-host` caller, complete outgoing boundary, final file
disposition and any required package-private adapter binding. Discovering a
same-named function body is not package admission.

`base/win32` is the first selected package slice. It contains the original
BaseSrv/client VDM command implementation and its direct original declaration
carriers:

- `base/win32/server/srvvdm.c`, `srvvdm.h`, `basesrv.h`, and `srvinit.c`;
- `base/win32/client/vdm.c` and `basedll.h`; and
- `base/win32/inc/base.h`, `basevdm.h`, and `basemsg.h`.

Every file above is byte-identical to its canonical selected OpenNT source
except registered true subsets required by a selected formal runtime.
The source selection, rejected OpenNT-4.5 `vdm.c` candidate and initial
interface dispositions are recorded in
[`m0-t291-s5-p2-basesrv-source-package-audit-001.md`](../../docs/etc/evidence/m0-t291-s5-p2-basesrv-source-package-audit-001.md)
and the shared MVDM/OpenNT host trackers.

`base/ntos/rtl/environ.c` is the selected D11 RTL slice. It is directly
reached by original `base/win32/client/vdm.c` and
`mvdm-host/dos/command/cmdenv.c`. Its source body was imported from
`O:\repos.external\OpenNT\base\ntos\rtl\environ.c` (source SHA-256
`6c67b0c09c40e9186ce28a735221634bade1d9402be085d965223092aa7d4f00`).
The selected body has only its three include/declaration seams changed:
`ntrtlp.h`, `environapi.h`, and `zwapi.h` retain the finite standalone
PEB/VM and private-symbol boundary; they provide no environment algorithm.
The imported file SHA-256 is
`4c0e0e870d97f351cd8b046c2901ba841b439c83b75fdb8a6e936b0c89cf2b4b`.

`base/ntos/rtl/error.c` and its generated `error.h` table are the selected
D12 RTL slice.  They are directly reached by Base VDM, COMMAND, DEM,
Redirector, NetLib and SoftPC error consumers.  They were imported from
`O:\repos.external\OpenNT\base\ntos\rtl\error.{c,h}` (source SHA-256
`88bf09ab98778e0ca58d9d232d3b7aba6115223f43eddf981f6e06b786e77e48` and
`179794837a8610488218e5e48768d4d79f3d160bb952c92b172d12f6807984d2`).
The imported contents differ only by the repository terminal-newline form.
The original table and mapping algorithm remain intact; the sole finite
standalone binding supplies the original per-thread `LastStatusValue` field.

The package is a source mirror only at this point. It is not an enabled build
or runtime input until every reached CSR/private-host interface has a named,
same-shaped disposition. A later package-named subfamily under
`adapter-opennt-host` is permitted only for a private interface that the
owning original source proves cannot compose directly; it must not absorb
MVDM or generic Win32 semantics.

`netapi/netlib/ntstatus.c` is the second selected package slice. It is the
complete, byte-identical status-conversion unit directly reached by original
`mvdm-host/vdmredir/vrnetapi.c`. It preserves only the original
`NetpNtStatusToApiStatus` and `NetpApiStatusToNtStatus` algorithms; it does
not admit NetAPI services, RAP transport, NetBIOS, CSRSS, or a network
provider. Its identical three-source provenance and bounded disposition are
recorded in
[`m0-t324-s1-original-redirector-owner-binding-baseline-001.md`](../../docs/etc/evidence/m0-t324-s1-original-redirector-owner-binding-baseline-001.md).

`netapi/netlib/copystr.c` is the third selected NetLib slice.  It is the
complete, byte-identical OpenNT and OpenNT-4.5 conversion unit directly
reached by original `mvdm-host/vdmredir/vrnetapi.c` through
`NetpCopyWStrToStr`.  Its source-defined OEM conversion and bounded
destination contract remain in the original translation unit; it does not
admit a NetAPI/RAP transport or a replacement Redirector provider.  Its
secondary NetLib references are intentionally exposed to the formal link
audit rather than reimplemented in an adapter.

`netapi/netlib/allocstr.c` and `netapi/netlib/initoem.c` are the complete,
byte-identical OpenNT/OpenNT-4.5 direct NetLib support units reached by that
same `copystr.c` body.  They retain the original allocation, OEM-string
initialisation and error direction; their remaining NetLib dependencies are
likewise admitted only when the formal link audit proves a finite original
source path.

`netapi/api/apibuff.c` retains the source-identified `NetapipBufferAllocate`
true subset required by the selected `allocstr.c` unit.  It preserves the
original internal forwarding contract to public `NetApiBufferAllocate`; the
unreached RPC/MIDL allocator routines from the same original file are not
admitted merely to make the larger historical API product shell link.

## Divergence register

OPENNT-HOST-014 restores BaseGetVdmConfigInfo (209 original lines, one call-site
change) with original SHA-256
`2d57d7b0edaf2534e9a5654d3a2e30a6d91e2a24aea04fcc9e282af747a8c667`.
Its hardcoded system32/ntvdm search is replaced by the configured worker path
boundary; original quoting, -f/-i/-w/-a construction remains source-owned.
base_config.h/.c supply validated immutable app configuration instead of the
NT4 WOW registry. No host registry, environment or system-directory mutation.

OPENNT-HOST-014 also restores unchanged BaseCheckForVDM (39 lines) from the
same pinned base/win32/client/vdm.c for the launcher's task-exit query, not
image classification. Normalized function SHA-256:
`c6cf2dfdab281ee73c257c5f0a051eebe3439355f273d4a2a5eae5fa7e9d7b85`.
Native NtQueryEvent uses the original event ABI through declaration-only
base_event.h; original BaseSrvGetVDMExitCode retains completion consumption.
No replacement exit policy or full Kernel32 process shell is imported.

OPENNT-HOST-031 admits the three local capture-memory functions from pinned
OpenNT `base/ntdll/csrutil.c`, not its CSR/LPC transport. Original source file
SHA-256 is `7a176482498e1aca00e9651c326a9b62a8ba2ada0aa7f90f8b585acd1a376e6a`;
the unchanged 235-line group has LF-normalized SHA-256
`41d54b575c6895809c956136c3a35067dc3b87610a6226d8ec23fab431e11cab`.
CsrAllocateCaptureBuffer, CsrAllocateMessagePointer and CsrFreeCaptureBuffer
retain their pointer tables, alignment and original calling convention (/Gz).
A local process-owned heap replaces the port heap; tags are metadata only.
No shared port section, pointer rebasing or CsrClientCallServer body is imported.
The original library replaces the test-owned allocator; heap walks, actual
heap exhaustion, source identities and link ownership are verified in S2.

OPENNT-HOST-030 admits only `_UserTestTokenForInteractive` from pinned OpenNT
`windows/core/ntuser/server/exports.c` (original file SHA-256
`dbeb0ad48349bf5887a1333f21b427d8e0fdd3cb226977a9dc786373e961c71f`).
This directly serves srvvdm.c's OkToRunInSharedWOW. Its 67-line body is unchanged
(LF-normalized SHA-256
`7b6c9072220144e4b77aca973682838333994c3bc962eb802143e210e912a520`).
The true subset crops all unrelated winsrv exports and replaces precomp.h with
the finite private binding. Outgoing closure is native token query, public
LocalAlloc/LocalFree and the admitted scoped interactive-logon predicate.
No USER/Win32k subsystem, windowstation list, CSR shell or new token-query
policy is imported. Original BaseSrv SYSTEM/impersonation policy is unchanged;
actual SYSTEM impersonation is not tested by this fixture. Source-first
admission and the exact replacement predicate are recorded in S2 evidence.

T412 S2 registers OPENNT-HOST-028 in `base/win32/server/basesrv.h`: the
`OPENNT_BASE_VDM_SERVER` compilation group includes the finite BaseSrv-private
declaration binding and omits the unrelated DFS device header. The original
header is otherwise retained and srvvdm.c remains byte-identical. This removes
the generated empty DFS header from the selected original-owner library test.
OPENNT-HOST-029 restores original RtlConvertLongToLuid in the selected
`public/sdk/inc/ntrtl.h` subset, including its __inline storage shape; only
trailing blank-line whitespace is normalized. Origin is the same pinned
OpenNT public/sdk/inc/ntrtl.h. No generated function-body replacement is
needed; positive and negative sign-extension assertions pass. See S2 evidence.

T412 S2 extends OPENNT-HOST-014 with the complete original GetNextVDMCommand
body (390 lines including its signature/comment), selected by
`OPENNT_BASE_CLIENT_VDM_COMMANDS` for the focused original-client/server graph.
The default product still selects the environment cohort; the command provider
is not yet selected in the deployed product. Source: pinned OpenNT revision
`5c5b979ec08c17d3ca2eb70e8aad62d26515d01c`,
`base/win32/client/vdm.c`; LF-normalized restored function SHA-256
`47cf285cbc505d9d43a50af16e283ea51d1d9ef8d2ff093795086d0ca327addd`.
Only cohort guards and the finite declaration include are added around it;
the function body is unchanged. The same cohort now also restores original
ExitVDM, SetVDMCurrentDirectories, GetVDMCurrentDirectories, CmdBatNotification
and RegisterWowExec as one contiguous 320-line source group. Its LF-normalized
SHA-256 is `2936de23dcbbdec23cf9ea23416606a7a3faeb96473d407e4bfaee00f89ac3aa`,
from the same pinned source/revision. No additional outgoing CSR operation
shape is needed beyond the four declared bindings. See the
[S2 execution evidence](../../docs/etc/evidence/m0-t412-s2-original-lifecycle.md).

BaseUpdateVDMEntry is also restored unchanged in that cohort (57 lines;
LF-normalized SHA-256
`de56cfccd08922ae57df7332cc8df70f60db186f44f177336188c2d236e9fd26`).
Its process-reference to wait-handle update is exercised against the original
server. The independent `OPENNT_BASE_CLIENT_CLASSIFIER` cohort now restores
GetBinaryTypeW (211 lines) and BaseIsDosApplication (60 lines), unchanged from
the same pinned source. LF-normalized hashes are respectively
`f7fd9766ea08e676a2fd457fb3fa508384630640032ce0615f752dc8a43f9fbd` and
`9487407a2efc02764466cedbabb4e89424fd12d87d11601cce9fa5ad477f3711`.
Its finite private header renames the imported Kernel32 symbol collision,
uses native NT file opening and supplies image declarations. The path binding
requests the native full-path result and marks the original relative result
empty, retaining the original full-path branch without passing an undersized
NT4 structure to modern NTDLL. Classifier bodies/branches are unchanged.
The focused x86 graph selects these bodies; final run16 selection is pending.

The command cohort also restores complete BaseCheckVDM (876 lines, same pinned
source; LF-normalized SHA-256
`579840e2d5598051d157d9e92871027b543ecfcba8d299a745928bfd71c6386e`).
No body change is made: command-tail/OEM conversion, PIF search, current
directory, startup capture and cleanup remain original. Private declarations
bind the finite process fields, modern Console capability predicate and the
historical Microsoft swprintf signature. The actual original client/server
test covers dispatch/deep copy, hotkey conversion and pre-dispatch failures.

| Identifier | Original purpose | Reason for divergence | Selected implementation | Files |
| --- | --- | --- | --- | --- |
| OPENNT-HOST-001 | `public/sdk/inc/ntexapi.h` declares the historical NT Ex system-service surface. | The selected non-MONITOR `softpc.new/host/src/nt_eoi.c` preserves its original include, and `nt_timer.c` reaches only its performance-counter/tick declarations. Importing the complete header would falsely enlarge the user-mode host closure. | Retain the source-identified true subset as the same-named OpenNT declaration carrier. The selected MVDM sources remain unchanged; no Ex service, controller, or kernel-VDM substitute is supplied. | `public/sdk/inc/ntexapi.h` |
| OPENNT-HOST-002 | `public/sdk/inc/ntpsapi.h` declares historical process/thread services. | Reached `nt_timer.c` needs only `NtAlertThread(HANDLE)` through the broad NT4 platform include closure; importing all process services would falsely enlarge the modern host boundary. | Retain that one byte-identical declaration. Its session-owned cancellation implementation belongs to the named MVDM host-out adapter, not this source mirror. | `public/sdk/inc/ntpsapi.h` |
| OPENNT-HOST-003 | `public/sdk/inc/ntrtl.h` is the original NT runtime declaration carrier. | The complete NT4 header reaches private product declarations outside the selected user-mode closure. | Retain a source-identified true subset of declarations reached by imported MVDM/OpenNT source; implementation binding remains in `adapter-mvdm-host-out/win32`. | `public/sdk/inc/ntrtl.h` |
| OPENNT-HOST-004 | `ntrtl.h` supplies the historical `DbgBreakPoint` declaration. | Modern SDK selection does not expose this exact declaration consistently. | Preserve only the source-facing declaration; the debugger adapter owns any enabled behavior. | `public/sdk/inc/ntrtl.h` |
| OPENNT-HOST-005 | `ntrtl.h` supplies the NT4 `TIME_FIELDS` layout and time-conversion declarations. | The selected modern include route omits this small public-NTDLL subset. | Preserve the original layout/declarations only; no CMOS policy or host behavior is added. | `public/sdk/inc/ntrtl.h` |
| OPENNT-HOST-006 | `ntrtl.h` supplies SoftPC timer RTL arithmetic, critical-section, and process-control declarations. | The selected modern SDK does not supply a coherent source-compatible declaration set. | Preserve the reached signatures and original `(HANDLE)-1` pseudo-handle spelling; an adapter must provide any non-exported behavior. | `public/sdk/inc/ntrtl.h` |
| OPENNT-HOST-007 | `public/sdk/inc/nturtl.h` carries NT runtime utility declarations. | Its full historical source closure is broader than the selected MVDM callers. | Retain only the reached declarations; this mirror adds no filesystem or process behavior. | `public/sdk/inc/nturtl.h` |
| OPENNT-HOST-008 | `ntpsapi.h` declares the reached `NtAlertThread(HANDLE)` surface. | The source-shaped public adapter must provide the historical name locally; ordinary `NTSYSAPI` import decoration would force an unavailable/private NT4 import instead. | Retain the original signature and select a local declaration only in the named adapter-backed SoftPC build; other consumers retain the historical declaration form. | `public/sdk/inc/ntpsapi.h`; `../adapter-mvdm-host-out/win32/{include/nt_thread_alert_compat.h,source/nt_thread_alert_compat.c}` |
| OPENNT-HOST-009 | `public/sdk/inc/ptypes32.h` carries the small historical 16/32 portability declaration set used by original SoftPC serial sources. | `nt_com.c` and `nt_wcom.c` directly include it, while the modern SDK has no same-named carrier. | Preserve the byte-exact selected OpenNT public header (SHA-256 `fbc65366df6b091aadbdc0ce12eb842bbd48691ccb1b9ce34c6d503226a7139b`); it contributes declarations only and no serial provider behavior. | `public/sdk/inc/ptypes32.h` |
| OPENNT-HOST-010 | `ntrtl.h` declares the original mutable RTL environment APIs reached by COMMAND. | The selected declaration subset initially omitted this small source-facing cluster, leaving original callers to compile by implicit declaration. | Retain the original declaration shapes; selected consumers enter the private `OpenNtRtl*` symbol domain implemented by the admitted original D11 body, not modern NTDLL or an adapter algorithm. | `public/sdk/inc/ntrtl.h`; `base/ntos/rtl/{environ.c,environapi.h}` |
| OPENNT-HOST-011 | `ntrtl.h` declares `RtlOemToUnicodeN`, reached by original SoftPC keyboard conversion. | The selected true subset omitted this fixed-buffer OEM-to-Unicode form although the original `nt_keycd.c` includes and calls it. | Restore only the original declaration shape; the public Win32 implementation remains adapter-owned. | `public/sdk/inc/ntrtl.h`; `../mvdm-host/softpc.new/host/src/nt_keycd.c` |
| OPENNT-HOST-012 | `ntrtl.h` declares OEM conversion forms reached by original NetLib `copystr.c`. | The admitted true subset omitted the fixed-buffer conversion and size contracts although the selected original Redirector dependency includes them. | Restore only the original signatures; the selected x86 NTDLL/import-library binding is verified as part of the Redirector network closure. | `public/sdk/inc/ntrtl.h`; `netapi/netlib/copystr.c` |
| OPENNT-HOST-013 | `netapi/api/apibuff.c` provides historical public and internal NetAPI buffer entry points. | The selected Redirector NetLib closure reaches only its `NetapipBufferAllocate` compatibility forwarder; importing the other functions falsely pulls an RPC/MIDL product shell. | Preserve the original function unchanged as a source-identified true subset and call the public modern `NetApiBufferAllocate` contract. | `netapi/api/apibuff.c`; `netapi/netlib/allocstr.c` |
| OPENNT-HOST-014 | `base/win32/client/vdm.c` supplies `BaseCreateVDMEnvironment`, `BaseDestroyVDMEnvironment`, and `BaseGetEnvNameType_U` before original Base creates an NTVDM child. | The full source file pulls the excluded Base/CSR process-creation product shell, while the selected CLI needs this exact environment projection before its direct SoftPC entry. | Preserve the three original bodies as a source-identified true subset. Its clone uses the admitted original D11 RTL body; the separate adapter owns only current-process installation/restoration around direct entry. | `base/win32/client/vdm.c`; `base/ntos/rtl/environ.c`; `../ntvdm/win32/mvdm_base_vdm_environment.c` |
| OPENNT-HOST-015 | `base/ntos/rtl/environ.c` provides original create/clone/query/mutate/destroy ownership for ordered UTF-16 MULTI_SZ environments. | Original MVDM/VDM callers require the complete D11 algorithm, but standalone cannot expose NT4's real PEB or private kernel VM service shell. | Preserve the selected body under its original-relative path. Its only outgoing closure is a private PEB projection, PEB lock, public user-mode NTDLL `Zw*VirtualMemory` declarations and private symbol names, supplied by the bounded adapter seams. | `base/ntos/rtl/{environ.c,ntrtlp.h,environapi.h,zwapi.h}`; direct consumers `base/win32/client/vdm.c`, `../mvdm-host/dos/command/cmdenv.c` |
| OPENNT-HOST-016 | `base/ntos/rtl/error.c` and generated `error.h` map NTSTATUS values to DOS/Win32 errors and record the reached status in the TEB. | Modern NTDLL exports the historical names, so it cannot be mixed with the selected original table under the public symbol names; the standalone process also cannot expose the NT4 TEB layout. | Preserve the selected original source/table under the original-relative path and route selected callers to a private symbol domain. The adapter contributes only the source-shaped TLS `LastStatusValue` carrier; it supplies no mapping entries or fallback policy. | `base/ntos/rtl/{error.c,error.h,ntrtlp.h}`; `public/sdk/inc/ntrtl.h`; `../adapter-mvdm-host-out/win32/{include/nt.h,source/opennt_support_rtl.c}` |
