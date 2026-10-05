# T432 S1 dual-width implementation design

## Scope and confidence

Source review on 2026-10-05, against main cd0077982 and the owner-accepted
T431 S2 production baseline 12160c657. This is a design delivery, not an x64
compile, injection or runtime result. S2–S5 remain the implementation gates
in the [plan](t432-dual-width-native-workers-hooks-plan.md). The sealed
S2/r027-runtime and published nine-image set are unchanged.

## Source and actual-link ledger

Paths below are repository-relative. Retained T431 map research identifies
linked bodies, not merely libraries listed on linker command lines.

| Mechanism/source | Origin and current consumer | T432 decision |
| --- | --- | --- |
| common/application_search.c | Project; Run16 and Hook32 share CWD/PATH and COM/EXE/BAT/PIF discovery. | Compile the same source for both Hooks; no new resolver, extension precedence or launcher syntax. |
| opennt-host/base/win32/client/vdm.c, GetBinaryTypeW | Original; Run16 and Hook32 classifier composition. Native branch rejects caller/target machine mismatch. | Keep mirror unchanged. Shared owned classification adapter handles a confirmed native mismatch using the existing image-section metadata path, not shell fallback or a second PE parser. |
| run16-exe/image_classification.c and hook_classification.c | Project; SEC_IMAGE/NtQuerySection and original legacy classifier wrappers. | Share recognition and machine/subsystem results; Hook-specific process-image lookup stays a small caller adapter. Preserve legacy recognition and existing shell/batch behavior. |
| ntsrv-exe/opennt/include/base_classifier.h | Project finite facade; historical section layout uses ULONG stack sizes. | Native-width-correct SIZE_T fields and NtQuerySection declaration in the facade; compile-time layout checks and actual known-image queries. No rewrite of original classifier policy. |
| opennt-host/base/win32/client/csrutil.c; selected RTL environment/error | Original bodies pulled by Run16, not NTVWM/NTCON. | Run16 stays x86. Do not port CSR capture or the whole BaseClient library merely to build NTVWM64. |
| ntsrv-exe/opennt/bindings command/payload/process/startup/values | Project bindings; actually pulled by NTVWM/NTCON maps. | Build the reached project bindings at each worker width, audit pointer/handle casts. Archive membership does not justify selecting unrelated original bodies. |
| common RPC client, service.idl; worker-base | Project common control/I/O/lifecycle mechanisms. | Same sources/protocol; architecture-local client stubs and libraries. NTSRV server and original DOS/WOW remain x86. |
| ntsrv-exe/opennt/source/worker_registry.c, reservation, native_commands.c | Project native worker selection/direct request ownership. | Add target machine to existing reservation/watch/selection flow, not a new registry or task kind. Verify the spawned process matches its selected machine. |
| ntsrv-exe/transport/worker_spawn.c | Project suspended worker admission and startup-only Job rollback. | Reuse unchanged transaction semantics for both worker widths. Its temporary startup Job is not participant observation or a running-target kill policy. |
| ntvwm-exe main/execution/presentation/next_command | Project actual native worker; true target wait and broker completion. | One source family, two builds; same READY/BUSY/completion/retirement and private hidden Console. |
| nthook32-dll installer/context/create_process/entry | Project; actual suspended child, copied context, interception and rollback. | One source family, width-selected DLL; eliminate target32-only/silent cross-width bypass when its replacement passes runtime gates. |
| nthook32-dll/detours/* | Selected MIT Detours 4.0.1, pinned e4bfd6b03e50de46b47abfbd1e46b384f0c5f833; retained T431 admission/hashes. | Same-width import update remains selected. Opposite-width import update uses the bounded official helper mechanism below, not private WOW64 calls or unmerged PR161. |

There is no reached x86 assembly body to port in NTVWM64. This is a source/map
conclusion, not proof that every selected project header compiles as x64.
Hook64's original classifier-only composition still requires an x64 build
proof; do not silently replace it if an unavailable dependency appears.

## Classification, target selection and paths

1. Keep the shared search order and original legacy classifier first. Return
   DOS/PIF/Win16 exactly as today. For native recognition, use the existing
   image-section metadata operation to obtain machine and subsystem. A legacy
   classifier ERROR_BAD_EXE_FORMAT may become a native result only when that
   operation positively confirms a non-DLL I386/AMD64 Windows image. It must
   not turn a corrupt image or arbitrary classifier failure into native success.
2. Run16's main.c currently falls through to COMSPEC on classifier failure
   before its native metadata call. A confirmed AMD64 image must instead go to
   native admission. Preserve established built-in/batch/shell fallback and
   arguments; do not require a new `--` separator or change CLI syntax.
3. Put shared recognition in the existing common source family, with a small
   original-classifier facade where required. Both Hooks and Run16 use it.
   Hook64 retains the original _X86_-guarded legacy recognition slice through
   a classifier-only build shim, not a global lie about the worker architecture.
4. NTSRV's StartNativeWorker/selection/reservation contract receives the target
   machine. It accepts only supported I386/AMD64, selects system32/ntvwm32.exe
   or ntvwm64.exe, and verifies the actual admitted worker process architecture.
   Existing reuse scans must filter machine as well as Console/root, state and
   authenticated identity. A worker rejects a delivered mismatching target
   before execution; it does not launch a second worker itself.
5. Direct requests must refer to the actual file resolved by the caller.
   In particular, x86 System32 can resolve to SysWOW64, while Sysnative names a
   native file but is not usable by an x64 worker. The existing metadata open
   can provide the actual final file path for broker delivery, without changing
   argv/CWD/environment. Validate resolution again at the receiving worker.
   Test explicit System32, SysWOW64 and Sysnative plus PATH aliases; never infer
   width from spelling. Do not change ordinary hooked native CreateProcess
   resolution into broker worker selection.

Final GUI/text routing remains in NTVWM; machine selection is not a second
subsystem policy. Monitor kind remains DOS=0, Win16=1, Win32=2. Width does not
create Observed task records or a separate scheduler.

## Build and wire boundary

Extend tools/build/New-T310OriginalSoftpcNinja.ps1 with isolated x86/x64
object, library, import-library, map and generated-stub paths under build/.
Compile shared C/C++ separately with the matching MSVC14.43 /MT and SDK22621
libraries. Never reuse an x86 object/library in an x64 link. Generate the
same service.idl client with /env win32 and /env amd64; only the existing x86
server stub goes into NTSRV. Confirm common NDR interoperability; generating
an amd64 stub is not itself runtime proof.

Native-machine RPC parameters change the service wire: synchronize
APP_PROTOCOL_VERSION and service.idl major in S2, reject old application/RPC
versions, and regenerate/relink affected x86 consumers too. Do not change the
worker/frontend I/O protocol solely for machine selection. Recheck actual
64-bit resource materialization by an x86 service: use authenticated system
handles/local duplicate operations, not trusted sender-pointer casts.

Use existing Hook source root for both binary targets, with separate .def
exports/build outputs as needed. Final product-relative system32 manifest is
eleven images: run16, ntsrv, ntcon, ntvdm, ntmon, WOW32, VDMREDIR, ntvwm32/64,
nthook32/64. All except the two native64 images remain x86. Replacement of
ntvwm.exe occurs only with a coherent passing package, not a mixed deployment.

## Installer/context transaction

Retain the actual native child created with CREATE_SUSPENDED. Parent holds its
real process/thread handles through installation. Detect its actual machine;
choose matching Hook DLL, or context-only delivery for the pinned x86 Run16.
Duplicate only the authorized private resources into that recipient, then
copy the versioned context, update imports if applicable, and resume exactly
once only if the caller did not request suspension. Return those same Windows
handles. Failure before publication terminates only this unpublished child,
closes its handles and undoes private attachments; no running tree cleanup.

native_hook.h already uses a fixed 64-byte, pointer-free header with uint64
recipient handle slots. Version 2 can replace unused_offset/unused_bytes with
the second Hook path span; retain explicit mode, recipient machine, total
length and reserved validation. Both DLL paths are needed so every recipient
can propagate either width. Validate contiguous bounded terminated spans,
machine equal to recipient, paired capabilities and recipient-width numeric
range before conversion. Do not enforce MAXDWORD on all x64 recipient values.
Context-only Run16 carries both paths but attaches no interceptor. GUI/new
Console/detached paths retain existing text-capability stripping.

DetourCopyPayloadToProcess uses remote allocation/write and a synthetic image;
modules.cpp locates its sections through the stored optional-header length,
not the reader's sizeof(IMAGE_NT_HEADERS). This supports investigating copied
cross-width context without a new channel. Actual allocation addressability,
payload discovery and resource validity in both directions remain mandatory
runtime gates, not established merely by the uint64 header.

### Bounded opposite-width helper choice

Microsoft's documented mechanism loads the matching Hook DLL in the matching
Windows rundll32, exports DetourFinishHelperProcess at ordinal 1 and skips
interception when DetourIsHelperProcess is true. It modifies the original
suspended child's imports; it is not the child's executor or parent replacement.
Reuse that selected source mechanism; do not add an installed helper EXE,
broker role, I/O owner, target observer or resident process.

Pinned creatwth.cpp's DetourProcessViaHelperDllsW cannot be adopted unchanged:
it derives the loader from WINDIR, resumes the helper twice, and waits forever.
A narrowly registered source adaptation is needed at that exact W helper
boundary: obtain the real Windows directory (not product guest root or
inherited WINDIR), select native System32/Sysnative versus SysWOW64 by required
loader width, resume the helper once, and wait for helper completion or actual
target death with a 10-second installation deadline. On timeout/error, stop
and join only the owned helper before target rollback. Preserve/GetLastError
and close all handles on every branch. Target execution time has no such limit.

This requires a source-policy divergence row naming the pinned file/hash,
changed function, unavailable external wait/path seam, retained MIT notice and
failure tests before S4 coding. A callback alone cannot fix its internal
infinite wait/double resume, which is why a local wrapper around the unchanged
API is insufficient. Keep import-update/restoration algorithms unchanged.
The DLL helper early-return/export changes belong to our owned entry/.def,
not another Detours algorithm. Hold the original target handle throughout so
its PID cannot be recycled into an unrelated process during helper execution.
Only after helper success may ordinary target execution begin. No mitigation
bypass, ASLR restriction or private WOW64 transition is admitted.

## Cross-width execution is not cross-worker execution

An ordinary 32-bit native program may create a 64-bit native child and wait on
its true process handle (and vice versa). The child can inherit that worker's
hidden Console; bitness alone creates neither a broker direct task nor a new
worker. Propagate the matching Hook and context into this real child.

If it calls x86 Run16 for DOS/Win16 or another explicit native request, NTSRV
selects the appropriate worker. Different workers own different hidden
Consoles. NTCON's existing zero/one I/O channel uses final-frame/input return,
release acknowledgement, incoming seed and parent-resume barrier. No width
branch in NTCON, no shared hidden Console between workers, and no second
handoff protocol. Windows parent waits remain Windows waits; only direct
broker receipts complete Run16's direct requests.

## Implementation verification gates

These are planned tests, not passes. Extend existing reproducible entrypoints;
save architecture/input manifests and raw results below each admitted build/ run.

| Stage / entrypoint | Required positive and negative assertions |
| --- | --- |
| S2 formal Ninja graph and service fixtures | Actual32/64 image machine, x86/x64 object isolation, correct worker selection/reuse and mismatched reservation rejection; invalid/DLL/legacy/batch/shared-search cases; RPC/app mismatch negatives. |
| S3/S4 tests/component-integration/nthook_install_test.cpp, built at both widths | 32→32, 64→64, 32→64, 64→32; selected DLL before immediate descendant creation, real target/primary thread, exact exit codes, no-inherit/HANDLE_LIST, Unicode env/CWD/streams, GUI↔CUI capability stripping, caller-suspended child still suspended. |
| S3/S4 same fixture context/malformed/rollback extensions | Context-only x86 Run16 from both parent widths; header/path/machine/version/range failures; missing/wrong DLL; copy/import failure, target death, helper timeout/cancel and no leftover helper/attachment/handle. No silently unhooked cross-width success. |
| S3–S5 real package CMD routes | Explicit native32/64 CMD and immediate child, CMD→Run16→COMMAND/MEM and parent output/result, appropriate WOW route; native32↔native64↔DOS broker handoff and ordinary mixed-width children tested separately. Fresh operation markers prove execution, not old screen/prompt or successful input submission. |
| S5 retained product/independent WOW observation entrypoints | Console17/Window17, typeahead, worker reuse/death, broker death, independent roots, original completion/final-frame/input-return and ordinary publication/hash/recovery assertions. Preserve WINMINE main UI and SOL/WRITE retained error frontiers distinctly. |

Both directions of context copy, native ABI and helper rollback are explicit
implementation risks. If one fails, preserve the accepted package and report
the failing source/transaction boundary; do not lower assertions or substitute
a silently unhooked process. No further architectural owner choice is needed
to begin S2; exact unsupported mechanics require bounded re-admission, not an
unapproved injector rewrite.

## Primary references and design-only verification

- [Microsoft Detours helper overview](https://github.com/microsoft/Detours/wiki/OverviewHelpers): matching DLLs, ordinal 1 and helper loader entry contract; retained source controls actual local behavior.
- [Microsoft MIDL /env](https://learn.microsoft.com/en-us/windows/win32/midl/-env): architecture-specific generated code, not permission to link one width's stubs into another.
- [Retained classifier/link audit](t431-native-launch-hook-design.md): original bodies versus project bindings and existing limitations; its former helper-free-only requirement is superseded by T432 admission.

S1 verification on 2026-10-05: tools/governance/Verify-DocumentationGovernance.ps1,
tools/governance/Test-DocumentationRelativeLinks.ps1 and git diff --check
passed after correcting the required baseline heading. Source-to-contract
review covered the selected classification, worker selection, context, helper
entry/wait and build paths above. No production source, running process, build
cache or published package is changed. Runtime capabilities stay unverified
until the corresponding S2–S5 tests pass.
