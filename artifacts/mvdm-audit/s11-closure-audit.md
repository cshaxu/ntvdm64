# T473 S11 closure audit — current working candidate

This is a bounded review of the active stage, not T closure. The earlier
foundation candidate was committed as `a8a0aef7e`; the current ten-image
release and startup-failure correction are recorded in
[wow-host-attach-failure](s11-foundation-evidence/wow-host-attach-failure/review.md). The
owner moved SOL/multi-window performance optimization to the queue tail;
the distinct intermittent native-return failure remains a correctness issue.
The current ten-image candidate and its results are identified in
[idle-comment-integrated](s11-foundation-evidence/idle-comment-integrated/review.md);
[retirement-error-integrated](s11-foundation-evidence/retirement-error-integrated/review.md) and [coherent-current](s11-foundation-evidence/coherent-current/review.md) are earlier candidates.

| Foundation | Current implementation/evidence | Remaining boundary |
| --- | --- | --- |
| Far-transfer validation | Project-added eager diagnostic reads were removed; current selected CCPU fixture passes off/matching/nonmatching trace and valid control. [Evidence](s11-foundation-evidence/coherent-current/far-exception-result.json). | Focused fixture has `/force:multiple` seams; it is not every guest exception or nested delivery. Audit other admitted diagnostic reads against original ownership. |
| WOW DLL entry | Generated bridge uses original one-byte BOOLEAN; controlled direct and host loader tests pass, and the actual selected link map names that bridge. [Evidence](s11-foundation-evidence/coherent-current/bridge-loader-result.json). | Real-provider diagnostics observe thread/process notifications, explicit unload after a missing export and both empty-state cleanup entries. A current-object [bitmap fixture](s11-foundation-evidence/current-bitmap-cleanup/review.md) exercises initialized-state use and shutdown. Populated live unload, registry-token cleanup and guest DLL lifecycle remain distinct boundaries. [Failure evidence](s11-foundation-evidence/wow-host-attach-failure/review.md); [notifications](s11-foundation-evidence/wow32-real-notifications/review.md); [explicit unload](s11-foundation-evidence/wow32-explicit-unload/review.md); [empty cleanup](s11-foundation-evidence/wow32-empty-cleanup/review.md). Real package normal-load evidence is [here](s11-foundation-evidence/current-wow32-real-loader/review.md). |
| NTVDM export boundary | Original template selected; supplemental original restored byte-for-byte, 271 ordinal contracts checked, and selected live VDD/MEM normal and exhaustion witness passed. [Evidence](export-source-recovery-review.md). | Remaining real memory-provider/task behavior belongs jointly to S11/S12; this is not proof of all consumers or failures. |
| Callback/execution ownership and window identity | Current graph/reclaim and native-nested fixtures cover pinning, retired generations, desktop incoming references, callback nesting, destruction and failed-return execution ownership. Shared-worker idle/dialog witness passed. [Evidence](s11-foundation-evidence/coherent-current/review.md). | The observed live `publish-native-return` error 1400 remains unlocalized: controlled dead-root and menu-owner paths can each produce it. Do not suppress the error or claim it fixed without identifying the real failure. [Evidence](s11-foundation-evidence/atomic-task-retire/execution-check/baseline-failfast-trace/review.md). |
| SAS and C-VID vector boundary | Selected-object and link/dependency witnesses characterize the differing headers and passing C-VID fixture. [Evidence](host-ch-scope/ccpu-evidgen-review.md), [SAS evidence](shard-3/sas4gen-vector-audit.md). | Selected SAS breakpoint-vector indirect consumers and full worker startup are not exhaustively traced; guest video mode/reset ownership continues in S13. No defect is inferred solely from differing declarations. |
| BIOS virtualization fallback | The selected PE's BOP B1 and the exact embedded ROM entries are verified. [Evidence](s11-foundation-evidence/b068-selected-boundary/review.md). | The relevant external driver, selector lifetime and actual fallback execution remain unverified. S13 owns device/reset evidence; this does not establish an ordinary WOW failure or authorize unrelated CCPU-internal repair. |
| Controlled CPU port permissions | Current port-routing witness and original owner mapping exist. The 117 controlled decoder/TSS cases passed after relinking to the current cached libraries. [Evidence](s11-foundation-evidence/ccpu-port-current/review.md). | S11 owns the selected decoder/privilege/fault entry; S13 owns device concurrency and reset. The fixture does not prove live guest execution or exact earlier worker library bytes. |
| Win16 loader dependency | [WIN87EM review](shard-3/win87em-build-input-role-review.md) distinguishes archive, installed/staged paths and loader ownership. | Actual Win2 dependency lookup and relevant FPU behavior remain S11/S14; absence in one inspected directory is not by itself a failure. Never rebuild or modify guest. |

