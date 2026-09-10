# M0 T199 S32: DPMI Package Provider Regression

## Result

`src/bx-vdm/bx_ntvdm_dpmi_package_session_v1.c` provides the one DPMI package
entry. It accepts only common `C4 C4 53 service` ingress, the existing
original-owner provider selection, and a classifier-validated `53:00..18`
record. It neither changes CPU state nor examines DPMI registers, descriptors,
memory, or guest frames.

OpenNT `base/mvdm/dpmi32/dpmi32.c` owns the ordered 25-entry dispatcher. The
source/ABI map establishes that the current profile has no admissible DPMI
handler: protected-mode/LDT/session, interrupts/faults, DOS bridge, DPMI
memory lifetime, and debug state remain component dependencies. Accordingly,
services `00..11` and `13..18` return the one package-deferred disposition.

`53:12` is the sole explicit unavailable result. Its original owner
`DpmiVcdPmSvcCall32` in `vxd.c` depends on historical VCD/VDD serial and
registry composition, which this CLI profile deliberately excludes. The
router returns controlled stop, without fabricating a guest register result.

## Reproducible evidence

`artifacts/build/t199-s32-dpmi-session-r1/t199-s32-dpmi-session.exe` is a
source-built MSVC x64 `/MT` regression. It routes each of the 25 callable
identities through copied ingress, provider selection, the shared DPMI plane,
and the new session. It proves exactly 24 deferred results and one unavailable
controlled stop. It also compiles with `/W4 /WX`.

No OpenNT dispatcher was linked, no native Bochs capability was added, and no
guest trace was run. This closes S32's complete routing/disposition package,
not the protected-mode DPMI component.
