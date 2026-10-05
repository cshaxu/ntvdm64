# T431 native launch hook detailed design

## Status, inputs and decision

This is the S1 implementation contract, not an implemented hook or production
admission. CURRENT remains the only active packet. Read with the
[source checkpoint](../evidence/m0-t431-s1-native-launch-hook-audit.md) and
[stage/test plan](t431-native-launch-hook-plan.md). Baseline18a582e6f contains
no hook implementation; the accepted T430 runtime is unchanged.

Procedure: reread current authorities, the proposal and pinned Detours source;
inspect original vdm.c classification, run16 native creation/frontend scope,
NTVWM execution and NTSRV capability binding. Recheck official creation,
duplication and payload contracts. This record separates a specified mechanism
from proof that its implementation works. No executable was built or run for
this design delivery.

Choose one suspended-child installation entrypoint and one copied bootstrap
context, shared by NTVWM's direct creation and an installed hook's child
creation. Do not add an injector process, worker, transport, registry or
frontend edge. The same-width installer candidate is Detours import-table
installation into a not-yet-running child. Its default cross-width helper
branch is prohibited. A helper-free four-width installer is **not selected
or proved**; that is a finite implementation-admission gate, not a promise
that this design has solved injection.

The design below fixes ownership, ordering, flags and failure contracts now.
S1 stays open for the three proofs listed under Admission gates. Do not
silently reduce the proposal's four-width acceptance to same-width support.

## Source-first decisions and module boundaries

| Mechanism | Source/current location | Selected disposition |
| --- | --- | --- |
| Legacy image recognition | opennt-host/base/win32/client/vdm.c, GetBinaryTypeW and BaseIsDosApplication | Reuse the existing classifier cohort through a narrowly selected launcher-owned static library. Retain original section-status/type/error rules; no extension-only or MZ-only classifier. |
| Native subsystem query | run16-exe/image_classification.c | Reuse OS image-section metadata. Opposite-machine rejection by the legacy classifier is not evidence of a DOS image. |
| run16 user search | run16-exe/application_search.c | Unchanged launcher policy. Do not use it as Windows CreateProcess search. |
| Native resource/creation primitive | run16-exe/native_launch.c; NTVWM execution.c | Retain restricted streams/capabilities, actual suspended child and original bind/resume/completion ownership. Installer consumes that child; it never creates a replacement CMD. |
| Hook/context installation | Proposed nthook-dll installer library | One entrypoint, explicit instance/transaction and resource ownership. Detours candidate is pinned in the checkpoint; no adoption or source-root creation yet. |
| API interception and recursion | Proposed nthook-dll | New bounded product behavior. Original Kernel32/CSR product shell is unavailable; neither original classifier nor a host facade supplies modern per-process interception. |
| Copied bootstrap context | Proposed common/protocol declaration and common/codec reader/validator | Fixed-width bytes and recipient-local attachments only. No injector, image search, authorization or task policy in common. |
| Receiver integration | nthook-dll for controlled native children; run16-exe for launcher context | Hook initialization uses the copied context; a real run16 consumes context without loading the hook DLL or intercepting itself. Both authenticate through existing NTSRV contracts. |
| I/O release and parent resume | Existing NTSRV routes, worker-base and worker execution | Unchanged. Hook only creates/submits a real launcher; it cannot grant a pipe, publish a frame, resume a guest or complete a broker record. |

Original recognition is composable and therefore remains the first recovery
rung. The full original Kernel32 creation/CSR dispatch shell cannot compose
without its NT4 subsystem dependencies. A same-shaped facade does not supply
the missing modern-process interception. Installation may adopt the pinned
MIT Detours slice after dependency/license/admission review; any modification
of that slice needs a precise exceptional-intrusion register. The interceptor
and bootstrap integration are the final newly authored boundary, not a
claim to have recovered original kernel injection.

Planned files, not created directories or existing providers:

```text
src/run16-exe/             selected classifier library + early context consumer
src/ntvwm-exe/execution.c  initial installer caller only
src/nthook-dll/            entry, create_process, installer, context instance
src/common/protocol/       hook bootstrap copied declaration
src/common/codec/          bounded bootstrap reader/validator
tests/                    contract, chain, width and isolation fixtures
build/M0-T431/...          generated files, objects, libraries, probes and logs
```

