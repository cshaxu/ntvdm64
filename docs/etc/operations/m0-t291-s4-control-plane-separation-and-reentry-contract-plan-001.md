# M0 T291 S4 — Control-plane separation and source-shaped re-entry contracts

## Objective

Recover the distinct original MVDM control-plane contracts reached by the WOW
foundation without collapsing BaseSrv command brokering, `NtVdmControl`, and
synchronous CCPU/monitor re-entry into one session operation dispatcher.

## Source-first scope

1. **`GetNextVDMCommand` capability.** Audit `vdmapi.h`, `cmdexec.c`,
   `cmdmisc.c` and `wow32/wkman.c` together. Preserve one same-shaped command
   request ABI with a session-local provider/queue contract, explicit
   availability, wait/wake and single-session arbitration result. The current
   `app_command_source` may remain a minimal provider only if it is clearly
   typed as such; it cannot be called a BaseSrv/CSR replacement.
2. **`NtVdmControl` capability.** Audit each currently reached control code
   and separate it from command brokering. The existing `VdmQueryDir` path
   receives its own typed adapter monitor/control binding and source failure
   results; it may not use the command request route simply because both
   mechanisms carry an operation number.
3. **`host_simulate` postconditions.** Inventory every reached original call
   site in WOW, COMMAND/DEM, DPMI, monitor and firmware package inputs. Keep
   the one same-shaped SoftPC entry, but record a separate owner-specific
   precondition, stop reason, register/stack outcome and failure rule for each
   call site. Do not declare a bare CPU loop semantically complete.
4. **WOW task binding.** Record the exact current one-thread projection limit,
   reject unbound/reentrant use and map the original TD/TEB lifecycle needed
   before a later worker or multi-WOW-task admission. No process-global task
   state, fast monitor or `CurrentMonitorTeb` is created.
5. **Focused proof and handoff.** Add source/ABI/failure ledgers and x86/x64
   fixtures for the separated currently reached contracts. Every unenabled
   BaseSrv/CSR, WOWEXEC, fast-monitor or complete callback-stream path gets a
   named subsequent owner and unavailable result.

## Non-goals

No WOW provider selector/body, Win16 guest load, private CSRSS/BaseSrv clone,
CCPU worker, current-monitor TEB, fast WOW assembler, global dispatcher,
generic adapter or extra mapping manager. No `src.old` input.

## Exit criteria

The product has one explicitly typed owner per original layer, no current
`NtVdmControl` call uses the command broker route, no `CallBack16` use uses a
session command/control route, and every reached `host_simulate` call site has
one source-backed postcondition disposition. The record distinguishes the
minimal single-session provider from unimplemented BaseSrv/CSR multi-caller
behavior instead of naming it complete.