The current CCPU high-page fixture's missing link seam, IRQ-ready setup and
marker output were repaired and its original USER validator passed. The
adjacent bit-fault fixture remains a recorded non-pass against the deliberately
held internal bit-index candidate; it was not silently weakened. [Evidence](s11-foundation-evidence/current-ccpu-execution-fixture/review.md).

The current registry binding object now has a controlled populated-token
shutdown witness, including explicit close, stale tokens, repeated shutdown
and reopening. It does not prove populated real-worker DLL unload.
[Evidence](s11-foundation-evidence/current-registry-shutdown/review.md).

The latest release inputs were also rechecked in a freshly resealed live
worker: the VDD memory exports and exhausted-reallocation rollback passed.
[Evidence](s11-foundation-evidence/published-export-live/review.md).

The diagnostic guest-read recovery is published after focused witnesses and
the Full gate. Its remaining coverage boundaries are in
[the candidate record](s11-foundation-evidence/diagnostic-stack-candidate/review.md).
An additional enabled CPU-trace guest-read correction has a reproduced
nonidentity-address mismatch, focused controls and a published Full gate.
[Evidence](s11-foundation-evidence/trace-scalar-candidate/review.md).
An original CCPU real-mode CALLF/RETF test also passed with trace disabled,
unmatched and matched; [scope](s11-foundation-evidence/ccpu-callf-trace/review.md).
The DPMI portion additionally has a selected-source original dispatch and
stack-underflow witness, with [production-object instruction comparison](s11-foundation-evidence/dpmi-selected-dispatch/review.md).

## Delivery status

The latest [scalar CPU-trace release](s11-foundation-evidence/trace-scalar-candidate/review.md)
supersedes the package identities below. It passed the 16-gate Full suite and
Winmine gameplay, and both delivery destinations matched all ten hashes. It
does not resolve the separately observed SOL 1400 or establish every boundary
listed in this audit.

The current candidate has passing Console-17, Window-17, Control-12, guest
manifest, bounded Calculator/Notepad/Write/Program Manager and shared-WOW probes.
The current candidate subsequently passed the Full suite; neither it nor the
bounded probes prove every row above. Earlier
package results must not be attributed to this manifest without identity
evidence. Mirror differences remain partial with textual estimates; the
estimates are not behavioral coverage. Held 486 and independently discovered
internal CCPU changes remain outside the owner-approved connection-boundary
scope. The Alt+Enter changes and queue-tail proposal were included in that
published delivery.

The owner explicitly authorized restarting Winmine and changing its profile.
The current candidate passed the Full suite, including its Winmine startup
frontier; [receipt](s11-foundation-evidence/idle-comment-integrated/review.md).
An independent Winmine gameplay probe also passed. Neither result establishes
the source of the intermittent 1400; correctness and audit review remain
before S11 closure.

The rebuilt retirement-error candidate passed graph and native-nested selected-object fixtures, Console-17, Window-17, Control, guest media, shared-worker idle/dialog, Calculator, Notepad, Write and Program Manager dialog witnesses. The controlled bridge test passed eight loader cycles. These results do not prove actual guest DLL failure/unload behavior or resolve the intermittent native-return 1400; the subsequent Full gate passed separately.

The remaining enabled CALLF and descriptor-store diagnostic read paths were
traced in [diagnostic read boundary](shard-3/ccpu-diagnostic-read-boundary.md).
They are opt-in and remain unproven under reentry or paging; the selected
normal-path far-exception fixture does not cover them. No failure or need for
an original CPU repair is inferred solely from this source finding.

## Live 1400 evidence: distinguish the separate runs

The historical SOL trace also records incoming error 1400 during three
successful sequences before the final failure. The current and pre-lock
source save incoming last-error at that trace point. This does not prove the
final error was stale, but prevents attributing it to a dead HWND from the
number alone. [Evidence](s11-foundation-evidence/sol-baseline-error-provenance/review.md).

