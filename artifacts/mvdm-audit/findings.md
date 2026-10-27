# Cross-shard findings (in progress)

## Divergence identifiers are not unique

A mechanical registry-table sweep found seven repeated IDs with distinct
rows; `registry-id-collisions.json` records line numbers and summaries.
Identical duplicate rows for DIV-227, DIV-228 and DIV-229 were removed.
Inline-only collisions still require a separate source sweep.

`MVDM-HOST-DIV-222` identifies both `dpmi32/modesw.c` 53:01
protected-mode entry and `softpc.new/base/keymouse/mouse_io.c` INT33
direct-callback handling; both have separate registry rows. The distinct
`MVDM-HOST-DIV-230` real-mode cache and HMA observer behaviors and
`MVDM-HOST-DIV-274` OEM sizing and mouse cursor publication behaviors have
also since been recorded separately in the registry, retaining the duplicate
identifiers. `MVDM-HOST-DIV-327` also occurs on CCPU `lmsw.c` and COMMAND
`cmdexec.c` observation paths, and in a separate protected-mode entry in the
register; determine whether the numbers describe one cohort or collisions.
The LMSW witness has since been explicitly added to the COMMAND witness row;
its collision with the separate stack fix remains. Identity
by ID alone is therefore ambiguous; resolve by source path and call site,
then assign and cross-reference distinct IDs when the exact historical
ownership and all consumers have been reconciled. This is a documentation
traceability defect, not yet evidence of incorrect runtime behavior.

## Deferred publication labels

Source divergence notes for VIDEO7-PRESERVE-MODE and DIV-455 had retained
candidate-only publication wording despite later package delivery. They
were updated using the T460 and T473 S9 records, preserving the distinction
between historical tests, published artifacts and paths not covered.

## Current scope

The frozen 4,684-path partition is exhaustive only for the MVDM source
union. The separately frozen 42,963 external audit rows still require
relevance and owner tracing; no completeness claim follows from partition
coverage. Partial means a raw mirror difference, not functional absence;
pending is not a final disposition.

## Interrupt-hook markers lacking direct registry match

A subsequent source-wide marker scan found ten distinct `MVDM-HOST-DIV`
identifiers absent from `src/mvdm/README.md`, including DIV-303 in the
CPU segment-load path and DIV-307/308/309 in interrupt-hook paths.
`unmatched-divergence-markers.json` records all twelve source references.
An exact identifier search in indexed documentation also found no matches. Their
comments describe default-off observations, but the absence of registry
entries is a traceability gap, not proof of behavioral change. Trace their
original admission and remove or register them only after checking the
actual call sites, activation conditions and diagnostic use.

## BOP 59 diagnostic reads exceed original behavior

`shard-1/bop9-diagnostic-guest-read-audit.md` and its source-linked fixture
compare the frozen original and current `MS_bop_9`: for a returning original
handler, the former performs zero guest reads and the latter performs eight
even with the diagnostic reporter modeled disabled. The handler still receives
the same AX in this fixture. This proves an additional memory-access boundary,
not a real device side effect or crash. The implementation group must remove
or safely gate the extra observations and test original handler/AX semantics;
DIV-258 now describes the actual current behavior rather than claiming that
only existing scalars are copied.

The same family appears in INT 15 and keyboard BOP entry observations.
`shard-1/int15-diagnostic-guest-read-audit.md` compares actual source bodies
in the stated PROD/PM-off fixture: original zero versus current two reads.
`shard-3/keyboard-bop-diagnostics-audit.md` links selected source and shows
original zero versus current four reads; a mock first-read failure prevents
the current path from reaching its original dispatch. The fault is injected,
not a demonstrated real guest exception. These three paths should be reviewed
as one diagnostic-access cohort, with distinct original handlers and tests;
disabled output is not evidence that caller-side memory sampling is disabled.
The bounded [diagnostic-access inventory](shard-1/diagnostic-guest-access-inventory.md)
also identifies BOP 06 and INT 31 mapping candidates; its pattern search is
not an exhaustive proof across every helper or macro.

## Disk-close counter failure leaves partial retirement

The selected floppy and fdisk close implementations close and invalidate their
native handles before a fallible guest SCS counter update. An actual-object
fixture with a failed-update seam confirms that both report failure while the
native open counts remain unchanged; floppy also retains its prior owner and
lock. See `shard-3/disk-close-failure-audit.md` for layout checks and the
precise mock boundary. Whether a real session can reach that failure is not
yet established. Review both providers as a single lifecycle group, preserving
the distinction between native cleanup and guest-counter failure.

