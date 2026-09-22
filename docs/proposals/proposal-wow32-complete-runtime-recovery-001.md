# WOW32 complete runtime recovery

## Objective and admission

Owner-directed queue-head successor to T420, dated 2026-09-21. This candidate
has no numeric T identifier until admission. Recover the complete selected
WOW32 provider and its immutable WOW16 consumers, including real WRITE.EXE
startup, use and exit. Inherit all unfinished WOW32 work and research from
T420 S40--S42 and all former S43--S46 obligations; none becomes a passing
result merely because its task owner changes.

T420 retains debugger, VDD and non-WOW32 aggregate acceptance. Its S42 closes
only as a reviewed research/code handoff with DOS regression, not W1/W2 or
WRITE acceptance. The successor must review retained implementation candidates
in place before extending them; it must not restart the research or treat a
saved source file, a linked symbol or a native fixture as a completed feature.

## Research and implementation inheritance

These records form the complete starting reading set, including their linked
source hashes, original owners, test procedures and superseded hypotheses:

- [S40 research conclusion](../etc/evidence/m0-t420-s40-wow32-research-replan.md).
- [S40 USER profile and experiments](../etc/evidence/m0-t420-s40-wow-user-profile.md):
  pinned immutable USER.EXE, 47 consumer patterns, 26 ordinary-thunk mappings,
  PMODE32 versus thunk paths and the 21-input/20-output registration ABI.
- [W1--W7 detailed checklist](../etc/operations/m0-t420-s40-wow32-closure-checklist.md).
- [S41 worker-domain evidence](../etc/evidence/m0-t420-s41-wow-registration-frontier.md),
  delivered at `801ec6e39`; adoption baseline `158f1f1a6` admits S42.
- [S42 original-owner and binding ledger](../etc/evidence/m0-t420-s42-w1-w2-lifecycle-binding-ledger.md):
  original task/class/window owners, registration slots, code and artifact
  identities, observations and explicit remaining production gaps.
- [S42 handoff and workspace disposition](../etc/evidence/m0-t420-s42-wow32-successor-handoff.md).
- [Earlier matching WOW32 provider](../etc/evidence/m0-t404-s5-wow32-matching-provider-build-001.md),
  [positional startup](../etc/evidence/m0-t404-s4-positional-wow-bootstrap-run-001.md),
  [P15 entry recovery](../etc/evidence/m0-t404-s3-p15-positional-ne-wow-entry-recovery-001.md),
  [hard-error recovery](../etc/evidence/m0-t404-s1-wow-hard-error-recovery-001.md)
  and [original command carrier](../etc/evidence/m0-t416-s2-wow-command-carrier-restoration.md).
  Historical execution depth is comparative evidence, not current acceptance.
- [S37 OEMUNI obligations](../etc/evidence/m0-t420-s37-oemuni-capability-review.md),
  [S38 DPMI32](../etc/evidence/m0-t420-s38-dpmi32-capability-review.md) and
  [S39 DOSX](../etc/evidence/m0-t420-s39-dpmi-guest-capability-review.md).
- [Source policy](../etc/operations/policy/source-policy.md),
  [MVDM divergence register](../../src/mvdm/README.md),
  [OpenNT-host provenance](../../src/opennt-host/README.md), and
  [worker WOW boundary](../../src/ntvdm-exe/wow/README.md).

Research artifacts remain under `build/M0-T420/S40`, `S41`, `S42`; runtime
observations remain under `O:/winnt/logs`. These disposable roots are not the
only record: tracked evidence and test sources carry conclusions, hashes and
reproduction requirements. Rebuild from committed sources before adoption.

## Proven frontier and unresolved production contracts

The local registrar is selected and its 20 outputs are non-null, but some are
explicit failure placeholders. Modern USER32 registration returned C0000022
in the retained probe. The worker uses the S41 guest-visible CCPU/DOSX domain;
native USER pointers cannot substitute for the pinned guest's data layout.
Original USER structures and algorithms remain the source of truth.

The protected-return GDT repair and checked USER build-layout identity have
advanced startup. The current proven failure is USER GetDesktopWindow at
0337:011F: guest TEB pDeskInfo and client delta are zero; desktop+8 therefore
reads DOS IVT bytes 055A0016, followed by an invalid WND dereference. It is a
missing host client-view publication, not evidence of a guest or CCPU bug.
The exact r20 witness and instruction signatures are in the S42 ledger and
`tests/observation/verify-wow-user-profile.ps1`. WOWInitTask is ordinal 011F;
010F is DosWowInit. A 010F trace is not proof of task initialization.

Outstanding work, each assigned below:

- Complete original-layout desktop/WND/handle graph, authoritative live
  ownership, relocation, updates, cache invalidation and retirement. The
  recovered WND=176 and DESKTOPINFO=100 layouts are schema proof only.
  `set_client_desktop` is a tested transport without a production producer.
