# T478 S45 existing inventory reconciliation

Status: active. This resumes deferred S1/S42 work; it does not create a new
source audit or reinterpret1546 audit identities as1546 defects.

Inputs are the stable crosswalk, pinned external original audit ledgers,
8f076ff6a and the unchanged S43 eleven-image package. Reconciliation must
retain separate architecture scope, current source/build binding, tested
behavior and coverage limits. Inactive OpenNT-4.5 rows remain comparison-only.
S43 fixes the class-brush collision; S44 explains the width via original
minimized sizing and modern metrics. Neither substitutes for remaining rows.

## Build-profile family

Read the three module1.1 source contracts and their distinct build conditions,
then compare the project GOAL, current generator and generated worker graph.
The audit explicitly labels implementation validation as not performed; that
is an audit coverage statement, not proof that the current product lacks CCPU.
Detailed dispositions and current build witnesses follow below.

## Build-profile dispositions

| Stable identity | Disposition and evidence boundary |
| --- | --- |
| DOS16:fba9809d0d636de17805 | Original386/MONITOR executor profile excluded by the permanent CPU30 retirement and CCPU-only GOAL. No kernel V86 service is imported or deferred. Revisit only after an owner architecture decision; this is not proof of all software CPU services. |
| DOS16:459781a6ded66751cc88 | Original non386 assembled CPU variant is an alternate executor, outside the sole selected CCPU40 profile. No missing application capability is established by this unselected variant. Revisit only if the supported executor profile changes. |
| DOS16:42c2581b0f33782a0100 | Selected original CCPU40 profile: generated base/CCPU flags contain CPU_40_STYLE, CCPU and C_VID;130 original CCPU compilation edges feed original-ccpu386.lib. Actual worker map assigns c_cpu_simulate to original-ccpu386:c_main.obj and host_simulate to original-softpc-host-roots:nt_cpu.obj. This closes profile selection only, not every CPU/memory/device contract. |

The [profile receipt](../../../artifacts/mvdm-audit/t478-final/s45-profile-selection.json)
pins current generator, graph, map, goal and release manifest. CPU_30_STYLE is
not defined anywhere in this graph. A naive MONITOR absence assertion would
be false: BIOS bios.c, keybd_io.c, video.c, printer.c and host nt_lpt.c retain
local MONITOR declarations. Generator comments at the source-profile boundary
explicitly distinguish these source-proven NTIO/video/peripheral selections
from executor selection. CPU40/CCPU remains present; do not erase valid local
conditions or claim all MONITOR branches are excluded.

Existing [S32 export evidence](m0-t478-s32-export-disposition.md) verifies the
selected original declaration/ABI profile and distinguishes metadata i386 from
CPU30 execution. Current S43 Full verifies16 bounded gates including current
PE ordinals, actual DOS Console/Window, WOW and lifecycle paths. S44 again
exercises the unchanged CCPU worker with real WINMINE. These are bounded
execution receipts, not exhaustive opcode/fault acceptance. No new test or
rebuild is justified just to reconfirm an unchanged profile selection.

## Checkpoint and remaining scope

This first mapping resolves three profile identities, not three new bugs.
The stable inventory still has1059 pending active identities, one bounded VDD
row and483 inactive-edition rows. Module1.2 startup and1.3 bootstrap remain
unmapped; their detailed failure claims must not be inherited from ordinary
successful launch alone. Other owner families and T473 handoffs remain S45
work. No production change, release replacement or task closure is claimed.

## Startup and bootstrap checkpoint

The [26-row source/evidence mapping](../../../artifacts/mvdm-audit/t478-final/s45-startup-reconciliation.json)
records each module1.2/1.3 identity, current owner, distinct coverage limit and
next action. Twelve rows now have bounded dispositions: ten retain selected
original owners with normal startup evidence; one retains a demonstrated prior
PIF correction; one excludes installation-only host boot-marker writes.
Fourteen rows remain pending, even though their owners are located. This is
not a claim of exhaustive failure testing or complete startup-family closure.

Original TimerInit/CpuEnvInit/nls_init/host_main ordering is retained within the
standalone worker begin/finish envelope. Current map resolves each owner.
CpuEnv uses its original list and a source-local renamed getenv, rather than
interposing on UCRT. Registry access is the selected read-only shadow provider;
S42's registry test must not be described as all CpuEnv value/failure tests.
CMD and DEM use the same selected system root, keeping their different length
conditions. DemInit still sets original error-mode flags before allocation.

