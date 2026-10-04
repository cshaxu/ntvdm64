# T428 S6 ownership and final sharing audit

## Scope and provenance

Baseline fc32b9043/db855a66c; published S5 r021 package. This is project-owned
private naming/caller cleanup, not original mirror extraction or wire change.
APP0.0.427/RPC38/I/O25, CLI and kind0/1/2 remain unchanged.

| Existing project implementation | Decision and target | Independent boundary |
| --- | --- | --- |
| NTCON native_console_frontend / run16_native_frontend | Frontend-owned frontend_session.[ch] and frontend_session API. Session service member becomes presentation. | Local Console buffers, input owner and renderer remain NTCON-private. |
| NTCON run16_console_channel/frontend/video | frontend_io_channel, frontend_console, frontend_video; update all production and test callers, remove old aliases. | Existing channel/dispatch/frame storage modules remain distinct resource owners. |
| NTCON frontend_native_keyboard | frontend_console_keyboard and console_records describe Windows INPUT_RECORD translation used by both workers. | Host layout translation belongs to frontend input owner; guest DOS interpretation remains original. |
| Build obj/run16 frontend objects | obj/frontend paths; regenerate audited Ninja graph/source manifest. | Actual launcher client/support objects retain their own source owner. |
| run16 launch_native / launch_gui | launch_win32_text / launch_win32_gui, explicit target class. | Same shared native request mechanism and unchanged CLI. |
| NTSRV service_copy_management_record | service_copy_vdm_management_record identifies original DOS/WOW reader. | Common management entry remains kind-aware; original records are not moved or replaced. |
| worker-base project shutdown close body inside connection.c | Move the unchanged local body into shutdown_close.c in the same library; both workers and the Console-client fixture link the production implementation. | RPC operations remain connection.c; no new callback, state, library, protocol or timeout. |

No second relationship registry, helper, scheduler, dedup or observer is added.
NTCON still selects text/configuration/DIB by message format, not worker kind.
Prior format names dos_text_frame/frontend_window_dos_frame/dos_surface are
absent from current frontend production sources. Original source names stay
in mirrors. Native30ms polling is out of scope and retained.

## Verification and retained failures

Mechanical renames are confined to audited project-private source/tests/build
selection. The ownership verifier now rejects retired API names and misplaced
frontend object edges, with negative controls. The final results below certify
the same coherent input, rather than a mixture of earlier candidates.

First affected build failed on two missed keyboard test members and stale
fixture wiring. Frontend-scope lifetime lacked FrontendIoDisconnected;
Console-client lacked the shutdown close link. The lifetime fixture also
retained the retired multiple-live-channel model. It now issues an explicit
broker disconnect between each of 34 sequential leases, asserts that the old
thread has joined/storage is freed before acknowledgment, and retains the
final live lease until root teardown. It does not permit unsolicited EOF to
replace broker control. Console-client links the actual production close body
without duplicating or substituting it.

Affected x86 /MT CCPU40 build passed after those test wiring fixes. Ownership
source/link verifier and negative controls passed. r003 service27 passed in
4214ms; production worker shutdown passed37 assertions, handles112/112.
The final lifetime fixture passed receipt/errors, no leaked handles, all34
leases, root preservation and broker shutdown precedence. r004 passed seven
worker-neutral input/codec/presentation/input-return cases. r005 actual private
Console full channel fixture passed, with stable handles439 and real pipe EOF.

Close runner's first attempt lacked its package logs directory and produced
no usable report. The next run passed actual callback/hung-callback/pipe-error
but uncovered an old service handle-count assertion: it omitted the service's
frontend_lifetime_changed event, which Stop always closes. Exact expected
counts now distinguish that event plus a retained canceled route from the
reconnect case which already removed the route. No leak tolerance is added.
Final close run passed all three transport-close cases and all29 catalogued
service variants. Unclaimed stop/reconnect are also added to the bounded
service runner; final r008 passed all29 cases in3857ms.

Final coherent candidate is r002/runtime (unchanged sealed WOW32 input from
S5; all changed EXEs/DLL refreshed from the rebuilt cache). r007 Product passed
against S5 r022 WOW frontiers: Console17 65940ms, Window17 75912ms, retained
WOW 67276ms; total214653ms including preparation and cleanup. The tested
eight-file identity is recorded in r007/runtime-manifest.json.

