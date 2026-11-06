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
