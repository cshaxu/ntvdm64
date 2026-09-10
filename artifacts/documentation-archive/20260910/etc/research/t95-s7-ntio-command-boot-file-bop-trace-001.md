# T95 S7: NTIO COMMAND boot-file BOP source trace 001

Date: 2026-08-11
Status: source-derived, implemented in adapter fixture; not a Bochs guest-run claim

## Question

The post-`50:11` trace reached `C4 C4 54 0C`.  This record determines whether
that boundary indicates a missing CPU/device mechanic or a missing OpenNT host
service, and defines the smallest faithful replacement admitted by S7.

## OpenNT control-flow evidence

| Step | Source evidence | Meaning |
| --- | --- | --- |
| CONFIG path request | `src/opennt/base/mvdm/dos/v86/doskrnl/bios/sysinit1.asm:1150-1165` | `ProcessConfig` loads `DS:DX` with the `config` filename buffer, then invokes `CMDSVC SVC_GETCONFIGSYS`, before `doconf`. |
| Selector identity | `src/opennt/base/mvdm/inc/cmdsvc.h:10-31` | `CMDSVC` is `BOP BOP_CMD; db func`; `SVC_GETCONFIGSYS=12` and `SVC_GETAUTOEXECBAT=13`, hence `C4 C4 54 0C` and `C4 C4 54 0D`. |
| Original host implementation | `src/opennt/base/mvdm/dos/command/cmdconf.c:51-103` | `cmdGetConfigSys` expands CONFIG content into a host temporary file and writes that temporary **OEM pathname** to the guest buffer at `DS:DX`; failure terminates the VDM. |
| Ordinary guest continuation | `src/opennt/base/mvdm/dos/v86/doskrnl/dos/file.asm:190-223` | The later DOS open uses `SVC_DEMOPEN`, so the pathname is deliberately consumed through the normal DOS file path, not by a COMMAND loader shortcut. |
| AUTOEXEC analogue | `src/opennt/base/mvdm/dos/v86/cmd/command/init.asm:1396-1416`, `cmdconf.c:106-151` | COMMAND requests the AUTOEXEC pathname and then uses normal `INT 21h OPEN`; the original host service has the same temporary-file design. |
| Read continuation | `src/opennt/base/mvdm/dos/v86/doskrnl/dos/handle.asm:447-469`, `src/opennt/base/mvdm/dos/dem/demhndl.c:93-209` | A normal file read reaches `SVC_DEMREAD`; the existing S7 `50:12/00/16/02` namespace subset is the intended successor path. |

## Conclusion

`54:0C` is neither evidence for FPU, IRQ13/PIC, firmware, nor a Bochs memory
map.  It is the first missing COMMAND host service on the established
source-level boot path.  Consequently the prior downstream fault symptoms
must not be used to admit any machine feature until this ordinary path has
been exercised.

OpenNT's original observable contract is a bounded DOS pathname copied to the
guest.  Its host-temporary-file implementation is not admissible for the
contained CLI profile: it would introduce ambient host storage and a host path
into guest semantics.  The source-shaped replacement is therefore:

| BOP | Original result | S7 contained result |
| --- | --- | --- |
| `54:0C` `SVC_GETCONFIGSYS` | temporary host path in `DS:DX` (buffer capacity 64) | `C:\CONFIG.SYS\0`, naming the already materialized immutable namespace file |
| `54:0D` `SVC_GETAUTOEXECBAT` | temporary host path in `DS:DX` (buffer capacity 64) | `C:\AUTOEXEC.BAT\0`, naming the already materialized immutable namespace file |

The selected drive letter comes only from the validated v2 profile placement;
the profile is currently C: (`drive_index=2`).  Neither handler reads guest
memory, opens a host path, creates a temporary file, parses CONFIG/AUTOEXEC
content, or changes CPU flags/registers beyond the standard BOP resume.

## Implementation and boundary

`src/bx-ntvdm-adapter/bx_ntvdm_cmd_boot_file_service.c` recognizes only real
mode `C4 C4 54 0C` and `C4 C4 54 0D`, validates the 64-byte `DS:DX` buffer
aperture, and creates one existing opaque multi-write transaction.  The
already registered generic Bochs multi-write consumer remains unchanged: it
receives only write ranges/payload and result/resume data, never the BOP,
COMMAND, path or namespace terms.  This is an adapter service, not a new
Bochs intrusion; no exception-registry row is required.

`bx-ntvdm-adapter-runtime-test` asserts both transactions:

- `54:0C` writes 14 bytes (`C:\CONFIG.SYS\0`) at validated `DS:DX`;
- `54:0D` writes 16 bytes (`C:\AUTOEXEC.BAT\0`) at the same kind of address.

The fixture passed on 2026-08-11.  It proves the adapter transaction only.
An exact fresh Bochs build/trace is still required to prove the generic
consumer commits it and to identify the next reached BOP or machine condition.
