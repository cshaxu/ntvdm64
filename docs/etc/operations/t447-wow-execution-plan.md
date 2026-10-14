# T447 WOW32 execution working plan

## Admission and boundaries

The owner accepts/closes T446 and admits the former queue head. Current
owns the sole active S1 brief. This plan inherits W01 from the
[candidate](../../proposals/proposal-wow32-message-task-execution-001.md)
and the [WOW program](../../proposals/proposal-wow32-production-completion-002.md),
without resetting stable capability IDs or declaring inherited work complete.

Modern USER remains the only message/reply transport. Original MVDM thunks
and composable OpenNT task algorithms remain their owners; WOW32 binds
native boundaries to original task execution and guest-memory lifetime.
No second scheduler, local SMS queue, guessed private USER identity, guest
patch or broad USER-server import is authorized.

## Sequence

Owner clarification: find recovery gaps primarily by tracing source, not by
repeatedly running tests to discover the next patch. Trace the original
caller/owner contract through selected production bindings, implementations,
and return/cleanup paths; enumerate missing and divergent edges and their
dependencies before choosing repairs. Tests are acceptance after connecting
new code, not the gap-discovery method. Already-started baseline observations
are retained as auxiliary evidence only and do not dictate implementation.

The owner further requires zero original-source diff where composable, or
the minimum necessary original-source diff. Do not author substitutes for
original logic that can be connected. Each proposed repair must first name
the existing original function, its selected/unselected body, and exact
dependency preventing direct composition; prefer connecting that body through
the smallest same-shaped seam. This applies to teardown as well as execution.
When original logic is proved composable, connect it and delete the equivalent
project-authored replacement in that same repair; do not retain duplicate
behavior or defer the replacement as future debt. Finding an original symbol
alone is not proof of composition: account for its actual state and lifetime
dependencies before declaring that condition satisfied.

| S | Scope | Required result |
| --- | --- | --- |
| S1 | Source-first recovery-gap inventory and dependency design. | Finite C01--C03/C09/execution-C10 original-owner/production-binding/implementation/cleanup matrix; classified missing or divergent edges, dependency order and acceptance cases. Existing E91/E92/E93 and application observations are auxiliary evidence. No production repair. |
| S2 | Message delivery/reply/reentry, beginning with its original callback-frame prerequisite. | Connect original callback address/failure semantics first, then original-owner message handoff and real Win16/Win16 plus both Win16/Win32 directions, including early/duplicate replies and nested execution. Callback prerequisite alone cannot close S2. |
| S3 | Task execution/cancellation/cleanup. | Init/yield/hung registration, waits, task loss and callback frame/lease retirement; no stranded sender or second execution owner. |
| S4 | Package acceptance and stable handoff. | Complete owned matrix and production regressions, coherent release, owner acceptance and explicit handoff to USER/object consumers. |

Only S1 is currently active. Later stages require sequential admission with
exact files, source recovery decision and acceptance cases. Replanning must
preserve all obligations rather than move unfinished implementation to S4.

## S1 inherited inputs and proof levels

- [T422 S3 checklist](../evidence/m0-t422-s3-message-task-checklist.md):
  preserve every unchecked capability, original taskman handoff and cleanup.
- [T422 lifecycle ledger](../evidence/m0-t422-s2-user-client-lifecycle-ledger.md):
  E91 native tests, E92 early-reply failure, E93 mock ordering are distinct.
  Current auxiliary reproduction records exit95 after early native reply
  returns114 but WOW execution remains with the receiver; see S1 evidence.
- [Coverage ledger](wow32-recovery-coverage-ledger.md) and
  [run protocol](m0-t422-s2-run-evidence-protocol.md): reuse existing audited
  source slots and test infrastructure; new outputs use build/M0-T447/S1.
- [T430 handoff](t430-non-wow-contract-plan.md#known-wow-handoff): G03 task,
  dispatch and callback-return proof belongs here. G03/G13 shared view stays
  W02; WRITE's initial provider investigation stays W03 until evidence proves
  a different owner. An OOM message does not establish heap failure.
- [T446 release evidence](../evidence/m0-t446-s1-carrier-visibility.md):
  accepted ca0abb7c4 package, independent prior frontiers and playable WINMINE.

The matrix records caller/provider, actual production wiring, exact test
entrypoint/input identities, normal and negative assertions, reentry,
cleanup, evidence level and remaining gap. Mock/native evidence cannot be
promoted into guest proof. Unavailable or unexplained outcomes remain open.
No runtime checks are claimed by this admission record.

## Phase and delivery constraints

The investigation traces initialization and execution providers in source;
it does not repeatedly launch applications to choose the next repair.
SOL and WRITE must become normally initialized during basic functionality,
and display their usable normal interfaces during basic display; preserved
historical failures are only non-regression, never those phase exits.
An actual dependency on another queued owner is explicitly promoted in a
bounded packet, not implemented as a temporary local substitute.

S1 admission/closure documentation does not require a new runtime release.
Any subsequent production-code P requires original x86 /MT CCPU40 and affected
tests, all DOS17 routes, independent three-app frontiers and accepted package
regressions, thirteen-image publication to both prescribed locations, hashes,
commit/push and clean workspace before presenting it for owner acceptance.
