# T478 final existing-inventory reconciliation

S42 resumes the unfinished S1 ledger/handoff reconciliation after S41 delivery
12e17a4fb. This is not a new source audit or an admission of new capabilities.

## Fixed inputs and accounting

[inventory-inputs.json](inventory-inputs.json) pins the read-only audit entrypoints
and ledgers. The seven ledger hashes match the S1 2026-11-05 pins exactly.
External Git metadata remains unavailable to the current read-only query; no
repository revision is invented and no external files are changed.

[inventory-crosswalk.csv](inventory-crosswalk.csv) preserves1546 stable identities:
308 DOS16/OpenNT,755 WOW32/OpenNT and483 inactive WOW32/OpenNT-4.5 records.
The inactive records remain historical evidence, not additional product work.
The1063 OpenNT identities are **not1063 bugs or implementation tasks**. Pending
means that this crosswalk has not yet attached an adequate current disposition;
S2–S41 and prior deliveries may already satisfy an identity. Source-site counts,
file counts, audit implementation labels and lexical hits do not prove either
missing product behavior or product completion.

The index generator is tools/audit/Index-T478ExistingInventories.py. It rejects
changed identity sets/contract metadata and preserves reviewed dispositions on
refresh. Only original-contract requirements applicable to the selected product
are binding; audit suggestions for new brokers, leases, generic rollback or
stronger teardown do not override the owner's original-semantics-only limit.

## VDD boundary

SEAM:9bb0ea6f8b8d82105384 combines optional startup registry loading and selected
ISV plugins. S41 supplies current evidence for real BOP loader/dispatch/cleanup
and the explicitly admitted MSCDEX plugin. It does not alone close registry
startup loading or arbitrary plugin APIs. The audit suggests a new versioned
manifest/broker/drain design, which is not an admitted product requirement.
Current nt_msscs.c contains both the historical commented call and a selected
SetupInstallableVDD call; applicability and current owner evidence must be
checked before asserting missing wiring. This row therefore remains partial,
not a newly declared defect and not falsely closed by the optical probe.

## Explicit inherited work to reconcile next

The T473 stage plan explicitly transfers WINMINE black-background and excess
width investigation as a bounded handoff. The S41 Full run labels WINMINE as
startup/frontier evidence, not gameplay or geometry acceptance. Therefore that
run alone does not close this handoff. Search existing repair/delivery evidence
and current ownership before considering any code change. The separate SOL
Options historical handoff and remaining source-owner handoffs likewise require
explicit disposition; they must not disappear behind the69-row finding table.

At this index checkpoint S42 remained active. No product code or release changes were made in this index
checkpoint; the exact11-image S41 release and Full16/16 remain authoritative.

Index checkpoint validation: four build-owned cases pass (fresh identities,
preserved reviewed evidence, changed metadata rejection and missing-identity
rejection); documentation governance and diff checks pass. This P1 does not
close S42 or claim product changes.

## Current bounded reconciliation

[Startup VDD and brush review](startup-vdd-and-brush-review.md) supersedes the
initial VDD pending note: actual current startup loading is verified. The
inherited class-brush collision has a current-source counterexample, while
its exact WINMINE impact and the separate width issue remain to be observed.

## S43 follow-up

[Delivered brush repair](../../../docs/etc/evidence/m0-t478-s43-gdi-color-alias.md)
restores actual gray background with the original converter unchanged, passes
Full16/16 and publishes11 images locally. S42's remaining reconciliation is
explicitly deferred, not completed. The normal-width handoff remains open.
CURRENT.md alone identifies the active packet.
