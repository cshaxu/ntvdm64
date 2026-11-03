# S13 interrupted test-failure recheck

## Task-order invalid-handle check

The task-order fixture exited with `0xC0000008` at its explicit
`GetHandleInformation` check on a closed handle. A standalone process using
the same Windows API also raised that status, and old/new NTREGAPI guard
preprocessed fixture outputs were byte-identical. The fixture now accepts
either the documented failed call with `ERROR_INVALID_HANDLE` or the exact
`STATUS_INVALID_HANDLE` exception, and checks a live handle as a negative
control. The full task-order fixture then passed all seven destruction cases
and the other recorded controls. This is not a product failure attribution.

## Historical T424 source-separation check

The manually invoked `verify-ntsrv-service-separation.mjs` fails at
`service_abandon_launch`: it requires every current function body to equal
commit `d83d2b212`, the pre-separation T424 S7 snapshot. The worktree's
`base_service.c` is identical to HEAD, and its history includes later T447,
T448 and console-retirement changes. Thus this is a frozen migration-time
assertion, not a valid current behavior contract. No current build, test or
governance caller was found. The obsolete checker was removed; its T424
acceptance and historical method remain documented in
`docs/etc/evidence/m0-t424-s8-ntsrv-service-separation.md`. This disposition
does not establish current NTSRV behavior or substitute for lifecycle tests.

The conditional-branch queue is paused while test failures are investigated.
These are not claims that the whole test inventory is clean.

* OEM PIF caller: both old and current headers failed from a working directory
  whose resulting OEM short startup path exceeded the original 64-byte limit.
  A short isolated directory now passes the five original scenarios; a separate
  long-directory case checks the original rejection and allocation cleanup.
  Run `tests/observation/verify-oemuni-pif-caller.ps1` with the fixture path.
* GFI object contract: the fixture omitted the production `PROD` definition.
  It now uses that definition, and the verifier retains child process handles
  before waiting so a fast process cannot lose its exit code. The current
  object passed 433 checks and the negative ABI compilation.
* WOW page-domain: the old Ninja executable edge had no repository callers
  and selected a duplicate, incomplete subset of the production provider.
  It and its exclusive duplicate object edges were removed. The worker fixture
  object and all default test assertions remain. The repeatable `page-domain`
  case in `verify-wow-task-lifecycle.ps1` builds that exact object, verifies
  its copied hash, links the current production-owner closure and checks the
  lifecycle marker. It passed with both the x86 worker and x64 configuration's
  x86 reference worker after regenerating both graphs. This is a replacement
  of the obsolete executable target, not a claim that its old link passed.
  The independent full menu-resource case also passed.