First-VDM is the original service-global consuming query. The current service
holds its lock around BaseSrvIsFirstVDM; it is not the startup Console-origin
bit, which remains cmdGetStartInfo's exact DosSessionId boolean. The existing
[T412 lifecycle evidence](m0-t412-s2-original-lifecycle.md) and
[final verification](m0-t412-s10-final-verification.md) identify prior query,
capacity and restart tests. Historical timeout figures there do not override
later lifecycle policy. Current Full demonstrates current normal startup and
retirement, not every historical race schedule.

The original NTIO loader still performs reset, then SetupInstallableVDD, then
LIM/scs and the exact image read/state/CS:IP publication. Current map resolves
InitialiseDosEmulation to original nt_msscs. NTDOS retains original demLoadDos
and its actual-byte streaming loop. S43 actual DOS execution supplies normal
boot evidence; zero/short-read or memory-failure injection is not claimed.
COMSPEC still comes from the guest SVC through its synchronous copy boundary;
original unbounded final copy is not certified for arbitrary long inputs.

### Existing corrections and exclusions must not be undone

[T420 S35](m0-t420-s35-xms-capability-progress.md) records that original
GetPIFConfigFiles freed CONFIG's override when LIM read it, before COMMAND's
second read. The retained DIV-157 lifetime correction keeps that same original
owner, frees replaced selections and does not introduce another parser.
[T420 S37](m0-t420-s37-oemuni-capability-review.md) records the OEM-to-ANSI
consumer handoff and corrected LIM output capacity. Current selected bodies
still contain those changes. Restoring the audited one-shot body would undo
a demonstrated repair. Their old runtime paths are read-only historical
receipts, never new publication destinations.

AddSystemFiles' C:/MSDOS.SYS and IO.SYS placeholder creation is explicitly
excluded by DIV-134 and the non-invasive product boundary. The selected no-op
does not fabricate files or claim their existence. No replacement installer or
virtual marker provider is required. The original first-query call remains.

### Pending subcontracts remain explicit

The fourteen pending rows cover PIF parser variants and caller copies,
CONFIG/AUTOEXEC byte accounting, temporary files, initial/merged environment,
early native initialization, PIF reply boundaries, service capture/consume,
launcher templates, CheckVDM admission, worker publication rollback, launcher
dual environments and first COMMAND frame. Each row has its own next action
in the receipt. Existing original realloc/WORD/Reserved capacity risks are
not silently fixed, falsely marked safe, or elevated to new tolerance tasks.
The generated BaseGetVdmConfigInfo object is sourced from the original
function; its object name alone is not evidence of an autonomous duplicate.
The bound immutable-template provider remains a separately recorded boundary
requiring its existing justification to be reconciled.

After this checkpoint1047 active identities remain pending, including these
fourteen. Current product/release bytes are unchanged; no new Full or local
release is necessary for this evidence-only P. S45 remains active.

## P3 launch-template disposition

The [identity receipt](../../../artifacts/mvdm-audit/t478-final/s45-launch-template-reconciliation.json)
resolves DOS16:4c1abcb8b4894efee724. The generated service carrier's complete
BaseGetVdmConfigInfo function equals the selected mirror after newline
normalization. This is not a claim that the selected mirror is byte-exact
upstream: OPENNT-HOST-014 supplies the package image boundary and
OPENNT-HOST-065 retains pointer-difference arithmetic without address narrowing.
The service map resolves the function to config-command.obj. Current
worker_registry.c binds trusted sibling image/kernel paths, invokes the original
function and restores its previous thread-local binding before handling errors.
Original quoting, -f, hexadecimal -i and WOW -w/-a construction remain there.

