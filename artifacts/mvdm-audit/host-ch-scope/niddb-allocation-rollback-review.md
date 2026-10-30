# NIDDB allocation failure rollback

S11 independent review identified two unverified lifecycle boundaries before
delivery. If a later device BOP observes the ID of a retired active VM,
`virtual_swap_instance` can dereference its cleared record; the current
candidate does not establish the INSIGNIA.386 byte-update order. A silent
fallback to another VM would also risk executing with the wrong identity, so
neither guest reachability nor a safe replacement is assumed. Separately,
system-VM allocation failure leaves the original `allocation_allowed` lock
in place; existing fault-injection tests retry initialization and therefore do
not prove a no-retry failure/exit contract. These are source and coverage
findings, not newly reproduced guest crashes. The original VxD implementation
has not been found in the indexed sources; guest binaries remain unchanged.

Source inspection of `src/mvdm/softpc.new/base/bios/virtual.c` identifies a
failure-path allocation leak in `allocate_NIDDB`. The committed baseline
contains that branch; the worktree now has an unpublished correction in
addition to the separate instance-retirement candidate. The defect has been
reproduced with controlled allocation failure, not in an installed guest.

The function allocates the per-VM pointer table with `host_calloc`, then
allocates each registered data block with `host_malloc`. If a block allocation
fails, it frees each non-null data block and returns FALSE without freeing the
table. The `vrecs` slot is only assigned after this loop, so later normal VM
cleanup cannot recover the unregistered table. `VxD_Sys_VM_Init` and
`VxD_VM_Init` report the failure through carry and `host_error`.
`host_def.h` maps these allocation functions to the host C runtime.

The potential leak is one pointer table per failed attempt, only when table
allocation succeeds and a subsequent instance-block allocation fails. Normal
creation and an initial table-allocation failure do not exercise this leak.
Selected `mouse_io.c` registers an instance data block, but actual VxD
notification and allocation failure on an installed system are not proven.
The existing [retirement candidate evidence](../mouse31-acceptance-review.md)
tests successful allocations, retirement and callbacks, not this failure path.

The worktree candidate now frees the table after the partial blocks and before
returning FALSE, registered as MVDM-HOST-DIV-468. The replayable
[diagnostic](niddb-rollback-run.py) compiles two source-derived objects with
hooks confined to `allocate_NIDDB`: the baseline omits that one free, while
the candidate includes it. Independent baseline runs report one live
allocation at each of the two block-failure ordinals. The candidate passes
failures at both block ordinals
with zero hooked live allocations, preserves the original carry/error reports,
and then successfully creates a VM. It uses
controlled host and VxD boundaries and is not the uninstrumented product
object or an installed guest test. The uninstrumented candidate object also
passes the existing retirement fixture: 15 checks, three callbacks and its
negative pointer-width compile (`build/t473-s10/niddb-div468-contract`).

A further [selected-object diagnostic](niddb-object-rollback-run.py) copies
the cached worker `obj/bios/virtual.obj` and redirects its three CRT allocator
symbol references to the fixture. GNU objcopy also changes section padding,
so the copied object is not claimed byte-identical to its input. The input object SHA-256 is
`1F9969AA61C93FD4591F7A3AC93B51EC087C85D1D492DE3656149E262B13B535`;
the intercepted copy is
`3D50B19AD67DA6BAD95060D1F75D72D09E07D39579997CDFAD8AB0BE3019C139`.
It passes an initial table-allocation failure, both injected block-failure
ordinals, all three carry/error checks,
subsequent VM creation and retirement with no tracked allocation left. This
strengthens the compiled-object evidence, but does not establish full product
regression or installed-image identity.
An independently compiled HEAD-source baseline object, using the same
compiler command, fails the same intercepted-object fixture at the first
block allocation: one table remains live and the original carry/error were
reported. Its input object SHA-256 is
`53200AB23072904616298B28B6872109CEA42938E31B3732704AC922F4CE9D75`.
The candidate object passes the same fixture, including the table-allocation
failure and both data-block-failure ordinals. HEAD-source compilation does
not assert identity to a previously published binary.