* NIDDB: the prior selected object fails the controlled inactive-VM
  retirement case, leaving a retired instance pointer in the master table.
  The actual guest provider's ordering and safe replacement for an active
  retired instance remain unproved; see `niddb-vmm-retirement-contract.md`.
  Rebuilding the held DIV-467/468 patch against current source in isolation
  passed the existing 15-check and three-VM-reuse fixture. A separate
  deliberately injected stale virtualising-byte control then exited with
  `0xc0000005` before the device call; the held candidate is therefore not a
  complete failure-handling solution. This control does not prove that the
  guest provider produces that ordering. A second build-only experiment uses
  the live BIOS virtualising byte when the former active record is retired,
  and a test termination seam when a byte names a missing or out-of-range
  record. Its ordinary, stale, out-of-range and active-stale controls behave
  as intended. This is not yet a product solution: VGA mode changes also call
  mouse state outside the BOP wrapper, so restoring a snapshot while the byte
  is temporarily stale may still affect another context. A further call-chain
  audit identified `mouse_pointer_route_changed` as another direct consumer:
  it is called both from the CPU mouse route and from `nt_event.c` during
  console suspension, and dereferences the same NIDDB mouse state. The latter
  call is inside `nt_block_event_thread`, whose selected callers are command
  paths; file location alone does not prove it runs concurrently with the CPU.
  A BOP-only check therefore does not cover every consumer, and this source
  audit alone does not establish or rule out cross-thread access. The current
  unshipped candidate also checks these two direct mouse entrances. Both x86
  and reference selected-object tests passed ordinary retirement, three-VM
  reuse, and three invalid-identity controls with a test termination seam.
  An isolated executable then linked the actual cached production
  `mvdm_softpc_termination.obj` and invoked its invalid-identity helper; it
  terminated with `0xc0000420`. Its other six unresolved owners were supplied
  by unused fixture seams, not by a replacement for that helper. A separate
  stale-identity executable explicitly linked the current `virtual.obj` and
  `mouse_io.obj`; the map attributes both direct mouse entry points to that
  actual object, and video/route entry each reached the test termination seam
  (`0xc0000420`) before touching instance state. A second link used the actual
  production fail-fast helper; the map confirmed its
  `mvdm_softpc_termination.obj` owner, and both entrances again exited with
  `0xc0000420`. These links used the existing `/force:multiple` fixture
  closure and controlled CPU/register seams. They do not validate NTSRV
  observation, valid mouse rendering, provider ordering, guest, or full-product
  behavior for this candidate.
  The mode callback writes `current_video_mode` and reads `virtual_screen`,
  both macros dereferencing `mm_handle` in `base/inc/mouse_io.h`. The route
  callback reads `cursor_flag` and calls the same instance-backed cursor
  operations. The `nt_event.c` call follows the event-thread stall handshake;
  it must not be described as proven simultaneous access without a witness.
  A source-wide call-site search found the selected product registration in
  `mouse_driver_initialisation` with both NIDDB callbacks null; this narrows
  callback-reentrancy evidence to the selected product source, not all
  external or hypothetical users of the original interface.
  A subsequent completed Win3.1 Enhanced `DOSONLY=1` run used a disposable
  installation copy and the current CCPU candidate package. A prebuilt BOP B1
  sentinel in the same startup path produced exactly one `eax=0000ffff`
  trace record, proving trace transport; no other B1 controls were observed.
  Desktop/mode and normal exit passed, and six original guest-system hashes
  were verified unchanged. See `build/t473-s13/niddb-w31-observation/`.
  This shows the tested installation did not need the optional multi-instance
  protocol in that run; it neither proves absence in other configurations nor
  justifies treating the retained original B1 entry as unreachable.
  The same controlled trace and successful startup/normal exit were repeated
  in all four Standard/Enhanced and DOSONLY 0/1 combinations. Four sentinel
  records and no other B1 records were observed in the aggregate trace; the
  trace does not include process identifiers, so it cannot establish which
  specific worker would have emitted a hypothetical record.
* Historical x87 span and paging contract mismatches remain recorded in
  `s13-bfs-depth1-batch188-x87-historical-control.json` and the progress log;
  matching an old baseline does not make the failing cases pass. Re-running
  every case on the currently selected x86 worker fixtures produced 4/8
  passing span cases and 28/64 passing page cases. Full case output and
  executable hashes are in `build/t473-s13/x87-current-matrix.json`.
  DIV-470/471 were previously held because they alter internal code of an
  already connected original component. For the owner's request to handle
  the observed test failures, only those two changes were rechecked in an
  isolated copy against the current header. The isolated archive was verified
  to contain only the replacement decoder member; 8/8 span and 64/64 page
  cases passed. The actual x86 worker and the x64 configuration's x86
  reference worker were then rebuilt from the candidate; each passed all eight
  segment-span and 64 cross-page cases. They are now an unshipped S13
  candidate and remain subject to product-wide tests and release gates.
* CCPU bit fault: case 0 fails with `#PF`, CR2 `9ffe0ffc`, rather than
  reading the signed-index location. A source-derived comparison compiled
  `c_main.c` with the pre-candidate HEAD header, replaced the original archive
  member by its full path, and produced the identical failure. This is not
  attributed to the x87 changes. The previously held DIV-469 candidate was
  then introduced for the owner's request to address all observed test
  failures. Both actual worker graphs passed the selected bit-index matrix,
  hardware oracle, and page-fault tests, alongside the x87 tests. The adjacent
  far-transfer fault-order fixture also passed. These targeted results are not
  guest execution or full release acceptance.

No guest image was changed. The coherent isolated package containing the
three CCPU candidates passed all 17 Full product gates. The test runtime's
ten component hashes matched the staged package manifest; NTVDM SHA-256 was
`F9B10320BB7B1CBED5D947D0FA0FD2A0EAEFE07A430C0868D5F51E733C83D0D3`.
Full timings are in `build/t473-s13/x87-candidate/full-run/full/timings.json`.
The Winmine, Solitaire and Write observations establish startup, not gameplay.
The package has not been deployed; the separate NIDDB issue remains open.
The full-tested package also passed the four Win3.1 Standard/Enhanced and
DOSONLY 0/1 combinations. Compared with the current release at the same
package identity, five host images are byte-identical, four differ only in
previously verified PE timestamp locations, and NTVDM differs in file size and
content. The latter is expected from the CCPU source changes but has not been
reduced to an exact byte/section attribution; see
`build/t473-s13/x87-candidate/release-comparison.json`.

