# T448 S13: uncertain TODO investigation

## Scope and baseline

The owner accepted S12 and admitted this investigation. The candidate is the
published 13-image package at `36e8d41f6`, with immutable guest inputs and
S12's recorded limits. The [transferred inventory](../operations/t448-lifecycle-plan.md#transferred-todo-inventory)
retains historical reports; its original wording does not override the S12
repairs. Do not modify or rebuild the guest.

## Bounded investigation plan

Run serially against a hash-pinned isolated copy of the current package. Each
background run records PID, commands, exit, observations and its own artifacts.
Use the existing test-only budgets, not new production timeouts. The S13
disposable output budget is 2 GiB and two hours per run, with a ten-minute
no-progress investigation threshold.

| Case | Witness and initial finite budget | Disposition before running |
| --- | --- | --- |
| Rapid native interaction | Three runs of the existing strict 12-round interactive launcher; preserve direct return on failure. | Passed 3/3; historical root cause unknown. |
| Outer CMD first input | Three same-Console DOS/native relaunch witnesses, preserving line-by-line output. | 3/3 inherited-environment timeouts before boundary; 3/3 short-PATH controls pass. |
| Two-session management close | Three actual RPC two-session cases; retain numeric errors and participant state. | Passed 3/3; historical root cause unknown. |
| Pre-prompt startup timeouts | Current S12's 17 DOS cases in each mode plus three cold launch controls; preserve an actual timeout as failure. | Current inherited-environment failure directly reproduced the known COMMAND EnvSiz overwrite in a diagnostic host; prior separate failures remain unattributed. |
| Common dialog | Original strict CDL2 identity/return probe, 16 rounds on current package. | Passed 16 rounds; historical root cause unknown. |
| Intermittent direct WINMINE/SOL exit | Existing real gameplay witnesses; 3 runs each, compare S12 evidence and preserve exact failing process identity. | 3/3 each pass; short runs cannot rule out long-play crash. |
| SOL reported layout | Check actual desktop/application geometry and captured gameplay; no assumption that the historical report equals the 427x320 environment. | Measured 585x384 client on 1280x720 desktop; original report geometry unavailable. |
| WRITE lifecycle | Inspect existing real frontier and original-owner plan; test only if an existing valid edit/exit witness is available. | Existing fixture checks four startup samples only; edit/exit stay with queued WOW32 continuation. |
| 8253 and zero-length VDMREDIR | Trace actual existing call/test evidence and distinguish speculation from a real symptom. | 8253 source difference lacks product symptom; VDMREDIR repaired in S12. |
| COM, suballocator, NTW16S, NTVWM, T271, document migration | Inventory actual prerequisites or accepted capability boundaries; do not classify untestable work as nonreproduced. | COM peer not established; other boundaries and future owners are recorded below. |

Owner permits closing a specifically targeted historical issue after valid
representative nonreproduction, preserving its original failure and unknown
cause. An unavailable prerequisite, unsupported fixture or failed setup is not
nonreproduction. Discovered production defects need an admitted repair scope.

## First observations, 2026-10-23 UTC

The isolated runtime is a physical short-path copy of the S12 candidate; the
13 shipped images were compared with `assets/release` before testing. The
guest remains unchanged. A hidden, owned PowerShell run (PID 17780) executes
the witnesses serially; details are temporarily under `build/t448-s13-native-r01`.

* The strict rapid-interactive native witness passed 3/3 runs, 12 launches per
  run, including final output and outer CMD exit 19. This is a bounded
  nonrecurrence, not an explanation or a causal repair of the original outer81;
  under the owner's S13 rule the old one-off leaves active TODO.
* The outer-CMD DOS/native/DOS witness failed 3/3 before its first DOS prompt:
  the observer timed out with Run16, NTSRV, NTCON and NTVDM still present.
  It therefore did not reach the historical missing-first-character boundary.
  Captures retain the actual processes and input sequence. The S11 evidence
  established that inherited large PATH can break this immutable guest before
  its prompt. Three diagnostic controls using a short PATH, without changing
  the package or guest, completed DOS MEM/EXIT -> native VER -> DOS MEM/EXIT
  and outer exit 19. These initial controls establish an environment-sensitive
  distinction but do not directly observe the guest overwrite; the subsequent
  diagnostic reproduction below supplies that observation.
  No product environment filtering or guest patch is authorized.
  These three valid controls also exercised both DOS commands without the
  historical missing initial character; under the owner's S13 rule that old
  one-off may leave the active TODO, with its original failure and unknown
  cause retained in the linked historical record. This does not close the
  separately observed inherited-environment startup failure.
