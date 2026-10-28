# Shard 3: NetBIOS, DLC buffers and DLL lifecycle

Proposed status: **partial** for `vdmredir/vrdll.c`, `vrnetb.c` and
`vrdlcbuf.c`. The [private-map runner](redir-private-map.ps1) relinks the
selected current objects to a build-owned DLL/map and checks that the existing
DLLs are unchanged. The [retained map](redir-private-map.txt),
[observer](redir-image-observer.py) and [observations](redir-image-observations.json)
verify that the privately linked executable sections exactly match released
`VDMREDIR.DLL`, bind the three original files to both source catalogs and
original editions, and compare 1/5/8 mapped object bodies to the released
image while masking relocation words. No behavior or full execution coverage
is inferred from matching code.

After temporary build cleanup, the [durable check](redir-durable-check.py)
rechecks the retained map, released image, source and cached objects, mapped
bodies, table pointer and all eight call targets without the private DLL.

The initial `VrDispatchTable` slot 0x25 resolves to original `VrNetbios5c`.
Eight relocation targets are additionally checked in the released DLL:
the project DLL-entry adapter -> original `VrDllInitialize`; its attach ->
`VrInitialize`, detach -> `VrUninitialize` and `TerminateDlcEmulation`; and
the original DLC `ProcessImmediateCommand` -> buffer allocate, get, free and
delete operations. This establishes selected initial machine-code links, not
successful execution, actual OS NetBIOS availability, or thread safety.

`vrdll.c` retains original process-attach and process-detach decisions. Its
local `ok` is not initialized for thread notifications, yet the project
adapter returns it; the actual effect of a thread notification and teardown
ordering is not verified. `vrnetb.c` owns original guest NCB conversion,
host NetBIOS submission and callback/completion queueing. `vrdlcbuf.c` owns
the emulated DOS buffer-pool calculations and segment/offset conventions;
the external DLC provider remains separate. Unresolved are real pending
cancellation, late callback and guest/worker teardown, protected-mode guest
post, buffer exhaustion and malformed guest ranges, lock ordering, and real
external DLC/NetBIOS behavior. Historical successful NCB operations and a
failed DLC provider probe are not current-package coverage of those cases.

All three source files retain 100% original bytes; behavioral restoration
percentages remain unmeasurable. The private link is an observation artifact,
not a replacement product image; original guest and product files are unchanged.
