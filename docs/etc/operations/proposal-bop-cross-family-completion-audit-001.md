# BOP Cross-Family Completion Audit And Integrated Verification Proposal

## Purpose

This candidate is admitted only after the eight preceding complete owner-package
candidates close: DEM (`50:xx`), COMMAND (`54:xx`), XMS (`52:xx`), DPMI
(`53:xx`), Redirector (`57:xx`), WOW16, debugger/VDD/top-level, and the
BOP-reachable bx machine/BIOS selector family.  It is a closure audit and
integration task, not a ninth implementation family.

## Required inputs

- The authoritative full BOP inventory and one package-level source/ABI/failure
  ledger from each preceding task.
- The bound route and provider disposition for every retained identity.
- Complete Direct and Readonly package regressions, including explicit
  unavailable/deferred outcomes.
- Declared native integration profiles and the resulting trace evidence.

## Required outcome

The task must reconcile catalogue cardinality, selector/service ownership,
provider route, mutation-profile disposition and whole-package regression for
the complete declared BOP population.  It must then run the approved
cross-family integration matrix and classify each observed gap by original
owner.  A gap becomes a later owner-package repair only through a new admitted
T; neither a trace hit nor this audit authorizes a leaf handler patch.

## Non-goals

This task does not reopen an already closed package merely because a trace
reaches a service.  It does not claim that historical optional host
compositions, broad guest execution or Overlay behavior exist.  Overlay
implementation remains the separately ordered tenth candidate.
