# WOW synchronous Winsock owner

`wow32/wsraw.c` matches both pinned original editions. The [observer](wow-winsock-raw-observer.py)
and [receipt](wow-winsock-raw-observations.json) verify the selected object,
released WOW32 code and all 23 initial Winsock table slots 1–23, including
their object relocations. This proves the initial binding, not that each API
was called or that arbitrary peers and failure states work.

The original owner converts guest addresses and 16-bit socket aliases, calls
the selected WSOCK32 provider, and maps results back; modern Windows owns
native sockets, while task-local Winsock state and the original socket list
own the guest alias relationship. For example, failed alias creation after a
successful native `accept` closes the newly accepted socket. Handle reuse,
cross-task lookup, blocking callbacks and guest-buffer movement need separate
behavior and lifecycle validation across these APIs.

The existing [T467 synchronous witness](../../../docs/etc/evidence/m0-t467-s2-synchronous-winsock-bridge.md)
exercised a Win16 UDP loopback, a blocking callback, cancellation and teardown
on its recorded package. It does not cover all 23 entries, external peers,
short buffers, task races or this current package's entire lifecycle. The
independent asynchronous context owner has separately registered local changes
and must not be described as byte-identical merely because `wsraw.c` is.
Source retention is 100%; a complete behavioral-restoration percentage is not
established. No guest or product image changed in this audit.
