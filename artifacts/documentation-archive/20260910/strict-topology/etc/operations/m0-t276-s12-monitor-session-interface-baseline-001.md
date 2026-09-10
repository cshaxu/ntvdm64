# M0 T276 S12 P5: monitor, session and startup-interface baseline

## Question

Which original interfaces are actually carried by the `softpc.new/host/src`
monitor, thread and startup sources, and which owner package must recover each
one without treating the historical `spchost` library as one linkable runtime?

## Inputs and method

The review uses the byte-exact selected MVDM source mirror, the T276/S5
source-declaration boundary ledger, and the T276/S12 P2/P3 per-source
classification. The companion ledger records each logical original call group
with source spans, the historical dependency it reaches, one recovery owner,
its prerequisite package(s), and a non-enabled disposition. It does not infer
reachability from a guest trace or a lexical declaration.

## Observations

- `nt_eoi.c` combines monitor-timed IRQ delay with SoftPC virtual-ICA state.
  Its `NtVdmControl(VdmDelayInterrupt)` operation and its `ica_*` calls have
  distinct owners: `adapter-vdm-monitor`/`session` versus
  `adapter-softpc -> adapter-bochs`.
- `nt_event.c` contains a real original worker/control-flow algorithm, but it
  reaches private NT console wait/alert APIs as well as public console APIs,
  SoftPC SAS/ICA/device calls, VDD callbacks and COMMAND exit behavior. It
  cannot become a standalone console or machine adapter.
- `nt_inthk.c` declares the original hardware, software and fault-hook
  registrations. These are selector-blind CPU callback contracts; they do not
  authorize a BOP-aware change in Bochs.
- `nt_thred.c` retains a CreateThread-shaped wrapper and original CCPU/monitor
  entry/exit ordering. A future session worker may provide stable lifetime and
  context, but must preserve this outward call shape before any source unit is
  enabled.
- `nt_reset.c` is a cross-owner lifecycle source: its original startup and
  terminal paths include console/UI, CCPU/SoftPC, configuration and process
  exit behavior. It is not a direct provider candidate.
- `nt_msscs.c` is a startup composition nexus, not a monitor source. It calls
  COMMAND, DEM, XMS and debugger initialization in sequence; initializes and
  transfers to NTIO; and later carries VDD memory/user hooks. Each downstream
  provider retains its own owner package.

## Decision and limits

All fifteen logical interfaces are now accounted without enabling a source
body. The P2 classification is corrected: there are four monitor/reset/event
sources, one session-worker source, and one cross-owner startup composition
source. A future recovery packet must use the ledger's predecessors; it may not
recover one leaf from `nt_event.c`, `nt_reset.c` or `nt_msscs.c` because a trace
encountered it.

Private NT4 console scheduler/alert operations, system-directory NTIO
discovery, direct host-process termination and VDD provider behavior are not
declared unavailable here; they are recorded as explicit adapter/package review
gates. No replacement ABI, source edit, build edge, guest load or Bochs change
is introduced by this audit.

## Verification

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass `
  -File tools/governance/Verify-T276S12MonitorSessionInterfaces.ps1 `
  -RepositoryRoot O:\repos.hobby\ntvdm64
```

Expected result: `T276 S12 monitor/session interface verification passed: 15 rows.`
