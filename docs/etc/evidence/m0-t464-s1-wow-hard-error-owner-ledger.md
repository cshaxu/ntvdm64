# M0 T464 S1 — WOW hard-error owner ledger

## Question

Which currently reachable WOW hard-error edges still have an owner that can be
composed on the modern host, and which are private NT4 server work that must
remain outside the product boundary?

## Inputs and method

This is a source-only audit. It rereads the selected original client and WOW32
bodies, the current x86 selection map, and the read-only
`ntvdm64-wow32-audit` server-contract record. No guest, product binary, or
runtime was changed or exercised.

## Owner and response ledger

| Reached edge | Original owner and build condition | Caller-visible result | Current authority / disposition |
| --- | --- | --- | --- |
| Generic SoftPC error while `VDMForWOW` | `nt_error.c::ErrorDialogBoxThread` selects `WowErrorDialogEvents`; the non-WOW `ERRORPANEL` branch is not selected. | `RMB_ABORT`, `RMB_RETRY`, or `RMB_IGNORE`; `RMB_ABORT` transfers to `W32HungAppNotifyThread(0)`. | Preserve original client-side mapping and the existing WOW32 terminator only. `ERRORPANEL` cannot substitute: it consumes `RMB_*`, while the private request returns system-error-box slots. |
| Guest USER ordinal 320 | `wudlg.c::WU32SysErrorBox` maps real- or protected-mode arguments and calls `WOWSysErrorBox`. | The original selected slot, `1..3`, returns to 16-bit USER. | Keep the original thunk, mapping, timer bracket, ANSI/Unicode conversion and four-word request shape. The modern receiver must return the original slot, never `IDOK`/`IDCANCEL`. |
| WOW event-hook failure | `wuser.c::WU32SetEventHook`. | Slot 3 continues; every other outcome sets `RET_FORCETASKEXIT` on the current task. | The caller remains sole task-exit owner. A dialog adapter may only return a slot; it must not terminate a task itself. |
| WOW exception handler | `wow32.c::W32Exception`. | Slot 2 enters the existing debugger path; slot 3 continues/handles; slot 1 reaches existing `KillTask`. | Preserve the original exception and task-cleanup tails. No new debugger or terminator is admitted. |
| Private receiver | `ntuser/server/harderr.c::DisplayVDMHardError`, reached by `NtRaiseHardError`. | Decodes three sparse `SEB_*` slots; returns the original slot ID, not a compacted native-button ID. | It depends on NT4 CSRSS/USER desktop attachment and is not composable on modern public USER. It is an explicit host boundary, not an unavailable original client body. |

## Exact modern boundary

The current `wow_hard_error_dialog.c` is reachable and selected, despite the
misleading unavailable-wrapper filename. It is a narrow receiver for exactly
the original four-parameter `STATUS_VDM_HARD_ERROR | 0x10000000` request, but
its current policy is not equivalent to the audited original receiver:

- it rejects no-explicit-default and multiple-default requests, while the
  original starts at displayed position zero and lets each later valid default
  replace the earlier one;
- it hard-codes English labels instead of the original message resources;
- it discards the original `SEB_CANCEL`/`CancelId` semantics; and
- it intentionally suppresses title-bar close without proving the original
  sparse-slot behavior.

The original UI server cannot be imported without recreating private USER,
CSRSS and desktop ownership. Conversely, deleting the receiver or routing to
non-WOW `ERRORPANEL` would change the observed ABI: the three callers above
need slot IDs and independently own their fatal actions.

## Bounded S2 admission

S2 owns only the current receiver's source-proven decoder and response policy:

1. retain the original `WOWpSysErrorBox` request producer and its timer
   ownership unchanged;
2. decode sparse slots, valid styles, later-default selection and `SEB_CANCEL`
   according to `DisplayVDMHardError`;
3. preserve slot IDs through public modal presentation, with no host-native
   result constants crossing the boundary; and
4. add focused decoder and response fixtures plus real selected caller
   acceptance where the original guest path is available.

S2 may not create a USER scheduler, a private desktop broker, a generic hard
error provider, or a second task/worker termination path. Report-only
hard-error modes, private attachment failure, full text-conversion loss
equivalence, and all non-WOW DOS INT 24 handling remain outside ERROR-01 and
are handed to their respective owners.

## Confidence and remaining boundary

Confidence is high for the selected caller/response/cleanup ownership because
each edge is present in selected original bodies and the current x86 map.
Runtime behaviour is not claimed by this S1 record. Exact NT4 privileged
desktop attachment remains unavailable by design; S2 must explicitly preserve
the listed application-visible response semantics rather than claim complete
NT4 server equivalence.
