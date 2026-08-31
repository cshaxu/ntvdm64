# Proposal: SoftPC pre-BOP startup continuity

## Question

What is the smallest complete original SoftPC/CPU40 owner cohort required for
the fixed normal runtime container to progress from the verified `NTIO.SYS`
first fetch to its first original `50h:11h` BOP crossing?

## Scope

- Freeze one normal, console-owning container, its selected immutable DOS
  media, firmware/media manifest and timeout before diagnosis.
- Review the complete reached original path from `InitialiseDosEmulation`
  through `c_cpu_simulate`, CPU40 recursive/event dispatch and `MS_bop_0`.
- Treat FDC, BIOS `INT 15h`, PIT/heartbeat, PIC/ICA, timer/thread lifetime and
  any reached SAS/RAM operation as one source-defined pre-BOP cohort.
- Recover only through the source-first ladder: original source, same-shaped
  binding, registered overlay, then newly authored behavior if every earlier
  rung is evidenced unavailable.
- Verify one original `50h:11h → MS_bop_0 → DemDispatch → demLoadDos`
  crossing in the fixed x86 container. Require paired x86/x64 compile/link
  evidence for the selected source closure.

## Exclusions

- No trace-selected DEM, COMMAND, XMS or other BOP-provider implementation.
- No synthetic FDC, PIC, timer, BIOS or interrupt result.
- No guest source/media rewrite, app-owned guest loading, alternate executor,
  Bochs route, kernel-VDM/CSRSS recreation, or network/WOW/VDD/debugger work.
- No claim that NTDOS, COMMAND, `EXEC`, child completion or parent restoration
  has run; those belong to the following minimal DOS execution package.

## Completion record

The admitted S must create an evidence record containing the original source
locations, call/worker/event ordering, selected binding or divergence, fixed
container identity, first crossing frame/register observation, paired build
results, and every still-unavailable successor owner. It must explicitly
distinguish startup-continuity evidence from DOS-program execution evidence.

## Confidence

High. Current fixed-container evidence already proves `NTIO` first fetch but
not the first `50h:11h` crossing, so this is the shortest missing prerequisite
before an executable DOS vertical slice. Its boundaries are original
SoftPC/CPU40 machine prerequisites rather than a new BOP family.

## Completion

T326/S1 has now observed the specified crossing in the unchanged fixed x86
container and paired source graphs link on x86 and x64. The next observed
null-address exception is post-crossing, so this proposal is closed; it does
not authorize a BOP repair. See
[the T326 evidence record](../evidence/m0-t326-s1-fixed-softpc-pre-bop-startup-cut-001.md).
