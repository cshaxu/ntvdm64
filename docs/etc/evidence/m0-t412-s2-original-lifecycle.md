# T412 S2 original BaseSrv lifecycle execution

## Composition and reproduction

Run `node tools/audit/Verify-BrokerOriginalLifecycle.mjs`. Each invocation
compiles the complete unchanged `opennt-host/base/win32/server/srvvdm.c`, a
test host, and the original RtlConvertLongToLuid body extracted from the
accepted original declaration carrier. The inline storage specifier is removed
only for that generated test object; the algorithm is not rewritten.

The test links native NT event, duplication, token, loader and heap interfaces
using the installed SDK import libraries. Its finite host supplies the current
process heap and one request context for the test's actual process/thread.
CSR lookup accepts only that process. It implements no VDM queue, selection,
capacity, reentry, first-query or completion policy. UserNotifyProcessCreate
remains null through its original guard; this fixture does not exercise USER
input-idle or interactive-token acceptance.

The existing declaration probe and unused-DFS umbrella exclusion remain
test-only. They are not the final product header boundary. All outputs are in
`build/M0-T412/S2/original-lifecycle`; the driver checks x86 PE machine, unchanged
owner bytes and the link map's srvvdm.obj ownership of key service functions.
No deployed EXE is changed.

## Passing original-body assertions

1. First-VDM returns true once, then false, and remains false after a DOS
   record is created and removed in the same service instance.
2. Original CheckVDM creates the DOS Console record with copied command and
   VDM_TO_TAKE_A_COMMAND, reporting VDM_NOT_PRESENT.
3. Original directory set/get reports the required size on a short buffer
   without consuming the stored data. Successful retry copies and consumes;
   another query reports zero remaining bytes.
4. Original UpdateVDMEntry acquires the test worker process reference and
   creates a real unsignaled paired parent event.
5. Original GetNextVDMCommand rejects a short command buffer without dispatch,
   then copies the command and sets VDM_BUSY on retry. Parent wait stays pending.
6. Reporting completion with exit code 7 wakes the parent, preserves the code
   and creates the original no-work worker wait. Both are real NT events.
7. Original reentry increments twice and decrements to one; that decrement
   signals the worker wait even though the count is not zero. The function's
   original success result is TRUE, not STATUS_SUCCESS. The initial test
   incorrectly expected zero and was corrected; the source was not changed.
8. Original DOS exit removes the record and releases the server-side waits;
   the test closes its remaining client references. Repeated exit is rejected.
9. Empty shared-WOW acquisition succeeds without a wait handle and clears the
   command length, preserving the original nonblocking branch.

## Limits and next implementation group

This is execution of the real original server owner, not a policy simulation.
It remains a same-process fixture: it does not prove RPC receipt ownership,
cross-process authentication, Console binding, original BaseClient retry/copy,
WOW task submission/notification, fault-injected allocation rollback or product
selection. The existing product still selects its old local service provider.

S2 remains active. Restore the finite product declaration/request/resource
bindings and original client routines, extend negative/coherent lifecycle
coverage, and select the recovered provider before retiring the local policy.
No net production-code reduction, S2 closure or three-product build is claimed
by this evidence checkpoint.

## Original BaseClient restoration continuation

The complete original GetNextVDMCommand function is now restored in
`opennt-host/base/win32/client/vdm.c`, under OPENNT-HOST-014's explicit
command-cohort selection. It contains 390 original lines; its LF-normalized
SHA-256 is `47cf285cbc505d9d43a50af16e283ea51d1d9ef8d2ff093795086d0ca327addd`.
Comparison with `O:/repos.external/OpenNT/base/win32/client/vdm.c` at revision
`5c5b979ec08c17d3ca2eb70e8aad62d26515d01c` proves an unchanged function body.
Its original copyright notice remains in the mirror file. The four finite CSR
declarations retain signatures from the same tree's `public/sdk/inc/ntcsrdll.h`.
No CSR implementation or full Base DLL was imported.

The current default environment cohort still compiles independently. The
focused graph selects OPENNT_BASE_CLIENT_VDM_COMMANDS and checks that
GetNextVDMCommand links from client.obj while BaseSrv functions link from
srvvdm.obj. It uses the existing original RTL error.c/table with that source's
actual formal per-edge compiler flags, not a replacement error mapper.