r009 `verify-native-gui-routing.ps1` passed all six actual GUI cases: startup,
explicit wait/exit37, GUI-to-text, same-worker resident reuse, GUI-only carrier
management close with surviving GUI target, and text-to-GUI-to-text/exit19.
r010 management.ps1 passed paired DOS/native frontend loss with receipt1067,
selected actual Console close, independent session input/exit23, unexpected
worker death with surviving target and explicit frontend close. These runs
were serial against the one global endpoint; Z: was removed in finally.

## Whole-package requirement audit

This is the final disposition of the proposal findings, not a claim that
private renames alone unify execution. The earlier source-linked evidence
records the implementation and retained failed attempts.

| Required result | Delivered mechanism and evidence | Deliberate independent boundary |
| --- | --- | --- |
| No second frontend-worker registry | Existing WORKER_WATCH and service_worker_root/service_bind_worker_root are the association authority. Removed native_root/frontend_associated copies; S5 paired binding/loss and r010 real loss/isolation pass. [S5](m0-t428-s5-worker-association.md). | Original task records and authenticated Console-context origin are not duplicate relationship registries. |
| Thin launcher parent restoration | run16 resume sends the service request without Console member census/worker selection. Authenticated origin plus exact completion selects the parent; premature/foreign/duplicate requests denied. [S3](m0-t428-s3-authenticated-parent-resume.md), retained nested Console/Window product gates. | Original DOS GetNextVDMCommand/block/resume remains unchanged; native reacquisition uses its existing acknowledgment. |
| Common route request/attach/rundown | One service route path; cancellation retention depends on pending/delivered/closed transport phase, not kind. Paired route-cancellation and actual prepared-native-root-loss cases pass in service29. [S5](m0-t428-s5-worker-association.md). | Original DOS/native command cancellation still belongs to its actual record owner. |
| Worker reference, classification and capability proof | Admitted native reference uses RetainCommandWorker; selection retains explicit DOS/WOW/native admission adapters and authentication. Shared production fixture covers wrong generation, event alias/signaled/duplicate registration, pending/closed grants and stale roots. | Original fVDM compatibility and native reservation/target proof cannot be replaced with a permissive generic worker test. |
| Residency and retirement | Removed native GUI-only ten-second timer; shared GUI carriers reside/reuse like shared WOW. Native self-created exclusive text final-empty retirement is tested; borrowed/nested requests cannot retire the root. [S2](m0-t428-s2-shared-gui-worker-residency.md), S5 and r009. | Original DosSessionId/CloseOnExit and separate-WOW policy stay in mirrors. Owner confirms no new exclusive GUI option. |
| Management occupancy and projection | Common outer kind initialization/worker enumeration reads source-owned DOS/WOW/Win32 records. r010 checks selected management close and independent roots. | Original records/locks/completion versus native real process/Console facts remain different; no observed task registry. |
| Shared management shutdown | Both workers call worker-base's same shutdown/close mechanism, production-linked37 assertions and real close/loss tests pass. [S4](m0-t428-s4-common-worker-shutdown.md). | NTVDM's original close handler/forced fallback and NTVWM's true Console close acknowledgment are distinct local operations, not duplicated transport. |
| Honest ownership/format names | This S6 inventory, generated source/link checks and negative controls cover all private renames and remove aliases. Frontend source/object ownership is NTCON; shared close body is byte-equivalent after newline normalization. | Text/config/DIB and Console/Window distinctions are message/backend formats, not worker-type policy. |

Review finds no changed src/mvdm, src/opennt-host or common/protocol input.
There is no new helper, registry, scheduler, command replay, lifetime policy,
CLI, DTO or wire revision. Existing 30ms native presentation polling remains.
The unchanged shutdown body is separated only to link the actual mechanism
in fixtures without importing unrelated RPC connection operations.

## Publication and acceptance

r011 publish.ps1 validated all eight r007 hashes, preserved the coherent S5
package in r011/recovery and published r002 to O:/winnt/system32. Guest media,
NTVDM.REG and configuration were untouched. r012 published-smoke.ps1 passed
empty/native-zero/MEM/EDIT in both Console and Window, plus all eight deployed
hash checks. Logs are O:/winnt/Logs2/t428-s6-final-console/window. Source review
confirms mechanical-only production renames and an unchanged close body;
fixture corrections retain actual ownership/ordering/handle assertions.
Governance, relative links, ownership negative controls and diff checks precede
the reviewed Git delivery recorded in CURRENT.

Physical RDP/focus is owner-waived/unobserved, not passed. WOW comparisons retain
the established frontier contract, not a new full-usability/gameplay claim.
No host scrollback or descendant observation capability is claimed. T428 stays
open for owner acceptance. Unrelated proposal/queue work remains excluded.
