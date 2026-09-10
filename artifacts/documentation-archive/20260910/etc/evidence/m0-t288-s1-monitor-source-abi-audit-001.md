# M0 T288 S1 — monitor and keyboard/IVT source/ABI audit

## Question

Which original source belongs to the complete Wave 5 monitor/keyboard
substrate, which interfaces must be recovered before any body is enabled, and
which historical mechanisms are owned by later packages rather than by a
local workaround?

## Inputs and procedure

- Canonical source roots: `O:\repos.external\opennt` and
  `O:\repos.external\opennt-4.5`.
- Current selected paths: `mvdm-host/{sim16,sim32,v86,ieuvddex}` and the
  currently misplaced but byte-identical
  `mvdm-host/softpc.new/base/keymouse` subtree.
- `New-T288S1MonitorSourceLedger.ps1` recursively hashes both canonical
  source paths and the current mirror, then writes the reproducible ledger.
- Source/build forms and reached XMS consumer paths were reviewed to partition
  the original interfaces in the accompanying interface ledger.

## Observations

- The source identity ledger has **78 rows**, all exact in both canonical
  roots: SIM16 8, SIM32 5, V86 35, IEUVDDEx 23 and keymouse 7.
- Source identity is therefore not an implementation blocker. The seven
  keymouse files are physically still under `mvdm-host`; their required mirror
  owner is `mvdm-softpc-firmware`. This is a root-placement correction, not a
  source modification, and is assigned to S2.
- `sim16` is a 16-bit DLL build and cannot be an x86/x64 host-static input.
  Its SIM16 exchange is a later WOW owner boundary.
- `sim32` contains both copy-style VDM memory calls and historical durable
  pointer forms. Only the former can be evaluated for direct modern
  composition; the latter needs the existing bounded session lease model.
- The V86 monitor sources carry the old CPU context, event, SAS, interrupt
  and thread contracts. They cannot be linked as a second CCPU executor;
  their source shapes instead define the required monitor/SoftPC/Bochs
  boundaries.
- IEUVDDEx contains a small VDM_TIB/context inspection surface appropriate to
  the monitor boundary, but its VDD reflection/profiling/debugger behavior is
  owned by the later VDD/debugger queue package.
- The original `UpdateKbdInt15` fragment is the production owner for the
  current XMS `52:09` consumer. It needs a minimal IVT state seam only; it is
  not authority for a generic keyboard, IRQ, controller, timer, mouse or
  console implementation.

## Interpretation

The source-first recovery ladder is viable: all selected files are already
exact mirrors, so T288 begins with direct original source and only adds a
same-shaped adapter where a current architecture requires it. No new adapter
component, mapping manager, Bochs intrusion or source-derived provider is
admitted by S1.

The complete per-file identity result is in
[the source identity ledger](../operations/m0-t288-s1-monitor-source-identity-ledger.tsv).
The 15 grouped original interface/owner/failure dispositions are in
[the interface ledger](../operations/m0-t288-s1-monitor-interface-ledger.tsv).

## Follow-up

S2 first corrects the exact keymouse root placement, then attempts only the
directly composable original declaration/control carriers. S3--S5 remain
bound by the recorded owner partitions; neither a trace nor an individual BOP
may select a replacement behavior.