Actual VxD/guest failure reachability and full regression are still unverified;
this is an unpublished candidate. The cached worker has now
rebuilt `virtual.obj`, `original-softpc-bios.lib` and `ntvdm.exe`; a staged
ten-component candidate has package ID `be4b6c70-2cb5-4b68-abf6-55365539deb9`.
Its NTVDM input SHA-256 is
`7B1E7A916D802C283D1326257BA83CE8955D1DDFE70E366960A176602509BBBF`
and its sealed image SHA-256 is
`A87E0BD23FE6DB8AFD97C6AF3D33B7A10BC50827BFB56915AEDBEF11858C849F`.
An immediate repeat of the same cached `ntvdm.exe` target reported
`ninja: no work to do`.
After a reviewer accidentally opened the worker build tree with an older
Ninja, its `.ninja_log` disappeared. The coordinator rebuilt the target with
the project's Ninja 1.13.2 and existing ccache; 532 build edges completed,
and the next identical build reported no work. The rebuilt unsealed image is
`2BB64593FE19FDB1720BDFD0724FF3B0424A9C57972C0DCDE0BE7ED320816C4A`,
different from the previously tested input. The raw PE `.text`, `.data`,
`.rsrc`, `.reloc` and detour sections match; `.rdata` differs by two bytes
within the debug-directory timestamp. This does not make the rebuilt file the
same tested artifact; it requires its own identity binding before delivery.
The first Full attempt stopped because its comparison cache lacked the
frontend test client. The second attempt completed all 14 selected Full gates;
[timings](../niddb-candidate-evidence/full-timings.json),
[runtime](../niddb-candidate-evidence/runtime-manifest.json) and
[package manifest](../niddb-candidate-evidence/package-manifest.json) bind
the result to this exact sealed image. The WOW gate only observes application
windows. The subsequent WINMINE gameplay probe passed flag/counter, F2 reset
and opening a cell, but returned 14 because its window remained after
`WM_CLOSE`; the runner subsequently cleaned its isolated scope. The same
private-desktop probe failed identically on the published baseline. A separate
owner-authorized visible-desktop candidate run passed flag/counter, reset,
opening a cell, normal window close and launcher exit zero. The private
desktop failures remain recorded; their cause is not determined. SOL gameplay
was also checked: both candidate and published baseline passed deal/draw and
owned window-close observations, but both launcher observers timed out, so
neither run is a complete gameplay/exit pass. The first candidate SOL Options
probe did not observe the modal dialog; the same baseline probe did observe
the dialog and parent recovery, but later failed because its launcher did not
finish. A repeat candidate probe likewise reached modal dialog and parent
recovery, then failed because its launcher did not finish. The first candidate
run's shallower observation remains an intermittent non-pass; the repeat does
not prove a cause or erase it. All three SOL Options runs failed the complete
launcher-exit assertion, including the published baseline.
Visible-desktop Options probes also failed: candidate twice did not find the
modal dialog and the baseline observed a modal close/parent-recovery failure.
The candidate window-creation trace recorded an explicitly claimed child
creation failing with error 5023 in each attempt, but does not identify which
enrollment/state operation returned that error. This is an actionable WOW
window-creation lead, not proof that either NIDDB change caused it. Further
attribution is required before changing production behavior.

The previously retained failure-only WOW32 diagnostic was restaged with the
candidate worker for a visible-desktop Options attempt. The modal dialog was
observed, but modal close or parent recovery failed; no `NegativeStateFailure`
or failed `WindowCreate` line was emitted. Its sealed diagnostic WOW32 hash is
`D87DFF1BC96CB1E92F58C2414E55A8BBA4B036A5B60EB467C12A22885A3F3365`;
its differently sealed NTVDM hash is
`4C7A9F36E1B32E2BBB2F895CE3EACCE87F0E75CC0E3D71F0BBCF6D71786B991D`.
This diagnostic alters executable layout and is never publishable. Its run
does not identify the original 5023 check or establish a fix.

A later seven-translation-unit failure-only diagnostic did reproduce 5023.
It identified `wow_window_words_attach_client_state` failing because
`mvdm_softpc_wow_page_domain_borrow_window` returned zero; the error then
propagated through `claim_explicit` during the dialog's creation. This is more
specific than a generic window-refresh failure. The borrow can fail because
the selected page-domain context is invalid, the full HWND lookup has no
matching current backing, or backing allocation/retirement rejected the pin;
the current trace does not distinguish those possibilities. Diagnostic source
copies and the package remain nonpublishable. No conclusion about an NIDDB
causal link follows from this observation.

A separate nonpublishable NTVDM diagnostic records those borrow preconditions
only on context or full-HWND lookup failure; its linked map binds the traced
function to the diagnostic object. Combined with the seven-unit WOW32
diagnostic, two visible Options runs passed dialog, parent and launcher exit;
a third observed the modal dialog but failed close/parent recovery. No
`BorrowFailure` was observed in those runs, so the original lookup failure is
not yet attributed. The WOW32 diagnostic also recorded an error in the
retired-object collection path on two successful runs; it is a separate
observation, not evidence that it caused the original creation failure.
The diagnostic's extra code can affect execution; these outcomes cannot
replace tests of the ordinary candidate.

