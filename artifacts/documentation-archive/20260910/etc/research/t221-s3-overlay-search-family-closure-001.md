# T221 S3 Overlay search family closure

The `demsrch.c` search owner package is closed for its admitted Overlay view.

The independent source-built MSVC x64 `/MT` fixture at
`build/M0-T221-S3/097` runs real checked transactions for pathname
`50:09 -> 50:0B` and FCB `50:0A -> 50:0C`. It supplies a host directory with
`ALPHA.TXT` and `ZETA.TXT`, then applies an Overlay tombstone for `ALPHA.TXT`
and a private `BETA.TXT` COW entry. Both encodings return `BETA` first and
`ZETA` next, proving Overlay visibility, continuation and host isolation.

The fixture exposed that merged enumeration needed a final DOS-name sort after
private entries were added. The shared enumeration now sorts its local entries,
matching the stable ordering expected by the search session.

Direct remains covered by the same fixture; Readonly remains read-only search;
Virtual remains explicitly unavailable pending a virtual namespace package.
No native trace was run.
