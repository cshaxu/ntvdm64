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

These observations do not establish all 13 exclusions are correct. S10 must
check each graph hit against its actual archive/link selection, object contents,
conditional compilation and existing per-file rationale; any unresolved or
contradictory case remains open and the ledger must be corrected. CPU, video,
multimedia, ToolHelp and other guest behavior remain in scope. No guest or
product image was modified or run.
