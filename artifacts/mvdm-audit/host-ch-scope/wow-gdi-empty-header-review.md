# WGMAN header physical-file role

`src/mvdm/wow32/wgman.h` consists of a single 245-byte comment and has no
non-comment text, declaration, macro or conditional. It is byte-identical to
both compared OpenNT originals (SHA-256
`191937F2EAE51746A02815DE843A897418DE73B19C99AABED93085A99E1D477C`),
so there is no mirror diff for this file. The frozen reconciliation independently
records `both-identical`; the source's original bytes remain untouched.

`precomp.h` includes the header and the current x86 WOW32 Ninja dependency
snapshot records its exact path in 88 groups. This is an included but inert
physical file, not an absent compiler dependency. Its associated `wgman.c` is
also separately reviewed as an empty original source in
[`wow-gdi-empty-owner-review.md`](../shard-0/wow-gdi-empty-owner-review.md).
Actual GDI declarations, table registrations and providers are separate and
must continue under their own partial and implementation obligations.

The [observer](wow-old-table-declarations-observe.py) and
[receipt](wow-old-table-declarations-evidence.json) verify the three file
hashes, comment stripping and exact dependency groups. Recommend **excluded**
for this comment-only physical file, not GDI capability. The same receipt
also inventories five old table declarations, but their debug/profile and
external-consumer boundaries remain pending; no global exclusion of those
headers is proposed here. No guest, product, build or shared ledger changed.
