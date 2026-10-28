# Excluded source-role graph and dependency sweep

The accompanying observer scans every currently excluded physical path using
normalized `/src/mvdm/` suffixes against four current Ninja graphs and their
dependency output. Its receipt is a snapshot of the ledger and graph hashes;
candidate substring matches require individual examination and are not
automatically classification errors. It does not cover other build profiles.

At this snapshot 13 of 2,727 excluded paths had a graph or dependency hit.
Twelve are object build rules and one is `ccpupig.h` in worker dependencies.
The latter was explicitly reviewed under the non-PIG preprocessor profile:
its PIG-only comparison declarations emit no tokens in eight checked source
contexts ([review](ccpu-header-macro-review.md)). A build rule does not prove
archive extraction or active code; the [WGMAN review](shard-0/wow-gdi-empty-owner-review.md)
already documents an empty selected object with linker directives.

The 13 hits were then examined in four bounded groups: [three empty host
objects](shard-0/excluded-empty-host-objects-review.md), [four CCPU roles](shard-1/excluded-ccpu-objects-review.md),
[three WOW32 link inputs](shard-2/wow32-excluded-object-recheck.md), and
[three host alternatives](shard-3/host-excluded-graph-role-review.md). These
reviews preserve exclusions only for the specifically inactive or alternative
file roles. Several objects are compiled and archived or linked, and two host
objects contain actual functions; map absence does not prove non-extraction
under `/opt:ref`. The shard records have been corrected accordingly.

These results do not prove feature completion or all profiles: the three host
alternative checks do not bind the observed objects and map to the newest
release image, and some depend on sampled callers. Revalidate those identities
and late or indirect callers during their recovery stages. CPU, video,
multimedia, ToolHelp and other guest behavior remain in scope. No guest or
product image was modified or run.