The next diagnostic reproduced a borrow failure with active domain, matching
owner thread and page directory, and a nonzero slot. A later re-read reported
the same full HWND through both `page_domain_read_owned_word` and
`c_sas_dw_at`. Therefore the earlier apparent identity mismatch is **not**
established: the trace measured those values after the failed check. A new
diagnostic captures the original context decision and the first lookup value
at the decision point. Its first run reached the modal but failed its close;
its second passed the complete Options route. Neither hit the target borrow
failure. Retain both outcomes; no repair or semantic relaxation is justified
by the later matching read alone.
Two further runs of that same decision-point diagnostic completed the entire
Options and launcher-exit route without hitting the targeted failure; their
probe, window and observer logs are retained under `niddb-candidate-evidence`.
They do not retroactively turn the ordinary candidate's prior failures into
passes or establish why instrumentation changed the observed frequency.

Static call-chain follow-up: explicit creation starts a native call after the
original WOW task can be parked. Its CBT enrollment acquires the USER runtime
lock and performs guest-backed identity publication and client-state attach;
the state attach calls `borrow_window`, which requires the owner thread and
CPU page context in addition to the HWND lookup. This is a candidate execution
ownership boundary to test, not proof that scheduler ownership was lost or
that the USER lock is insufficient. A fix must first capture a failing
decision-point sample or a controlled counterexample; it must not relax HWND
generation checks or treat a missing backing as success. Two additional
decision-point diagnostic runs passed the full Options route, again without
the target failure.

The PID-bound visible-desktop probe was extended to exercise multiple Options
open/close cycles on one SOL process. An eight-round diagnostic-only run
completed all eight modal interactions, but its fixed 60-second observer
budget expired before final launcher completion; that is a test-budget
non-pass. The runner now budgets repeated cases separately. A four-round run
against the uninstrumented candidate completed all modal/parent checks and
the launcher exited zero. Both results are retained in
`niddb-candidate-evidence`; neither erases earlier failures or establishes
that the 5023 defect is repaired.
With the corrected observer budget, the next eight-round diagnostic run
completed five Options cycles, then failed to observe the modal on round six;
no `BorrowFailure`, `LookupMismatch`, or failed `WindowCreate` was recorded.
This is a separate unclassified non-pass, not the earlier 5023 attribution.
These builds do not prove guest VxD
lifecycle behavior. Do not claim the defect
explains a mouse or Win386 symptom without guest evidence.

Additional no-retry selected-object evidence: after a system-VM initialization failure at table allocation or either of two data-block allocations, the candidate retains `allocation_allowed=FALSE` until an explicit `System_Exit` or `NIDDB_System_Reboot`. All six controlled cases then restored allocation eligibility, left zero intercepted allocations, and allowed registered device data to be deallocated (`../s11-foundation-evidence/niddb-no-retry`). The fixture intercepts allocation symbols in the actual cached COFF object and supplies controlled register/VxD boundaries. It does not establish that the original provider sends System_Exit or reboot after a failed Sys_VM_Init, nor that an installed guest follows either route. It resolves the host cleanup behavior conditional on that event, not the original-provider ordering question.

The same six no-retry cases were extended to exercise the API, not just the active flag: before System_Exit/reboot, allocation is rejected and an attempted deallocation leaves both existing device handles intact; after the event, a new device allocation and deallocation succeed. The selected-object fixture passes all six cases with zero tracked transient allocations. This verifies the original lock's observable host behavior under these controlled inputs, but still cannot prove delivery of the required event by the absent guest provider.

The no-retry matrix now has a reusable selected-object runner and fixture at `tests/observation/verify-niddb-no-retry.py` and `tests/mvdm-host/niddb_no_retry_fixture.c`. The candidate passed all six explicit markers; the committed-source baseline control reproduced the table leak (`mode=0 ordinal=1 live=1`) and was accepted only as an expected negative. The runner records the input and intercepted-object hashes and constrains all build outputs and selected objects to the repository build tree. Formal receipts are under `s11-foundation-evidence/niddb-no-retry`. This is an independently invocable controlled test; it has not been claimed as part of the Full product gate or as guest-provider evidence.

Current-worktree recheck: Ninja reports the selected `virtual.obj` up to
date, and the six no-retry cases passed again in
`build/t473-s11/niddb-no-retry-current`. The input object SHA-256 is
`483500E02FE4CC019533A91C5A61FF954E4E45351BC75D128A00090D870339EF`;
the current source SHA-256 is
`45D1E266418E382136875506990C924537121AE245FB8E92F92A8157AB7D3C94`.
This preserves the same controlled-provider limitation and does not resolve
guest VxD exit ordering or make the candidate publishable by itself.

The allocation rollback fixture now occupies all four supported instance slots
and injects failure into the table allocation and each of the four data
allocations, for both System_Exit and reboot. All ten candidate cases passed,
including locked pre-exit deallocation, zero intercepted live allocations,
and a successful replacement allocation after exit. The same fixture detected
the expected table leak against the source-derived baseline at its first data
allocation failure. Receipts and markers are in
`../s11-foundation-evidence/niddb-all-slots/`. This extends controlled object
coverage; it does not establish the missing guest provider ordering.
