# M0 T308 S1 — Monitor / NtVdmControl user-mode contract

## Objective

Close the directly composable user-mode monitor control subset without
recreating the historical kernel VDM service. Retain original `VDMSERVICECLASS`
spellings and the reached `VDMQUERYDIRINFO` / `VDM_TIB` layout; provide only the
existing session-bound `VdmQueryDir` callback contract.

## Original source and boundary

- MVDM calls are in `mvdm-host/dos/dem/demsrch.c`,
  `mvdm-host/v86/monitor/i386/*`, `mvdm-host/softpc.new/host/src/*`,
  `mvdm-host/dpmi32/*` and later VDD/debugger paths.
- Original `NtVdmControl` entered the NT kernel VDM service. That is a hard
  non-invasive boundary on the modern host.
- `adapter-mvdm-host-out/monitor` preserves the reached user-mode spelling and
  session thread carrier; `adapter-mvdm-host-out/softpc` / `adapter-bochs`
  own machine mechanics, and individual source packages own their service
  semantics.

## Admitted work

1. Verify `VdmQueryDir` exact structure/status dispatch and `VDM_TIB`
   current-thread binding under both host architectures.
2. Produce a concise service-class disposition: machine contract,
   later DPMI, later VDD/debugger, or hard unavailable boundary. No generic
   `NtVdmControl` dispatcher is introduced.
3. Verify no app or BOP layer owns a competing monitor route.

## Exclusions

`VdmStartExecution`, interrupt queue/delay, initialization/features, LDT,
printer, Adlib, PM CLI and process-query service behavior are not implemented
here. Their typed future owner is recorded instead of returning success.

## Exit

The one user-mode form is formally proven on x86/x64; all other reached forms
have exactly one owner/boundary disposition and no kernel or second-executor
mechanism enters the product.
