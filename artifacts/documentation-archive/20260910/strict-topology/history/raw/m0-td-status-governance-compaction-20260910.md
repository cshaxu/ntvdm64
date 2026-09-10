# M0 Td — Status governance compaction

## Status

Closed 2026-09-10. This is standalone documentation governance work; it does
not allocate a numeric T, admit an implementation packet, or change product
source, build, runtime, guest media, or evidence conclusions.

## Owner request

Reduce the oversized `docs/STATUS.md` safely so project sessions can resume,
while retaining every existing status record.

## Scope and non-goals

The bounded change separates the current control document from its accumulated
closed-task ledger. It does not reclassify historical claims, delete evidence,
select a next product package, or alter `QUEUE.md`, rules, design, source, or
build inputs.

## Inputs and preservation

- Pre-compaction `STATUS.md`: 1,132,143 bytes, SHA-256
  `fce40f7d9ea6f57495e806547392840cd20f94a0e7941cef0fc0c86012cb6a0b`.
- Exact retained snapshot:
  [status-ledger-snapshot-20260910.md](../../artifacts/documentation-archive/20260910/etc/evidence/status-ledgers/status-ledger-snapshot-20260910.md),
  verified with the same SHA-256 before the current control document changed.
- Current-status facts: the former leading `Current Work` record states that
  no M/T/S packet is active and that M0 T401 is closed.

## Procedure

1. Create and hash-verify an exact pre-compaction copy.
2. Retain that copy under `docs/etc/evidence/status-ledgers/` as the historical status ledger.
3. Replace `docs/states/CURRENT.md` with a compact no-active-packet record that links
   the latest product closure, the historical ledger, and the authoritative
   queue.
4. Update the document inventory hashes and run documentation governance and
   link/whitespace verification.

## Result and follow-up

The historical ledger remains available for provenance lookup without being
loaded as live status. Future work must admit exactly one bounded packet to
`states/CURRENT.md`; detailed attempts and closed-task chronology belong in evidence
and history records. Candidate work remains in [QUEUE.md](../states/QUEUE.md).
