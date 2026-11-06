# T478 S45 existing inventory reconciliation

Status: active. This resumes deferred S1/S42 work; it does not create a new
source audit or reinterpret1546 audit identities as1546 defects.

Inputs are the stable crosswalk, pinned external original audit ledgers,
8f076ff6a and the unchanged S43 eleven-image package. Reconciliation must
retain separate architecture scope, current source/build binding, tested
behavior and coverage limits. Inactive OpenNT-4.5 rows remain comparison-only.
S43 fixes the class-brush collision; S44 explains the width via original
minimized sizing and modern metrics. Neither substitutes for remaining rows.

## Build-profile family

Read the three module1.1 source contracts and their distinct build conditions,
then compare the project GOAL, current generator and generated worker graph.
The audit explicitly labels implementation validation as not performed; that
is an audit coverage statement, not proof that the current product lacks CCPU.
Detailed dispositions and current build witnesses follow below.

## Build-profile dispositions

| Stable identity | Disposition and evidence boundary |
| --- | --- |
| DOS16:fba9809d0d636de17805 | Original386/MONITOR executor profile excluded by the permanent CPU30 retirement and CCPU-only GOAL. No kernel V86 service is imported or deferred. Revisit only after an owner architecture decision; this is not proof of all software CPU services. |
| DOS16:459781a6ded66751cc88 | Original non386 assembled CPU variant is an alternate executor, outside the sole selected CCPU40 profile. No missing application capability is established by this unselected variant. Revisit only if the supported executor profile changes. |
| DOS16:42c2581b0f33782a0100 | Selected original CCPU40 profile: generated base/CCPU flags contain CPU_40_STYLE, CCPU and C_VID;130 original CCPU compilation edges feed original-ccpu386.lib. Actual worker map assigns c_cpu_simulate to original-ccpu386:c_main.obj and host_simulate to original-softpc-host-roots:nt_cpu.obj. This closes profile selection only, not every CPU/memory/device contract. |

The [profile receipt](../../../artifacts/mvdm-audit/t478-final/s45-profile-selection.json)
pins current generator, graph, map, goal and release manifest. CPU_30_STYLE is
not defined anywhere in this graph. A naive MONITOR absence assertion would
be false: BIOS bios.c, keybd_io.c, video.c, printer.c and host nt_lpt.c retain
local MONITOR declarations. Generator comments at the source-profile boundary
explicitly distinguish these source-proven NTIO/video/peripheral selections
from executor selection. CPU40/CCPU remains present; do not erase valid local
conditions or claim all MONITOR branches are excluded.

Existing [S32 export evidence](m0-t478-s32-export-disposition.md) verifies the
selected original declaration/ABI profile and distinguishes metadata i386 from
CPU30 execution. Current S43 Full verifies16 bounded gates including current
PE ordinals, actual DOS Console/Window, WOW and lifecycle paths. S44 again
exercises the unchanged CCPU worker with real WINMINE. These are bounded
execution receipts, not exhaustive opcode/fault acceptance. No new test or
rebuild is justified just to reconfirm an unchanged profile selection.

## Checkpoint and remaining scope

This first mapping resolves three profile identities, not three new bugs.
The stable inventory still has1059 pending active identities, one bounded VDD
row and483 inactive-edition rows. Module1.2 startup and1.3 bootstrap remain
unmapped; their detailed failure claims must not be inherited from ordinary
successful launch alone. Other owner families and T473 handoffs remain S45
work. No production change, release replacement or task closure is claimed.
