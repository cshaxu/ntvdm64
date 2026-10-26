# T461 S3 — durable Registry overlay design

## Objective

Extend the sole parent-owned `NTVDM.REG` overlay from S2's worker-local mutable
view to a durable, atomically replaced product file. Retain the selected
original WOW caller bodies and the S2 token binding unchanged.

## Ownership

`ntvdm_shadow_registry.c` remains the one owner of baseline parsing, mutable
keys, value tombstones and serialization. Each WOW32 binding remains only a
worker-local opaque-token table. A product-wide named mutation mutex guards
read-modify-write commit; it is not a second Registry provider. Windows
Registry stays read-only and only the S2 admitted roots remain observable.

## Commit contract

On a successful mutating original Registry call, the parent serializes the
complete effective product overlay to a build/product-owned temporary sibling,
flushes it, and atomically replaces `NTVDM.REG`. A failed write leaves the
previous file and current in-memory view unchanged. Delete records persist as
explicit tombstones so a lower baseline value/key does not reappear after a
restart. The S2 view remains available before persistence is introduced; S3
must not change original caller error conversion, token retirement or host
Registry admission.

## Verification

Use a self-owned `NTVDM.REG` fixture to prove create/set/delete, restart,
failed write rollback, no partial file and two-worker serialization. Re-run
the formal x86 graphs and coherent product gate after the complete connection.
No guest media is modified or rebuilt.
