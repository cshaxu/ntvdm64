# Transaction-local projection index reuse

**Correction:** the isolated candidate NTVDM link emitted LNK4014 and LNK4006:
its archive removal used a basename while the archive member is stored as
`obj/adapter-softpc/mvdm_softpc_wow_page_domain.obj`. The old object remained
first and the candidate object was ignored. Therefore the candidate-package
SOL A/B/A timings below did **not** test index reuse and must not be used as
candidate evidence. The selected-object fixtures linked the candidate object
directly. The subsequently integrated source build and its separate graph
fixture and integrated-package tests are unaffected by this candidate-link
error; their own hashes and logs identify that later build.

The project-owned page-domain adapter first validates a copied window graph and
builds a 65,536-entry `ULONG` slot-to-row index. The same call then passes its
unchanged links to the commit helper, which previously allocated, zeroed and
built a second identical index. The integrated change passes the already
validated index to that helper for this one synchronous transaction. The public
standalone links entry still creates, validates and frees its own index. No
index survives the call, and there is no additional window identity authority.

This removes one 262,144-byte requested heap allocation and its initialization
and duplicate population per full projection. That is an allocation-size
accounting result, not a measurement of process working-set reduction or a
claim that the modal delay is fixed. The change is confined to the project
adapter; no MVDM mirror or guest bytes change. Once an allocation is removed,
failure of that now-unnecessary allocation can no longer fail the transaction;
remaining allocations and their failure paths retain their existing checks.

The isolated candidate passed actual-CCPU graph-snapshot and window-borrow
fixtures, and the nested native-call fixture using the candidate source with
the fixture's existing renamed seams. The first nested fixture attempt linked
two page-domain definitions and failed before execution; the corrected attempt
selected one candidate owner and passed. The production source was then
integrated and the x86 worker graph rebuilt successfully; the integrated
graph-snapshot fixture passed, including descriptor failure, missing desktop,
unpublished state, relationships, retirement and reuse witnesses. Candidate
SOL A/B/A passed nine dialog interactions, but as corrected above its worker
did not contain the candidate and it is not a candidate performance test.
Each probe's manifest and source/build hashes are
retained here. Candidate and integrated NTVDM hashes differ and those candidate
SOL results are not evidence of a full integrated product pass. Full coherent
package, other failure injection, broader DOS/WOW regressions and publication
remain outstanding.

The integrated ten-image package was subsequently staged with its NTVDM input
hash matching the current rebuilt worker. Console-17 and Window-17 both passed
all cases; three SOL Options interactions passed; and two distinct Win16 tasks
in one WOW worker completed the shared-idle/common-dialog witness (17 dialogs,
16 explicitly closed, idle continued after dialog-task retirement). The
corresponding staged manifests and summaries are retained in the integrated
subdirectories. The shared-idle witness uses a freshly sealed package identity
and records its own manifest. These tests do not constitute Control or Full
acceptance and do not establish an end-to-end performance improvement.

The complete selected Control suite was then rerun against this integrated
package with an identical ten-file test cache: all 12 gates passed. Its
runtime manifest and gate timings are in `integrated-control`. An earlier
attempt was rejected before the first gate because its cache files did not
match the newly sealed runtime, and is not counted as a product failure.
The Full suite and remaining S11 acceptance are still outstanding.

Further isolated actual-application probes passed Notepad Find, Write Find,
Program Manager New, and SOL gameplay (deal, stock draw, close and launcher
completion). Per-probe manifests and outputs are in the corresponding
`integrated-*` directories. They are bounded witnesses, not proof of editing,
persistence or the complete Full gate. The Winmine gate has not been run.
The current package also passed the immutable guest-media contract: all 59
manifested files match; configuration files are outside that contract. Input
hashes and the receipt are retained in `integrated-guest-media`.

The real-CCPU graph fixture now explicitly supplies two different full HWND
generations sharing one low-word slot to the projection entrance. It asserts
`ERROR_INVALID_PARAMETER`, no published window and unchanged free virtual
memory, and the runner requires the new duplicate-slot marker. The current
source-built fixture passed with the selected page-domain owner identified in
its link map; [output and hashes](integrated-duplicate-slot/) are retained.
This covers a key assumption behind passing the validated slot index to the
commit helper, but is not exhaustive allocation-failure or concurrency proof.

The adapter is registered as ADAPTER-WOW-104 in the worker owner README. That
registration changed the source comment and caused a rebuild; the new worker
differs from the previously tested worker at six bytes, all within the PE COFF
and debug-directory timestamps ([comparison](registration-rebuild.json)).
This byte observation does not make different sealed packages identical. A
new coherent package with the rebuilt worker passed three SOL Options rounds
and the 59-file immutable guest-media check; its separate manifest and SOL
output are under `registered-sol`. Control, DOS matrices and Full for this new
package remain unverified and must not inherit the earlier package's results.

For that newly registered package, Console-17 and Window-17 were then rerun
and all 34 expected return codes matched. The full selected Control suite was
run separately with a byte-identical ten-image cache and passed all 12 gates.
The matrix and control manifests, summaries and timings are retained under
`registered-matrix` and `registered-control`. A combined supervisor first
completed the matrices but could not start Control because the first child
left a temporary PATH in the supervisor process; the independent Control run
used its normal environment. This is not a Full-suite or publication pass.

The registered package separately passed actual Notepad Find, Write Find,
Program Manager New and SOL gameplay probes. The shared-WOW idle/dialog
witness also passed with two distinct tasks, 17 dialogs, 16 explicit closes
and idle continuing after the dialog task retired. That witness reseals a
fresh identity and retains its own manifest. Results and package identities
are in the five `registered-*` application directories. They do not cover
the Winmine gate or all user interactions.

The registered package also ran the hash-recorded Win16 Calculator, displayed
its own window and About dialog, and completed the launcher. The selected guest
declares a static WIN87EM import; the `registered-calc` witness does not
identify the actual loaded module or prove arithmetic and pre-3.0 behavior.
