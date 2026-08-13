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

## Audit Gate

Before any new Bochs semantic intrusion, a read-only audit must classify every
exception-register entry as **retain**, **move to adapter**, **retain
default-off**, or **remove**. The record must identify source presence and
current build/link use. The candidate task and its admission evidence are kept
in `docs/QUEUE.md`; individual exception evidence remains in
`docs/etc/research/adapter-external-intrusion-exceptions.md`.
