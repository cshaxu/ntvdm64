# MVDM-host `i386` conditional semantic audit

## Candidate package

**MVDM-host `i386` conditional semantic audit and capability recovery plan.**
This is the first candidate after the active packet closes. It receives a
numeric T identifier only when it is admitted into `STATUS.md`.

## Problem

OpenNT MVDM uses `i386` preprocessor conditionals for more than host-CPU
syntax.  `i386` means the historical 32-bit x86 compilation target; it is not
defined for the x64 target, and it is not evidence that the modern x86 build
has NT4 V86 or MONITOR capability.  Depending on the original package, a
conditional can encode direct 32-bit pointer identity, VDM linear-address
remapping, descriptor handling, monitor callback behavior, CCPU/V86
selection, or an optional WOW/VDD/debug product path. Modern x86 and x64
builds must not discard either branch merely because neither reproduces the
original NT4 VDM product shell unchanged.

## Decision

Every `#if defined(i386)`, `#ifdef i386`, `#ifndef i386`, and equivalent
negated form in selected `mvdm-host` receives one ledger row before a later
owner package relies on it.  The ledger must state the x86 branch and the x64
branch separately: `#ifndef i386` code is an x64 candidate, not a reason to
discard it. `i386` may select only compilation syntax or two implementations
already proven semantically equivalent for the chosen product profile. All
product capability decisions use a named owner and capability; no conditional
may imply the presence or absence of V86, MONITOR, guest remapping, or device
semantics by itself.

The preferred recovery result is one architecture-neutral implementation of
the original observable contract.  The audit must therefore first attempt to
identify a shared x86/x64 path—just as `__debugbreak()` replaced two spellings
of the same `int 3` contract—rather than preserve an historical `i386` split
by default.  A retained architecture conditional is allowed only for a real
calling-convention, object-layout, compiler-syntax, or irreducible platform
mechanism difference.  If recovery needs a new x64 binding or a shared adapter
to make the contract uniform, that registered divergence is preferred over
silently accepting divergent behavior in the original branches.

If a branch relied on native host pointer/handle identity, both x86 and x64
use the session-owned mapping-manager surrogate and the established bounded
lease rule. The x86 build never gets an identity-map shortcut. If a branch
requires guest execution, remapping, interrupt delivery, stack switching or
monitor behavior, its owner is the selected machine/monitor path; it is not
silently made unavailable by a preprocessor symbol.

## Scope and S breakdown

### S1 — physical conditional inventory

Enumerate all selected `mvdm-host` i386 conditions with exact source location,
enclosing symbol, original package, source identity and both branch bodies.
Detect nested and compound predicates. Separate compile-only syntax selection
from any branch that changes observable MVDM behavior.

### S2 — semantic and owner classification

Classify each behavioral condition as one of:

- host pointer/linear-address identity or external-page remapping;
- descriptor/LDT/DPMI capability;
- CCPU, CPU, V86 or selected-machine prerequisite;
- monitor callback, guest stack or re-entry transaction;
- public Win32 host capability;
- WOW, VDD, BDE, debugger or extension-product profile; or
- explicitly non-host-runtime historical material.

For each, record the original observable purpose, the selected x86 behavior,
the selected x64 behavior, source-recovery rung, capability owner and test
obligation. Pointer-bearing forms explicitly identify the relevant session
mapping-manager instance and the no-raw-alias rule.

The ledger also records whether the final recovery is `unified`,
`architecture-specific-required`, or `unresolved`. A
`architecture-specific-required` row names the exact irreducible reason and
the mirror/adapter divergence registration; absence of `i386` on x64 is never
such a reason by itself.

### S3 — recovery and queue integration

Specify the smallest same-shaped source, adapter, overlay or unavailable path
for every ledger row. Update the immediate owner-package proposal for rows
that need recovery. Retain historical source unmodified unless a later
admitted package authorizes a registered divergence.

### S4 — audit closure

Review the complete ledger for unclassified branches, branch asymmetry and
x86-only identity shortcuts. Run documentation governance and `git diff
--check`; no host build, BOP implementation or machine behavior change is
part of this candidate.

## Non-goals

- No preprocessor bulk replacement and no deletion of either original branch.
- No source, adapter, overlay, BOP, device, monitor or selected-backend
  implementation change.
- No reintroduction of kernel VDM, CSRSS, a raw host pointer ABI, or
  `src.old` input.
- No claim that an inactive historical product branch is unsupported until its
  owner/disposition is recorded.

## Exit criteria

The ledger classifies every selected `i386` conditional and its exact branch
meaning. Every behavioral condition has a named owner, an explicit x86 and
x64 disposition, and a `unified`/`architecture-specific-required` recovery
decision; every pointer/handle identity case names the required session
mapping-manager path; and all later affected package proposals have a precise
dependency note. The worktree contains only governance/evidence records.