## Clipboard formats require per-format failure and ownership tests

`shard-2/wow-clipboard-format-failure-review.md` separates the T460
CF_TEXT/CF_DIB witnesses from unverified CF_HDROP, metafile-picture and OLE
descriptor rejection paths. The original `CopyDropFilesFrom16` also has a
source-level uninitialized-result path when `GlobalLock16` fails. No failure
injection or runtime leak has yet been demonstrated; preserve format-specific
ownership and nested handles when designing the tests and any recovery.

The later source-location audit registers DIV-305 and DIV-307/308/309.
DIV-306 and DIV-308 include descriptor reads and far-transfer translation
before the reporting switch. A controlled real CCPU paging/exception fixture
reproduced a semantic difference with logging disabled: the original OpenNT
far-transfer source returns #GP(8), while the current selected source returns
#PF with CR2=0x0fff for the same cross-page descriptor. The first extra read
belongs to DIV-306; DIV-308 has not yet executed at that fault. See
`shard-3/cpu-far-diagnostic-audit.md` for exact state and bounds. Recover
original exception ordering in the S11 execution-foundation group and test
both disabled/enabled diagnostics, normal transfers and guest entry. The JSON
is a historical scan, not an up-to-date list of missing entries.

## Fast WOW provider requires corrected attribution

`wow32/i386/fastwow.asm` is identified by its source and `fastwow.h` as
a fast protected-mode API/callback mechanism. The S1 ledger grouped it
with generated-table files described as excluded by `!IF 0`; that reason
needs file-specific verification. Its monitor-TEB dependency, the selected
slow path, observable semantics and potential recoverability remain to be
traced. Do not infer a working replacement or a justified exclusion from
the existing grouping.

## SoftPC overwrite owner needs fresh trace

The S1 unselected-source disposition describes an ntstubs.c wrapper, but the
r10 map and selected source identify `accessfn.c` as the wrapper, forwarding
through the C-VID SAS vector to the same original CCPU no-cache-invalidation
leaf. See `shard-0/sas-overwrite-provider-audit.md`. DIV-092's stale direct-call
attribution was corrected; DIV-189 records the vector binding. Early calls,
individual runtime callers and guest behavior remain unverified. This is an
audit mismatch, not an asserted runtime defect.

## DDEML original provider boundary

See `shard-2/ddeml-owner-boundary.md`. Original WOW host thunk uses a
modern-host-unavailable private `DdeGetCallbackInstance` in its callback
chain, while guest `wow16/ddeml/ddeml.c` has a separate implementation.
Classic WM_DDE acceptance does not prove DDEML, and unselected host thunk
does not prove the entire guest capability absent. Assess recoverability
against instance/callback identity, nested calls and teardown rather than
marking the provider excluded based on private API absence alone.

## Generic thunk x86 helper selection

The original `wow32/i386/sources` selects `callpr32.asm`; the current x86
product instead compiles the portable C branch. A controlled call through the
actual selected `wkgthunk.obj` fails with a mismatched cdecl callee, while the
original assembly helper returns and restores the stack. A separate Win16
probe exercising a nested callback failed on the baseline and passed with a
locally composed candidate using the original helper and an EBX-preserving
adapter. See `shard-0/wow-generic-thunk.md` for source identities and the
limits of each witness. The helper itself clobbers nonvolatile EBX, so direct
linking without an ABI boundary is insufficient. Recover this as part of the
WOW execution group; wider calling conventions, exceptions, task cleanup and
coherent package regressions remain unverified. The old generic `!IF 0`
rationale does not justify excluding this helper.

## WOWFAX components

See `shard-1/wowfax-trace.md`: a selected WOW32 Registry client exists,
but the separate original fax GDI driver and UI modules are not established
as deployed. Existing print acceptance explicitly did not cover fax install.
Source suggests an initialization risk in the install path; it has not been
reproduced and requires further original-source and path evidence.

## CPU profile and compiled-source distinction

Shard 1 found `base/ccpu386/invd.c` in the selected compilation inventory,
but its implementation is conditional on `SPC486` and the current SPC386
profile dispatches the corresponding opcodes to the original invalid-opcode
path. Compilation of the file is not proof of a 486 instruction provider.
The target CPU capability and guest compatibility contract must be checked
before excluding that implementation or enabling an alternate CPU profile.

## Multimedia structure translation selection