Additional passing assertions use the original client to query first-VDM,
receive a short-buffer failure and ERROR_INVALID_PARAMETER, retry with the
returned required size, copy the real command, and retrieve empty shared-WOW
results. Capture allocations are drained after both failure and success. The
test transport supplies capture storage and dispatch only; the restored client
supplies capacity propagation, error mapping, result copy and cleanup policy.

The finite PEB/TEB used here is test-owned and supplies the same local Console
key and status carrier. It is not a modern PEB reinterpretation, registered
cross-process Console proof or a production authentication implementation.
The restored wait loop is source-identical but a wake-and-retry through the
client still needs its own asynchronous test. Other BaseClient lifecycle
routines and product bindings remain S2 work; the old selected product policy
has not yet been retired. Adding original source reduces the missing-source
scope, but is not a claimed net line deletion or product rollout.

## Coherent client lifecycle and real wait continuation

The same original source cohort now includes ExitVDM,
SetVDMCurrentDirectories, GetVDMCurrentDirectories, CmdBatNotification and
RegisterWowExec: a contiguous 320-line original block, LF-normalized SHA-256
`2936de23dcbbdec23cf9ea23416606a7a3faeb96473d407e4bfaee00f89ac3aa`, from the same
pinned source/revision. Together with GetNextVDMCommand, 710 original lines
have been restored; this is restored source volume, not net code deletion.
The driver validates both source hashes and each selected client symbol owner.

Additional passing tests now cover:

- Original client directory size query, retained data on short capacity,
  successful copy/consume and capture cleanup.
- Original batch start/terminate notifications create/remove the actual
  server BAT record for the fixture's Console key.
- Original RegisterWowExec registers a real test-owned message-only window
  through native USER loading and original PID/sequence checks. Clearing the
  invalid registration removes the saved HWND. This does not prove guest
  WOWEXEC message delivery or shared-WOW task execution.
- The original client waits on the real server-created event. Only after the
  server returns that wait handle does a test thread submit NEXT.COM through
  original CheckVDM. The original queue/event code wakes the client. Exactly
  two client dispatches occur; the retry carries exit code zero rather than
  the initial 99, and the returned command and drained captures are asserted.
- Original ExitVDM routes through original BaseSrv cleanup, signals the
  pending parent and closes the returned worker-side wait reference. The test
  verifies the closed reference is invalid before any further allocation.

The first directory capture implementation in the test needed its allocation
capacity rounded to the original four-byte message-pointer alignment. This
was a test-transport correction, not an edit to the original directory code.
No new outgoing interface shape or service policy was required for this group.
Native program classification, launch/update client routines and product
resource/authentication bindings remain unfinished; S2 stays open.

## Launch update and native path ABI continuation

Original BaseUpdateVDMEntry is restored unchanged (57 lines, LF-normalized
SHA-256 `de56cfccd08922ae57df7332cc8df70f60db186f44f177336188c2d236e9fd26`,
same pinned source/revision). The lifecycle test now sends its actual test
process handle through that original client and original server, verifies
conversion to the unsignaled parent wait and retains the subsequent dispatch,
completion and cleanup assertions. Total restored original client source is
767 lines; this is not net deletion or product selection.

Classification review found a concrete ABI hazard before importing its body.
Original GetBinaryTypeW allocates RTL_RELATIVE_NAME locally. Its original
declaration in public/sdk/inc/nturtl.h contains STRING plus HANDLE: 12 bytes
on x86. The installed native RtlDosPathNameToNtPathName_U writes a fourth
DWORD. `node tools/audit/Verify-BrokerPathNameAbi.mjs` proves this with an
overallocated guarded fixture for both absolute and relative paths: the DWORD
at offset 12 changes from A5A5A5A5 to zero, while four guard DWORDs beginning
at offset 16 remain unchanged. The original 12-byte storage would be too small.

