# Proposal — Win3.1 bidirectional application launch routing research

## Status

Owner requested both research directions at the end of the T task queue on
2026-11-06. This is an unnumbered candidate, not admission into the active
task. [Queue](../states/QUEUE.md) owns its order; the ongoing guest multitask
bridge remains separate.

## Questions and scope

1. Can a launch initiated by double-click inside guest Windows 3.1 be
   intercepted before guest loading and forwarded to host Run16 through the
   existing NTSRV architecture? Distinguish Program Manager icons, File
   Manager executable launches and application-initiated loading. Investigate
   original loading interfaces and supported interception mechanisms rather
   than assuming a mouse/message hook or DOS EXEC hook covers them all.
2. Can an MS-DOS VM request otherwise dispatched through cmdExec32 to a host
   window program instead ask guest Windows to load that program? Establish
   the earliest suitable routing boundary and how a guest Windows task can
   perform the load in its proper execution context while the calling VM
   waits only as required and other guest tasks continue.

Evaluate a separately authored Win16 launch/cooperation program plus the
existing .386 bridge, explicit proxy shortcuts and any demonstrably supported
transparent interception alternative. These are research candidates, not
approved or proven implementation mechanisms. Explicit proxy launch must not
be reported as transparent interception of arbitrary executable double-clicks.

## Semantic and architecture requirements

- Separate execution location from frontend/presentation location. A host
  text process may still use the originating guest VM as its frontend.
- Assess defaults and per-program choices for DOS-prompt versus guest-shell
  origins and text versus window programs. DOS graphics is not automatically
  a text case. Reject unsupported guest program formats rather than silently
  overriding the selected execution location.
- Preserve explicit target identity, arguments, working directory and
  environment; avoid resolving a different executable through another PATH.
- Define startup acknowledgement, real failure propagation, optional waiting,
  task/VM retirement and recursion prevention. Do not fake a guest instance
  handle for a remotely launched application without an explicit contract.
- NTSRV retains host lifecycle authority. Guest Windows owns guest loading;
  a VxD/BOP callback cannot simply invoke Win16 application APIs in an
  inappropriate execution context. COMSPEC semantics must not be changed
  merely because presentation is inside guest Windows.
- Original guest binaries remain immutable: no rebuilding, on-disk patching
  or runtime hot-patching. New independent addons remain subject to source
  policy and explicit admission. No new transport is presumed necessary.

## Research deliverable and admission boundary

Produce source-backed call chains for both directions, a coverage matrix,
feasible alternatives with tradeoffs, lifecycle/return-value contracts and a
bounded implementation/test plan. Clearly distinguish retail Win3.1 behavior
from OpenNT WOW kernel source and unverified hypotheses. Identify any feature
requiring a policy exception rather than silently weakening existing rules.
Implementation admission follows that review; this queue addition does not
expand the current multitask bridge task or authorize guest modification.
