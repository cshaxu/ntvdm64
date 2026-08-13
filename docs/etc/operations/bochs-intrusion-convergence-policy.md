# Bochs Intrusion Convergence Policy

## Purpose

This operational policy records the project-specific convergence rule for
external Bochs/OpenNT intrusion entries. It is not an architecture authority;
the target component model is in `docs/design/ARCHITECTURE.md`.

## Rule

The exception register is an admission ledger, not a count of live patches.
It may contain implemented, approved-but-pending, rejected, removed, and
diagnostic-only entries. Live state is established only by comparing each entry
with imported source, generated build rules, and final link maps.

Every retained Bochs-side project intrusion must fit one of these mechanical
classes:

1. fixed-width exception event and CPU/instruction-state copying;
2. checked ordinary guest-RAM read/write;
3. typed CPU result, resume, or controlled stop; or
4. minimum machine embedding lifecycle.

All guest-service identity, routing, provider choice, source-derived behavior,
CLI capability, and failure disposition belong to the adapter composition.
The historical machine-handler route, including selection of any admitted
handler island, belongs to the adapter machine-composition plane. Bochs accepts
only opaque typed mechanical requests and returns typed mechanical results.

## Repair Package

The active T196 package is a convergence **repair**, not an audit-only task.
Its first admitted S is read-only and classifies every exception-register entry
as **retain**, **move to adapter**, **retain default-off**, or **remove**. The
record identifies source presence and current build/link use. That audit is a
repair prerequisite, never T196's completion condition.

Only after that S establishes the complete repair set may subsequent admitted
S tasks make the corresponding changes. The expected sequencing is:

1. record-by-record audit and target-boundary decision (S1/S2);
2. remove or migrate every admitted nonconforming or retired delta (S3);
3. repair and minimize the retained generic mechanics, minimal embedding, and
   build-only surface, retaining only individually justified selector-blind
   contracts (S4);
4. reconcile the complete repaired upstream diff, exception register, and
   generated/current build surface (S5); and
5. rebuild the minimal closure and run focused boundary regressions (S6).

No new Bochs semantic intrusion is admitted while this repair package is open.
The following BOP runtime package may begin only after every one of these
repair stages closes. Individual exception evidence remains in
`docs/etc/research/adapter-external-intrusion-exceptions.md`.
