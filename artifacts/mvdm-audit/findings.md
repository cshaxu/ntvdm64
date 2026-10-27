# Cross-shard findings (in progress)

## Divergence identifiers are not unique

A mechanical registry-table sweep found seven repeated IDs with distinct
rows; `registry-id-collisions.json` records line numbers and summaries.
Identical duplicate rows for DIV-227, DIV-228 and DIV-229 were removed.
Inline-only collisions still require a separate source sweep.

`MVDM-HOST-DIV-222` identifies both `dpmi32/modesw.c` 53:01
protected-mode entry and `softpc.new/base/keymouse/mouse_io.c` INT33
direct-callback handling. Both have separate rows in `src/mvdm/README.md`
(lines 1248 and 1413 at this audit). `MVDM-HOST-DIV-230` appears in
`dpmi32/modesw.c` at the real-mode context branch while its registry row
describes `softpc.new/base/ccpu386/ccpusas4.c` HMA observation. `MVDM-HOST-DIV-274` also labels both `oemuni/file.c` OEM path sizing
and `softpc.new/host/src/nt_mouse.c` cursor publication, while the registry
currently describes only the former. `MVDM-HOST-DIV-327` also occurs on CCPU `lmsw.c` and COMMAND
`cmdexec.c` observation paths, and in a separate protected-mode entry in the
register; determine whether the numbers describe one cohort or collisions.
Identity
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

The original `wow32/i386/sources` explicitly selects `callpr32.asm` along
with `fastwow.asm`; the helper copies arguments and restores the x86 stack
for `wkgthunk.c::WK32ICallProc32`. The current WOW32 build graph and map
do not name that helper, while the selected original C file also contains
a non-i386 fallback. The compiler-selected branch, actual x86 call path,
and equivalence for Pascal/cdecl and callee cleanup require a focused
comparison with existing real-guest generic-thunk tests. The old generic
`!IF 0` rationale cannot establish that this helper is unnecessary.

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