- Replace the temporary active-guest rejection of identity-only handle
  publication with complete object publication. The rejected implementation
  left a free-list successor in phead while reporting a live typed handle.
  Neither that old success nor the present refusal is functional acceptance.
- Connect task/thread/worker destruction to the original lifecycle owners.
  The cleanup helper's failure/retry fixture passes, but cleanup/dispose have
  no production caller. WOWCleanup also serves module release, so it cannot
  unconditionally destroy a live thread. Inspect worker detach ordering
  before memory release, callback cancellation and per-thread withdrawal.
- Finish real guest callback, scheduler, message, class/window/dialog paths;
  native tests with substituted callbacks do not establish guest closure.
- Finish graphics/resources/fonts/menu publication, clipboard/DDE, remaining
  thunks, hard errors and all registrar placeholders. Reuse already recovered
  bodies and fixtures after current-source rebuild, rather than duplicating them.
- Prove both selected PMODE32 direct-data consumers and ordinary thunk
  consumers; immutable guest compile-time selection cannot be changed by a
  host compiler flag. No unsupported alternate guest profile is implied.

## Proposed sequential S packets

Numbers below are local proposed S positions, activated only after T admission.
Each owns a complete bounded lifecycle and connects every usable production
entry immediately. No completed packet may leave its own wiring for a later S.

| S | Complete scope | Required completion evidence |
| --- | --- | --- |
| S1 | USER client objects and per-thread desktop view | Adopt S41/S42 carriers; complete source-backed desktop/WND/handle representations, relocation, publication/update/retirement, thread cache/TEB ownership and failure rollback. Prove immutable USER bootstrap crosses the recorded GetDesktopWindow fault with real objects, and exercise stale/reused handles and withdrawal. No dummy desktop or parallel USER policy. |
| S2 | W1/W2 task, class, window, message and dialog lifecycle | Finish original InitTask/yield/wait, hung-app registration, CallBack16 enter/return, class registration, create/subclass/send/post/dialog and module/task/thread/worker cleanup as one graph. Bind cleanup before backing release. Prove real guest task and nested callback reachability plus cancellation, failure and repeated cleanup. |
| S3 | W3/W4 graphics, resources, clipboard and DDE | Complete all selected GDI/font/glyph/DIB, bitmap/icon/cursor/menu and clipboard/DDE families; source algorithms, object identities, ownership transfer, malformed inputs, allocation failure, native/guest content checks and repeated release. |
| S4 | W5/W6 services and complete registration | Kernel/DOS/OEM, remaining selected COMM/printing/hooks/sound thunks and hard-error response/termination; every selected manifest entry assigned and implemented or explicitly disposed. Replace residual placeholders, verify all 21 inputs/20 outputs and original BOP51/W32Init initialization/rollback. Own OEM-WOW-DIR and OEM-WOW-DELETE implementation. |
| S5 | W7 complete WOW32 and immutable WOW16 acceptance | Former T420 S45/S46 together: real WRITE startup, editing, save/use, close; original loader/task/callback return; real selected-family workloads and OEM-WOW-DIR/DELETE/WIN16-INTEGRATION; repeated tasks, failure/worker teardown and subsequent DOS usability. No fixture-only selected operation or substitute application closes this packet. Final original-source/diff accounting and owner acceptance report. |

OEM-WOW-DIR includes non-ASCII directory/environment synchronization in both
reached directions and rejected updates without false state. OEM-WOW-DELETE
includes ordinary and retained-file branches, rename/delete rollback, temporary
file cleanup and font-removal fallback, with branch evidence. S5 proves these
through real Win16 task creation, callback return and teardown. OEM-DBG-PATH
stays with T420 S43; any specifically WOW-dependent integration found there
must be recorded here without claiming it passed.

## Mandatory gates and limits

Every implementation S rebuilds formal MSVC x86 `/MT` CCPU40 targets, tests
its actual production owners, and passes all 17 text-gated direct/nested
COMMAND/MEM/EDIT routes, including EDIT followed by MEM at the same prompt.
Record exact source revision, artifact hashes, commands, markers, failures
and cleanup. Earlier green runs on different hashes are not final evidence.
Commit/push each reviewed delivery; keep build products under `build/`, test
inputs under `O:/winnt/tests`, and runtime observations under `O:/winnt/logs`.

For every family retain the source-first decision: original owner, exact
source/hash, composability, unavailable dependency, smallest same-shaped
binding, ABI/layout/ordering/failure and retirement condition. Original files
may be imported at original paths; invented mirror files/overlays, recursive
USER/CSRSS server import and parallel provider policy remain prohibited.
Guest media is immutable. Proven original-guest defects are registered with
reproducers and limitations, never silently repaired or counted as passes.

Report `mvdm` and `opennt-host` mirror additions/deletions separately, restore
format-only drift to upstream bytes, and separately measure retained/removed
adapter and other autonomous code. Importing an original file and deleting
a duplicate are different metrics. Completion requires working selected
functionality as well as minimum necessary differences. The owner performs
the final T acceptance; reaching the next startup fault does not close it.