[T412 configuration evidence](m0-t412-s2-original-lifecycle.md#original-worker-command-construction)
explains why NT4's machine WOW registry and hardcoded system32 image search are
not standalone configuration authorities. base_config.c supplies finite immutable
values and original 16 MiB reservation, not another argument parser. Its current
source retains capacity/keyword checks. Restoring the installation registry would
not remove a duplicate implementation; it would replace the admitted package
configuration boundary with an unrelated host installation dependency.

[T416 recovery](m0-t416-s2-wow-command-carrier-restoration.md) already removed
the unpopulated WOW fallback and retained the original missing--a failure path.
Its lifecycle fixture records DOS/WOW construction, spaces, session ID and
rejection. Inspection confirms those assertions still exist at lines515-548 of
tests/broker/original_server_lifecycle.c. That historical fixture explicitly
references O:/winnt and was not rerun or presented as fresh runtime evidence.
S43's unchanged Full and real WOW startup cover the current normal path only.
No claim is made for arbitrary registry templates, allocation failures or all
worker publication/rollback contracts; the latter retain their separate rows.

This evidence-only checkpoint leaves13 startup rows and1046 active inventory
identities pending. No product change, rebuild, publication or new tolerance is
needed for this row. S45 and the full T478 objective remain open.

## P4 current PIF caller and environment-copy evidence

The [receipt](../../../artifacts/mvdm-audit/t478-final/s45-pif-caller-reconciliation.json)
resolves DOS16:e38a5b13166e6e9c8136 with current x86 CCPU40 owner inputs.
The maintained OEMUNI generator builds in the stable
build/.ninja/msvc-x86-release/oemuni-fixtures cache and links the formal worker's
original cmdpif.obj. Background runner9132 completed build and both fixture
processes with exit0; its 120-second build and30-second per-case deadlines were
not reached. Exact commands, logs, hashes and markers are in the receipt.

Caller cases cover OEM non-ASCII startup paths, original command CRLF, task
metadata, non-PIF bypass, missing directory/file, wrong extension, four-field
cleanup and the original64-byte startup-directory rejection. The separate
current copy-adapter fixture covers acquire/release/allocation failure,
unterminated input, tiny input, non-first-task bypass and snapshot disposal.
GetPIFData is a fail-closed substitute in the caller fixture; environment
expansion is a copying substitute. Neither proves binary PIF extension parsing
or environment expansion. DOS16:6183631816abcfe32197 stays pending: inspection
of the old default-parser fixture finds only a no-extension/default witness,
not 286/386/WNT/Win95 precedence or partial-state validation.

The first current build failed to link two later unrelated scalar/FDC imports.
The test now supplies fail-closed substitutes for session_register_teardown and
guest_memory_lease_copy; entry exits99, so neither can silently simulate success.
The second run rejected this deep worktree's over64-byte directory before the
caller test. The runner now accepts an explicit WorkRoot; this run used an owned
TEMP child, not a drive alias. The third run exposed the old fixture's long-path
spelling assertion: original cmdCheckForPIF intentionally selects GetShortPathName
before SetCurrentDirectory. The corrected oracle compares that short spelling.
Eight nested short components make the rejection control long even with8.3 names.
Successful runs remove their owned directories bottom-up. Failed attempts remain
as diagnostics and are not counted as passes. None requires a product change.

The active packet permits this bounded existing-fixture maintenance. The release
remains S43, with no rebuild/publication needed for unchanged products. Twelve
startup rows and1045 active identities remain pending; S45 is not closed.

## P5 bounded original PIF parser evidence; row remains pending

The [partial receipt](../../../artifacts/mvdm-audit/t478-final/s45-pif-parser-partial.json)
adds eight current production-object cases for DOS16:6183631816abcfe32197.
The new focused fixture links formal x86 CCPU40 obj/host/nt_pif.obj, supplies
valid packed STDPIF/extension bytes through an owned temporary file and calls
GetPIFData. It does not copy the parser or rebuild/modify guest media. The
maintained OEMUNI generator exposes oemuni-pif-parser-fixture.exe in the stable
fixture cache. Run build/t478-s45/run-parser-p5-r2.ps1, background PID15448,
completed both build and fixture with exit0 and PIF_ORIGINAL_PARSER_8_ROWS_OK.
The runner imposes120-second build and30-second execution deadlines.

Observed: no-extension defaults;286 XMS values;386 overrides286 regardless of
ordering;Win95 idle-derived55/55 priorities override386 regardless of ordering;
WNT subsystem extraction while config loading is ignored; original default
initialization leaves seeded global priorities73/27 unchanged. Empty startup
strings remain unallocated. Valid-file execution closes the handle, allowing
fixture deletion after all cases. This does not certify failure-path close.

The first link failed on three product-root/config imports. The fixture now
supplies fail-closed substitutes for them; unexpected entry exits99. The inputs
use explicit PIF paths and IgnoreConfigAutoexec, so those providers are not
exercised. That failed build is retained separately and is not a pass.

Reading the complete original audit acceptance narrowed the remaining work:
default-path provider identity, exact short-read/seek/close and partial-error
state, repeated retained WNT config paths, allocation retry/IGNORE, and fast-paste
call history. The source checks read==-1 rather than exact byte counts and does
not reset every process global in SetPifDefaults. These facts do not authorize
read-exact retries, global resets or malformed/cyclic-file tolerance. Keep the
existing DIV-157 ownership repair, whose demonstrated original defect was
already recorded. No new product gap has been established in this checkpoint.
The row stays pending, as do twelve startup rows and1045 active identities.
No product change or new publication; S45 remains the sole active S.

## P6 PIF call history, retained paths and real open failures

The [history receipt](../../../artifacts/mvdm-audit/t478-final/s45-pif-history-partial.json)
extends P5's fixture against the same original production nt_pif.obj. Background
runner37536 builds and runs with exit0, preserving P5's eight cases and adding
PIF_ORIGINAL_REPEAT_IGNORE_REPLACE_OPEN_FAILURE_OK. The generated script and
logs are under build/t478-s45/parser-p6; deadlines remain120/30 seconds.

A386 record sets fast paste and priorities70/40. Subsequent WNT records preserve
those globals while setting FIRST.NT/FIRST.AUT, ignoring SECOND selections,
then replacing them when IgnoreConfigAutoexec is cleared. Every selection is
queried twice for CONFIG and once for AUTOEXEC. This demonstrates the retained
DIV-157 repeated-read contract and replacement values. Source review shows
free of both prior paths before replacement allocation; this test does not
instrument allocation/free counts or certify allocation failure.

An actual exclusive native handle denies the parser's open. After releasing
that handle, the fixture deletes the file and repeats against the missing path.
Both calls return FALSE, restore default XMS and clear dwWNTPifFlags, while
retaining fast paste, priorities and SECOND.NT. There is no new global reset or
rollback. The successful file deletion also observes normal parser handle close;
post-open read/seek failure cleanup is not exercised here.

The ASCII config-path conversion boundary is a controlled identity substitute
with capacity checks and a call count. Original ExpandEnvironmentStringsOem and
original PIF state remain linked. This tests the retained-path owner, not the
production ANSI converter or non-ASCII expansion. Root/default discovery still
fails closed if entered. Default-path identity, short-read/seek partial failure
and allocation retry/IGNORE remain pending. No product bug or missing connection
has been demonstrated by this checkpoint; no original malformed-input tolerance
is added. The row remains pending and S45 stays active, with unchanged counts
and unchanged S43 release. No O:/winnt write or push.

## P7 real default-root connection and original read limitation

The [root receipt](../../../artifacts/mvdm-audit/t478-final/s45-pif-root-partial.json)
uses a separate focused fixture linking the same original nt_pif.obj plus actual
session, guest-memory lease, firmware and system-root implementations. There is
no root-provider substitute. An owned temporary directory contains a valid
_default.pif. Binding that directory to the real session makes the original
GetPIFData empty-name path succeed; deleting that exact file makes the same call
return FALSE with original defaults. A short root-output buffer reports the
required size. Normal parser closure permits deletion; unbind/dispose and owned
directory cleanup pass. Background runner33076 reports build0, fixture0 and
PIF_REAL_SESSION_DEFAULT_ROOT_PRESENT_ABSENT_OK, under120/30-second deadlines.

The read-only original audit source-evidence file pins OpenNT commit
5c5b979ec08c17d3ca2eb70e8aad62d26515d01c and nt_pif.c hash
4ae2c1d2c4a66dc63c9b2ddd673fcae067f2fbdab953dc8d96aecfcf03c262bc.
Its exact observations at original lines160-180 match the current source's
seek/read failure branches: close and FALSE on -1; no exact-size comparison.
A positive short read is not converted into a complete record by the parser.
This is original behavior, not a missing project read-exact implementation.
Do not add retry, zero-fill, rollback or claim truncated records are safe.

The remaining qualification is post-open partial-error execution and allocator
retry/IGNORE. The original ch_malloc loop and its dialog reply remain selected;
source presence alone is not reported as runtime evidence. Existing malformed,
nonterminated or cyclic record limits do not become new tolerance requirements.
The stable identity remains pending rather than claiming these cases passed.
No product sources, guest bytes or published binaries change. S45 remains active
with twelve startup rows/1045 active identities pending; no O:/winnt write or push.

## P8 PIF disposition: selected original owner, no new product repair

The [consolidated disposition](../../../artifacts/mvdm-audit/t478-final/s45-pif-disposition.json)
resolves DOS16:6183631816abcfe32197 with explicit original limitations. P5-P7
already established current production-object extension ordering/defaults,
repeated state and retained configuration, actual open failures, and real
session/default-root identity. P8 completes the bounded error-contract witness.

The fault fixture includes the tracked nt_pif.c translation unit unchanged;
macros substitute only _lopen/_lread/_llseek/_lclose and malloc after native
headers are included. No transformed mirror copy is a build input. The existing
OEMUNI fixture generator supplies CCPU40 formal flags. Nine cases inject failure
at each of five reads and four seeks in a valid386-plus-tail chain. Every case
returns FALSE and closes exactly once. Failures after the386 payload retain its
XMS fields rather than rolling back. A complete control succeeds and closes once.
Direct ch_malloc tests assert the actual malloc-failure dialog options: Retry
retries and succeeds on the second allocation; Ignore returns NULL after one.
The dialog reply is controlled, not an interactive UI acceptance claim.

Background runner38832 completes build and fixture with exit0 and
PIF_ORIGINAL_9_IO_FAILURES_CLOSE_PARTIAL_STATE_RETRY_IGNORE_OK. Commands and
120/30-second deadlines are retained in build/t478-s45/run-fault-p8.ps1, with
fault-p8 logs. Production sources/release objects are unchanged. This fault
witness is distinguished from the P5-P7 direct production-object/native-file
witnesses; none alone is presented as whole-system acceptance.

The evidence establishes current original ownership and bounded original
semantics, not every PIF flag/value permutation or malformed-input safety.
Positive short reads are not checked for completeness; unterminated fields and
cyclic extension chains have no newly invented recovery. These original limits
remain limits under the owner's no-new-tolerance instruction. Retain the already
justified DIV-157/DIV-278 path ownership/encoding bindings. No autonomous parser
or missing host implementation was found to remove or restore here.

Fifteen startup identities now have bounded dispositions; eleven startup rows
and1044 active identities remain pending. S45 and T478 remain open. This is a
test/evidence P only: no new product build/Full/publication is required for the
unchanged S43 package, and no O:/winnt write or push occurred.

## P9 original temporary-file helper disposition

The [receipt](../../../artifacts/mvdm-audit/t478-final/s45-temp-file-disposition.json)
resolves DOS16:ca680961be9e8806496a as an existing original owner with bounded
native lifecycle evidence. Current worker map binds cmdCreateTempFile to
original-mvdm-command:cmdredir.obj. The source retains allocation, GetTempPath
zero/oversize gates, scs/0 filename generation, Windows-directory/root fallback,
original security attributes and OPEN_ALWAYS/shared read-write/TEMPORARY open.
[T427 S2](m0-t427-s2-own-image-root-bindings.md) already restored the real host
Windows-directory fallback and removed the irrelevant firmware dependency.
There is no outstanding product-root scratch repair to repeat.

The old T290 standard-handle fixture does not call cmdCreateTempFile. The added
focused fixture therefore compiles the exact function body extracted from current
cmdredir.c, with a checked boundary and source hash. The generated graph depends
on that source, and its implicit include dependency rebuilds the fixture. No
product source is generated or changed. Actual native files in an owned temporary
directory verify TMP/TEMP selection, returned path, inheritance flag, shared
read-write opening, written bytes, close/delete and directory cleanup. Background
runner20140 completed build0/test0 and
CMD_ORIGINAL_TEMP_PATH_INHERIT_SHARED_RW_CLEANUP_OK with120/30-second deadlines.

Failure paths are source-reviewed, not newly fault-injected. In particular the
second GetTempFileName result is not checked before CreateFile; this existing
original limitation is not a safe-failure claim or a reason to add tolerance.
No fallback is forced to write into the host Windows directory or drive root.
There is no new ACP/DBCS, ReFS or short-name-disabled qualification. These limits
are separate from the normal original helper wiring and its tested lifecycle.
The CONFIG/AUTOEXEC parser and its byte accounting remain a distinct pending row.

Sixteen startup identities have bounded dispositions; ten startup rows and1043
active identities remain pending. S45 remains active. Product and the published
S43 package are unchanged; no new Full/publication, O:/winnt write or push.

## P10 CONFIG/AUTOEXEC original-owner and limitation disposition

The [comparison receipt](../../../artifacts/mvdm-audit/t478-final/s45-config-parser-disposition.json)
resolves DOS16:4673504e1fe696895ecf. It reuses the original audit's pinned full
function observations, not its proposed future hardening requirements. After
removing comments/whitespace/line directives, ExpandConfigFiles differs only in
GetWindowsDirectory -> mvdm_softpc_system_copy_root plus its length/zero result.
WriteExpanded and WriteFileAssert are token-identical. This is a bounded function
comparison, not a bytewise whole-file mirror certificate. Current map binds all
three to original-mvdm-command:cmdconf.obj; the unchanged S43 startup exercises
normal boot, not every parser boundary.

The [T420 S35 boot audit](m0-t420-s35-xms-capability-progress.md) independently
found no omitted original host environment pre-sizing step. Original explicit
SHELL /E is preserved; no synthesized /E, discarded environment entries or
forced DOS=HIGH is admitted. The original parser is already selected, and no
parallel autonomous CONFIG parser was found in this caller chain.

Source-origin limitations remain: CHAR cchEnv accumulates derived environment
bytes, and allocation uses raw input file length. For PATH=x with no CRLF, six
raw bytes produce PATH/NUL/x/NUL/final-NUL, requiring eight bytes. This is a
source-derived extent example, not a newly executed heap-corruption test. The
original final index/realloc uses the narrowed counter. Third-party /c and /e
text also uses the original fixed achRawFile and unbounded concatenation.
SetEndOfFile/CloseHandle outcomes remain ignored; exact raw read and output
write failures retain the original fatal route. WriteExpanded consumes input
bytes rather than reporting expanded output length. All of these algorithms
are present in the pinned original, not introduced by the project root binding.

The external audit proposed checked growth, wider bookkeeping and new extent
failure behavior as future acceptance. Those are proposals, not existing product
authority; the owner's later original-semantics-only/no-new-tolerance instruction
controls this T. Do not adopt them as mandatory new functionality, or mislabel
these original limits as missing host wiring. They are explicitly unverified for
adversarial/oversized inputs, and no claim of safety is made.

This source/evidence P changes no runtime. Seventeen startup rows now have
bounded dispositions, nine remain pending;1042 active inventory identities
remain pending. T478/S45 stay active, the S43 release stays unchanged, and no
O:/winnt write or push occurs.

## P11 initial and merged environment ownership

The [environment comparison](../../../artifacts/mvdm-audit/t478-final/s45-environment-disposition.json)
resolves DOS16:c3ee8f2820af6f055614 and DOS16:cd7494c32de5fcd66cdc. Consistent
original cmdenv observations are merged by original line number; conflicting
observations reject comparison. The complete cmdCreateVDMEnvironment,
cmdSetEnvironmentVariable and cmdExpandEnvironmentStrings bodies are token-identical
to the pinned original (657,526 and284 tokens). cmdGetInitEnvironment differs
only by DIV-193's location/lease declarations, ES:0 acquisition replacing
GetVDMAddr, release and the registered invalid-address result. Original snapshot,
filter/conversion order, required-paragraph retry and copy/free logic remain.
Current worker map resolves all four functions to original cmdenv.obj.

The initial helper retains its original final realloc defect: it tests the old
pointer before assigning the new result. WORD byte capacity/counters are also
original. The [T420 S37 trace](m0-t420-s37-oemuni-capability-review.md) records
first-entry BX0010 ->0119 and the second in-bounds copy overwriting the guest's
discarded INIT EnvSiz. Its zero diagnostic process exit without the guest marker
was not a passing workload. That known guest lifetime limitation does not justify
a host-side automatic /E argument, environment truncation, forced DOS=HIGH or
new guest modification. Existing S43 normal startup is bounded current evidence,
not proof of arbitrary large-environment safety.

Merge/set/expand retain source-defined order and duplicates, only-first-match
replacement, first COMSPEC/windir filtering, custom percent expansion, and phase
specific conversion. Longer replacement deletes before growth; failure is not
rolled back. Existing empty deletion returns TRUE, absent empty deletion FALSE.
The original success path does not free a separately allocated long-expansion
temporary; some failure paths free the block without clearing its stored pointer.
Those are source-origin lifetime limits, not a missing alternative environment
manager. The external audit's safer realloc ownership, counter widening, adjusted
copy extents and cleanup proposals are not silently adopted under this T's
original-only/no-new-tolerance instruction. No claim is made that the original
fault cases are safe or that all proposed runtime matrices were executed.

This is source/evidence reconciliation, not a product repair. Nineteen startup
identities have bounded dispositions; seven remain pending and1040 active
inventory identities remain pending. S45/T478 remain active, S43 release is
unchanged, and no O:/winnt write, new publication or push occurs.

## P12 early startup and PIF request-control disposition

The [receipt](../../../artifacts/mvdm-audit/t478-final/s45-startup-control-disposition.json)
reconciles DOS16:2dff4e2e17c9be86c67d and DOS16:901a32f49bf85e8d18cb.
Current host_applInit retains video/keyboard/mouse selection, lowercase-f
admission, case-insensitive w with its original early break, hex i task identity,
DOS/WOW stream choice, main-thread duplication and ICA/UI/event order. Its
registered host-start/close cohort and event cleanup guard are retained, not
replaced by another startup manager. Maps select original nt_reset/nt_event
and the existing mvdm_softpc_event_thread helper.

The existing event_thread_startup_failure fixture was rebuilt against current
join and alert implementations in the stable fixture cache. Background runner2280
returned build0 and both suspended/running cases0, with S35_CHILD_RETURNED_0.
The suspended child's procedure never enters; the started child exits with the
expected native alert status. The fixture owns a two-second child deadline and
its child cleanup; the outer runner bounds build120 and each case30 seconds.
This revalidates the proven [T420 S35 repair](m0-t420-s35-xms-capability-progress.md),
whose historical real CONFIG fault witness exited naturally. No claim is made
that every startup switch/failure or the unavailable-native-alert fallback was
retested. Current implementation prefers NTDLL NtAlertThread; the header's old
APC-only description is not used as implementation evidence.

config's PIF request advertises the original buffers/lengths and selected DOS,
WOW or separate-WOW state. Its reply flows through selected original
BaseSrvGetNextVDMCommand/BaseSrvFillPifInfo, not an invented PIF broker. Original
server size checks accept ReservedLen equal to capacity and return the stored
length. Original client capture uses AnsiStringReserved.Length+1, including NUL;
config declares RESERVED_LENGTH129 then writes Reserved[GetPIF.ReservedLen].
At exact129 this is an original extra-terminator out-of-bounds risk. The pinned
prior audit explicitly records the same producer/consumer combination. No such
overflow was forced in the product; this is source-proven risk, not a safety
pass. Its proposed physical130/logical129 padding is not adopted as new tolerance
under the current owner direction. Ordinary startup evidence does not erase it.

Twenty-one startup rows have bounded dispositions; five startup rows and1038
active identities remain pending. S45/T478 remain active. Changes here are tests,
fixture build wiring and evidence only; the product/S43 release stays unchanged,
with no O:/winnt write, new runtime publication or push.

## P13 launcher dual-environment disposition

The [caller/owner receipt](../../../artifacts/mvdm-audit/t478-final/s45-launcher-environment-disposition.json)
resolves DOS16:1f6a1765004d85a79063. Current run16 builds a separate Unicode
MULTI_SZ for WIN16DIR and strips private frontend/execution/stream carriers;
it does not mutate its process environment. Original BaseCreateVDMEnvironment
still owns path normalization and Unicode-to-ANSI conversion, and the launcher
map resolves create/destroy to opennt-base-client:client.obj. Original bounded
USHORT lengths and short-path fallback remain; no new capacity policy is added.

The ANSI record goes to original BaseCheckVDM. For WOW/separate WOW only,
common_guest_environment_root replaces SYSTEMROOT in that guest record, with
its existing bounded copied-block ownership. The Unicode block goes to
OpenNtBaseClientStartVdmWorker without this guest-root rewrite. The common done
path calls original BaseDestroyVDMEnvironment for both buffers. Thus the package
root adaptation is a finite source/consumer boundary, not a second environment
projector or authority to rewrite native SYSTEMROOT.

[T427 S2](m0-t427-s2-own-image-root-bindings.md) documents that existing split
and its bounded projection/normal product tests; current S43 normal DOS/WOW
startup is reused. The explicit/cloned environment fixture checks Unicode
identity, not every ANSI result or launcher branch. The separate WOW boot
fixture tests the initial guest environment in the worker, not these launcher
outputs. Neither is misreported as fresh or complete launcher fault coverage.
No new test is needed merely to repeat the unchanged source-owner mapping; no
arbitrary oversized/DBCS/failure safety result is claimed.

Twenty-two startup rows have bounded dispositions, four remain pending;1037
active inventory identities remain pending. Command capture/consume, CheckVDM
admission, worker rollback and first COMMAND frame stay separate open rows.
S45/T478 remain active; product and release are unchanged, without O:/winnt
writes or push.

## P14 command delivery disposition

The [command receipt](../../../artifacts/mvdm-audit/t478-final/s45-command-delivery-disposition.json)
resolves DOS16:263d8b0026d041d346f9 as current original ownership with bounded
historical evidence. Current maps bind GetNextVDMCommand to client.obj and
BaseSrvGetNextVDMCommand to srvvdm.obj. Inspection follows the actual common
codec, client RPC and locked NTSRV dispatch, not the retired adapter paths.

Original environment queries return required length without deleting VDMINFO.
PIF queries return before normal consumption; they are not wholly mutation-free:
console adoption can occur even when filling PIF fails. Ordinary capacity
failure reports all required lengths and retains the pending command. Successful
copy marks DOS busy or WOW dispatched and releases command data; separate WOW
releases its console record. Parent-completion notification precedes this size
check in applicable states and is not transactionally undone on failure.
Original client event wake sets SECOND_TIME and clears ExitCode before retry.

Common transport allocates copied payload and reply before dispatch, carries
required length separately from storage capacity, and validates reply identity
before applying it. Events and streams use typed RPC attachments. This avoids a
new allocation in payload finishing; it does not prove atomic rollback across
later attachment or RPC failures. NTSRV management labels observe the original
owner, and environment/PIF queries do not authorize normal worker I/O delivery.

The [T412 record](m0-t412-s2-original-lifecycle.md) and retained fixture assert
short-buffer retention, successful consumption, real wait/wake with NEXT.COM,
two dispatches, retry exit code zero and drained captures. These are historical
in-process results, not fresh certification of current RPC failures. The old
Verify-BrokerOriginalLifecycle runner still names removed adapter-opennt-host
paths and an obsolete configuration equality; it was not executed or counted
as a pass. If fresh focused execution is needed, maintain that runner against
the current owners first. Query nonconsumption here is source evidence, not a
new runtime test. No original fault tolerance, rollback or capability is added.

Twenty-three startup identities now have bounded dispositions; three remain
pending (admission, worker rollback and first COMMAND frame).1036 active
inventory identities remain pending. S45/T478 remain active. This is evidence
only; unchanged S43 product/release and Full evidence are retained, with no
O:/winnt write, publication or push.

## P15 admission and worker ownership disposition

The [admission/worker receipt](../../../artifacts/mvdm-audit/t478-final/s45-admission-worker-disposition.json)
resolves DOS16:86818e62324bec6dccaf and DOS16:7f2437800dace598a2ec as wired
owners with explicitly bounded historical evidence. Source/function comparison
against the pinned OpenNT files proves BaseSrvCopyCommand identical after
newline normalization. BaseCheckVDM differs only by the registered hotkey
DWORD-through-ULONG_PTR cast, its explanatory comment and line directives.
Maps bind the launcher/server to these original bodies.

Run16 supplies its inherited stream triple and original PIF new-Console flag.
Original BaseCheckVDM retains capture, field conversion, tail and cleanup;
common transport carries aliased streams as typed receipts and treats Console
identity separately. Service reply capacity is validated before original Check
publishes its record; original CopyCommand deep-copies the fields. The T412
fixture checks tail CRLF, application/environment/startup copies and hotkey
handling. Its allocation-failure check is at CLIENT capture allocation, not
server deep copy. No fresh runtime run is claimed by this source checkpoint.

The original server allocation failure frees VDMInfo before accessing its
child pointers. This ordering is byte-identical in the compared body and was
already identified in the pinned DOS16 audit. It remains an original
memory-exhaustion limitation under the owner's no-added-tolerance direction,
not a safe cleanup result or a newly missing provider. The client test cannot
be cited as coverage of this unsafe server path. Original command budgets,
USHORT widths and encoding behavior remain without an expanded fault matrix.

Current worker creation belongs to NTSRV, superseding the old launcher's
CreateProcess recipe. The service selects its trusted package worker and
original configuration, reserves the authenticated Check result, creates
suspended, prepares the worker, performs original Update, duplicates export
handles and admits the frontend before Resume. The startup Job is temporary;
pre-resume failure cleans only that owned child. Service failure cleanup
cancels its wait, closes its process reference, invokes original partial/full
undo and releases the reservation. Successful handoff is not launcher-owned
process termination. The original independent-console no-parent-event branch
uses a service process-exit receipt, retaining its completion meaning.

[T424 S4](m0-t424-s4-broker-centered-launch-control.md) preserves the failed
borrowed-event export and its correction, followed by real DOS/native/DOS
relaunch and final delivery. Current broker_frontend_bootstrap_test still
asserts rejection without prior Check and malformed environment without exported
handles. These are bounded historical observations, not fresh or exhaustive
post-Check rollback fault injection. S43's unchanged normal startup and Full
remain regression context; neither implies all allocation/Resume/transport
failures were exercised. No restoration of obsolete launcher ownership or
private CSR shell is needed.

Twenty-five startup identities have bounded dispositions; the first COMMAND
frame remains pending.1034 active identities remain pending overall. This P
changes evidence only, retains S43 assets, and leaves S45/T478 active. No
O:/winnt write, new publication or push.
