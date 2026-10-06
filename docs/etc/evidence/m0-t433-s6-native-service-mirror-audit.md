# M0 T433 S6 — NTSRV native service mirror audit

## Question, inputs and boundary

Owner admits S6 and requests the mirror-diff audit before production edits.
Can one AMD64 NTSRV retain the original BaseSrv algorithms, existing fixed32
task/receipt fields and mixed-width control/resource contract with incremental
original diff no larger than delivered RUN16 S5?

Baseline: S5 production39fe1f8a93c855a59298877ec10a93d8dd8bb92d,
closurea009cdea975e25e9719af54899909db4317978f2; sealed
build/M0-T433/S5/r007-final-runtime, published O:/winnt/system32.
APP0.0.433/RPC41/I/O25. NTSRV is still I386. Cache and published NTSRV hashes
both31DF5B7786A6FF7BD1CECEB3044E10FB0DAE72CD1A39E3832DBF22CE97C206A6.
Actual ntsrv.exe.map hash
AFC2006D8BE220F2BC80A66C053DA8D0493168C2B0B841B679DCBC5C51C94067.
Graph/hash/revision and source/member ledgers are retained under
build/M0-T433/S6/r001-audit through r005-layout. All current production source
and published binaries remain unchanged; proposed source copies are build-only
research, never selected production or publication inputs.

Pinned upstream srvvdm.c at O:/repos.external/OpenNT/base/win32/server has hash
C1E2177C6C00679D85CFA475F620841F6736B0E56D8DBF790B71AFE33E1ED80B;
current source hash342D10E9610778DF828E33FF4095E7FDB64A94E3BC899A2DBF43F02338814FBB.
Byte/normalized content already differs through existing registered
OPENNT-HOST-031/064 boundaries. They are retained baseline, not new S6 edits;
the audit does not call the current file byte-exact upstream or discard those
verified wait/reentry mechanisms. r001 original-provenance.json pins this fact.

## Image-matched selected closure

The actual x86 map selects35 translation units. Original bodies are only
base/win32/server/srvvdm.c and base/ntos/rtl/error.c. The former owns DOS/WOW
records, queues, completion, block/reentry and cleanup. The latter remains
the already native-compatible original error table/algorithm. No environ.c,
time.c, largeint-selected.asm or movemem-selected.asm member is pulled from
the linked RTL archive. Their presence in an archive is not an NTSRV dependency.
No MVDM/CCPU unit enters the service image.

srvinit.c is not selected. Project base_dispatch.c retains its finite original
dispatch-table mapping; project server_exports.c supplies the admitted finite
interactive-token boundary. Service-private modules, resource/wait/stream/
registry bindings, RPC security/receipt/payload/message transport, root and
native-launch codec remain at their existing owners. Generated MIDL server
and the selected generated config fragment are explicitly tracked, not
represented as newly imported original files.

The graph/archive/member ledger uses actual map membership before recursively
visiting source edges. Root/static archives not selected by the linker are
excluded. This is build/link evidence, not proof every retained function runs.

## Exact proposed original diff

Only srvvdm.c needs new nonzero-carrier expression edits. No original header,
RTL algorithm, DOS/WOW structure or wire declaration change is proposed.

| Source lines | Meaning / existing source path | Minimal proposed change |
| --- | --- | --- |
| 632,865 | DWORD WOWExec PID from GetWindowThreadProcessId, used in HANDLE-shaped CLIENT_ID lookup, not a process resource. | `(HANDLE)(ULONG_PTR)pid`; retain exact32-bit PID. |
| 848,1551 | Authenticated CSR ClientId.UniqueThread carries a numeric TID; original notification signature takes DWORD. | `(DWORD)(ULONG_PTR)UniqueThread`; no width change to callback. |
| 849,1552 | TargetHandle from BaseSrvCreatePairWaitHandles: standalone wait binding supplies a32-bit receipt in the original HANDLE-shaped field. | `(DWORD)(ULONG_PTR)TargetHandle`; do not narrow a real event. |
| 1872 | Original hParent low-bit tag removal before DOS-record match. Standalone service builds it from uint32 parent_receipt. | Mask through ULONG_PTR and cast back to HANDLE; preserve bit0/tag and existing match/completion order. |

Source proof: service_wait_deliver retains the real event behind an even
uint32 receipt; base_wait.c returns that receipt, not the event, to the original
duplicate call. base_service.c:673–674 constructs hParent from parent_receipt.
broker_vdm_receipt_accept reserves bit0 for waits and refuses counter overflow.
The real event/process resources stay native HANDLEs and are attached separately.
base_process.c obtains PID from GetProcessId on its retained actual process,
before storing it in CLIENT_ID. No sender-supplied PID becomes authority.

The unchanged original also casts eleven NULL literals to ULONG/ACCESS_MASK.
Modern C NULL is void* zero, producing narrowing diagnostics on AMD64 even
though no address exists. A srvvdm-only `/DNULL=0` build binding compiles those
same numeric/null-pointer zero values without editing eleven source sites.
This is not an upstream-C definition claim: pinned ntdef.h uses void* NULL in C
and integer0 in C++. Both representations denote zero/null in these uses.
Nonzero HANDLE/PID/receipt conversions remain strict errors until explicitly
adapted; no warning suppression or generic pointer-truncation facade is used.

