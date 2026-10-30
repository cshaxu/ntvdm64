# T473 S11 closure audit — current working candidate

This is a bounded review of the active stage, not T closure. The candidate
was published and committed as `a8a0aef7e`. The
owner moved SOL/multi-window performance optimization to the queue tail;
the distinct intermittent native-return failure remains a correctness issue.
The current ten-image candidate and its results are identified in
[idle-comment-integrated](s11-foundation-evidence/idle-comment-integrated/review.md);
[retirement-error-integrated](s11-foundation-evidence/retirement-error-integrated/review.md) and [coherent-current](s11-foundation-evidence/coherent-current/review.md) are earlier candidates.

| Foundation | Current implementation/evidence | Remaining boundary |
| --- | --- | --- |
| Far-transfer validation | Project-added eager diagnostic reads were removed; current selected CCPU fixture passes off/matching/nonmatching trace and valid control. [Evidence](s11-foundation-evidence/coherent-current/far-exception-result.json). | Focused fixture has `/force:multiple` seams; it is not every guest exception or nested delivery. Audit other admitted diagnostic reads against original ownership. |
| WOW DLL entry | Generated bridge uses original one-byte BOOLEAN; controlled direct and host loader tests pass, and the actual selected link map names that bridge. [Evidence](s11-foundation-evidence/coherent-current/bridge-loader-result.json). | Controlled provider is not a complete real guest DLL attach, failure, notification and unload lifecycle. |
| NTVDM export boundary | Original template selected; supplemental original restored byte-for-byte, 271 ordinal contracts checked, and selected live VDD/MEM normal and exhaustion witness passed. [Evidence](export-source-recovery-review.md). | Remaining real memory-provider/task behavior belongs jointly to S11/S12; this is not proof of all consumers or failures. |
| Callback/execution ownership and window identity | Current graph/reclaim and native-nested fixtures cover pinning, retired generations, desktop incoming references, callback nesting, destruction and failed-return execution ownership. Shared-worker idle/dialog witness passed. [Evidence](s11-foundation-evidence/coherent-current/review.md). | The observed live `publish-native-return` error 1400 remains unlocalized: controlled dead-root and menu-owner paths can each produce it. Do not suppress the error or claim it fixed without identifying the real failure. [Evidence](s11-foundation-evidence/atomic-task-retire/execution-check/baseline-failfast-trace/review.md). |
| SAS and C-VID vector boundary | Selected-object and link/dependency witnesses characterize the differing headers and passing C-VID fixture. [Evidence](host-ch-scope/ccpu-evidgen-review.md), [SAS evidence](shard-3/sas4gen-vector-audit.md). | Selected SAS breakpoint-vector indirect consumers and full worker startup are not exhaustively traced; guest video mode/reset ownership continues in S13. No defect is inferred solely from differing declarations. |
| Controlled CPU port permissions | Current port-routing witness and original owner mapping exist. [Evidence](ios-owner-review.md). | S11 owns the selected decoder/privilege/fault entry; S13 owns device concurrency and reset. Distinguish controlled object tests from live guest execution. |
| Win16 loader dependency | [WIN87EM review](shard-3/win87em-build-input-role-review.md) distinguishes archive, installed/staged paths and loader ownership. | Actual Win2 dependency lookup and relevant FPU behavior remain S11/S14; absence in one inspected directory is not by itself a failure. Never rebuild or modify guest. |

## Delivery status

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
