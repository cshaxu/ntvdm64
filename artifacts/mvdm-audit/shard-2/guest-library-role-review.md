# Shard 2 guest library role review

The combined pending inventory contains 32 DOS/Win16 guest .lib paths, 12 in shard 2. Exact path/status/shard intersection against combined-initial-status.csv corrects the prior 13-entry list: `softpc.new/base/system/obj/i386/system.lib` is an unrelated COFF archive and was removed. Its basename is not `wow16/lib/system.lib` and prior matches were invalid.

Among 12, 9 have OMF LIBHDR, complete record framing to dictionary and specific original 16-bit production or linked consumer chains (see CSV); remaining 3 stay pending. Each candidate has byte-identical original/current library SHA and byte-identical relevant original/current recipe SHA. The framing does not validate checksums or dictionary semantics. One historical lib16 archive composition is a library input rather than a runtime target by itself. Retail OLE producer is conditional on non-DEBUG; no inference of debug profile or installed-image origin.

Roles concern historical guest-build files, not guest functionality or host-runtime admission; no guest changes or builds. Frozen source→distributed-image lineage remains unproved. CSV records exact paths/lines and recipe hashes. Original mentions without a link command are not treated as proof.
