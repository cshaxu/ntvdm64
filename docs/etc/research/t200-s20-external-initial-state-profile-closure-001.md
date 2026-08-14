# T200 S20 — external initial-state profile closure

The v8 source-built profile explicitly selects the r18 evidence digest. bx-vdm
maps only that digest to a copied, opaque 53-byte three-range initial-state
record. Unknown digest, absent v7 field, and post-reset access are rejected.
The direct-composition x64 `/MT` closure passed at
`artifacts/build/t200-s20-composition-r4`; it proves selection, copied record,
and reset invalidation. No RAM publication, machine initialization, CPU loop,
or guest execution occurred.
