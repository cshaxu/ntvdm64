# Released DLC event and post owners

The [observer](redir-dlc-post-image-observer.py) verifies that
`vrdlcpst.c` matches both original editions and binds all 11 mapped original
functions to the released VDMREDIR code, masking COFF relocation fields. The
[snapshot](redir-dlc-post-image-observations.json) records source, object,
map and release identities. The code includes original DLC event handling,
receive processing and hardware interrupt publication.

The prior [DLC completion-event review](../shard-1/vdmredir-dlc-image-recheck.md)
retains a distinct conditional initialization failure candidate. Neither
static binding proves that a DLC provider is present or a guest reaches it.
Verify actual async completion, interrupt ordering, provider failures and
thread/handle teardown before claiming that DLC behavior is restored. This
review did not run a guest or alter any product or guest image.
