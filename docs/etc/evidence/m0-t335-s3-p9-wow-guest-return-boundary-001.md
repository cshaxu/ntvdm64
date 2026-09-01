# M0 T335 S3 P9 — WOW guest-return boundary

## Result

The selected host-side `CallBack16` source body is now bound through its
fixed-frame leases, but its source-defined completion requires an original
Win16 guest trampoline.  It must not be replaced with a host callback stub.

This packet records the resulting boundary before any linked callback fixture
is claimed.  It is a precondition for S3's remaining vertical verification,
not a claim that a WOW callback has executed.

## Original completion path

The original host body in `mvdm-host/wow32/wcall16.c` constructs a
`CBVDMFRAME`, changes the simulated VDM stack and calls the original CCPU40
`host_simulate()` entry.  Its post-call AX:DX extraction expects that guest
execution has completed the callback transaction.

The matching guest owner is
`mvdm-guest/win16/kernel31/wow16cal.asm`:

1. `WOW16Call` enters `BOP_WOW` (`51h`) for the normal WOW16-to-host thunk
   direction.
2. `WOW16_From_CallBack16` is the separate return selection when the saved
   return identifier denotes a host-issued callback.
3. `WOW16WOWCallback16` invokes the target Win16 procedure and the original
   callback completion path restores the frame/stack state expected by
   `CallBack16`.

Thus the non-fast callback is one host/guest transaction.  A C-only fixture
which returns directly from `host_simulate()` would omit the source-defined
guest half and would be false evidence.

## Reached startup boundary

The repository already carries the immutable DOS and Win16 guest media below
`build/output/` and the selected `mvdm-guest` mirror.  Separately,
`app/entry.c` deliberately does not yet select, read and install a Win16 image
into the active SoftPC guest address space or provide its guest lifecycle.
The original BOP `51h` body in
`mvdm-host/softpc.new/host/src/nt_bop.c:MS_bop_1` also retains its original
dynamic `WOW32` provider load (`W32Init`, `W32Dispatch`) rather than a static
host substitute.  No current product route loads `kernel31/krnl386.exe`,
installs the original WOW32 provider, or reaches the guest callback
trampoline.

Therefore the active profile has an exact, source-backed unavailable
boundary at guest-WOW activation; it has not reached a callback return.  This
is narrower than a GUI/WOWEXEC/CSRSS claim and does not authorize a newly
authored loader, synthetic guest bytes, BOP leaf patch, or host callback stub.

## Remaining S3 work

The remaining direct S3 question is whether the existing original owner
sources can establish a declared, immutable-media guest activation route
without broadening the profile.  That route must select and install the
already-carried original Win16 kernel media, retain `MS_bop_1`'s
source-shaped `W32Init`/`W32Dispatch` ordering, and demonstrate the real
`CallBack16 -> CCPU40 ->
WOW16_From_CallBack16` return.

If source/ABI inspection proves that this necessarily requires the separate
WOWEXEC/BaseSrv or GUI product shells, it is an exact successor-owner
transfer, not a reason to invent a callback completion.  GUI/USER/GDI,
FASTBOPPING, CSRSS/BaseSrv transport, kernel VDM and cross-process broker
behavior remain excluded.
