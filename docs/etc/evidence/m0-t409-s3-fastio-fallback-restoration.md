# M0 T409 S3 — original fast-I/O fallback restoration

## Result

The autonomous `demfastio.c` overlay is removed. The original owner,
`base/ntos/vdm/x86/rdwr.c:NTFastDOSIO`, is a kernel trap-frame handler whose
VDM-TIB, IRQL and kernel I/O dependencies are prohibited standalone boundaries.
The selected standalone contract is therefore the original DOS caller's carry
fallback, not a user-mode `ReadFile` reimplementation.

Both fast-I/O slots select static `demFastIoUnavailable`. It sets CF and does
no I/O, guest-memory, AX, file-position or IP mutation. Unchanged guest
`handle.asm` then invokes `SVC_DEMREAD` or `SVC_DEMWRITE`, which owns the
normal operation and position update. `MS_bop_0` continues to consume the
service byte once; the kernel handler's `EIP += 4` cannot be transplanted.

S3 is 14 added and 107 removed lines, net **-93**, including the full 95-line
overlay.

## Four-rung disposition

| Rung | Disposition |
| --- | --- |
| Original source | `NTFastDOSIO` was audited; its DOS caller/fallback is directly selected guest source. |
| Smallest same-shaped seam | One CF-only refusal at the original user-mode dispatch slot. |
| External-code intrusion | Not used. |
| New fast-I/O behavior | Not used; the Win32 seek/read/guest-lease implementation is deleted. |

## Verification

| Check | Result |
| --- | --- |
| `Verify-T409FastIoFallback.ps1` | Pass: both slots use one CF seam; no selected overlay remains; original guest read/write fallback sequences are present. |
| Formal x86 graph | Pass: Node 22.22.1 generation and full link in `build/M0-T409/S3/r003-fastio-fallback`; product size 3,237,376 bytes. |
| Staging | Pass: `O:\ntvdm64\ntvdm32.exe`, SHA-256 `9578eac34b441139bb1ab4e2d37fd843ce805b3f040664bf284779f1d9fd313a`. |
| Runtime reachability | Generic BOP trace records 50:42/43 when reached. Automated MEM times out identically before/after T409, so no fast-I/O hit is claimed. |

This completes D08 under the original CF fallback. Fast write remains on the
same original slow path; no fast-write worker exists.