The later isolated package combining the CCPU and unshipped NIDDB candidates
also completed all 17 Full product gates; its timings are recorded in
`build/t473-s13/niddb-candidate/full-run/full/timings.json`. All ten host
component SHA-256 values in that run's `control-cache` match its staged
`product-system32-manifest.json`; its NTVDM hash is
`9801447DD663FFBB6331177BE4CFA9384071B42E16EDDCCD3975C8F328E4F1D5`.
This evidence covers the listed product gates, not real multi-DOS-VM VxD
protocol sequencing or the still-unresolved allocation rollback issue. It
does not establish that the combined candidate is suitable for release.

Subsequently, DIV-468 was applied to release the unpublished instance table
on allocation failure. The selected x86 production object was rebuilt and
`verify-niddb-no-retry.py` passed ten controlled failure cases: table allocation
and four data-block allocation ordinals, each followed by system exit or
reboot. The fixture supplies a fail-fast seam for the separate DIV-467 change;
it does not exercise that seam. Receipt and hashes are in
`build/t473-s13/niddb-rollback-current/result.json`. The Full result above
predates DIV-468 and must not be attributed to the newer object.
The same current fixture also confirmed the expected leak against the earlier
HEAD-source-derived baseline object (one live allocation at the first failed
data-block allocation); see `build/t473-s13/niddb-rollback-prechange/result.json`.
That earlier object is a source-derived control, not an identical build of the
current pre-change candidate. An initial attempt accidentally used a previously
fixed diagnostic object; it passed the contract and was rejected as a baseline.
The x64 configuration's reference x86 worker object was also rebuilt and
passed the same ten rollback cases; its distinct object hash and receipt are in
`build/t473-s13/niddb-rollback-reference/result.json`. This is not an x64
implementation of the 32-bit worker or an installed-guest result.
The x86 worker was subsequently relinked from the DIV-468 object; its VdmTib
ownership check passed and the unsealed NTVDM SHA-256 is
`F5A3B4F0B11574E853C4933B613907FB48DB0BD70F39CC30E5B4F05DA681BF07`.
This relinked image has not been packaged, fully tested, or deployed.
It was subsequently sealed in an isolated package and passed all 17 Full
product gates with the project's short `O:\tmp` runtime stage. All ten images
in that run's control cache match the package manifest; the sealed NTVDM hash
is `B9385ABA3625E563418411A3FB2BECD1203133DE5C0FA6E1D5F20610C8C80E8A`.
Results are in `build/t473-s13/niddb-rollback-final/full-run-correct/full/`.
An earlier run using a long TEMP path failed WOW initialization and is retained
separately in `full-run/`; it is not counted as a product pass. Full gates do
not prove real NIDDB multi-VM notification ordering or gameplay acceptance.

Depth-1 `dpmiint.c` `i386` context investigation: the T289 S5 graph generator
names this source and defines `i386`, but its required
`src/mvdm-host-overlay/dpmi32/dpmi_interrupt_registration.c`,
`src/mvdm-support/inc`, and `src/mvdm-platform-abi/source/public/sdk/inc`
paths do not exist in the current tree. A literal search of current docs,
tests and tools found no caller naming that generator. This establishes that
the historical graph is not directly runnable as written, not that every
supported `i386` context has been excluded. The branch remains pending until
the remaining actual contexts and the original structure are reconciled.
The current `verify-dpmi-swint-selected.ps1` obtains its compiler command
from the x86 worker graph. The PM allocation trace generator similarly copies
the DPMI owner's graph flags before adding its trace define. These two actual
test paths therefore do not independently establish the historical `/Di386`
profile as supported. This is a source-chain observation, not a fresh test run
or a proof that no other supported test uses the flag.
The current x86 worker's `dpmiint.obj` command explicitly selects `_X86_`,
`CPU_40_STYLE` and `CCPU`, but does not pass `/Di386`. A scoped search found
no `#define` or `#undef i386` in the DPMI, MVDM public include, project SoftPC
include or compatibility include trees. This does not cover all transitive
SDK headers or alternative test invocations, so it is supporting evidence
only; `_X86_` must not be treated as equivalent to `i386`.

