# Part 2: DOS dispatch and IOCTL source-role reconciliation

This supplements the per-file identities and original references in `dos-tools-dispatch-review.md`; only owned `dos/v86/doskrnl/dos/dispatch.txt` and `msioctl.asm` are classified here. Guest and its binaries remain immutable.

## Actual historical build chain

Original `dos/v86/doskrnl/dos/makefile:177-181,312` builds `ioctl.obj` from `ioctl.asm` and `inc/ioctl.inc`, and includes it in NTDOS. `msdos.lnk:18` also names `ioctl.obj`. This is stronger than a filename guess: the selected recipe does **not** name `msioctl.asm` as that object's source. `ioctl.asm:3-28` documents the NT DOS $IOCTL path and its own original per-function limitations; individual historical comments are not proof of today's runtime status. A different IBM-profile wrapper cannot be equated to selected `ioctl.asm` just because it includes `IOCTL.INC`.

`dispatch.txt` is assembly-form historical dispatch content and its header describes DOS INT21 dispatch and critical-section boundaries; this does not establish the currently packaged binary is assembled from this `.txt`. The currently relevant callable DOS services and host adapter must be preserved independently of either file's historical build role.

| Owned path | Decision and limitation |
| --- | --- |
| `dos/v86/doskrnl/dos/msioctl.asm` | Alternate IBM-flag wrapper around IOCTL.INC; retain historical comparison. Observed NTDOS recipe selects `ioctl.asm`, not this wrapper. Alternate builds and semantics unverified, so not a universal exclusion. |
| `dos/v86/doskrnl/dos/dispatch.txt` | Retain historical source/dispatch evidence; not an established selected NTDOS build input. Historical dispatch text is not a substitute for verifying the actual DOS dispatcher or all original locking semantics. |

## Interface, failure and observations

DOS owns handle/device service and its original request state; protected-mode callers have original DOSX translation before the actual DOS/DEM path. Do not replace the complete IOCTL behavior with a host default or treat the wrapper as a second owner. Translation must preserve caller buffers and returned status on real failure; device state and host resources remain owned by their actual providers. Source and recipe do not demonstrate whether a historical guest binary was built from these exact source files.

Historical `docs/etc/evidence/m0-t420-s38-dpmi32-capability-review.md:251-308,509` describes tested direct/nested guest IOCTL translation, controlled full/short/error transfers, specific media/printer queries and negative paths. Its tests use `tests/observation/dpmi_int21_state.asm`, including controlled mock modes. The record explicitly does not claim physical media success or a new DEM function-68 provider. Those historical results are not rerun in this read-only review and do not establish all real device lifetimes or every subfunction.

S11 should keep protected-mode pointer/length/error return and nested transition coverage; S12 should own guest DOS handle and IOCTL dispatch; S13 owns real device behavior and teardown. S19 should bind prior witnesses to the current exact immutable guest and coherent release; only add specific failing caller-driven fixes. No guest rebuild, alternate wrapper activation or unproven physical-device success is authorized.

Replay: inspect lines noted above, `rg -n -i "ioctl|dispatch" src/mvdm/dos/v86/doskrnl/dos/makefile src/mvdm/dos/v86/doskrnl/dos/msdos.lnk src/mvdm/dos/v86/doskrnl/dos/ioctl.asm`; rerun the part-2 dos-tools-dispatch observer. No runtime tests or modifications to guest, product or shared ledger.
