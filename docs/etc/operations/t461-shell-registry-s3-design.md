# T461 S3 — durable Registry overlay design

## Objective

Extend the sole parent-owned `NTVDM.REG` overlay from S2's worker-local mutable
view to a durable, atomically replaced product file. Retain the selected
original WOW caller bodies and the S2 token binding unchanged.

## Ownership

`ntvdm_shadow_registry.c` remains the one owner of baseline parsing, mutable
keys, key tombstones and serialization. Each WOW32 binding remains only a
worker-local opaque-token table. A product-wide named mutation mutex guards
read-modify-write commit; it is not a second Registry provider. Windows
Registry stays read-only and only the S2 admitted roots remain observable.

## Commit contract

On a successful mutating original Registry call, the parent first acquires one
product-wide named commit mutex and reloads the current `NTVDM.REG` footer.
It then applies the original call to that one parent-owned view, writes the
overlay as a private comment footer beside the unchanged ordinary `.reg`
body, flushes a temporary sibling and atomically replaces `NTVDM.REG`. The
footer carries mutable key/value records plus explicit key tombstones; it is
parsed only by this façade and is not a second Registry provider. A failed
write restores the pre-call in-memory overlay and leaves the accepted file
unchanged. The selected original callers expose recursive `RegDeleteKey`, not
`RegDeleteValue`; the internal value-tombstone representation is retained only
as a future-safe data shape, not claimed as a newly exposed API. S3 must not
change original caller error conversion, token retirement or host Registry
admission.

## Verification

Use a self-owned `NTVDM.REG` fixture to prove create/set/delete, restart,
failed write rollback, no partial accepted file and two-worker serialization. Re-run
the formal x86 graphs and coherent product gate after the complete connection.
No guest media is modified or rebuilt.