Depth-1 `nt_aorc.c` context: `Generate-T335S3WowCallbackCcpuNinja.mjs`
names this file and inherits flags from the T313 CCPU lifecycle graph. The
T313 generator specifies `/DCCPU` and `/DCPU_40_STYLE`. This is a source-level
candidate test context, not a fresh successful invocation and not proof that
it remains supported; preserve the two outer arms until its status is resolved.
The T313 base and T335 add-on graphs were subsequently regenerated with current
paths, and the T335 `nt-aorc.obj` edge compiled successfully with `/DCCPU` and
`/DCPU_40_STYLE`. Object SHA-256:
`B538EF264C3418962EB4E3F52483EE396AD84345F46C2852B98CC52B6450FCA1`.
This validates that object compilation in this generated test context, not the
full T335 link or execution and not whether it is a required current gate.
The generated T313 base graph was then built as a whole; it failed compiling
`ccpusas4.c` at `gmi.h` DIV-618 because its flags omit `C_VID`. The current
product graph explicitly defines `C_VID`. The failure is recorded in
`build/t473-s13/t313-context/retry.log`; the T335 executable therefore was not
linked or run. This is evidence of a stale test profile, not an assertion that
the product graph failed or that no supported test can use `nt_aorc.c`.
Depth-1 `bootstra.c` SECURE alternative: the existing queue records no product
translation-unit context. A direct search in build tools and tests found the
mirror rejected-condition verifier naming the file and its four SECURE
controls, but no direct build edge or `bootstrap1` fixture. This is a bounded
search, not proof that every indirect or historical test context is absent;
the selected alternative remains pending rather than being deleted.
With `C_VID` added to the isolated T313 graph, compilation reached the link
but `ccpu-lifecycle.exe` failed with eight missing current provider symbols,
including CCPU event wait, fast BOP, worker lifecycle, and descriptor globals.
Several providers exist in current product sources; the historical graph does
not link their current closure. Full diagnostics are in
`build/t473-s13/t313-context/cvid.log`. The T335 executable remains unlinked;
no product build failure is inferred. Its retention or replacement still needs
a test-ownership decision based on the current selected-object coverage.
The T313 bounded-execution fixture itself is still used by the current T310
worker graph as `ccpu-high-linear-page-test.exe` and as the bit-matrix fixture;
its seam object is also used by current tests. Consequently the fixture cannot
be deleted as obsolete merely because the old T313 graph no longer links.
Historical T406 documentation already recorded its older nine-symbol link
failure and explicitly did not claim the complete historical lifecycle workload
as verified. Current targets cover selected parts; full T313/T335 semantic
coverage equivalence has not been proven.
The actual x86 worker graph's `ccpu-high-linear-page-test.exe` was rebuilt and
ran to exit 0, reporting `CCPU_HIGH_LINEAR_PAGE_BOUNDARY_OK` along with scalar
port layout and CPU start/reentry markers. The link used its existing
`/force:multiple` test fixture seams; this proves that selected fixture path,
not replacement of every historical T313/T335 scenario. The historical graph
remains an unresolved test maintenance item.
The x64 preset's reference x86 worker target was also rebuilt and the same
high-linear-page fixture exited 0 with the expected boundary, port layout and
start/reentry markers. Its existing `/force:multiple` seams and the lack of
proof of full historical T313/T335 equivalence remain the same limitations.
The T279 generator defines `/Di386` specifically for `suballoc.c`, whose
`suballcp.h` includes the pending `suballoc.h`. All four named source inputs
exist, and two governance scripts still refer to the generator. This is a
specific candidate `suballoc.h` test context, not evidence that it includes
`mvdm.h`; the graph and governance checks have not been executed in this pass.
The historical T279 x86 graph was regenerated and attempted. Its compile stops
before reaching the tested suballoc logic because its include paths omit the
current `src/non-mvdm/public/sdk/inc/ntpsapi.h`; all four compile edges fail
at that missing header. This is a stale test graph, not a passing supported
`/Di386` context and not a product-build failure. T279 still has governance
references, so its applicability and replacement remain unresolved.
Disposition: removed the obsolete T279 generator and its two historical
closure/build verifiers. The old verifier required deleted `mvdm-support-overlay`
files and x64 artifacts, while its parent rejected the generator's current
`mvdm-host` path outright; these were not coherent current gates. A repository
search found no remaining code references to the removed entries. The current
T420 suballoc lifecycle graph built and ran successfully, reporting five
lifecycle/data/failure controls. It uses the current supported configuration,
not the historical `/Di386` branch, so it does not prove that branch reachable.
Historical documentation is retained as history.
After the T279 removal, a whole-source/build/test literal search still finds
`/Di386` in historical T288, T289 and T309 graph generators and in the
current T310 export-definition preprocessing command. T309's two source
inputs and its forced error ABI header exist. Their actual include closure and
whether the historical graph remains an applicable supported test are not yet
established. The T310 use targets an export definition, so it must not be
assumed to select this suballoc header without an include witness.
A literal scan of all existing `build/.ninja/**/build.ninja` files found no
`/Di386` or `-Di386` flag. This scans graph text, not compiler built-ins,
response-file contents or separately generated historical graphs, and thus is
not on its own a proof of global absence.
T335's original callback fixture specifically asserts a real outer CCPU BOP,
original `CallBack16`, nested `host_simulate` and D6 FE return ordering. Current
`verify-wow-callback-abort-ccpu.ps1` exercises callback failure/unwind against
the selected CPU closure; its scope does not establish those T335 normal-path
ordering assertions. Therefore deletion of the T335 fixture or its graph as
fully superseded is not justified by the abort test alone.
An isolated T335 reconnection experiment regenerated its graph with `C_VID`
and compiled its original fixture, original `wcall16.c`, test seams and lease.
Linking those against the current worker fixture closure failed with two
unresolved callback-control owners: `wow_user_worker_active` and
`wow_user_native_call_failed`. Their source owners are respectively
`ntvdm-exe/wow/wow_user_session_binding.c` and
`wow32-dll/source/wow_user_task_lifecycle.c`; no empty replacement was added.
See `build/t473-s13/t335-context/current-link.log`. The test has not run.
A second isolated link added the actual worker session-binding and WOW task
lifecycle objects. It exposed further runtime, task, and USER scheduling
owners instead of producing a valid test binary; see `current2-link.log` under
`build/t473-s13/t335-context`. The attempted closure has not been promoted to
a test or used as runtime evidence.
The two owners were additionally compiled in isolated function-section mode
and linked with the real worker runtime and `/OPT:REF`; unresolved WOW USER
and task interfaces remained (`current3-link.log`). Function-level linking alone
does not close the old T335 harness. No unresolved-symbol ignore or success
stub was used, and the source tree was not changed by this experiment.
Depth-1 `host_def.h` `i386` controls `SafeLoadLibrary`: selected non-i386
form expands to `LoadLibrary`; the i386 form declares a separate function.
Current `nt_bop.c` and `nt_msscs.c` use the name. A legacy trace fixture also
defines it; this search alone does not establish its compilation macros. The
branch cannot be converted based only on the name or the x86 host ISA.
The `SafeLoadLibrary` trace support fixture is named only by the historical
`tools/historical/opennt-pre-t260/CMakeLists.txt` graph found in this search.
That graph references `local/trace/fixtures/config_prefix_stop_support.c`,
which is absent under its own source directory, while the retained fixture is
now under `tests/legacy/opennt`. This narrows that particular reference to a
stale historical graph; it does not close all possible test include contexts.
After the two candidate mouse entry edits shifted source lines, four REAL_VGA
entries in `s13-rejected-legacy-branches.json` were moved by four physical
lines; original-body and provenance hashes were not changed. In the actual
MSVC x86 environment `test_mirror_rejected_conditions.py` then passed 235
retained guards plus its listed compound-profile controls. Its initial manual
invocations without `--scratch` and without the MSVC environment were runner
usage/environment failures, not product or profile results.
The last recorded depth-1 queue (`195r104`, baseline commit c97d4ee2...) has
4,083 current-source groups and 5,070 arms: 766 unselected-file, 670 newly
excluded, 1,699 already excluded, 1,874 retained-observed, nine retained
original assertions and 52 pending. These are that queue's recorded statuses,
not a fresh complete reconciliation after the current uncommitted edits and
historical test-entry removal. No next-depth body review is implied.
A manual `EstimateMirrorDiffRetention.py --help` invocation failed at its
frozen-status assertion for `bde/bde.def`: the script does not implement a
help flag and compares the current tree against the historical initial ledger.
It stopped before writing its historical CSV/summary. A source search found no
current script or documentation caller; applicability and safe replacement of
this audit utility are not established. It is not a product test pass/failure.
An independent comparison of the 195r104 pending snapshot with the earlier
captured product include contexts reports 39 pending arms in files with zero
recorded product contexts and 13 with at least one. The comparison is saved
in `build/t473-s13/pending-195r104-context-comparison.json`. These are file
inclusion counts from captured data, not branch reachability or a fresh scan;
none of the 52 statuses was automatically changed.
A case-insensitive filename search of current build/test tools for files with
no recorded product context found `bootstra.c`, `cdrom.h`, `gore.h`,
`host_msw.h`, `isnkrnl.h`, `novell.h`, `smeg_hea.h`, and `spc_icon.h`
named only by the rejection-condition checker in that search. Several other
names have build or fixture references. This is basename text matching (which
can have collisions), not include-resolution proof; no branch disposition was
changed from this observation.
