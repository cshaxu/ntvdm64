# M0 T317 S1 — DEM/COMMAND SoftPC vertical-slice boundary reconciliation

## Objective

Replace the retired Bochs-era vertical-slice assumptions with one source-first
whole-cohort recovery map for the selected x86 SoftPC `CPU_40_STYLE` product.
The result is a bounded implementation plan for original DEM and COMMAND
providers, not a trace-derived BOP patch list.

## Inputs

- Original selected `mvdm-host/dos/dem` and `mvdm-host/dos/command` source
  cohorts, their dispatcher tables and original `sources` manifests.
- Original Base VDM client/server evidence slices under `opennt-host` and the
  current Base VDM protocol boundary ledger.
- Current same-shaped `basesrv`, `softpc`, `monitor` and `win32` adapters,
  session mapping-manager interfaces, and selected CCPU40 Ninja graph.
- Earlier T305/T309 material only as historical evidence; it must not select
  Bochs, CPU30 or a former adapter route.

## Required audit

1. Inventory every dispatcher-selected original DEM and COMMAND translation
   unit by table owner, source-file manifest and incoming BOP family.
2. For every boundary call that leaves those cohorts, resolve the complete
   original definition/owner and select exactly one disposition: original
   source, named existing adapter, explicit original-shaped unavailable branch,
   or later owner package.
3. Trace Base VDM request, capacity, retry/re-entry and child-return forms
   across the original COMMAND callers and current bounded Base VDM/session
   implementation. Keep Base VDM broker and monitor control planes separate.
4. Trace every SoftPC-facing guest lease, register, address, event and
   termination form to the CCPU40 machine route; flag any former Bochs route as
   invalid rather than treating it as an available provider.
5. Group the selected original providers by full translation-unit cohort and
   prerequisite order. Each later implementation S must own whole cohorts,
   including their common state/data files and local regression surface.

## Exclusions

No provider-body replacement, no BOP selector implementation, no partial
table link, no guest trace run, no cross-process BaseSrv broker, no kernel
VDM/V86, no CPU30, and no Bochs source or build input.

## S2 admission rule

S2 may begin only after this plan names one smallest implementable cohort with
its complete outgoing closure. It must attempt original translation-unit
composition before introducing any new adapter logic.
