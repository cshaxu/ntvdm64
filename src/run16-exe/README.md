# run16

`main.c` is the public CreateProcess-shaped CLI. It discovers the requested
image, invokes the selected original classifier, starts/connects BaseSrv,
creates and registers a suspended worker, then waits through the original
parent-completion route. `console_probe.c` is its short-lived private Console
membership helper. Neither file owns BaseSrv DOS/WOW policy or worker state.

`console_frontend.c` owns the bounded direct-channel Console operation
dispatcher. Native handles remain local; `product-abi/console_io.h` contains
only copied protocol data. The selected source retains stream, scroll, fill,
cursor and mode operations rather than reconstructing Console output from
periodic screenshots. `console_channel.c` pumps the authenticated direct byte
channel, cancels outstanding I/O before releasing buffers, and watches worker
death. Stream, cells/readback, geometry, modes, cursor, raw read/peek and native
prepend are connected, as are input/output code-page and the three existing
private window geometry/state queries, plus selected original ANSI title calls.
Original guest input conversion and execution remain in ntvdm. Native pointer
queries/setters are routed too; their native query/error and mock setter tests
do not certify physical focus/clipping cleanup. The remaining S2 ownership and
failure checklist is explicit in the active packet; these connected operations
do not alone certify the complete DOS I/O migration.

Protocol 8 adds copied indexed-video BEGIN/DATA/TEXT operations. The channel's
`console_video.c` assembles contiguous chunks and publishes only a complete
frame; monotonically increasing serials reject stale frames after text return.
Channel teardown releases pending and published storage. This receiver owns
no guest painter and currently no Window; worker API binding and later S4
rendering remain separate acceptance obligations. Local addresses never enter
the video descriptor or payload.

`frontend_scope.c` owns the root frontend lease, its notification pump and
direct Console channels. DOS and native CUI roots use the same scope;
descendants resolve an inherited restricted kernel capability through BaseSrv
and submit their original command without creating another frontend. Root
channel ownership is independent of each launcher's task/process completion.
Completed channel entries are retired during pump activity; scope teardown
joins and releases any remaining entries without terminating their workers.

Protocol 9 routes native Console display-mode queries/updates to this owner.
Native failures remain failures; these operations do not implement the later
product display flag or kvm-window fullscreen policy.
Native GUI windows remain native-owned. The internal capability locator is
removed before building the original DOS environment; it is not guest data.
These are S2 bindings under verification, not hidden Console/Window support.
