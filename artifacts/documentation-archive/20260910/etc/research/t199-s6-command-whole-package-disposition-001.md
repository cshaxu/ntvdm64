# M0 T199 S6 Whole COMMAND Package Disposition

## Decision

COMMAND is one `bx-vdm` host-service package. It consumes common ingress, registry selection, copied CPU state, profile-selected immutable resources, and checked guest-memory actions. It does not link `CmdDispatch`, retain historical guest pointers, pass host handles to the guest, or move console, process, CPU, BIOS, or Bochs semantics across ownership boundaries.

The next implementation is one COMMAND facade and one all-service regression. The facade owns package admission; subordinate providers own only their recorded copied contracts. Trace has no scheduling authority.

## Full service disposition

| Component | Services | Disposition | Required package behavior |
| --- | --- | --- | --- |
| Lifecycle | `00` | Deferred | Decline until normal DOS terminal-result transport is separately admitted; do not overload a host process exit code. |
| Launch | `01,02,04,05` | Existing bounded CLI candidates | Migrate copied launch/COMSPEC/CWD/SCS fragments behind facade; no ambient CWD or raw pointer retention. |
| Original no-op | `03` | Implemented exact no-op | Preserve no-state four-byte continuation. |
| Execution/redirection | `06,08,0A` | Explicitly unavailable | One source-derived package failure route; never expose host handles or start a host process. |
| Execution/redirection | `07,0B` | Deferred | No executable classification, launch, child return, pipe, or redirection capability. |
| Console/keyboard | `09` | Deferred | No VDD, GUI, keyboard device, thread, or console activation. |
| Console/keyboard | `0E` | Existing bounded CLI candidate | Migrate fixed KEYB-not-installed response (`DX=0`); nonzero `DX` remains deferred. |
| Paired config | `0C,0D` | Existing immutable namespace candidates | Migrate CONFIG/AUTOEXEC materialization; no temporary host file or mutable host filesystem. |
| Environment | `0F` | Deferred | COMSPEC is not initial-environment provision; require immutable environment/write ABI. |
| Session | `10` | Deferred | Require explicit CLI session-policy declaration; do not infer ambient console state. |
| Sentinel | `11` | Non-callable | Common ingress rejects before facade. |

## Failure model

An explicit-unavailable service produces one documented typed failure, never plausible success. Deferred members remain pass-through at the facade until their complete subprovider is admitted; they do not inspect extra guest memory, modify RAM, or acquire host resources. Existing candidates retain their positive/negative contracts but run only after the facade approves identity and component.

## S7 admission

S7 may add `bx_ntvdm_command_package_facade_v1`, migrate listed existing candidate/no-op routes behind it, and add source-built all-17-service plus sentinel regression. It must prove no bypass, unavailable behavior, deferred no-result behavior, and migrated candidate positive/negative paths. It may not add lifecycle, environment, process, handle, VDD, GUI, or keyboard capability, and it may not run a trace.
