# M0 T309 S1 — DEM/COMMAND ingress closure audit

## Question

Can the original DEM and COMMAND BOP ingress forms be composed without
linking the historical aggregate `nt_bop.c` and therefore without importing
its unrelated SoftPC/PIC executor dependencies?

## Inputs

- `mvdm-host/softpc.new/host/src/nt_bop.c:146-161` (`MS_bop_0`)
- `mvdm-host/softpc.new/host/src/nt_bop.c:317-325` (`MS_bop_4`)
- `mvdm-host/dos/dem/demdisp.c` and `mvdm-host/dos/command/cmddisp.c`
- current `adapter-mvdm-host-out/softpc` scalar register and checked SAS
  forms, plus the closed Base VDM and monitor contracts.

## Physical closure

`MS_bop_0` has this original order:

1. Read the one service byte at current CS:IP.
2. Call `DemDispatch(ULONG service)`.
3. Advance 16-bit IP by one.
4. Notify historical disk-idle logic unless the service is `14h` or `15h`.

`MS_bop_4` has this original order:

1. Read the one service byte at current real-mode CS:IP through `sas_load`.
2. Call `CmdDispatch(ULONG service)`.
3. Advance 16-bit IP by one.

The two dispatcher translation units are source-composable on both widths.
The aggregate `nt_bop.c` instead imports `nt_eoi.h` and its historical
`VDMVIRTUALICA`/PIC executor universe for unrelated selectors. That dependency
does not occur in either selected entry body.

## Source-first disposition

1. **Direct original unit:** rejected only for the aggregate unit: compiling
   all selectors reaches the excluded historical PIC/second executor.
2. **Smallest same-shaped binding:** selected. Build a private,
   source-shaped true subset within the `mvdm-host` mirror library containing
   only `MS_bop_0` and `MS_bop_4`; preserve their public spellings, order,
   dispatcher calls and IP result. The subset is not a new adapter ABI.
3. **Intrusion:** not selected. No Bochs/CPU/PIC change is required.
4. **New behavior:** not selected. The only necessary x86/x64 divergence is
   the `MS_bop_0` raw `Sim32GetVDMPointer` dereference: a checked one-byte
   real-mode SAS read must replace the unsafe host-pointer alias. The existing
   `sas_load` spelling already supplies the same physical guest-byte result.

The original idle notification remains an explicit same-shaped adapter form.
Its historical body only clears/awakens a CCPU idle waiter; no such waiter is
admitted in this session profile. It must therefore preserve the call spelling
and no-op/notification contract without inventing an executor or app policy.

## Next local closure

The next P may add the private `mvdm-host` subset and the minimal idle binding,
then compile the two original dispatchers against a deliberately selected
local handler cohort. It must not use a synthetic selector router as a product
provider; a fixture may only use test-local handlers to prove ingress ordering.

## P1 verification

`New-T309DemCommandIngressNinja.ps1` was executed outside the sandbox under
MSVC `/MT` for both `x86` and `x64`. Both generated graphs compiled the private
source-shaped subset with `/WX`, linked the focused harness, and printed
`PASS: source-shaped DEM/COMMAND BOP ingress subset`. The harness proves the
two original dispatcher-before-IP orders and both branches of the original
DEM idle condition; its dispatcher functions are test-only observation stubs,
not a product route.
