# Original redirector dispatch table in released image

The [observer](redir-dispatch-release-observer.py) independently compares
`vdmredir/vrdisp.c` against both original source editions, binds the selected
COFF dispatcher to the released VDMREDIR code and verifies all 50 table
entries against object relocations and the retained private link map. The
[snapshot](redir-dispatch-release-observations.json) records each service
index, target and owner. It also checks cached and release executable sections
match, without claiming the whole DLL files are identical.

The mapped entries cover initialization, named pipes, mailslots, NetBIOS, DLC
and network services. Their presence and correct static pointer values do not
prove guest requests can reach them, network providers exist, asynchronous
completion is safe, or failures and teardown work. See the [redirector
lifecycle review](../shard-3/redir-role-review.md) and [DLC failure review](../shard-1/vdmredir-dlc-image-recheck.md).
No product, guest, network or provider was run or modified by this review.