The probe builds x86 /MT under build/M0-T412/S2/path-abi, calls native NTDLL,
frees its returned path and changes no runtime files. It proves write extent
for these cases, not the semantic meaning/ownership of the extra DWORD or
all path forms. This is an integration constraint for the pending classifier,
not a diagnosed fault in the currently deployed classifier or in original
NT4 logic. The next restoration must supply a correctly sized finite native
declaration binding and verify image statuses, malformed names, DLL rejection
and cleanup before selecting the original classifier. Do not patch the
classification algorithm or invoke modern NTDLL with the undersized old type.

## Native classifier dependency verification

The next source review located the original path-conversion body in pinned
OpenNT `base/ntdll/curdir.c:1755`, not the kernel RTL directory. Its optional
relative-name result borrows the current-directory handle and points inside
the allocated full-path buffer. This explains why original GetBinaryTypeW
frees the full-path allocation but does not close that directory handle.
It does not establish ownership of the modern extra DWORD; a declaration-only
binding still requires that check. No NTDLL implementation is imported.

The x86 /MT path ABI probe now also exercises the original classifier's native
image dependency against the four real package files. It opens execute-only
file handles with read/delete sharing, calls NtCreateSection with SEC_IMAGE
and PAGE_EXECUTE, closes resources and executes no guest code. Its driver
records sizes and SHA-256 identities and checks the inputs are unchanged.
Command: `node tools/audit/Verify-BrokerPathNameAbi.mjs`; results remain under
`build/M0-T412/S2/path-abi/result.json`.

| Input | Actual native status | Original GetBinaryTypeW disposition |
| --- | --- | --- |
| MEM.EXE | C0000130, STATUS_INVALID_IMAGE_PROTECT | DOS binary |
| COMMAND.COM | C000012F, STATUS_INVALID_IMAGE_NOT_MZ | BaseIsDosApplication suffix path |
| EDIT.COM | C000012F, STATUS_INVALID_IMAGE_NOT_MZ | BaseIsDosApplication suffix path |
| system32/WRITE.EXE | C0000131, STATUS_INVALID_IMAGE_WIN_16 | WOW binary |

All four exact status assertions pass; no section is returned. Status names
are checked against pinned OpenNT `public/sdk/inc/ntstatus.h`. This is native
dependency evidence, not execution of the restored classifier, guest runtime
acceptance, PE/DLL/malformed-image coverage or S2 closure. The finding supports
keeping the original classification branches rather than introducing another
binary parser. In the final composition only run16 invokes file classification;
basesrv receives BinaryType for original DOS/WOW policy and ntvdm is the worker.

## Original classifier restoration and relative-path boundary

The independent launcher classifier cohort now compiles and links original
GetBinaryTypeW (211 lines) and BaseIsDosApplication (60 lines) from the pinned
OpenNT vdm.c. Their bodies are unchanged; the verifier compares normalized
text against the source and fixed SHA-256 identities:

- GetBinaryTypeW: `f7fd9766ea08e676a2fd457fb3fa508384630640032ce0615f752dc8a43f9fbd`.
- BaseIsDosApplication: `9487407a2efc02764466cedbabb4e89424fd12d87d11601cce9fa5ad477f3711`.

This restores another 271 original lines, totaling 1,038 across the S2 client
groups; these are restored source lines, not net deletion. Source ownership
remains opennt-host. The private symbol maps to OpenNtBaseGetBinaryTypeW to
avoid Kernel32's import, and the link map proves both bodies come from the
original client object rather than the native convenience API.

The first linked run passed all four guest types and native PE but failed the
missing relative-file test: native status C000003B mapped to error 161 rather
than file-not-found. A direct probe showed nonzero relative-name length with
NULL ContainingDirectory and NULL extra DWORD. Merely enlarging the NT4
structure therefore did not restore the original contract. Original NT4
curdir.c publishes a relative name only with a borrowed current-directory
handle. The modern observed API does not supply that pair.

Recovery disposition:

1. Original classifier and suffix algorithm compose unchanged and are retained.
2. Original NTDLL curdir.c requires its native PEB/current-directory ownership
   and process-heap/lock environment; importing that subsystem is outside the
   finite boundary. Instead the same-shaped adapter requests the existing
   native full-path result (optional relative output NULL), clears the original
   relative record and lets unchanged BaseClient take its existing full-path
   branch. No extra output DWORD, directory reference, parser or state machine
   is needed. Returned path allocation/free ownership remains native heap.
