# M0 T478 — original host recovery closure

## Scope and completion standard

The owner requires maximum useful recovery of original mvdm-host/non-mvdm
code, replacement of duplicate local implementations where composable, and
minimum necessary mirror differences. Original semantics are the ceiling:
no added fault tolerance, guest changes, alternate profiles or new abilities.
The final clarification explicitly rejects exhaustive audit/test coverage as
a completion requirement. Only concrete required missing original functions
or qualified duplicate replacements warrant further implementation.

## Delivered work

The [stage plan](../etc/operations/m0-t478-finding-stage-plan.md) and
[69-finding classification](../etc/evidence/m0-t478-s1-classified-findings.md)
preserve all original issues and subsequent dispositions. S2-S41 are closed:
original generic-thunk assembly, ForceDOS propagation, composable original
network enumeration, OLE conversion and VCDEX were restored; demonstrated
host ownership, buffer, ABI and callback defects were repaired at their owners.
Existing implementations and accepted original limits were retained.

[S43](../etc/evidence/m0-t478-s43-gdi-color-alias.md) fixes the inherited
GDI color alias collision; [S44](../etc/evidence/m0-t478-s44-window-sizing.md)
attributes the width observation to native minimized sizing, requiring no fix.
[S45 P20](../etc/evidence/m0-t478-s45-inventory-reconciliation.md#p20-finite-recovery-candidate-disposition-and-s45-closure)
disposes the remaining concrete inherited candidates and closes S45. It does
not claim every inventory identity or original failure mode was tested.

The original recovery candidates have been implemented, already have an
appropriate owner, are outside the admitted product, or retain an explicitly
accepted unavailable original boundary. No qualified recovery remains in the
existing issue/handoff set. T478 is closed; no next S is admitted.

## Verification and publication

Latest production commit487ce265c (S43) publishes eleven-image package
22e35db2-13bb-419d-ba68-7a169cc939e1. Its
[result receipt](../../artifacts/mvdm-audit/t478-final/brush/s43-results.json)
records Full16/16, focused and real guest checks and publication hashes.
No src or assets/release changes exist between that commit and the closure
baseline41e35db73. S44's release checks and S45's bounded fixtures are evidence
only. Closure rechecks every release image against its manifest and that S43
receipt; it does not rebuild or republish unchanged products.

Final receipt: [closure verification](../../artifacts/mvdm-audit/t478-final/closure-verification.json).
Documentation governance and diff review are required for this local closure
commit. No O:/winnt write or push.

## Retained limitations

- Kernel directory fast-index, private RAP/Browser/DLC or raw printing provider
  boundaries remain explicitly registered; no replacement subsystem is added.
- Original allocation/exhaustion and invalid-input limits remain; optional
  device, physical optical/audio and arbitrary callback failure coverage are
  not certified.
- S15's native-worker/frontend codepage synchronization is separate Console
  ownership debt in [TODO](../states/TODO.md), not claimed repaired here.
- Guest payloads/configurations, standalone tools, CPU30 and excluded profiles
  stay outside scope. Historical incomplete inventory mappings are preserved;
  they are not pending T478 functionality.