* Three native/DOS rapid-alternation witnesses also timed out after the first
  native output and before the first DOS request completed. They do not
  validate subsequent rounds and must not be counted as passing. A subsequent
  short-PATH diagnostic completed 12 immediate native/DOS pairs and outer
  exit 19; this is an environment comparison, not a product workaround or
  proof of the precise guest-level cause of the earlier timeouts.
* Three two-session management witnesses each closed the selected real
  NTVWM/CMD worker and showed the independent session still accepting input
  and exiting with its expected code 23. The historical one-off RPC failure
  did not recur in these controlled cases; owner-authorized removal from the
  active TODO is not a claim that its original numerical error or cause is
  known. Retain the linked historical failure evidence.
* The authored Win16 common-dialog probe completed 16 create/destroy rounds:
  the last round reported created/closed/hooks 16/16/16, `DestroyWindow` AX=1,
  `IsWindow` AX=0, and no reported errors. Normal and exit cleanup passed.
  The old one-off missing second report did not recur; it may leave active TODO
  by owner instruction, while its original unlocated failure remains recorded.
* Three real WINMINE gameplay witnesses passed flag/counter, reset, cell open,
  window close and launcher completion. They are short scripted interactions,
  not proof that the owner's long-play intermittent WOW/application exit is
  absent or that its cause is known.
  A further opt-in stress witness (owned PID 36896, exit 0) completed 1000
  alternating cell actions with regular F2 resets on the same game window,
  checked worker/window survival throughout, and then closed normally. Its
  `probe.txt` SHA-256 is
  `a9d213b2ae0e1bb60b44bcf3b34b36a8ab29306bc719eaaf2e202fc885892c06`.
  This is still a bounded synthetic interaction, not proof against rare
  longer-duration failures or a basis to remove the owner's retained risk.
* Three SOL gameplay witnesses completed stock draw, Deal, second draw and
  normal close. The first actual client was 585x384 on a 1280x720 desktop with
  1300x740 maximum tracking size. This measured isolated desktop is not proof
  that it matches the geometry of the owner's separate layout report, and
  short scripted play cannot exclude the reported intermittent long-play exit.
* The historical VDMREDIR zero-capacity entry in the transferred inventory is
  superseded by S12 DIV-361, not an outstanding S13 defect. Its focused and
  package evidence is in the S12 ledger; do not import the old unresolved text
  back into TODO.
* In `softpc.new/base/system/timer.c`, the selected `NTVDM` path calls
  `clocksSinceCounterUpdate`, which has a special timer-zero path and otherwise
  uses `initialCount` as its modulus. The
  alternate non-NTVDM `updateCount` switches to `0x10000` after mode-zero's
  first terminal count. This source difference explains the reason for the
  old suspicion; neither source comparison nor a passing unrelated workload
  establishes a user-visible fault on the selected path. Keep it as an
  unverified boundary, not a proven defect or a justified runtime patch.
* The current Windows serial-device map lists only `COM3` (`USBSER000`); this
  read-only inventory alone does not establish a CTS-capable peer. The earlier
  external COM acceptance remains unverified, not a nonreproduction or pass.

The serial run `build/t448-s13-followup.ps1` used owned PID 23516 and
finished with exit 0. Its case results, including the management and game
children, are summarized above; this does not erase the separately failing
inherited-environment run. Scratch evidence is retained while S13 is active.
SHA-256 summaries, for later scratch disposition:

| Scratch artifact | SHA-256 |
| --- | --- |
| `build/t448-s13-native-r01/results.json` | `95730f3bd78f402fdc28bba62785b7ed5b902944d74d61a25c08375b2dd35473` |
| `build/t448-s13-followup-r01/results.json` | `0a391108d4811a9256b3dce4e3d52e7e849897c24cdf641a2c4ac3f0cd62a123` |
| `build/t448-s13-cdl-r01/guest-report.json` | `ca500d13b5fed6b22e43941bbad7d893db7439f981710aedf571de028639b9ae` |
| `build/t448-s13-rapid-control-r01.txt` | `0038ca42f6699952e1eaa7f7ef727593cb5c71547023ef42bcf37cc2082bef1e` |

## Boundaries still requiring an explicit disposition

* The inherited-environment pre-prompt timeout has current failure evidence and
  now has the diagnostic attribution below. This does not identify the causes
  of the two separate older startup timeouts. Current S12 DOS17 in each mode
  and S13's three cold-start controls did not reproduce those original
  observations; under the owner's rule they leave active TODO without a causal
  repair claim. The original guest limitation remains established and retained
  in TODO, not counted as fixed.
