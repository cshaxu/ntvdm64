# M0 T282 S12 P1 — bounded copy binding evidence

The existing numeric `mvdm_guest_location` facade now supplies two
source-independent operations needed by S9: a caller-bounded C-string read
and an exact host-to-guest byte copy.  Both use only the thread-bound
session's existing `guest_memory` mapping-manager instance and synchronous
leases; neither returns or retains a native guest pointer.

The formal Ninja fixture passed under MSVC `/MT` on both x64 and x86. It
checks terminated string copying, unterminated maximum rejection, exact guest
write commit, and existing invalid-address rejection.

This P1 does not modify a COMMAND mirror file or enable a BOP route.