Two earlier shared-worker diagnostic runs identified native-return 1400 on
different dead mandatory HWND roots from the project window table. A later
`two-task-ownership-nonpass` trace established one precise interleaving: while
one thread owned the USER runtime and projected a copied root, a second thread
retired that exact window generation without owning the USER runtime; native
capture then failed on the now-dead copied root. See the corresponding entries
in the [investigation](s11-dialog-performance/investigation.md). These runs
used experimental diagnostic providers, not the current sealed candidate; they
do not prove that the separately observed SOL baseline failure took that path.

The current `refresh_graph` first collects pending retirement, then builds its
mandatory roots from the remaining `TYPE_WINDOW` table entries. Its separate
menu refresh subsequently queries each enrolled window. Thus the current code
still has multiple possible sources of 1400. The working candidate now acquires
the task/runtime owner around terminal retirement and includes focused lifetime
tests, but those controlled tests and limited live passes do not prove that
every relevant cross-thread interleaving is eliminated. The root witness is not
permission to discard an invalid required root or proof of the current SOL
failure. The experimental diagnostic must not be treated as a released image.

The published candidate also passed a real Winmine plus 17-dialog sibling
retirement run using an unchanged, hash-verified prebuilt Win16 witness.
[Receipt](s11-foundation-evidence/current-mine-dialog-retirement/review.md).
It covers that combination but does not force the historical interleaving or
identify the SOL failure.

The earlier deterministic property-only scheduling test used an older
experimental terminal body, so it did not directly test the current body.
It has now been rebuilt with a temporary copy of the current source and three
test-only event hooks. With a desktop-capable test context the fixture exited
zero and reported `first_phase=1 held=2 unheld=0 retired=1`, followed by
`WOW_PAGE_DOMAIN_LIFECYCLE_OK`. Current source SHA-256 is
`04DB4AC5376F69811DC40A103AB3C8ABF2781A9D1AC7C894D5977B074FC14CA1`;
the instrumented copy is
`E9DDAE6A49D6A14A87E1E5463EA482A70931C83F53821A6E98A1B483DE9C84E4`;
fixture executable is
`D2C068DB436C2152C403DACC63705CDBBDF760A4175E231005E07B752DE978B2`.
Inputs, output and temporary preparation script are under
`build/t473-s11/terminal-schedule-current` and
`build/t473-s11/prepare-current-terminal-schedule.py`. The first run from a
restricted token exited 2 before producing assertions; it is not a product
result. The passing test uses the earlier fixture and a resource-only second
thread with property metadata; it verifies exclusion at the two instrumented
points, not copied guest state, two real task threads, full provider identity,
or the unlocalized SOL failure. It is not a Full gate or release package.

## Raw evidence disposition

Before delivery, 1,338 untracked raw audit outputs and one generated Python
cache were classified without discarding their bytes. Copies were verified by
SHA-256 before their originals
were removed from the working tree. Their original paths, hashes and sizes are
in [the archive index](s11-raw-evidence-archive-index.json); the recoverable
copies remain under `build/t473-s11/raw-evidence-archive/` during this active
task. This is a temporary task archive, not a reusable cache or a promise of
permanent retention. At the T boundary, retain any still-needed evidence or
explicitly dispose of the archive under the build cleanup policy. Compact
audit reports, source-role tables, directly linked witnesses and production
tests are retained separately in the delivery. No guest or user file was moved.
Twenty-four compact receipts were subsequently restored from that archive and
matched against the recorded hashes, because several reports referred to their
containing directories rather than linking each receipt individually.

Documentation governance verification passed after updating this closure
audit's candidate identity. The current package's bounded evidence is linked
above; earlier SOL results remain attached only to their own manifests.

## Current cached SAS vector identity check

The existing read-only PE/map probe was rerun against the current selected
`build/.ninja/msvc-x86-release/worker/ntvdm.exe` input, SHA-256
`181232BF842294B776E6677C6FFEB80E123A2B23ECFDCF4F43AFF9C69E3241D9`,
matching the published candidate's recorded cache input. Its matching link map
identifies the 58-slot initializer: `Sas_touch` (54) and
`SasVirtualiseInstruction` (56) both point to the selected breakpoint fallback,
and `IsPageInstanceData` (57) is null. The probe verified both fallback
opcodes in the PE. This refreshes the prior r10 binary observation; it does
not establish runtime reachability, validate every consumer, or justify
changing the original CPU. The sealed deployment image differs in hash from
this pre-sealing input and was not itself the subject of this PE observation.

