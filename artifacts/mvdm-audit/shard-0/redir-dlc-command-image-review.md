# Released DLC command and initialization owner

The [observer](redir-dlc-command-image-observer.py) verifies original source
identity, binds 17 selected object bodies to released VDMREDIR code, and
checks the three relevant event-reference relocation values. The first event
is referenced twice and the completion event is a distinct reference. The
[snapshot](redir-dlc-command-image-observations.json) records all identities
and bindings. The earlier [failure analysis](../shard-1/vdmredir-dlc-image-recheck.md)
describes why the second creation's check may inspect the first event.

This proves the specified code and event references are present, not that
the DLC provider exists, that guest traffic reaches initialization, or that a
real event creation failure occurred. Provider availability, failure injection,
interrupts and asynchronous worker teardown remain S17 obligations. No guest
or product execution or modification was performed by this binding check.
