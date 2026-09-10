# M0 T310 S8 P4 CMOS reset call-contract closure

## Source result

The selected BIOS `reset.c` calls `cmos_clear_shutdown_byte`, provided by the
selected original NT CMOS implementation `base/system/cmosnt.c`. No selected
public declaration carrier exposes the helper. `reset.c` now publishes its
exact `void(void)` contract at the call boundary; provider body and BIOS reset
ordering are unchanged.

## Validation

After cleaning `obj/bios/reset.obj`, formal Ninja rebuilt
`original-softpc-bios.lib` and `original-softpc-system.lib` on x64 and x86.
Each archive completed with zero errors and no reviewed CMOS C4013 call.
Unrelated existing BIOS/System diagnostics remain visible.
