# T194 S17 reached DEM continuation source audit 001

Date: 2026-08-12  
Packet: M0 T194 S17  
Disposition: source-backed profile-input blocker; no provider, Bochs, or guest
change admitted.

## Question and inputs

S16 reaches `50:3B` and then `50:0F` at `9346:7439` before native pass-through
reaches the repeated `0000:0A84` state. This audit uses only the imported
OpenNT source and current adapter source:

- `src/opennt/base/mvdm/inc/dossvc.h`;
- `src/opennt/base/mvdm/dos/dem/demdisp.c`, `demmisc.c`, and `demgset.c`;
- `src/opennt/base/mvdm/dos/v86/doskrnl/dos/msinit.asm`; and
- `src/bx-ntvdm-adapter/bx_ntvdm_adapter_runtime.c` plus the drive service.

## Source facts

`SVC_DEMGETDRIVES` is `0x0F` in `dossvc.h`. `demdisp.c` dispatches it to
`demGetDrives`; `demgset.c` documents it as the MSINIT request that returns a
logical-drive count in AL/AX with CF clear. The original handler has a
two-drive A/B baseline and counts only contiguous removable/fixed/CD/RAM
drives from C onward. `msinit.asm` immediately stores its AL result in
`numio`.

The immediately preceding `50:3B` is `DemIsDebug`, not a drive or filesystem
operation. `demmisc.c` returns AL=0 for a non-debug session. S16's progress
from `50:3B` to `50:0F` is therefore consistent with the admitted non-debug
result; it is not evidence that `50:3B` is missing.

The current adapter implements `50:0F` only when its installed startup session
has a 26-slot immutable `host_drive_inventory`. At
`bx_ntvdm_adapter_runtime.c:651`, absence of that inventory ends the
host-service dispatch before `bx_ntvdm_dem_drive_service_v1_dispatch` at
lines 680--683. The drive service itself resumes after four BOP bytes, writes
AX and clears CF only when supplied an inventory. This matches the original
contract and the prior source-derived drive-projection record.

S13's exact v6 profile contains no `host_drive_inventory` member. The profile
parser intentionally permits that omission, but startup consequently installs
no inventory. Thus the observed `50:0F` cannot receive its required result;
the following native exception/pass-through state is a downstream consequence
of that unserved BOP. The trace does not establish an independent cause for
`0000:0A84`.

## Disposition

This is neither a Bochs CPU/device defect nor a reason to add a new per-BOP
handler. It is a bounded profile-capability omission at an already admitted
source-derived provider seam. It also does not grant DOS file access or host
filesystem projection: the required value is a finite drive-type inventory.

## Follow-up

T194 S18 must first reconcile that immutable inventory with the owner's CLI
include/exclude drive-policy contract. It must decide and evidence the
snapshot's origin, filtering order and default before creating a successor
v6 input root. No new run is admitted by this conclusion.