3. No changed expression inside either original function is required.
4. No newly authored classifier policy is required. The adapter body is three
   statements, implementing only the declared path-result binding.

The private declaration header also selects native NtOpenFile rather than the
existing DEM DOS-path wrapper, retains the 48-byte x86 image-information
structure and binds omitted native declarations/status constants. Existing
private support PEB gains ImageBaseAddress from GetModuleHandleW(NULL), not a
cast into modern PEB memory. Test suffix initialization uses the original
baseinit.c values. Notices remain with imported source; no new source family
or redistribution permission is claimed.

`node tools/audit/Verify-BrokerOriginalClassifier.mjs` passes:

- MEM, COMMAND and EDIT classify as DOS; WRITE as WOW.
- The fixture's native x86 PE classifies correctly by absolute and relative path.
- Loaded Kernel32 DLL is rejected with error 193 and untouched type output.
- Controlled non-MZ .COM/.pif/.exe inputs follow the original suffix policy;
  .bin is rejected with error 193 and untouched type output.
- A missing relative file returns error 2/C0000034 and untouched type output.
- Original-body hashes, link providers and x86 PE machine are checked. The
  actual support translation unit is compile-checked; fixture bindings provide
  the small process/TEB fields, not classification policy.

The original lifecycle regression and native path/image probe also pass.
Generated malformed inputs and binaries remain under build/M0-T412/S2;
no guest program is executed and no deployment is changed. Native x64 image
rejection, OS/2 media, path-race cases and final production selection remain
unproven. S2 stays active: BaseCheckVDM and the remaining coherent launch
closure, finite product bindings and old local-policy retirement still remain.

## Complete original launch-request restoration

BaseCheckVDM is restored as one unchanged 876-line function from the same
pinned vdm.c. LF-normalized SHA-256 is
`579840e2d5598051d157d9e92871027b543ecfcba8d299a745928bfd71c6386e`.
The verifier checks both source equality and this fixed identity and requires
the linked symbol to come from client.obj. S2 now restores 1,914 original
client lines; this is source recovery, not net deletion of the old provider.

The original function retains DOS/PIF/new-console decisions, command-tail
extraction, OEM/ANSI conversions, short-path/PIF lookup, directory limits,
startup fields, capture construction, BaseSrv dispatch and cleanup. It does
not classify the file again. The launcher supplies the previously determined
BinaryType. In the focused test actual BaseCheckVDM calls actual BaseSrvCheckVDM;
the transport only dispatches and the original server owns the copied result.

Finite outgoing bindings, after reviewing the whole body:

- Existing native/public path, conversion and heap APIs supply the original
  operations; no new parser, PIF policy or environment algorithm is authored.
- Private PPEB binds the existing support carrier, with startup WindowFlags
  from GetStartupInfoW and standard streams refreshed through GetStdHandle.
  These are process-local fields, not a native PEB cast or IPC identity.
- Original public/internal/base/inc/conroute.h identifies NT4 Console pseudo
  handles by low bits. The modern predicate uses GetConsoleMode and preserves
  LastError. Original stream filtering stays in BaseCheckVDM. Cross-process
  Console binding and resource attachment acceptance remain S3 work.
- Compilation exposed a pre-ISO swprintf call in the original hotkey branch.
  The private header selects Microsoft's _swprintf shape, as documented by
  installed SDK corecrt_wstdio.h's _CRT_NON_CONFORMING_SWPRINTFS mapping.
  No original formatting expression or hotkey policy is changed.

Recovery ladder: the complete original function compiles unchanged through
these finite bindings (rung 2); direct modern headers lack those historical
process/Console/CRT contracts (rung 1 cannot close alone). Neither mirror-body
intrusion nor autonomous launch policy is necessary (rungs 3/4 rejected).

`node tools/audit/Verify-BrokerOriginalLifecycle.mjs` now additionally passes:

- Quoted full application path and whitespace yield the exact /? CR/LF/NUL
  command tail in the original server-owned record.
- Application name, double-NUL environment, title, desktop and reserved
  fields survive capture cleanup in the original deep copy; captures drain.