Shard 1 reports that `wow32/wmmstru2.c` is present in the selected build
graph while its reverse MCI structure thunk bodies are under original
`#if 0` and do not appear as object symbols in the current map. This
requires tracing actual MCI/MMCallProc32 dispatch and the guest/host data
contract; a compiled object or ordinary sound acceptance alone does not
prove those reverse translations or asynchronous physical-device behavior.

`wow32/wmmedia1.c` likewise appears in the current link graph but its
original aux/joystick thunk block is compiled out by `#if 0`. The original
guest-side aux/joystick code supplies distinct paths, so determine the
actual immutable MMSYSTEM and driver invocation before calling this a
missing capability or a justified provider exclusion.

## Caret evidence conflict

T457 S4's summary labels a real dialog probe as input/caret evidence, but
the underlying T457 S2 detailed record limits the real guest probe to
keyboard-driven dialog behavior and marks the separate keyboard/caret
desktop-state fixture as a non-pass. Do not count the summary alone as
proof that `wucaret.c` guest caret semantics were exercised. Resolve using
the actual run inputs and focused guest/caret evidence.

## CCPU SAS vector trap fallbacks

Shard 3 traced original `cSasPtrs` initialization through `sas_init` to
`Cpu.Sas`: the selected product vector contains `c_sas_touch` and
`c_VirtualiseInstruction` fallbacks that execute a breakpoint. The product
object relocations and disassembly establish their presence, not that a
guest can reach them. A direct source search alone cannot exclude indexed
indirect or generated-code use. See `shard-3/sas4gen-vector-audit.md` for
the source chain, missing original implementation evidence and needed
consumer/contract review; avoid replacing these with success stubs.

## Product build selection inventory correction

`dbg/dbg.c` is reported as not-product-selected by the frozen inventory and
the S1 disposition calls it fixture-only. Current `ntvdm.exe` transitive
Ninja inputs include it; the product map attributes `DBGDispatch`, `DBGInit`
and related symbols to `debugger-bindings:dbg.obj`. Thus the earlier selection
claim is stale. `graph-selection-discrepancies.csv` is a systematic
comparison of the 4,684 frozen paths with the actual `ntvdm.exe`,
`WOW32.DLL` and `VDMREDIR.dll` transitive Ninja inputs; this was the only
not-selected-to-input discrepancy. The VDMREDIR closure includes the
NTVDM import-library build, so its transitive source inputs cannot be read
as DLL object ownership. Compile selection still does not prove map
extraction or behavior for the remaining inputs.

## Host reset state registration needs ownership review

`softpc.new/host/src/nt_reset.c` retains DIV-147 application-open/closed
state and a host cleanup guard, whereas a DIV-147 registry row says its
former reset entries were restored to original code; the same ID is also
used for CPU declaration and SIM32/session work. Trace the current T448
lifecycle ownership and actual call order before deciding whether this is
necessary state, an obsolete duplicate, or a registry-description defect.
The conflicting description is not itself proof of a runtime bug.

## SAS vector projection layout

Shard 3 records an original name table with 55 fields and a current generated
58-field vector with different tail offsets. The two trap fallbacks are
installed in current slots; the original name table itself has not been found
as a runtime index consumer. Numeric/indirect/compiled consumers must be
traced before asserting misdispatch or treating the fields as unreachable.
See `shard-3/sas-vector-fields.csv` and `shard-3/sas4gen-vector-audit.md`.

## Cross-worker console codepage and ForceDOS propagation

The current native-CUI and NTCON console carriers have independently
observable codepages after a native child changes its output codepage. The
separate DOS-output witness and its exact observation limits are recorded in
`shard-1/standalone-tool-chain-audit.md`; identify the original console owner
and prove a repair with both guest and native output rather than adding a
second independent codepage owner.

`CREATE_FORCEDOS` is a separate original startup contract. The selected
one-hop native-context consumer rejects its bit even though the service value
codec preserves the bit in focused fixtures; see
`shard-3/forcedos-original-owner-audit.md`. This is an evidenced boundary gap,
not a complete guest reproduction. Recover the original Win16/OS2 NE
classification and nested semantics with existing transport, then verify
real execution and teardown in its corresponding implementation stage.

## WOW SYSTEM.INI shadow contract

`shard-2/wow-kernel-profile-module-review.md` traces the original private
profile thunks to a session-owned temporary copy for the exact bare
`SYSTEM.INI` name. If shadow setup fails, the caller passes the bare name
through to the host profile API. DIV-297 previously described the selected
root file as the direct API input; its registry now reflects the actual path.
Write persistence, host mapping on failure, and worker teardown remain
unverified, so this is a documented contract gap, not a proven runtime bug.
