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
`build/output/` and the selected `mvdm-guest` mirror.  The default portable
stage contains the selected DOS and firmware assets, but it does not yet place
`kernel31/krnl386.exe` beside the product.  That packaging fact is distinct
from the original activation contract: app removes only its own declaration
option and transparently passes original `-f`, `-w`, and `-a <kernel>` tokens
to the original SoftPC entry.  `cmdmisc.c::GetWowKernelCmdLine` remains the
original owner that reads the process command line and builds the guest EXEC
record from `-a`.

The original BOP `51h` body in
`mvdm-host/softpc.new/host/src/nt_bop.c:MS_bop_1` retains its original dynamic
`WOW32` provider load (`W32Init`, `W32Dispatch`) rather than a static host
substitute.  P11 subsequently proves that a console-owning run with the
already-carried retail `krnl386.exe` supplied through that original `-a`
contract reaches original DOS BOP dispatch.  It does not reach BOP 51 or a
callback return, so the callback boundary remains unproven.  This does not
authorize a newly authored loader, synthetic guest bytes, BOP leaf patch, or
host callback stub.

## Remaining S3 work

The remaining direct S3 question is whether the existing original owner
sources can continue from the now-reached DOS execution path through the
declared original `-a` target, retain `MS_bop_1`'s source-shaped
`W32Init`/`W32Dispatch` ordering, and demonstrate the real
`CallBack16 -> CCPU40 -> WOW16_From_CallBack16` return.  A later package
layout task may place the selected immutable Win16 media beside the product;
it must not change the original command/guest lifecycle.

If source/ABI inspection proves that this necessarily requires the separate
WOWEXEC/BaseSrv or GUI product shells, it is an exact successor-owner
transfer, not a reason to invent a callback completion.  GUI/USER/GDI,
FASTBOPPING, CSRSS/BaseSrv transport, kernel VDM and cross-process broker
behavior remain excluded.