- Original ExitVDM removes the newly admitted record.
- STARTF_USEHOTKEY with value 42 yields exactly `hotkey.42 test reserved`,
  clears the source-defined flag/handle and preserves the server copy. The
  fixture discards the original routine's freed temporary reserved pointer.
- Injected capture-allocation failure returns out-of-memory without dispatch;
  NULL environment returns invalid-parameter without dispatch or live capture.
- Earlier first-VDM, capacity, event/wake/retry, directory/BAT/WOW registration
  and teardown assertions continue to pass. No implicit-function or mismatched
  argument warnings remain in this final lifecycle compilation.

The original classifier regression also passes and compile-checks the actual
support translation unit. This is still same-process focused protocol proof,
not product selection, shared-WOW launch, full PIF coverage, cross-process
transfer or guest execution. S2 remains active; formal owner selection,
remaining launch dependencies and old local-policy retirement are not done.

## Original owner libraries without test declaration scaffolding

The selected lifecycle build no longer force-includes tests/broker/
source_compile_probe.h, creates an empty ntdddfs.h or generates a non-inline
RtlConvertLongToLuid body. The source audit identified the DFS include as an
unrelated final include in basesrv.h: srvvdm.c uses no DFS declarations. The
registered OPENNT_BASE_VDM_SERVER header group omits that include and imports
the finite package-private base_server.h instead. All service algorithms and
the entire srvvdm.c file remain unchanged.

The private header retains audited native/event/token/loader and original CSR
record declarations. Its request-thread accessor replaces the historical
TEB lookup but must be supplied by authenticated process binding; it does not
create authentication or service policy. Tests bind their own caller context.
This is recovery rung 2, not importing a CSR subsystem or manufacturing an
empty subsystem header. The older S1 compile probe remains historical test
evidence, but is not an input to this selected library build.

Original public/sdk/inc/ntrtl.h's RtlConvertLongToLuid inline is now retained
in the mirrored header subset. It preserves its original body and __inline
shape (blank-line trailing whitespace normalized), so the prior generated
external definition and object are unnecessary. Tests compare the inline
against the pinned original and assert positive and negative sign extension.
Three disposable files were removed from the validated S2 build root:
luid.c, luid.obj and ntdddfs.h. They were generated test products, not source
or runtime assets; their old recipes remain in Git history.

The lifecycle driver now creates opennt-base-server.lib and
opennt-base-client.lib and links the actual service/client owners from those
archives. Provider-map assertions require both library/object identities,
reject luid.obj, and build-log assertions reject the three retired include/
generated-source dependencies. The complete original launch/lifecycle tests
pass with these files absent; the classifier/support compile regression also
passes. Outputs remain under build/M0-T412/S2/original-lifecycle.

This proves reusable original-owner library composition, not final product
graph selection: the deployed executable still selects the old local policy.
Authenticated product context, resource transfer and final process wiring
remain required. S2/T412 are not closed by these library tests.

## Source-owned shared-WOW identity admission

The complete outgoing boundary of original BaseSrv OkToRunInSharedWOW was
reviewed with original windows/core/ntuser/server/exports.c and
windows/core/ntuser/kernel/security.c. The first contains a directly reusable
token-query helper; the second assumes the first global kernel windowstation
is the only interactive one. S1 already admitted a per-user/session/logon
replacement for that unavailable singleton predicate, not a success stub.

Only `_UserTestTokenForInteractive` is imported as a true exports.c subset.
Origin: the same pinned OpenNT revision; original file SHA-256
`dbeb0ad48349bf5887a1333f21b427d8e0fdd3cb226977a9dc786373e961c71f`;
67-line function SHA-256 (LF normalized, without trailing newline)
`7b6c9072220144e4b77aca973682838333994c3bc962eb802143e210e912a520`.
The helper's token sizing/query, LocalAlloc/LocalFree, AuthenticationId output
and status behavior are unchanged. The included LATER block stays inactive.
No other winsrv function or USER subsystem is admitted; no original notice
is removed and this is not a distribution/license grant.

Recovery ledger:

1. The original token helper is composable unchanged and retained under its
   original-relative opennt-host path. Modern native token query and public
   allocation supply its ordinary dependencies.
2. Its NtUserTestForInteractiveUser edge cannot call NT4's private syscall or
   access grpwinstaList. The finite binding captures the trusted broker process
   AuthenticationId only when its process windowstation is visible, then binds
   that immutable scope to the dispatch thread. It compares the original
   helper's queried LUID, not a wire-provided identity. Missing/invalid scope
   or mismatch returns access-denied; no windowstation is created or modified.
   This is the S1-approved scoped replacement, not global interactive-user
   enumeration. The initializer rejects NULL and fails closed when host queries
   cannot establish this scope.
3. No expression in srvvdm.c or the imported token helper is changed.
4. No autonomous token-query or SYSTEM-impersonation policy is introduced.
   Only unavailable interactive-scope capture/binding/comparison is new.

The lifecycle fixture installs the linked original helper into the original
service callback slot, captures its own real interactive scope, and stamps the
test-authenticated caller identity into the local message header. Tests now
exercise original BaseCheckVDM -> BaseSrvCheckWOW -> OkToRunInSharedWOW ->
original token helper -> finite predicate. They assert:

- no bound scope rejects with access-denied, no WOW record and drained captures;
- a deliberately mismatched expected AuthenticationId likewise rejects (this
  is a controlled predicate-negative test, not another user's real token);
- matching actual logon admits WRITE's command record as VDM_NOT_PRESENT and
  preserves its application/task identity and original WowAuthId;
- original partial-creation undo removes the record and resets WowAuthId to
  the original invalid LUID; the thread scope is unbound before leaving it;
- an invalid token fails without modifying the caller's AuthenticationId output;
- earlier DOS/client/server/registration/wait/cleanup tests still pass.

Function hash/source equality and library map ownership are checked by
Verify-BrokerOriginalLifecycle.mjs; outputs remain in the existing S2 build
root. These tests do not run WRITE, load modern winsrv, create a VDM worker or
alter tokens. Actual SYSTEM impersonation, another logon/session, non-visible
windowstation initialization and production RPC identity binding are not yet
verified. The formal broker must install this source helper and bind a trusted
scope before service dispatch. Product selection and T412 closure remain open.

The scope capture/predicate explicitly refuses the original winnt.h SYSTEM_LUID
as an interactive-user identity, even on a visible windowstation. A controlled
predicate test verifies this refusal. This does not disable original BaseSrv's
SYSTEM-caller impersonation branch: that branch must match the independently
captured real interactive-user scope, not grant access to SYSTEM itself. A
broker started under SYSTEM cannot derive an interactive-user scope from its
own token and therefore fails shared-WOW scope initialization rather than
guessing another user's identity.

## Original local capture memory recovery

The original BaseClient requires three capture-memory functions. The admitted
slice is CsrAllocateCaptureBuffer, CsrFreeCaptureBuffer and
CsrAllocateMessagePointer from pinned OpenNT base/ntdll/csrutil.c. Its original
file SHA-256 is
`7a176482498e1aca00e9651c326a9b62a8ba2ada0aa7f90f8b585acd1a376e6a`;
the unchanged 235-line function group (LF, no final newline) hashes to
`41d54b575c6895809c956136c3a35067dc3b87610a6226d8ec23fab431e11cab`.
The original copyright notice remains; this grants no distribution rights.

Recovery ladder: the three original bodies compose unchanged using a private
process heap. The declaration-only base_capture.h binds CsrPortHeap and omits
historical heap tag metadata. No source-body intrusion or autonomous allocator
is needed. CSR/LPC transport, shared port sections and pointer rebasing are
explicitly rejected; CsrClientCallServer remains a separate transport boundary.
The original pointer tables contain local addresses, never RPC wire values.
Original routines trust their callers' counts; they are not hostile-input
validators. The future wire binding must validate copied fields independently.

Verify-BrokerOriginalLifecycle.mjs compiles the original slice with its /Gz
calling convention, checks source equality/hash and verifies all three symbols
come from opennt-base-client:capture.obj. The fixture deletes its three authored
allocator functions and observes a real private heap using HeapWalk. Tests cover
four-byte alignment, zero-length entries, pointer-table field identities,
MAXLONG rejection and drained allocations. A genuinely exhausted non-growing
heap makes original BaseCheckVDM fail before dispatch with out-of-memory;
destroying that heap releases its deliberate exhaustion allocations.

The focused x86 run passes these checks and the existing DOS/shared-WOW
lifecycle tests; build and result records remain under
build/M0-T412/S2/original-lifecycle. This does not execute guest programs.
Production heap initialization/drain/destruction and RPC integration remain
unfinished. The deployed ntvdm32.exe is unchanged; S2 and T412 remain open.

## Launcher task exit query recovery

Original base/win32/client/process.c GetExitCodeProcess calls BaseCheckForVDM
before querying a native process. This is task-exit retrieval, not executable
type classification. run16 needs this source-owned path for the parent event
returned by BaseUpdateVDMEntry: worker lifetime is not guest task lifetime.

The unchanged 39-line BaseCheckForVDM is restored in the command cohort of
opennt-host/base/win32/client/vdm.c. Pinned original file SHA-256 is
`3f03d0dbb08e0163f2d9cf415daad0981e42e1b1855f6f48a3b59022b7374173`;
function SHA-256 (LF, no final newline) is
`c6cf2dfdab281ee73c257c5f0a051eebe3439355f273d4a2a5eae5fa7e9d7b85`.
Original notice remains. Total recovered command/classifier client groups now
contain 1,953 original lines; this is not a net deletion count.

Recovery ladder: original body composes unchanged; NtQueryEvent exists on the
host with the original event ABI. base_event.h retains the ntexapi.h type shapes
and declaration and is shared privately by client/server headers. There is no
new runtime adapter, source-body edit, process API replacement or recursive
Kernel32 import. CsrClientCallServer remains the known finite transport seam.

The lifecycle test now dispatches BasepGetVDMExitCode to unchanged
BaseSrvGetVDMExitCode. It verifies invalid and process handles reject without
changing output; the real parent event returns STILL_ACTIVE before completion;
completion returns 7, moves the sole record to VDM_READY and clears its parent
association. A second query returns the original no-matching-record value zero,
not a fabricated retained exit-code cache. Remaining wait/retry and cleanup
tests pass after this source-owned consumption transition.

The verifier checks exact original body/hash and client-library map ownership.
This is focused x86 lifecycle evidence, not deployed run16 or guest execution.
Final process-local event attachment and CLI return-code propagation remain S3/
S4 integration work. Product binaries and current T/S status are unchanged.

## Original worker command construction

Original process.c calls BaseGetVdmConfigInfo after service admission selects a
new worker. This is launcher behavior. Neither broker nor worker reclassifies
the target. The original 209-line function is restored with one call-site
adaptation under OPENNT-HOST-014. Original function hash (LF, no final newline):
`2d57d7b0edaf2534e9a5654d3a2e30a6d91e2a24aea04fcc9e282af747a8c667`.
The original file/revision is the same as the preceding client restorations.

The full original configuration group was reviewed: BaseGetVDMKeyword and
GetVDMConfigValue read the machine WOW registry and expand its environment.
That host-installed configuration is not this standalone package's authority.
Those bodies are rejected, not reimplemented as a registry emulator. The finite
base_config binding receives app-owned image and kernel-stem paths, validates
the original 256-byte template envelope, quotes/newlines and keyword/capacity
contract, and returns the original 16 MiB reservation value. It owns no files,
handles, guest memory or launch/service policy. Bound storage is immutable and
thread-local in use, and must outlive calls until unbound. It is not wire input.

Unchanged BaseGetVdmConfigInfo cannot support the agreed layout: it literally
searches for backslash-system32-backslash-ntvdm before finding the image end.
One source call is therefore replaced by OpenNtBaseVdmImageEnd, which returns
the already configured image boundary. All remaining source text is checked
against upstream. This avoids creating a fake system32 image or changing argv[0]
to an executable different from the one launched. Original quoting and internal
-f, hexadecimal -i, WOW -w/-a, extension and short-name conversion are retained.
These are worker arguments supplied by run16, not public diagnostic switches.

The x86 test constructs DOS and WOW command lines, including an image path with
spaces and session 0x12ab. It verifies native short-name conversion against the
existing KRNL386.EXE without executing or modifying it. Unbound configuration,
oversized/quoted input, short output and wrong keyword pairing reject. The
original routine retains its historical error behavior; this binding does not
claim to repair arbitrary malformed original registry configuration. Run16 must
validate actual files and lossless ANSI conversion before binding; those app
calls and real worker startup are still integration work, not proven here.

The verifier checks original hash, the exact single source substitution and
client-library symbol ownership. Earlier lifecycle tests remain required.
This adds finite configuration glue, not net code removal; superseded current
launch-declaration code cannot be deleted until real product selection passes.

## Formal owner archive integration

New-T310OriginalSoftpcNinja.ps1 now exposes the explicit x86 target
opennt-broker-owners with opennt-base-client.lib, opennt-base-server.lib and
opennt-base-bindings.lib. Nine compilation members are recorded with paths,
hashes and explicit non-product-link disposition in source-manifest.json.
Client command/classifier cohorts and capture compile independently of the
server forced header; only server owners receive their private server binding.
Capture retains /Gz. No fixture object or generated declaration is archived.

The first graph attempt omitted per-member flag suffixes because PowerShell
array concatenation parsed the flag expression incorrectly. The resulting
capture calling-convention error was fixed in the generator with parenthesized
flag expressions, not by altering original function signatures. All nine
members and three archives then built; regeneration followed by the same
target reported no work to do.

Both existing verifiers accept OPENNT_BROKER_OWNER_BUILD. In that mode they
compile only fixture and original RTL error support, then link the actual
formal archives rather than separately compiling the service/client/bindings.
Map checks identify original library providers; result.json records the owner
build directory. Both suites pass under build/M0-T412/S2/formal-owners:
DOS/WOW worker arguments, lifecycle/capture/wait/cleanup, task-exit consumption,
scoped WOW admission, real-media classification and negative classifier cases.
The fixture still supplies local process/dispatch mechanics; it is not RPC or
guest execution evidence.

Observed archive SHA-256 values:

- Client: `768b471bd25136e0397c6cd6e7215bbf25af1765d60d2208087cb53277e8915d`.
- Server: `8bb79537154de71a8bd5227a2127e3f2bb16b44af4d3182d38c8fcf1f60a73e6`.
- Bindings: `d5087f426a59fcd37e97287f41841b175e44662a1ef970b07ab636617c085ab8`.

These are reusable formal graph targets, not a new default product policy.
The current EXE still selects basesrv-bindings.lib and the old local provider.
Three-process entry/transport selection and its retirement gate remain open;
neither S2 closure nor a new deployed executable is claimed by this delivery.

## Original generation registration and process cleanup

Source review traces original srvtask.c BaseSrvCreateProcess after successful
CSR registration: it marks the registered process fVDM and calls
BaseSrvUpdateVDMSequenceNumber. srvinit.c's process-disconnect callback calls
BaseSrvCleanupVDMResources. The complete original implementations already
exist in the formal server archive; no new cleanup policy or source import is
needed. The standalone process watcher must eventually provide authenticated
registered process generations at these edges, not accept a wire PID/sequence.

The focused lifecycle fixture now admits both DOS and shared-WOW tasks through
original BaseCheckVDM, creates real parent events through BaseUpdateVDMEntry,
and invokes original registration/cleanup using controlled process carriers.
For each branch it proves:

- the first sequence assignment persists; a second assignment does not replace it;
- cleanup for another sequence leaves the parent event pending;
- a matching sequence without fVDM does not remove the VDM task;
- matching VDM cleanup signals the parent event and removes the DOS/WOW record;
- shared-WOW cleanup clears its notification window and sequence identity;
- repeated cleanup is harmless, and the caller closes its surviving event copy.

Tests pass against the formal owner libraries, along with earlier lifecycle
checks. Provider-map assertions name both original functions. No actual worker
was terminated in this fixture: OS death detection, authenticated registration,
concurrent cleanup and process-generation allocation remain real-process
integration requirements. Source-owned cleanup coverage does not prove those
mechanics or authorize a new shadow record table.
