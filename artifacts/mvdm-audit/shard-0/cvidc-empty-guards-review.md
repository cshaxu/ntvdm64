# Empty original C-VID guard headers

Ten individual headers contain exactly an `#ifndef`, a matching empty
`#define`, and an `#endif`; both original editions and the current mirror have
identical bytes. The [observer](cvidc-empty-guards-observer.py) verifies that
finite syntax and the [receipt](cvidc-empty-guards-observations.json) records
per-file identities. Their individual empty declaration roles can be excluded
from behavioral ownership even if a build were to include them.

This does not exclude the CPU, video, read/write, shift, rotate, string or
translation capabilities named by the headers; those owners remain separately
audited. Headers with actual macros, declarations or unknown contents are not
part of this group. No source, guest or product image was removed or modified.

The separate `inc/APISTRUC.INC` file is zero bytes in both original editions
and the current mirror. Its original makefile names it as a generated input;
the file-only disposition does not exclude the nonempty `apistruc.h` or
any DOS redirector/network functionality. See the independent
[DLC review](../shard-3/dlc-window-role-review.md) for that boundary.