* `tests/observation/verify-wow-write-frontier.ps1` checks four retained startup
  snapshots and explicitly does not test editing, saving or normal exit. No
  existing valid edit/exit witness was found by the S13 fixture inventory.
  The first queued WOW32 continuation proposal explicitly owns WRITE's normal
  startup and usable depth. Preserve that work there; do not certify the full
  lifecycle from startup alone.
* Real COM requires an actual CTS-capable peer or separately authorized virtual
  pair. NTW16S's documented glyph, custom-drawing, scrollbar and fixed-layout
  limits remain capability boundaries. NTVWM's 20 ms active output sampling
  remains the already justified exception, not a new S13 repair.
* The original suballocator's partial rollback is a conditional limitation:
  the selected current callbacks do not expose the relevant fallible backing
  operations. Do not call a hypothetical provider failure reproduced or
  erase the condition on future providers.
* Preserve T271 BOP/package evidence reconciliation and historical document
  migration as the owner requested; they are engineering work, not product
  failures disproved by these tests. The two items remain in TODO.
* The owner-reported game exits and SOL layout remain separately unverified
  beyond the bounded observations above; the report's actual desktop geometry
  and an occurrence with product identity and failure diagnostics are absent.
* The two previously duplicated TODO rows for the original COMMAND discarded
  INIT problem were merged into one P1 row with both original evidence chains
  and the S13 trace linked. This is deduplication, not closure of the defect.

## Direct guest-state attribution of the current inherited-environment failure

An isolated host-only diagnostic was built with
`tools/build/Build-T420S35EnvironmentTrace.ps1` from the current x86 worker
object graph. The uninstrumented graph's `ntvdm.exe` SHA-256
`47b2819993712c66d8db9e62409e9843bfca53ffe12abbd83d9a34cec3663f4c`
matches the published and S12 package image. The diagnostic `ntvdm.exe` SHA-256
is `1f40b138caf1edb58fcb71b02e976d4348adc839b4003d9833cae42146ff9a35`;
it wraps the unchanged original environment service solely to read and log
the guest state immediately before and after that service. It was never
published. The diagnostic does not modify guest binaries or add guest-memory
writes, and no product environment was modified.
The candidate and installed `COMMAND.COM` both retain SHA-256
`908a77ac617c2d741f0aa1b73f73973dcf29adc91f092e5bcb02173c8c732c43`.

On the inherited-PATH observer run, the same DOS relaunch witness timed out
before the first prompt. The trace shows the valid initial environment MCB
owned by the COMMAND PSP, with ES=049F and 0177 paragraphs, containing the
original `EnvSiz` at DS:203C. Immediately before the service it was 0177;
after the service it was 3D4D, with unchanged MCB owner and size. The source
and original guest map identify DS:203C as INIT's `EnvSiz`, later reused by
`EndInit`. Thus this diagnostic reproduction directly observes the previously
documented original guest discarded-INIT overlap and overwrite, not merely a
correlation with PATH length.

The same diagnostic host under the short-PATH control completed the entire
DOS/native/DOS sequence and outer exit 19. Its MCB was 00F6 paragraphs and did
not contain DS:203C; the observed value remained 00F6 before and after the
service. This control supports the mechanism but does not establish a general
safe environment length. The direct trace reproduces and attributes the
current diagnostic run; instrumentation cannot retroactively prove that every
earlier timeout had the same cause. Under the standing guest policy, no guest
patch, hot patch or host environment filtering is applied.

The inherited trace SHA-256 is
`12bf1345b4d76c15dcdfc8d0b60f4a138d38c9df12e6e65cd7aff16f69528239`;
the combined inherited/control trace is
`e6f52ad11cf1621e7438bcff4a6a948756eb1a5d7ae6245b0cff25d340a7053b`.

## Owner-directed S14 handoff

The owner subsequently declined the host workaround and directed that the
case remain solely as the known original guest limitation in TODO. The S14
handoff described below is superseded; no S14 implementation is authorized.

The owner requests that S13-reproduced work be packaged for S14. The reproduced
inherited-environment startup failure and its DOS/native relaunch scenarios
form one bounded handoff, with the direct attribution and control limitations
above. [S14's planned scope](../operations/t448-lifecycle-plan.md#s14-s13-reproduced-startup-failure)
requires a policy-compatible host-side assessment without guest changes or
unapproved environment manipulation. The existing original-guest P1 TODO is
the canonical retained limitation, not a second independently observed bug.
The unobserved game crash, SOL layout report and unavailable COM peer are not
claimed as reproduced or transferred under this handoff. S13 remains active
pending its owner acceptance; this plan does not admit S14 as active.