The proposed nthook32.dll/nthook64.dll are siblings of run16.exe in system32.
NTCON/NTMON do not link the installer. Worker-base does not acquire injected
process state. There is no dependency on run16.exe as a running classifier
service: the selected classifier library is a build-time reuse boundary.
No MVDM/OpenNT-host mirror body is changed by this design.

## Process creation and installation order

### Initial direct native text target

1. NTSRV delivers the existing authenticated direct request to NTVWM.
2. NTVWM uses its existing resource primitive to create the actual target
   suspended. Existing request/stream ownership remains unchanged.
3. Verify target architecture and supported installation conditions. Select
   the matching DLL from the installer's own admitted package, not child PATH,
   CWD, COMSPEC or an arbitrary environment path.
4. Duplicate the minimal root/execution capabilities into the actual child;
   copy recipient-local bootstrap data and install the hook in one transaction.
5. Bind the real target with the existing BindNativeTarget contract.
6. Resume the initial thread using the existing worker-owned resume step.
7. NTVWM waits/reports the actual target result exactly as before. NTCON handles
   presentation only; NTSRV owns the direct receipt and lifecycle decisions.

Installation is before broker binding, so an installation failure cannot
leave an already bound live target without a hook. If binding/resume fails,
use existing uncommitted-startup rollback and finish the request with failure.
The hook does not introduce a second worker completion or process watcher.

### Native child created by a controlled text program

1. The interceptor takes a per-call context and preserves the caller's original
   A/W arguments, structures, environment interpretation and error boundary.
2. Internal creation or an excluded flag/security case goes directly to the
   saved original entrypoint, with no install or authority propagation.
3. For an eligible native creation, call that same A/W entrypoint once with
   temporary CREATE_SUSPENDED; do not rewrite its executable or arguments.
4. Inspect the *actual created target*, not just a preflight pathname. Native
   GUI/new-Console/noneligible targets receive no frontend context/hook.
   The implementation must establish the subsystem from trustworthy metadata;
   reopening a raced pathname alone is not proof of the created image.
5. For eligible text children, invoke the same installer/context transaction.
   A real product run16 child gets context-only seeding, no API interception.
6. Remove only the temporary suspension when the caller did not originally
   request CREATE_SUSPENDED. If it did, leave the child suspended and return
   its actual process/thread handles and IDs immediately; do not wait for an
   initialization event that requires that thread to run.
7. Return actual Windows process objects. No child-exit wait, receipt, task
   insertion, frontend ownership or parent scheduling is added to the hook.

An API hook does not cover direct native system calls or every arbitrary
in-process DLL that bypasses it. Real CMD entrypoint coverage must be measured.
Intercept the effective exported A/W functions, accounting for API-set
forwarding; use a thread-local depth guard so an A-to-W implementation call
cannot redirect or install twice. Concurrent calls have independent child
transactions; no global launch lock spans broker calls or child execution.

### Confirmed DOS/Win16 request

Recognition is a preflight only for an unambiguous, proven application
selection. A recognition failure/unknown image does not authorize rewriting
the call. For a confirmed supported DOS/PIF/WOW request, create the pinned real
run16 with the original target and untouched argument tail, then seed its
context using the same child transaction. Preserve original run16 syntax;
do not add a mandatory -- or implicitly force --wait on Win16.

The parent gets a real run16 process/thread, not a fake DOS process. Waiting
for it gives the established DOS result; Win16 follows existing startup-only
behavior. Querying that process's image, remote memory or primary thread sees
run16. Exact DOS-thread/debugger/image equivalence is not promised.

Example: controlled CMD creates COMMAND through this launcher; run16 binds
the existing execution context at NTSRV; NTSRV coordinates old native I/O
release and DOS acquisition. COMMAND exits through the original DOS record;
run16 returns that receipt and restores the parent through the existing
barrier. CMD resumes its own Windows wait/output. None of these steps is an
interceptor-owned scheduler or direct NTCON control call.

## Selection, arguments and unsupported cases

