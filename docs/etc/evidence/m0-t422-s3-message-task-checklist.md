# T422 S3 message, callback and task execution checklist

S3 resumes after additional S2 planning delivery 8ed124dbe was pushed.
Its original admission remains valid; S2 is no longer active.
Under the dependency-first revision, dialog-specific consumers are S8 (formerly
S5); object ownership stays S4. Historical S5 references below mean that
original assignment, not the newly assigned module/memory S5.

## Admission and baseline

S2 preservation delivery 532f7fa16 is pushed. Status admits only S3 under
the owner's ordered continuation. The revised proposal and coverage ledger
assign C01--C03, C09 and execution-related C10 here; object lifetimes are S4,
dialog-specific behavior S5. Required dependencies are integrated explicitly,
not left unwired. S2 evidence is reused, never silently promoted to a pass.

Use the [S2 run protocol](../operations/m0-t422-s2-run-evidence-protocol.md)
with new build roots below build/M0-T422/S3 and logs below O:/winnt/logs.
No desktop activation or input while the owner is using the computer.

## Completion checklist

Every production-code P first passes the [execution gate](../../rules/EXECUTION.md#every-p-regression-and-side-test-publication-gate):
capability tests, DOS17 and predecessor-depth non-regression, then verified
publication of dtmgr/run16/basesrv/ntvdm/WOW32/VDMREDIR and required original
media/configuration to O:/winnt. Pure documentation P commits require only
documentation governance/link/diff checks, not runtime testing. Reconcile
S2 E53 gameplay with retained artifact/profile evidence; do not adopt E92's
known failure as the acceptable frontier. Documentation checks alone are not
a new P delivery or runtime acceptance.

- [ ] Before each production-code P: execute and record all three independent WINMINE/SOL/
  WRITE frontier rows using the [proposal matrix](../../proposals/proposal-wow32-complete-runtime-recovery-001.md#mandatory-three-application-frontier-matrix-for-every-p).
  Compare against the previous P and historical best verified behavior;
  retain exact errors/waits, configuration and six-component identities.
  Known failures stay open; new regressions or unverified comparisons block
  delivery. No new runtime result is asserted by adding this checklist item.

- [ ] C01: native normal/early/duplicate reply, same/cross-task nested sends,
  foreign peers and Win16/Win32 directions; single USER transport and no
  stranded or concurrent WOW execution owner.
- [ ] C02: all selected blocking call/return and Get/Peek/Wait/modal paths;
  native-only targets, cancellation/timeout where supported, receiver/sender
  loss; restore data protection separately from execution ownership.
- [ ] C03: actual CallBack16 frame/lease and recursive CCPU entry/return;
  selected WND/dialog/hook/enumeration gateways, exception/cancel/destruction
  and task-frame restoration. Specific object/dialog semantics stay with S4/S5.
- [ ] C09: original task initialization/register/yield/wait/hung registration,
  multiple tasks and failure recovery; no replacement scheduler.
- [ ] C10 execution slice: pending callback/wait cancellation and task/thread/
  worker-loss cleanup exactly once; do not retire another live task on module
  cleanup. Resource owners prove their own release ordering in their S.
- [ ] C11 local gate: enumerate selected original callers, production binding
  and registration edges, justify retained diff and remove obsolete autonomous
  policy; evidence every selected edge, not just the observed crash.
- [ ] C12 local gate: final x86 /MT build, real guest family matrix, DOS17,
  exact artifacts/config/logs, governance/review, commit/push and clean tree.

Only check complete contracts with linked run/evidence identities and expected/
actual results. Existing native and mock tests retain their evidence level.

## First source checkpoint

Read current wow32-dll/source/wow_user_task_lifecycle.c and the existing
wow_user_task_lifecycle_fixture.c after the S2 push. No code changed yet.

- Native-call begin deschedules a bound cross-task sender and retains a
  stack outer-call message/previous psmsSent; unknown/same-task targets follow
  a data-lock-only branch. These branches need one coherent native-boundary
  audit, not speculative fixes based on target names.
- Native-call end resolves the receiver and calls DirectedScheduleTask then
  sender-side xxxSleepTask; callback leave removes a resumed receiver only
  on callback unwind. E92 already shows native reply returned while the sender
  remains blocked at the execution gate. Review the receiver-side handoff
  contract before implementing a same-shaped ReplyMessage binding.
- The current private access map has no ReplyMessage wrapper. Existing native
  fixture calls real ReplyMessage; E93 tests original order with mocks. Both
  are useful controls, neither is real guest complete acceptance.

Next work is the bounded original taskman/ReplyMessage ordering and complete
native-call/callback boundary audit against these actual production branches.
Do not introduce local SMS delivery, guessed native private identity, a second
queue or a second CCPU owner. No new test pass, runtime deployment or resolved
early-reply claim is made at admission.
