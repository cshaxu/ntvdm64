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

The selected `softpc.new/host/src/nt_inthk.c` contains DIV-307, DIV-308
and DIV-309 markers; an exact search of `src/mvdm/README.md` and the
indexed evidence directory found no direct matching identifiers. Their
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

The prior S1 unselected-source disposition describes an ntstubs.c
`sas_overwrite_memory` leaf. Current worker map instead attributes
`_sas_overwrite_memory` to `original-softpc-cvidc:accessfn.obj` and
`_c_sas_overwrite_memory` to `original-ccpu386:ccpusas4.obj`; it does not
by itself show extraction of the ntstubs.c wrapper. Compare actual callers,
semantics and conditional selections before deciding equivalence or
exclusion. This is an audit mismatch, not an asserted runtime defect.

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
