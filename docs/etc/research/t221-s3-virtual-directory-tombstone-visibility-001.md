# T221 S3 P33: Virtual directory tombstone visibility

The Virtual private-root view now applies directory tombstones before a node
query or child enumeration. A tombstoned private directory hides its complete
private subtree, so later namespace mutation, wildcard and search users see
one consistent namespace. No host namespace is consulted.

The same view now supplies private `directory_empty`: it uses the identical
node and direct-child rules, so a later rmdir/mutation backend will not invent
a separate directory interpretation.

The existing Virtual probe was rerun under MSVC x64 `/MT` in
`build/M0-T221-S3/106`; both root/token fixtures passed with `hostIo=false`.
Its root fixture now proves that tombstoning `BIN` makes `BIN\TEST.COM`
absent and that a private `EMPTY` directory is empty. This remains
provider-foundation evidence, not a BOP-route claim.
