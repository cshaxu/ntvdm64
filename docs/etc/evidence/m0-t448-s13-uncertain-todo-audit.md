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
| Pre-prompt startup timeouts | Current S12's 17 DOS cases in each mode plus three cold launch controls; preserve an actual timeout as failure. | Current inherited environment reached three timeouts; control passes, exact cause unproved. |
| Common dialog | Original strict CDL2 identity/return probe, 16 rounds on current package. | Passed 16 rounds; historical root cause unknown. |
| Intermittent direct WINMINE/SOL exit | Existing real gameplay witnesses; 3 runs each, compare S12 evidence and preserve exact failing process identity. | 3/3 each pass; short runs cannot rule out long-play crash. |
| SOL reported layout | Check actual desktop/application geometry and captured gameplay; no assumption that the historical report equals the 427x320 environment. | Measured 585x384 client on 1280x720 desktop; original report geometry unavailable. |
| WRITE lifecycle | Inspect existing real frontier and original-owner plan; test only if an existing valid edit/exit witness is available. | Pending; startup is not lifecycle acceptance. |
| 8253 and zero-length VDMREDIR | Trace actual existing call/test evidence and distinguish speculation from a real symptom. | 8253 source difference lacks product symptom; VDMREDIR repaired in S12. |
| COM, suballocator, NTW16S, NTVWM, T271, document migration | Inventory actual prerequisites or accepted capability boundaries; do not classify untestable work as nonreproduced. | COM peer not established; remaining boundaries need disposition before closure. |

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
  nonrecurrence, not an explanation or a causal repair of the original outer81.
* The outer-CMD DOS/native/DOS witness failed 3/3 before its first DOS prompt:
  the observer timed out with Run16, NTSRV, NTCON and NTVDM still present.
  It therefore did not reach the historical missing-first-character boundary.
  Captures retain the actual processes and input sequence. The S11 evidence
  established that inherited large PATH can break this immutable guest before
  its prompt. Three diagnostic controls using a short PATH, without changing
  the package or guest, completed DOS MEM/EXIT -> native VER -> DOS MEM/EXIT
  and outer exit 19. This establishes an environment-sensitive distinction but
  does not directly observe the guest overwrite or prove the identical cause.
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

* The inherited-environment pre-prompt timeout has current failure evidence;
  a changed PATH is an informative control, not a direct capture of the
  overwrite or proof of this run's root cause. Do not close it as an old
  nonreproduction. The large-environment guest limitation remains independently
  established and retained in TODO.
* WRITE's S12 startup witness does not test editing or exit. Preserve the
  owner-approved WOW32 continuation as its proper execution owner; do not
  certify the full lifecycle from startup alone.
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
