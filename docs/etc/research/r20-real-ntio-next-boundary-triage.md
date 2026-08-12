# r20 real-NTIO next-boundary triage

## Evidence

The source-built `NTIO.SYS` fixture completed the existing NTDOS write
transaction, then reached these raw, copied boundaries:

| Fault RIP | Bytes | Historical classification | Disposition |
| --- | --- | --- | --- |
| `0x55c` | `C4 C4 50 3B` | `MS_bop_0 -> DemDispatch(0x3b) -> DemIsDebug` | adapter now supplies the non-debug `AL=0` result and resumes at `RIP+4`; do not infer any other DEM service from it |
| `0xb75` | `C4 C4 50 11` | `MS_bop_0 -> DemDispatch(0x11) -> demLoadDos` | known NTDOS-load path; later repeat observed after its one-time transaction was consumed |
| `0xa976` | `C4 C4 50 0A` | `MS_bop_0 -> DemDispatch(0x0a) -> demFindFirstFCB` | host filesystem/search capability missing; it remains a separate holistic capability boundary |

The source classification is from `host/src/nt_bop.c` (`MS_bop_0` reads the
following service byte and calls `DemDispatch`) and the zero-based
`apfnSVC[]` table in `dos/dem/demdisp.c`.

## VGA stop is a protection result, not a device admission

An earlier run later had `CS=A700`, `EIP=0035`, yielding physical fetch address
`0xA7035`. Bochs `BX_MEM_C::getHostMemAddr` deliberately rejects direct reads
in `0xA0000..0xBFFFF` because that aperture is mapped VGA I/O; its CPU
prefetch panic is therefore expected. This is not evidence that adding a VGA
device, a VGA mapping exception, or a Bochs memory patch is required.

The earlier interpretation of `fault_rip` as a physical address was wrong.
For this real-mode Bochs exception record it is the offset/RIP, while the
copied CS selector separately supplies the segment base. The new default-off
accepted-resume trace records the relevant sequence:

| Copied state | Meaning |
| --- | --- |
| `CS=0070:EIP=0475`, `SS:SP=0000:0700`, `C4 C4 50 11` | NTDOS image transaction; the generic write commits `0x6cd2` bytes at `0x8b00` and resumes at `0479`. |
| `CS=010d:EIP=0516`, `C4 C4 12` | source-derived BIOS memory-size BOP, resumed with its checked AX delta. |
| `CS=8dc8:EIP=055c`, `C4 C4 50 3b` | `DemIsDebug`, resumed at `0560` with AL delta. |
| `CS=9346:EIP=7439`, `C4 C4 50 0f` | `SVC_DEMGETDRIVES`; adapter consumes the explicit 26-slot inventory, returns AX and CF=0, and resumes at `743d`. |
| `CS=9346:EIP=7484`, `C4 C4 50 1b` | `SVC_DEMSETDTALOCATION`; adapter performs its one source-derived two-byte RAM read, registers opaque locators, and resumes at `7488`. |
| `CS=9346:EIP=7490`, `C4 C4 50 32` | `SVC_DEMSETHARDERRORINFO`; adapter registers two opaque locators and resumes at `7494`. |
| `CS=9346:EIP=749d`, `C4 C4 54 05` | `SVC_CMDSETINFO`; adapter registers four opaque CMD/DOS locators and resumes at `74a1`. |
| `CS=9346:EIP=7588`, `C4 C4 50 46` | `SVC_DEMGETDPBLIST`; now implemented as an adapter-owned, source-derived opaque multi-write transaction and runtime-verified by the r23 dedicated fixture. |

Thus `0A84` was not a control transfer into the middle of the initial NTIO
image. It was the consequence of allowing the formerly unhandled `50:0f`
boundary to take native #UD delivery. No Bochs device, firmware alteration or
memory exception is admitted by this result.

## r20 reset-baseline correction: negative result

The original startup-plan implementation incorrectly applied a complete
zero-filled CPU image (including an invented `EDX=0303h`) after the NTIO RAM
write.  The historical `InitialiseDosEmulation` source authorizes only
`setCS(0070h)` and `setIP(0000h)` after reset.  The plan was narrowed so its
preflight rejects every GPR, FLAGS and non-CS segment mutation; Bochs now
applies only a generic real-mode CS:RIP entry delta and retains its native
post-reset baseline.  The dedicated HLT plan fixture passed after the change.

The bounded real-NTIO re-run still reached `CS=0000:EIP=0A84`, but the trace
now identifies its preceding unhandled `50:0f` cause. Therefore the prior
full-state overwrite was a boundary defect that required removal, but it was
not the cause of the observed stop. This is negative evidence: it neither
authorizes a Bochs device, a firmware alteration, nor an uncontracted adapter
service.

## Completed DTA gate

`SVC_DEMSETDTALOCATION` (`50:1B`) is closed as an adapter-only locator
registration. Its source registers DS:AX/DX/CX and reads only the low word of
`DOSWOWDATA.lpSftAddr` at DS:(SI+32); no host pointer crosses the seam. The
r20 trace proves its resume at `7488`. `SVC_DEMSETHARDERRORINFO` (`50:32`) is
likewise closed only as a two-locator registration. `demFindFirstFCB` remains separate: its FCB buffer, namespace, error
and iteration contract must not be synthesized from this inventory work.
