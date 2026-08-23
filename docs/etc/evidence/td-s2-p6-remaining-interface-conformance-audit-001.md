# Td S2 P6 — Remaining Interface-Conformance Audit

## Scope and method

This audit covers every tracker row not classified by P3--P5: 50 Redirector,
one WOW16, 16 Debugger, nine top-level/machine BOPs, and 53 dependencies.
Each row is classified from its actual source/route owner, not from a selector
number or a passing fixture alone.

## Results

The P6 overlay covers exactly 129 rows.

| Disposition | Count | Interpretation |
| --- | ---: | --- |
| `opennt-shaped-facade` | 9 | Bounded original source/table fragment retains its call form. |
| `registered-exception` | 2 | Generic selector-blind machine mechanics only. |
| `migration-debt` | 21 | An active source-derived/project-shaped seam needs facade migration or a new exception audit. |
| `deferred` | 69 | Missing whole owner composition; no silent success is allowed. |
| `not-applicable-guest` | 28 | Source-built guest code or build provenance, not a host interface. |

The key distinction is Redirector: `rdrsvc.h` and guest protocol fragments are
valuable source evidence, but the historical `VDMREDIR` provider body is not a
direct composition input. Existing lifecycle, mailslot and async-pipe routes
are therefore migration debt, not claims of original-provider parity.

The same rule prevents `bx-core`/`bx-mantle` from becoming DOS owners: NTDOS,
COMMAND, PDB, arena and guest Redirector assembly are explicitly guest-owned.
Machine fragments are classified separately and retain only selector-blind
mechanical exceptions.

## Verification and follow-up

`Test-TdS2P6RemainingAudit.ps1` verifies all 129 remaining rows exactly once.
P7 must now reconcile all 321 tracker rows, reconcile every migration debt and
exception, and leave T257 S5 frozen unless that global closure can positively
admit its required source-shaped frame transaction.
