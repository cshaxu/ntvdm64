# S11 V86 monitor removal: working record

The owner directed removal of the NT hardware-V86 monitor routing introduced in S11, while preserving the software CCPU V86 implementation and the protected-mode monitor. This is a published partial baseline, not S11 closure.

- Restored CCPU INT3, INT and INTO dispatch to the pre-S11 path and removed the added host V86 trap and DPMI V86 reflector. CCPU VM mode, paging, instruction execution, protected-mode monitor, and WOW/DPMI protected-mode handling remain.
- Removed tests asserting the deleted reflector, retaining and running CCPU software-interrupt and protected-mode boundary tests. This is not proof of every V86 semantic.
- Local x86 NTVDM and monitor fixture built. The selected CCPU boundary suite passed 134 cases including V86 software interrupt and the protected-monitor mode boundary. The selected DPMI object fixture passed protected rollback and INT 2A mode controls.
- An exploratory A20 odd-word RAM control (case 145, not in the previous suite) failed: fffff=5a, zero=66, above=a5. No baseline comparison has been established; do not attribute its cause to this change. Full DOS, WOW, Win3.x, physical mouse, and release tests remain outstanding.
- Guest images were not modified or recompiled.

## Historical hardware monitor inventory

A tracked-file and filesystem search found no `src/mvdm/v86/monitor`, `src/mvdm/v86/scaffold`, `emv86.asm`, or `vdmints.c`: the standalone hardware-monitor package was already absent from this repository. `softpc.new/obj.vdm/sources` records its exclusion from the link. Names under `src/mvdm/dos/v86` are DOS guest sources, not the hardware-monitor package and were not touched.

`dpmi32/i386/dpmi386.c` is a mixed source: its kernel-monitor/fast-BOP provider is guarded by `!CPU_40_STYLE`, while its debug-register service is linked. The production map resolves `switch_to_real_mode` to `modesw.obj` and `DpmiGetFastBopEntry` to `dpmimscr.obj`; the mixed source cannot be deleted as a whole. The shared `inc/v86def.h` supplies context types used by host and WOW and is also not a standalone monitor.

The added public virtual-IF helper was removed after restoring its protected-mode operation locally. Final product map contains none of the deleted `host_v86_*`, `DpmiV86SoftwareInt*`, or `host_cpu40_set_virtual_if` symbols.

Win386 mouse probe reached the Win3.1 desktop but could not inject movement: the private cross-process hook failed with error 126. Prior unchanged baseline and standalone cross-process controls recorded the same error. This is a probe failure and is not a passing mouse regression result.

## Final candidate verification and publication

The final candidate passed 134 selected CCPU boundary cases and the selected DPMI object fixture; the full 16-gate product suite passed. Win3.1 standard/enhanced × DOSONLY 0/1 all started, reached the expected desktop/mode and exited; 86 guest binary hashes matched in both trees. Winmine gameplay, SOL gameplay, WRITE lifecycle and four SOL Options rounds passed. These do not validate physical mouse movement. The complete ten-component package was published to both release and installed System32, with all 20 destination hashes checked. Evidence and the mouse-hook failure are retained in [s11-v86-monitor-removal-evidence](s11-v86-monitor-removal-evidence/).
