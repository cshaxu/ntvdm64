# M0 T227 S1 — BOP semantic code-completion admission

## Objective

Turn the existing 203-entry BOP inventory from route/fixture coverage into
locally executable, owner-package semantic coverage for the declared Direct
and Readonly profiles. Each endpoint must reach an actual bound provider
result: original-provider reuse, the smallest source-preserving seam,
source-derived behavior, or an original-source-proven no-op, unavailable,
deferred, or typed-stop result. The outcome must be verified by the owning
package's local regression, not by a guest trace.

## Why this replaces the prior S1 brief

T226 established the 203-entry inventory and a formal fixture target for each
entry. Its manifest expressly limits that claim to test-inventory coverage,
not provider implementation completeness. The owner has now directed that
the current task complete code and local BOP semantics before a later task
performs multi-program guest integration and cross-component repair.

The earlier T227 guest parent/child/return admission is retained as evidence,
but is deferred back to Queue as the next integration candidate. It changes
no runtime behavior and is not discarded.

## Ordered package work

1. Reconcile all ledger entries against their actual bound source and local
   assertion; generate a semantic-gap list, not merely a fixture mapping.
2. Close DEM's 73 entries as one package, including Direct/Readonly behavior,
   source-proven unavailable results, and family regression.
3. Do the same for COMMAND (17), XMS (12), DPMI (25), Redirector (50), and
   Debugger/top-level-machine/WOW (26), in that order unless a proven shared
   boundary requires the next package.
4. Run the complete local suite and regenerate the semantic ledger. No native
   guest trace is an acceptance input to this task.

## Constraints

- Follow the source-recovery ladder for every repaired or newly bound route.
- Do not invent BOP behavior from a trace hit and do not synthesize DOS, EXEC,
  PSP, child, or host-child behavior.
- Keep selector recognition and OpenNT semantics in `src/bx-vdm`; Bochs stays
  selector-blind and mechanical.
- Direct and Readonly are the only currently implemented modes. Overlay is
  ABI-reserved; Virtual is not selected.

## Acceptance

For every one of the 203 entries, the resulting ledger must name the real
bound implementation/failure behavior and a passing local Direct/Readonly
assertion. A route that only identifies a facade, fixture, or placeholder is
an open semantic gap. The next T may then use the immutable guest sequence
for integrated execution and owner-domain defect repair.