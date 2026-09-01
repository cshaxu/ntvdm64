# M0 T335 S3 P1 — WOW scoped-frame lease

## Delivered boundary

`adapter-mvdm-host-out/wow/wow_callback_frame_lease.{h,c}` is the sole new
WOW binding in this packet.  It adapts only the original temporary
`GETFRAMEPTR` / `FLUSHVDMPTR` / `FREEVDMPTR` lifetime:

- acquire a bounded read or write view by numeric guest address;
- commit only a write view;
- clear the view after every release.

It obtains the current session through the existing thread binding and uses
only that session's guest-memory mapping-manager instance. It does not store a
native pointer, create a task, set SS:SP, call `host_simulate`, schedule a
callback, route a BOP or select a WOW provider.

## Verification

Fresh disposable Ninja graphs and the focused fixture completed successfully:

- `build/M0-T335/S3/wow-callback-frame-x86`: 6 compile/link actions, fixture
  exit status 0;
- `build/M0-T335/S3/wow-callback-frame-x64`: 6 compile/link actions, fixture
  exit status 0.

The fixture proves all three source-required lease directions: an acquired
read view cannot commit, a written callback-frame view commits exactly once,
and an uncommitted write view does not change guest memory.

## Limit

This is not an enabled `CallBack16` route. The still-active S3 work is to
connect this scope at the original `wcall16.c` acquisition/flush/release sites
and prove the original CPU40 recursive return condition. `FASTBOPPING`,
WOWEXEC/BaseSrv broker work, GUI/USER/GDI and guest Win16 service execution
remain outside this P1.
