# WOW and debugger original-owner restoration

## Status and dependency

Owner-approved candidate planning on 2026-09-12: place this package immediately
after [broker closure](proposal-cross-process-broker-closure-001.md) and before
the final [WOW16 workload recovery](proposal-wow16-single-process-lifecycle-recovery-001.md).
This allocates no numeric T and admits no code repair. Reconcile the open
audit/restoration task at admission so each repair has one implementation owner.

Broker-first sequencing establishes task identity, delivery, completion and
reentry before integrated WOW verification; it is not a technical prerequisite
for every function below. If a named row is a source-proven immediate blocker
of broker integration, explicitly transfer that bounded dependency to the active
packet rather than waiting mechanically or silently expanding broker scope.
D27 may be removed earlier under a bounded admitted cleanup with evidence;
otherwise it remains in this package.

## Scope and recovery decisions

The [original-owner audit](../etc/evidence/opennt-replacement-audit-20260912.md#consolidated-discussion-index-confirmed-owner-overlap)
defines seven individually tracked rows, not seven proved runtime failures.

| Point | Current difference and required disposition |
| --- | --- |
| D17 | Autonomous task-owned font list/count and ANSI path classification duplicate original GDI font.c policy. Restore original tracking, Unicode paths and remote-path rules through finite allocation/path/GDI bindings. Verify load counts, task cleanup, non-ASCII paths and failure behavior without attributing upstream-shared behavior solely to the replacement. |
| D18 | GetGlyphOutlineA loses GetGlyphOutlineWow's ignore-rotation compatibility. Prove an original-shaped finite binding preserving font transform and result behavior, including rotated-text metrics/printing cases; importing a wrapper alone is insufficient and full GDI server import is excluded. |
| D19 | Autonomous fixed-slot thread/event scheduler differs from original USER taskman.c policy; linked status does not prove callback reachability. Establish actual registration/call paths first. Remove unused helpers or recover the required original priority/event-count policy through finite bindings, with yield/wait/wake/cleanup tests. Do not grow a replacement scheduler. |
| D20 | Autonomous DIB cursor/icon parsing differs in format coverage, default dimensions, hotspot scaling and shared identity. Verify reachability and recover original clres.c conversion/resource policy while binding modern object creation; do not import private USER object management wholesale. Test dimensions, masks, hotspots, sharing, malformed input and allocation failure. |
| D21 | Autonomous hard-error decoding/default/cancel handling differs from DisplayVDMHardError. Restore original classification, allowed/default/cancel actions and response values. Keep final public Win32 presentation app-owned under current policy; no private desktop/CSR shell or fabricated successful response. |
| D22 | Autonomous dbg_* initialization, dispatch and state replace original MVDM dbg.c and reduce branches. Restore composable original bodies and unconditional state retention, with precise outgoing context/event bindings. Original RaiseException event delivery disproves blanket CSR-only exclusion; it does not prove full debugger closure. Verify ordinary and attached-debugger behavior separately. |
| D27 | Inactive callback-stack helper duplicates the original CallBack16 task-data branch. Recheck current supported graphs and all consumers, then delete exclusive helper/declarations/fixtures if still unused. Keep original wcall16.c stack selection; inactive duplication is not proof of simultaneous execution. |

## Ownership and proposed stages

Non-MVDM USER/GDI originals belong in `opennt-host` at their upstream-relative
paths; MVDM debugger and callback owners remain in `mvdm-host`. Adapters supply
only proved missing mechanics. These are not broker algorithms: guest execution,
WOW scheduling, font/icon conversion and debugger policy must not migrate into
the project broker. No x64 target, CPU semantic patch or complete USER/GDI/CSR
subsystem is admitted. Preserve current native Console usability.

For every row record current selected implementation, original owner, evidenced
reason for divergence, exact restoration/removal, and acceptance. Proposed
sequential stages, not active S allocations:

1. Revalidate callbacks/build selection and remove unused D27 and any proved
   unused D19/D20-exclusive carriers; retain required original paths.
2. Recover D17/D18/D20 resource and GDI policy, proving finite external bindings.
3. Recover needed D19 scheduling and D21 error-response policy with actual
   callback integration, not link-only evidence.
4. Restore D22 debugger ownership; run integrated tests and reconcile all seven
   rows, remaining diffs and workload limitations before closing the package.

## Acceptance and limits

### U06, U07-C and U08-S explicit ownership

U06 belongs here: trace actual registration and calls for D19/D20/D27, not
only compile/link selection. Verify cursor/icon format, masks, dimensions,
hotspots, sharing and malformed/error behavior through reached consumers.
Proved unused exclusive helpers may be removed; an unexercised required
callback remains unverified. The later WOW16 package validates integration,
not missing helper restoration.

U07-C covers callback/task/resource lifetime: task exit during callbacks,
allocation failure, shared-resource release and teardown without stale guest
access or double release. Use the shared U07-M memory contract and U07-T
thread-termination contract rather than duplicate them.

U08-S covers original WOW cooperative scheduling waits, yield/wake ordering,
event counts, task exit and failure behavior. Bind shared native waits through
U08-W from CCPU40 event recovery. An unavailable private USER operation must
have original-owner evidence and explicit disposition, not an empty successful
return or a new scheduler. Each of these subitems needs independent evidence
before closing the candidate; WRITE execution alone does not prove all cases.

Use source identity, selected x86 CCPU40 build/link evidence, positive and
negative boundary tests, and actual reached runtime consumers. Verify resource
cleanup, callback reachability, task return and error/cancel behavior; no empty
success stubs. Unavailable host environments or unresolved mandatory semantics
remain explicit limitations requiring owner disposition, not passing rows.
Report original source added, autonomous code removed and retained adapter/diff
footprint separately. Full WRITE start/run/return remains the subsequent workload
package's acceptance, not implied by broker or helper composition.

Builds stay under `build/`, runtime observations under `O:\ntvdm64\logs\`;
publication follows the executable contract admitted at implementation time.
Run governance, relative-link and diff checks. Candidate documentation is not
runtime repair or authorization to install fonts or attach a debugger now.

### T407 platform-carrier receiver

T407 S6 transfers FastWOW's guest TEB/TD-prefix projection here. First prove
whether the selected WOW provider reaches `fastwow.asm`; then verify original
`WOW32Reserved`, `vpStack`, `vpCBStack` and `FastWowEsp` ordering at first
entry, callback return, task exit and worker reuse. A modern host TEB must
never enter guest memory. If private TEB lifecycle cannot compose, retain only
the documented finite guest projection and prove failure/reset/teardown. See
[T407 S6 evidence](../etc/evidence/m0-t407-s6-platform-carrier-disposition.md).
