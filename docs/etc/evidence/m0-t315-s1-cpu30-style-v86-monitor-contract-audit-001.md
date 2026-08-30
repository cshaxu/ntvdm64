# M0 T315 S1 — Original `CPU_30_STYLE` V86 monitor contract audit

## Question

Can the original OpenNT `CPU_30_STYLE` V86 monitor path be added to the
current Win32/x86 SoftPC product with the same source-first, non-invasive and
bounded-run acceptance standard already used for the CCPU40 C interpreter?

## Inputs

- Canonical original source: `O:\repos.external\opennt\base\mvdm\v86\monitor`.
- Cross-check source: `O:\repos.external\opennt-src-2\nt\private\mvdm\v86\monitor`.
- Current selected CCPU40 source/build graph under
  `src/mvdm-host/softpc.new` and `tools/build/New-T313CcpuLifecycleNinja.ps1`.
- Current session mapping-manager, monitor and SoftPC adapter interfaces.

The two original `CPU_30_STYLE` trees agree on the reached build controls and
sources.  No V86 monitor source is currently selected in `src/mvdm-host`; this
S is read-only and does not import or alter it.

## Procedure and observations

The audit read the original `sources` and `i386/sources` controls in both
source trees, enumerated their selected translation units, then compared
their CPU/SAS entry symbols and build predicates against the selected CCPU40
Ninja manifests and current adapter/session boundaries.  The exact
`NtVdmControl` operations and `FastEnterPm` calls are present in both original
copies.  The current product has no selected `v86/monitor` source directory
or V86-monitor build member; its CCPU40 manifests intentionally reject that
input.

## Original profile identity and terminology correction

`v86/monitor/sources` selects `-DCPU_30_STYLE -DMONITOR`.  This is **not a
CCPU30 C-language instruction interpreter**.  `CCPU` means the C CPU used by
`softpc.new/base/ccpu386`; that code exports `c_cpu_simulate()`.  The
`CPU_30_STYLE` monitor itself exports `cpu_simulate()` and delegates real-mode
execution to NT4 kernel VDM through `NtVdmControl(VdmStartExecution, NULL)`.

Its i386 build member list is:

- `i386/monitor.c`
- `i386/fastpm.asm`
- `i386/sas.c`
- `i386/spcstubs.c`
- `i386/thread.c`
- `i386/int.c`

`proflib.c` is an optional profiling member.  This is a separate monitor and
SAS composition, not a switch that can be applied to the selected
`softpc.new/base/ccpu386` CCPU40 translation-unit set.

## Reached contracts and disposition

- **V86 monitor transition — not currently recoverable by a public binding.**
  `i386/monitor.c` calls `NtVdmControl(VdmFeatures, ...)`, `FastEnterPm()` and
  `NtVdmControl(VdmStartExecution, NULL)`.  These describe the NT4 kernel VDM
  monitor state transition, including V86 execution and monitor-owned thread
  state.  Modern public Win32 does not provide an equivalent operation.  The
  current `adapter-mvdm-host-out/monitor` has no such same-shaped provider;
  adding one would require an owner-approved new monitor implementation, not
  an original-source binding.

- **Interrupt delivery — kernel VDM boundary.** `i386/int.c` invokes
  `NtVdmControl(VdmQueueInterrupt, ...)` against a monitor thread.  This is a
  kernel VDM queued-interrupt operation, not the current bounded
  `VdmQueryDir` facade.  A direct adapter cannot reproduce it merely with a
  public event because the original contract includes monitor scheduling and
  V86 resume ordering.

- **SAS address model — source body cannot be selected unchanged.**
  `i386/sas.c` reserves/maps historical low process address ranges and uses
  those addresses as ordinary host pointers.  The project mapping-manager
  rule requires guest identities and pointers to cross the boundary only by a
  bounded session-owned lease.  A same-named facade could preserve a narrow
  API shape through that manager, but the resulting backend is necessarily a
  new adapter/overlay rather than the original direct-pointer body.

- **Fast protected-mode path — unavailable without the monitor contract.**
  `i386/fastpm.asm` implements the `CPU_30_STYLE` monitor's CPU/TEB/V86 stack
  transaction.  It depends on the preceding kernel-monitor state and cannot
  be treated as a standalone CPU loop or substituted by CCPU40.

- **Monitor worker/thread path — unavailable without the monitor contract.**
  `i386/thread.c` and `i386/spcstubs.c` use monitor-thread and VDM context
  mechanics.  A project session worker may later be designed to provide an
  equivalent *product* contract, but it is not a direct build binding for
  this original package.

## CCPU40 C-interpreter comparison

The accepted CCPU40 configuration is the original `softpc.new/base/ccpu386`
execution/SAS package plus its selected original device graph.  It does not
select `v86/monitor`; current formal scripts explicitly forbid `CPU30`,
`MONITOR` and `V86` inputs.  The V86 monitor therefore cannot be evidenced by
merely recompiling the CCPU40 graph with `CPU_30_STYLE` defined, nor may
CCPU40 act as its fallback.

## Result and bounded next step

The source/configuration identity is closed, and every reached V86-monitor
boundary has a named disposition.  The requested **compile-and-run** result
is blocked by the original profile's kernel VDM monitor contract, not by a
missing source file or a routine build switch.

The smallest honest follow-on is an owner decision on one of these mutually
exclusive product directions:

1. permit a new, explicit non-invasive `CPU_30_STYLE` monitor compatibility profile
   with its own source-shaped session/interrupt/SAS acceptance plan; or
2. retain the V86 monitor as a fully mirrored historical source profile but do
   not claim a runnable `CPU_30_STYLE` product under the non-invasive policy;
   or
3. change the product boundary to permit NT kernel VDM integration.

Until that decision, no V86-monitor build manifest, fixture or fallback is
admitted.  This preserves the requested original semantics rather than
producing a compile-only shell that cannot execute the original monitor path.

## Confidence

High for the selected source set and the `NtVdmControl`/direct-pointer
boundaries: both provided original trees agree, and the reached calls are
explicit in the selected translation units.  The effort needed for a possible
new non-invasive monitor profile remains intentionally unestimated pending
owner selection.
