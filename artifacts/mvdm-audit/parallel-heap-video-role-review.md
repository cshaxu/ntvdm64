# Original WOW heap and C-VID accessors

At base `fbe870930`, independent checks bind original `wheap.c`, `wheap.h` and
`sinit011.c` to the selected release. All are partial and retain 100% original
source text; no complete behavior percentage follows from code identity.

The [heap review](shard-3/wow-heap-role-review.md) includes a bounded
single-threaded allocation/free/growth fixture and selected call edges. Real
cross-task serialization, failure cleanup and explicit unload remain open.
The [SINIT011 observation](shard-0/sinit011-image-observations.json) binds 21
original generated accessor bodies; actual guest use, scratch state and
lifetime remain open. Neither guest nor product images were changed.