CreateProcess and run16 deliberately have different resolvers. Microsoft
documents application-name and null-application search/ambiguity separately.
Keep native creation on its original API; do not replace it with SearchPath,
run16's COM-first lookup or a process-global WOW64 redirection toggle.

| Input | Contract |
| --- | --- |
| Explicit application pathname with an independently bounded argument tail | Candidate redirect after original legacy classification and exact command-tail tests. Preserve a different supplied argv[0]; do not assume it equals applicationName. |
| Null application with an unambiguous quoted/explicit executable token | Candidate only after proving native token interpretation and preserving the token/tail boundary. |
| Null application, bare name or ambiguous unquoted spaces | Native passthrough until source/actual-CMD tests prove the exact selected image and precedence. Never guess the DOS candidate from CLI search. Ordinary bare COMMAND acceptance remains an open gate if CMD supplies this form. |
| Native image, DLL, unsupported subsystem, malformed or missing image | Native API/result, not COMSPEC fallback or forced run16. Classification errors cannot replace unrelated native errors. |
| BAT/CMD | Ordinary native command-interpreter behavior. No direct legacy-image redirection based on suffix. |
| Explicit run16/internal product image | Never redirect recursively; only real run16 may receive authenticated context-only seed for same character session. Identify the admitted product image, not an arbitrary equal basename. |

For a redirect, modify an owned command-line copy only. Preserve A/W code-page
semantics, Unicode/ANSI environment selection, drive-CWD entries, current
directory, standard-handle aliases and the meaningful startup fields. A supplied
environment is not replaced by the hook's environment. Command length overflow
after prefixing run16 is a bounded failure, not truncation. Tests must compare
the real child arguments and redirected bytes, not just the displayed prompt.

| Flags/boundary | Installation/redirection decision |
| --- | --- |
| Ordinary creation; priority/error-mode/group flags | Supported candidate with unchanged Windows flags and child attributes; only temporary suspension is added internally. |
| CREATE_SUSPENDED | Preserve caller suspension; commit installation before return. No forced initialization wait or extra resume. |
| bInheritHandles TRUE/FALSE; supported explicit HANDLE_LIST | Preserve caller inheritance/list. Private bootstrap resources are duplicated separately, not made globally inheritable. Do not replace/expand the caller's list. |
| CREATE_UNICODE_ENVIRONMENT; STARTF_USESTDHANDLES | Preserve exact encoding/stream semantics. Invalid stream values are not repaired into Console I/O. |
| CREATE_NEW_CONSOLE, DETACHED_PROCESS, CREATE_NO_WINDOW | Native passthrough without inherited character-session authority. Legacy redirect in these modes is not claimed by this initial scope. |
| DEBUG_PROCESS, DEBUG_ONLY_THIS_PROCESS | Native passthrough; do not interfere with debugger-owned initial suspension, events or image identity. |
| Alternate token/logon, elevation, protected process, unsupported mitigation/attribute | No hook coverage/security bypass. Pass through on the original API; preflight unsupported native installation conditions before modifying anything. |
| Explicit parent-process attribute, unknown extended attributes | Passthrough until exact semantics/security/resource tests admit the combination. Never blindly rebuild an attribute list. |
| VDM-specific separation/force flags | Do not discard or translate silently; admit only a source-proven mapping to existing run16 admission. Otherwise preserve native failure/passthrough. |

This is a finite boundary, not a claim that arbitrary STARTUPINFOEX input is
transparent. Cases important to actual CMD cannot be excluded merely to make
the fixture pass; they must be resolved before that CMD route is accepted.

## Bootstrap data, authorization and loader lock

The installer owns a per-child transaction. Proposed bootstrap format1 is a
copied little-endian record with this64-byte header; layout must be asserted
independently by each admitted compiler, not inherited from native packing:

| Byte offset | Fixed-width field | Meaning |
| --- | --- | --- |
| 0,4 | uint32 bytes, version | Exact complete record length; version1. |
| 8,12 | uint32 mode, machine | TEXT_HOOK=1 or LAUNCHER_CONTEXT=2; actual Windows target machine constant. |
| 16,20 | uint32 flags, reserved | Version1 flags/reserved are zero; reject unknown bits. |
| 24,32 | uint64 root_attachment, execution_attachment | Recipient-local duplicated references, both present or both absent. Text/context delivery requires both. |
| 40,44 | uint32 launcher_offset, launcher_bytes | UTF-16 absolute product run16 path span. |
| 48,52 | uint32 hook32_offset, hook32_bytes | UTF-16 absolute sibling Hook32 path span. |
| 56,60 | uint32 hook64_offset, hook64_bytes | UTF-16 sibling Hook64 path span; absent only in an explicitly32-only probe, never a four-width accepted package. |

The carrier uses one fixed project payload identifier, not an arbitrary
address provided by an environment variable. Cap bootstrap data at64KiB,
including terminated UTF-16 paths. No pointer, callback, CRT allocation,
target command or process-tree history appears in it. Locators use64-bit storage with checked target-width
conversion; they are not broker IDs or authoritative sender HANDLEs.
Reserved fields must be zero; spans must be terminated, nonoverlapping,
in-bounds and overflow-checked. The initializer copies validated data into
its owned instance before releasing the installation payload.

The process-local installer API accepts borrowed actual process/initial-thread
handles, target machine, mode, borrowed validated root/execution capabilities
and owned-package path views. It returns an explicit status plus disposition:
COMMITTED, UNMODIFIED, ROLLED_BACK or ROLLBACK_FAILED. It never resumes the
thread, closes caller process/thread handles, waits for target exit, or returns
a CRT-owned allocation across libraries. Its caller alone decides rollback
of an unpublished creation and publication of PROCESS_INFORMATION. The
context-only mode performs capability/payload delivery without installing a
DLL. One payload reader/validator is shared by hook and launcher receivers;
an absent payload means ordinary unseeded startup, but a malformed payload is
not silently treated as absence.

This is an installer-to-child bootstrap record, **not a new NTSRV RPC or
worker-I/O message**. No report/observation ABI is added here. Any future
service wire change must separately synchronize application/IDL revisions.

Root/execution resources are the existing event-object capabilities, with
only their required rights. Use DuplicateHandle against the held actual child
process, with non-inheritable recipient references. This works independently
of the caller's bInheritHandles; public documentation allows duplication
between32/64 processes. That does **not** prove cross-width payload writing
or DLL installation. A Console handle is not transferred this way.

For inheriting native children, the copied context is still the hook's source
of authority; stale inherited environment numbers cannot override it. For
non-inheriting native children, never write parent handle numbers into the
environment and pretend they were inherited. For real run16, an early bounded
context consumer supplies recipient-local references to its existing frontend
scope, before ordinary task admission. With no seed, existing CLI behavior is
unchanged. No hook DLL need be loaded in run16 and no new launcher switch is
required. Context-only delivery and its reader need a real fixture before use.

At first safe use, authenticate through existing BindConsoleContext followed
by RetainFrontendRoot. NTSRV already validates connected process identity,
compares actual objects, binds execution origin and rejects closing/mismatched
roots. Bootstrap version/path text/PIDs alone are never authorization.
S1 source inspection establishes those current calls, not proof that the
future injected client's link/width boundary works.

The DLL's DllMain may perform only audited bounded local bootstrap/import
restoration and the minimum interception setup. No broker RPC, child launch,
thread join, session lookup or ready-event wait under loader lock. Complete
path validation/once-initialization/authentication on first intercepted call
outside loader lock. If hook attachment cannot safely occur before executable
entry with the selected library, that is an installation blocker, not grounds
to create a resident bootstrap thread/helper. Never promise ready-before-main
by waiting on a caller-suspended target.

Pin paths from the authorized installer and the actual loaded hook module.
Do not call current common own-EXE root derivation inside CMD and mistake
Windows' system directory for this package. ANSI DLL-name encoding in the
selected installer is a gate: reject unrepresentable package paths explicitly
until a supported loader contract is demonstrated; never silently load a
different DLL or create an unapproved short-path alias.

## Resource ownership, failure and handoff