Rejected alternatives: widening DWORD task/PID/callback/wire fields, importing
CSR transport, moving original algorithms into an adapter, or rewriting eleven
literal-zero sites simply for compiler diagnostics. The latter would bring
original changed code lines to18 and exceed the S5 comparison budget. If the
scoped zero-literal binding is rejected, present that larger explicit diff for
owner review instead of silently increasing the budget.

## Project-only changes and budget

Strict native compilation identifies15 equivalent numeric-carrier conversions
across six existing project units: base_service.c1, service_core.c2,
frontend_registry.c4, base_stream.c3, base_wait.c2 and base_process.c3.
PID comparison/storage uses ULONG_PTR between DWORD and HANDLE; stream/wait
receipt lookup and materialization do the same for uint32 IDs. Actual HANDLEs,
lock boundaries, callbacks, order and resource delivery/revoke behavior stay.
These are not mirror changes or permission to add a second task/resource table.

| Comparison dimension | Delivered S5 increment | Proposed S6 increment |
| --- | --- | --- |
| Historical body/declaration-carrier paths | 4: vdm.c, csrutil.c, ntcsrmsg.h, existing standalone zwapi.h. | 1: srvvdm.c. |
| Original algorithm body files touched | 2, same algorithms. | 1, same algorithm. |
| Changed executable/declaration lines excluding registration comments | 14:4 client,5 capture,2 metadata declarations,3 VM API declaration lines. | 7 expressions, no declaration lines. |
| Original structure/header additions | Local pointer metadata/API width adjustments. | None; existing native declarations already scale local pointers. |
| Original algorithm/control-flow changes | None. | None. |

S5 raw diff including comments is29 additions/15 deletions; proposed S6 raw
expression-only research copy is7 replacements. Production registration
comments are not added yet and must be reported separately on implementation.
Reused S5 adaptations are baseline, not counted as a new S6 change. Thus the
current proposal is below S5 in every comparable source-diff dimension.

## Attempts and actual verification

- r001: archive/source selection and baseline hashes. First invocation of
  Windows PowerShell lacks Get-FileHash under its inherited module path; no
  compilation result is claimed. PowerShell7 under the existing x64 toolchain
  executes the audit. Initial missing owner include roots are recipe failures,
  not source-port requirements.
- r002: native modern declaration ingress and original owner include roots.
  Strict `/MT /W4 /we4013 /we4311 /we4302 /we4312` compilation identifies
  original nonzero-carrier and NULL-literal sites plus the six project units.
- r003: scoped NULL=0 eliminates only the literal-zero diagnostics; seven
  original source sites and15 project sites remain. Srvvdm diagnostics count8
  because the tag expression contains two problematic casts.
- r004: exact proposed copies under build only, seven original and15 project
  sites, compile all35 selected units successfully. Link them with the finite
  public Win32/NTDLL/RPC/CRT libraries, producing ntsrv-audit.exe with machine
 8664 and a native map. No unresolved CSR shell or x86 assembly dependency.
  The audit EXE is not executed as a service, staged or deployed.
- r005: same original declaration/layout and numeric-tag fixture on both ABIs;
  native and x86 reports distinguish expanded local PORT/CLIENT_ID/CSR/message/
  record layouts from fixed36/16/32 copied values. High32-bit IDs
  0x80000002/0xfffffffe retain identity and low-bit tagging. Initial fixture
  `/c` invocation creates only an object; the corrected separate link and
  x86 legacy_stdio binding are retained, not counted as initial passes.

Actual x64/x86 local sizes: PORT_MESSAGE40/24, CLIENT_ID16/8, CSR_THREAD120/72,
CSR_PROCESS184/112, BASE_API_MSG248/144, DOSRECORD40/24, CONSOLERECORD80/48 and
WOWRECORD40/24. These structures are not copied wire payloads. Numeric ULONG/
DWORD and wire values remain32-bit. Native and x86 fixture exits are both0.

Compile reproducer: execute each retained audit.ps1 through the existing
MSVC14.43/SDK22621 x64 wrapper with pwsh.exe; it generates native MIDL41,
records source commands/hashes and compiles under its own build root. r004
prepare.ps1 derives only enumerated exact candidate replacements, then runs
that audit. Candidate link command/objects/map and layout fixture source,
build/link logs and outputs remain in the same evidence roots. There is no
functional CCPU row for this source/ABI-only service audit.

## Review conclusion and next gate

The bounded original-diff proposal is feasible and smaller than S5. This
audit does not prove AMD64 service registration, live x86 NTVDM/WOW reentry,
cross-width system_handle transfer, real completion/rundown or publication.
Those are S6 implementation/runtime gates, not compile claims. Preserve the
original caller/lock/cleanup paths and x86 build, register every actual mirror
edit, build native service/MIDL/dependencies, then retain service/RPC negative
and lifecycle tests, Console17/Window17, independent WOW frontiers and coherent
ten-image publication/deployed checks. No bitness-only version bump is needed
unless implementation demonstrates an actual wire change requiring review.
S6 remains active at this audit checkpoint; S7 is not admitted.
