# Proposal: No-External-Dependency OpenNT BOP Closure

## Purpose

Close the first dependency layer of the live BOP tracker in one T: every
eligible original OpenNT BOP body, explicit original no-op/failure, small
source-shaped helper, and BOP/dependency lifecycle chain whose prerequisites
are local and already available. The objective is complete source/ABI closure
for the whole no-external-dependency slice, not a trace-selected collection of
services.

## Eligibility gate

An item may enter only when its `BOP-…` row and every referenced
`BOP-DEPENDENCY-…` row prove all of the following:

- no new bx-core or bx-mantle machine capability is needed;
- no NT4 private, removed, or x64-incompatible API needs a compatibility
  decision;
- no unresolved Redirector, WOW16, DPMI, VDD/debugger, guest EXEC/PSP, or
  external cross-owner lifecycle contract is required; internal BOP/session
  dependencies are eligible when every node belongs to the same frozen slice;
- its original failure/no-op behavior may be retained when that is the
  historical contract.

Existing implementation is re-audited rather than assumed complete. A source
mirror with one blocked branch stays in the tracker but is not counted as
phase completion unless the blocked branch is explicitly outside the declared
leaf profile and preserves its original failure rule.

## Admission plan

1. Freeze the eligible tracker-ID set, including its internal dependency DAG,
   and record the source/ABI/failure map for every node.
2. Recover each shared original owner file as one group, using original source
   and a named shim only for already-supported public host ABI differences.
3. Close each producer/consumer chain inside the frozen slice, preserving
   original layout, order, continuation and failure behavior.
4. Run whole-slice positive, negative and no-bypass regression; update every
   affected BOP and dependency row before the one bounded integration
   observation.

## Exit rule

Every frozen ID is either source/ABI complete with its declared local
lifecycle, or has a source-proven original failure/no-op. Any newly exposed
machine, legacy API or external cross-owner dependency transfers—not
silently—to candidate 2 through the tracker.