| Boundary | Owner and action |
| --- | --- |
| Before native creation | No child exists. Failure leaves original API result/last-error contract; free per-call copies. |
| Created, suspended, not returned/bound | Installer owns only its new remote allocations/import edits/bootstrap copies; caller owns process/thread handles. Record undo state before mutation. |
| Install fails before mutation | NTVWM direct startup fails using existing rollback. Descendant native call may return the actual native child without a hook only under an explicit tested propagation-gap policy; never claim it is controlled. |
| Install partially modifies a native descendant | Restore original imports/protection, remove payload and close only installer-created attachments before native passthrough. If complete restoration cannot be proved, do not resume a corrupt child; rollback this unhanded creation and return installation failure. |
| Legacy redirect/context seeding fails | Do not run a launcher without its required association, silently create another frontend, or retry as native after side effects. Roll back only this unhanded launcher; return a bounded failure. |
| Actual handoff succeeded | Caller owns returned process/thread; child owns committed private attachment copies. Later report/authentication/parent failure cannot tree-kill the target or undo its execution. |
| Context invalid, broker/root gone | Existing authenticated failure semantics; no reconnect/replay, PID guess, new worker policy or alternate root. Native passthrough and rejected legacy redirection are distinguished explicitly. |
| Normal/forced process exit | Windows closes process resources. DllMain detach is not a completion source; NTVWM/direct broker records keep existing result ownership. |

Every rollback preserves the original diagnostic before cleanup calls can
overwrite last error. Injection failure must not publish stale PROCESS_INFORMATION
handles as success. Only the creator's unpublished suspended child is a
startup rollback resource; an already returned child or its descendants are
not. No long-lived Job pairing or kill-on-close is introduced.

Native propagation gaps cannot be silent acceptance. S2/S3 must define and
assert the bounded case verdict, while required ordinary CMD chains fail
acceptance if propagation is missing. A failure after initial ResumeThread
cannot be made transactional by terminating an already running program.

## Width/toolchain gates and proof matrix

| Installer caller → child | Current evidence | Required proof |
| --- | --- | --- |
| x86 NTVWM/hook → x86 text | Detours same-width candidate, not runtime-tested | Actual DLL loaded before first child creation; copied context, handles, caller suspension and rollback. |
| x86 NTVWM/hook → x64 text | Pinned default update and exact PR161 head reject width; wrapper uses forbidden helper | A separately admitted helper-free full-width memory/installation facade, including protection and undo; installed private exports alone do not prove it. |
| x64 hook → x64 text | Same-width candidate; hook-only island not yet admitted | Separate x64 objects/CRT/DLL and correct native ABI; actual64 CMD nested/legacy chains. |
| x64 hook → x86 text/run16 | Pinned default update rejects; official PR161 offers an unmerged source candidate | Same installer contract with restoration/payload-layout review and runtime proof, including context-only run16 seed. No helper fallback. |

DuplicateHandle's cross-width contract solves resource duplication only.
CreateRemoteThread/LoadLibrary pointer recipes or compiling two DLLs do not
solve cross-width addressability, safe pre-entry initialization or rollback.
Do not invent a second unrelated injector per direction, force64 children
under4GiB, globally disable ASLR, bypass mitigation policy or adopt unapproved
private WOW64 transitions.

Hook32 selects the current original x86 classifier cohort. Hook64 must retain
the same recognition contract through an audited architecture-correct facade:
the current historical SECTION_IMAGE_INFORMATION/RTL/PEB declarations are
not an x64 ABI, and the original native same-machine check is not a general
native-type test. Keep native metadata/legacy recognition distinct. Do not
patch vdm.c to accept64 images or link x86 objects into Hook64. Whether the
original selected cohort can compose unchanged with that private ABI facade
is a named remaining proof, not permission for a second PE parser.

Before S2/S3 implementation admission, register the exact installation slice,
notices, local modifications, source manifest and narrowly hook-only toolchain
exception. Product EXEs and MVDM remain x86 /MT CCPU40. Future Hook64 and width
fixtures alone use the separately admitted native x64 /MT island. No new source
root, imported library, x64 build or mirror change is admitted by this record.

## Acceptance sequence and admission gates

Tests are planned, not passing results. Extend the existing plan with these
operation-specific assertions; normal product matrices do not replace them.

