# T198 S6 Mantle-to-VDM Ingress Map 001

## Question

What is the smallest linkable connection from the selector-blind generic
`#UD` record to the existing holistic `bx-vdm` BOP plane, without restoring
the retired product runtime or adding a service-specific recognizer?

## Inputs

- `src/bx-mantle/bx_ntvdm_generic_ud_bridge.h`.
- `src/bx-vdm/bx_ntvdm_exception_abi.h`, `bx_ntvdm_cpu_state_abi.h`,
  `bx_ntvdm_instruction_window_abi.h`, and `bx_ntvdm_cpu_result_v2.h`.
- `src/bx-vdm/bx_ntvdm_bop_ingress_v1.c` and
  `bx_ntvdm_bop_provider_registry_v1.c`.
- The retired `src/bx-core/cpu/exception.cc` composition and current
  `bx_ntvdm_adapter_runtime.c` as rejected comparison paths.

## Record Map

| Generic copied field | `bx-vdm` record | Rule |
| --- | --- | --- |
| magic/version/bytes | fresh local records | Do not reinterpret or forward the generic magic. Initialize each destination at its own ABI version. |
| cpu id, vector, error, fault RIP | `bx_ntvdm_exception_event_v1` | Set kind to `CPU_EXCEPTION`; copy scalar values exactly. |
| execution mode, GPRs, EIP/EFLAGS, six selectors | `bx_ntvdm_cpu_state_v1` | Exact copied-value map; descriptor caches and CPU objects never cross. |
| window byte count and 15 bytes | `bx_ntvdm_instruction_window_v1` | Copy at most the fixed 15-byte maximum and set `valid_bytes` to the copied count. |
| v2 pass-through result | generic bridge return zero | The core therefore takes its native exception path. The generic outcome's pass-through enum is not an accepted core outcome. |
| v2 resume result | generic typed resume outcome | Copy RIP, low-16 GPR delta and CF-only flags. Reject any invalid v2 result. |
| v2 stop result | generic typed stop outcome | No delta or RIP; core maps it to the finite stop latch. |

## Dependency And Link Decision

`bx-core` continues to call only the fixed bridge declaration.  The bare
finite target links the default implementation in `bx-mantle`, which declines.
A future composed target instead links one `bx-vdm` implementation of that
same fixed bridge symbol.  That implementation includes the fixed mantle ABI
header and calls `bx-vdm` inward functions; `bx-mantle` does not include or
call `bx-vdm`.  The two implementations are mutually exclusive link inputs.

The initial `bx-vdm` object closure is limited to the exception/state/window,
CPU-delta/result, BOP ingress and provider-registry objects plus a focused
bridge test.  It has no CLI, environment, Win32, session, payload or guest-RAM
dependency.  It is a MinGW-w64 x86 C island and crosses to the MSVC x86 Bochs
island only through the fixed copied C ABI.

## Explicit Rejections

`bx_ntvdm_adapter_runtime_v1/v2/v3/v4_dispatch` cannot be the first bridge
callee: it owns environment installation, profile/session state and later
read/write/gather transaction paths.  Its v3/v4 actions cannot fit the generic
typed outcome and require a separately admitted mantle-owned mechanical
transfer contract.  Existing individual service implementations likewise do
not enter this S6 closure.  Ingress classification and registry disposition
are global table operations; no selector/service case is added here.

## Verification Planned

Compile and run one focused x86 C test which supplies a fixed `C4 C4 50 11`
window and proves: copied record conversion, common ingress classification,
registry selection, and a resulting generic decline.  Repeat with non-BOP,
incomplete and unknown-selector windows to prove no accidental acceptance.
Then source-build the separate MSVC finite CPU5 fixture with the default
mantle bridge to prove that the alternative implementation has not contaminated
the bare-machine closure.  A composed cross-island link is deferred until its
artifact/CRT contract is recorded.

## Interpretation

This is a global-plane attachment map, not an implementation of individual
BOP services.  It makes the first composed target capable of recognizing every
inventory form through one ingress table while preserving the original native
exception behavior for every not-yet-admitted provider.
