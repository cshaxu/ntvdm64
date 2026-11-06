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

## Startup and bootstrap checkpoint

The [26-row source/evidence mapping](../../../artifacts/mvdm-audit/t478-final/s45-startup-reconciliation.json)
records each module1.2/1.3 identity, current owner, distinct coverage limit and
next action. Twelve rows now have bounded dispositions: ten retain selected
original owners with normal startup evidence; one retains a demonstrated prior
PIF correction; one excludes installation-only host boot-marker writes.
Fourteen rows remain pending, even though their owners are located. This is
not a claim of exhaustive failure testing or complete startup-family closure.

Original TimerInit/CpuEnvInit/nls_init/host_main ordering is retained within the
standalone worker begin/finish envelope. Current map resolves each owner.
CpuEnv uses its original list and a source-local renamed getenv, rather than
interposing on UCRT. Registry access is the selected read-only shadow provider;
S42's registry test must not be described as all CpuEnv value/failure tests.
CMD and DEM use the same selected system root, keeping their different length
conditions. DemInit still sets original error-mode flags before allocation.

First-VDM is the original service-global consuming query. The current service
holds its lock around BaseSrvIsFirstVDM; it is not the startup Console-origin
bit, which remains cmdGetStartInfo's exact DosSessionId boolean. The existing
[T412 lifecycle evidence](m0-t412-s2-original-lifecycle.md) and
[final verification](m0-t412-s10-final-verification.md) identify prior query,
capacity and restart tests. Historical timeout figures there do not override
later lifecycle policy. Current Full demonstrates current normal startup and
retirement, not every historical race schedule.

The original NTIO loader still performs reset, then SetupInstallableVDD, then
LIM/scs and the exact image read/state/CS:IP publication. Current map resolves
InitialiseDosEmulation to original nt_msscs. NTDOS retains original demLoadDos
and its actual-byte streaming loop. S43 actual DOS execution supplies normal
boot evidence; zero/short-read or memory-failure injection is not claimed.
COMSPEC still comes from the guest SVC through its synchronous copy boundary;
original unbounded final copy is not certified for arbitrary long inputs.

### Existing corrections and exclusions must not be undone

[T420 S35](m0-t420-s35-xms-capability-progress.md) records that original
GetPIFConfigFiles freed CONFIG's override when LIM read it, before COMMAND's
second read. The retained DIV-157 lifetime correction keeps that same original
owner, frees replaced selections and does not introduce another parser.
[T420 S37](m0-t420-s37-oemuni-capability-review.md) records the OEM-to-ANSI
consumer handoff and corrected LIM output capacity. Current selected bodies
still contain those changes. Restoring the audited one-shot body would undo
a demonstrated repair. Their old runtime paths are read-only historical
receipts, never new publication destinations.

AddSystemFiles' C:/MSDOS.SYS and IO.SYS placeholder creation is explicitly
excluded by DIV-134 and the non-invasive product boundary. The selected no-op
does not fabricate files or claim their existence. No replacement installer or
virtual marker provider is required. The original first-query call remains.

### Pending subcontracts remain explicit

The fourteen pending rows cover PIF parser variants and caller copies,
CONFIG/AUTOEXEC byte accounting, temporary files, initial/merged environment,
early native initialization, PIF reply boundaries, service capture/consume,
launcher templates, CheckVDM admission, worker publication rollback, launcher
dual environments and first COMMAND frame. Each row has its own next action
in the receipt. Existing original realloc/WORD/Reserved capacity risks are
not silently fixed, falsely marked safe, or elevated to new tolerance tasks.
The generated BaseGetVdmConfigInfo object is sourced from the original
function; its object name alone is not evidence of an autonomous duplicate.
The bound immutable-template provider remains a separately recorded boundary
requiring its existing justification to be reconciled.

After this checkpoint1047 active identities remain pending, including these
fourteen. Current product/release bytes are unchanged; no new Full or local
release is necessary for this evidence-only P. S45 remains active.