1. Argument fixture records a fresh per-operation nonce, actual target path,
   argv/environment/CWD and redirected bytes. Compare native hooked/unhooked
   results for selection, errors, process/thread objects and flags. Cover
   quoted/null application, differing argv[0], aliases and explicit handle lists.
2. Bootstrap fixture proves no-inherit children receive only the private
   authorized copies, stale environment is not accepted, context-only run16
   consumes the right root/origin, and two roots cannot cross-bind. Count owned
   handles before/after failed installs and verify remote rollback.
3. Width fixture starts32/64 native targets with caller-suspended and ordinary
   creation; proves the selected DLL and an immediate child creation are
   intercepted. Include missing/wrong DLL, bad payload, target death and failed
   remote-write/import-undo. Process lists alone do not prove no helper: pair
   controlled creation evidence with selected-source/build-callgraph review.
4. Actual CMD fixture observes the entrypoint/arguments used for ordinary bare
   COMMAND, nested CMD and Win16. No observation-only service tasks or production
   scheduler added for instrumentation. Confirm the chain uses the real production
   installer; imported functions alone are not evidence of runtime coverage.
5. I/O chain fixture covers both DOS/native directions, no-inherit creation,
   final-frame/input return, parent output, error code and independent sessions.
   Distinguish actual consumption from an input write and a fresh prompt from
   old screen content. Preserve typeahead, startup-only WOW and GUI passthrough.
6. Every production P retains Console17/Window17, independent WOW frontiers,
   affected negative/lifecycle tests and coherent runtime publication/recovery.
   Extend the manifest with verified hook DLLs and any actual load dependency;
   do not substitute research/staged DLLs into O:/winnt.

Remaining S1 gates, in dependency order:

- **Installer:** prove a single helper-free four-width mechanism, or deliver
  the exact unavailable boundary for owner re-admission. Unmodified Detours
  is already excluded as the four-width answer.
- **Classifier/selection:** prove the Hook64 ABI facade and actual ordinary
  CMD selection/entrypoints; freeze any necessary minimal null-application
  handling from that evidence, not from guessed CLI behavior.
- **Bootstrap:** prove pre-entry copied context delivery/consumption and
  no-inherit authorization/rollback, with no loader-lock broker traffic.

Detailed design is complete at the contract level. These feasibility/runtime
proofs remain open; S1 is not declared closed and S2 is not auto-admitted.
No production capability, injection success, new package publication or
cross-width equivalence is asserted by this documentation P.

The source checkpoint's follow-up pins PR161 head and installed ntdll exports.
It narrows the remaining installer gate to32->64 full-width installation and
rollback. The current S1 does not admit private WOW64 calls/transitions or
executable probes; further work at that boundary needs a finite research
admission. This is a selected-mechanism blocker, not a claim that helper-free
cross-width installation is universally impossible. Keep all four directions
in the eventual acceptance matrix; do not silently reduce the target.

Design-delivery verification on2026-10-05: Verify-DocumentationGovernance.ps1,
Test-DocumentationRelativeLinks.ps1 and git diff --check passed. Reviewed the
changed architecture/status/index/plan and this contract against the source
checkpoint. Other-session proposal/TODO changes retain their incoming hashes
and are excluded from this delivery. No production source/build/runtime input
changed, so no runtime regression or redeployment is claimed or required by
the documentation-only P rule.

## Primary references

- [CreateProcessW](https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-createprocessw): native selection, mutable command line, environment, inheritance and startup contract.
- [DuplicateHandle](https://learn.microsoft.com/en-us/windows/win32/api/handleapi/nf-handleapi-duplicatehandle): recipient process references, rights and cross-width duplication; not proof of DLL injection.
- [DetourCopyPayloadToProcess](https://github.com/microsoft/Detours/wiki/DetourCopyPayloadToProcess): candidate copied-context carrier through remote allocation/write; not yet adopted or width-proved here.
- [Pinned installer source](https://github.com/microsoft/Detours/blob/e4bfd6b03e50de46b47abfbd1e46b384f0c5f833/src/creatwth.cpp): actual width rejection/helper boundary; source/license hashes retained in the checkpoint.
