# Original WOW heap, virtual memory and shared spool owners

Three byte-identical original source owners are partial after independent
replay of their released WOW32 object bindings: [message heap](shard-1/wow-message-heap-review.md),
[virtual-memory thunks](shard-1/wow-virtual-memory-review.md) and
[spool/shared carrier](shard-1/wow-spool-carrier-review.md). Actual guest
execution, failure, ownership and teardown remain open in the referenced
reports and [recovery worklist](recovery-worklist.md).

The memory provider map has not been bound to the released NTVDM image;
the spool carrier is also used by deferred window positioning. Original
source retention is 100%, not a numerical estimate of recovered behavior.
No product or guest image changed.