## Explicit creation enrollment lock witness

`wow_window_creation_enroll_internal` enters the runtime lock before calling
`enroll_internal_locked`, which invokes `claim_explicit`; the latter records
that lock before its task callback and reacquires it for enrollment. Thus an
unlocked entry to the public enrollment path alone is not evidence of an
unlocked explicit attachment. A test-only wrapper around the current
`wow_window_words_attach_target` observed seven controlled 16-bit task
attachments with runtime held, task flag set and a non-null synthetic TDB; 29
other attachments also held it. The existing native-creation and injected
unwind fixture exited zero, including cleanup and retry. This replaces the
incorrect earlier assumption that the fixture never supplies a TDB: its main
function initializes one. It is a controlled single-thread fixture, not a
real Win16 guest, concurrent rollback proof or diagnosis of SOL 1400.

The production creation source SHA-256 was
`B62CB2A26E86B4E3731C6C1CBD42DCF715833863B19204E111EA32B2ED887B04`;
the temporary observer wrapper SHA-256 was
`E51D1F5B5506DBCAE935C3E98EE26A92002E3981DE0186C8774401BE4FB51090`.
The non-publishable wrapper, isolated fixture output and test runner remain
under `build/t473-s11/creation-lock-study/`. A restricted-session attempt
reported desktop access error 5 and was not counted; the desktop-capable run
was `run3`. No product or guest was changed.

### Injected post-attachment acquisition failure

A separate temporary test-only wrapper forces the acquisition immediately
after a successful explicit attachment to fail with `ERROR_NOT_ENOUGH_MEMORY`.
The controlled 16-bit task creation returns failure and records
`CREATION_FAIL_DETACH_HELD result=1`: the exact detach call observed the
runtime lock held. The same fixture then creates normally, exercises its
existing exception/unwind case, verifies empty object slots and exits zero.
The wrapper does not alter the production source. Its SHA-256 is
`EAB4103E1AE946A8E4060F31276406E95ECE2346CC0DFAAFDCFE27712310F40A`;
the modified fixture copy is
`40D8FE45A7E27C00C63D9B85154F8F389F542FD7633D8DBE32F90D317A7F15C4`.
Inputs and output are under `build/t473-s11/creation-lock-study/failrun` and
its parent. This verifies this injected failure in one controlled task, not
all failures, parallel guest execution, or the observed SOL 1400.

## Current released SOL lifecycle repeat

The later published diagnostic-read release passed eight separate debugger
attachment intervals and Options opens/closes without a captured exception.
[Receipt](s11-foundation-evidence/published-sol-fault-capture/review.md).
The capture changes timing and does not resolve the historic 1400; its 10 s
observation delay is not a performance measurement.

The production candidate's visible-desktop probe completed eight consecutive Options opens and closes and returned launcher exit 0. The exact ten-image stage manifest, probe and observer reports are in [sol-current-lifecycle](s11-foundation-evidence/sol-current-lifecycle/). WOW32 SHA-256 was `6EF2791521DB2B962BFB38E800143C840CBCC3AF70417FDF7BA4E83605C34741`. A separate failure-stage diagnostic completed 24 rounds without recurrence; [receipt](s11-foundation-evidence/current-return-stage-repeat/review.md). These bounded checks do not attribute or resolve the earlier intermittent 1400. The first sandboxed production attempt could not allocate the existing short staging directory and did not exercise the product; the authorized desktop-capable run passed.


A fresh [published baseline shared-task witness](s11-foundation-evidence/published-shared-retirement-current/review.md)
also passed 16 explicit dialog closes and task retirement alongside surviving
Winmine gameplay, with all ten release image hashes matched. It does not force
the previously observed race or attribute the historical SOL 1400.

A [real-worker DOS startup-to-exit observation](s11-foundation-evidence/sas-real-worker-observation/review.md)
now covers one immutable MEM launch from process creation under the debugger:
the worker and launcher exited zero and neither selected tail breakpoint was
observed. The observer explicitly handled the original marked BOP 5B break;
other exceptions were not silently ignored. This is bounded dynamic evidence,
not proof for all guests, profiles or external consumers.
The same SAS observer also covered an original [Win16 task startup and
exit](s11-foundation-evidence/sas-real-worker-win16/review.md) with no tail
breakpoint; the resident worker's later lifetime remains outside that run.
