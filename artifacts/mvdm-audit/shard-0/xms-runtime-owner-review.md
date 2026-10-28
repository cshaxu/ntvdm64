# XMS initialization, A20 and UMB source owners

The [observer](xms-runtime-owner-observer.py) compares these four sources
against both original trees and binds ten representative functions from the
actual XMS objects to the released NTVDM code. It verifies the captured cached
and released code sections match; full images and non-code sections are not
claimed identical. The [snapshot](xms-runtime-owner-observations.json) records
object, map, release and source identities.

`xms.c` initializes the shared allocator; `xmsa20.c` owns XMS A20 requests
and uses the selected SAS wrapping functions; `xmsmisc.c` supplies memory
query, system page size and interrupt-hook notifications; `xmsumb.c` retains
the original UMB list and guest A20-state pointer. The original dispatcher
links the corresponding service entries. Each file is partial: representative
bindings do not prove every branch or actual guest invocation.

The [XMS service audit](../shard-3/xms-service-role-review.md) and
[allocator failure audit](../shard-2/suballoc-xms-callback-failure-review.md)
retain initialization, zero-length, exhausted-pool and callback boundaries.
In particular, `xmsInitUMB` retains a guest A20-state address for later
writes and `ReserveUMB` precedes node allocation; pointer retirement and
allocation-failure behavior require real owner and teardown evidence, not
an assumption of a product fault. Test guest A20/UMB operations, repeated
initialization, failures and teardown under the current image in S12. No
guest, product or build file was modified or executed by this source binding.
