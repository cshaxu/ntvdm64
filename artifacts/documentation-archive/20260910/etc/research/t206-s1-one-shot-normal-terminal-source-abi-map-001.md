# M0 T206 S1 — one-shot normal terminal source/ABI map

## Question

What source-owned path and fixed-width boundaries are required to publish a
truthful normal DOS completion from the one-shot COMMAND profile to the CLI,
without treating a controlled stop, fixture result, or process exit as a DOS
result?

## Inputs

| Input | SHA-256 | Fact |
| --- | --- | --- |
| `cmddisp.c` | `2A906E7100DA9DB6F16BAB91C767B149F28E63A0DB2803297812A8E86D2B19D6` | `cmdReturnExitCode` is callable COMMAND service `0x0B`. |
| `cmdexec.c` | `98A941095D47AE73EF9731D389A433203C7184B0A016B62E24B202BB89F829D0` | Its `RETURN_ON_NO_COMMAND` branch is historical broker composition. |
| `bx_ntvdm_adapter_runtime.c` | `435AFE68902C0BC7DF6E8C7FF676C3F7B7F3F579B6EEF4F2794071BBE96F826A` | The current route gates a `54:0B` handler behind `0x11`. |
| `bx_ntvdm_cmd_get_next_service.c` | `294E557D4DDB906F52EE93952046382BCFF62723F15575B5F4D6C5604432FCBE` | The handler itself accepts `C4 C4 54 0B` and retains a package-local exit byte. |
| `bx_ntvdm_engine_run_v1.c` | `D043B1859CF57A89F4D1F1498108969FDD339E3D51909CF1D15B757B4A2A25DD` | Ordinary completion has no engine producer. |
| `bx_ntvdm_engine_contract_v1.h` | `02BFB65978A75EF40D813C0FCA83755FF421EFE41B2D0F728E6ACC6D21C92BC5` | Ordinary completion has no copied DOS-result field. |
| `ntdos64_native_cli.c` | `875C08E84502EB9E48034C85C448BCE2C7D6F022B986A2E2D8170A99A070402E` | CLI currently maps prospective ordinary completion to process exit zero. |

## Observations

The original table places `cmdReturnExitCode` at `0x0B`. The current handler
preserves that exact wire form, resumes COMMAND and copies the low `DX` exit
value into package state. But the installed adapter route asks the common
COMMAND gate for `0x11`, the original sentinel; this makes the handler
unreachable.

Repairing the gate alone cannot produce a terminal result. An accepted `54:0B`
must resume COMMAND. No handoff carries its package-local exit byte to the
engine, whose public result branches produce only rejection, budget,
controlled stop or machine failure. The engine enum and CLI zero-exit mapping
are prospective, not evidence.

T202 retains the historical host re-entry branch as unavailable because it
depends on BaseSrv/CSR, VDMINFO pointers, handles and Redirector state. A
one-shot declared-plan terminal must not recreate that broker or use a host
process exit as a guest result.

## S2 admission

S2 may recover the whole one-shot COMMAND terminal handoff: correct the
`54:0B` gate; define a versioned copied terminal record created only at
declared-plan exhaustion; prove absence for `50:3D`, `FE`, budget, fixture and
host-child outcomes; and regress `54:01`, `54:0B`, `54:11`, repetition and
one/two-slot plans. Engine and CLI publication remain later T206 work.
