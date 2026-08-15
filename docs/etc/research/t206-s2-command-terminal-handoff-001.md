# M0 T206 S2 — one-shot COMMAND terminal handoff

## Question

Can the declared one/two-slot COMMAND session recover original `54:0B`
`cmdReturnExitCode` semantics as an adapter-local, fixed-width result without
turning it into a Bochs stop, a historical BaseSrv/CSR re-entry, or a CLI exit?

## Inputs

| Input | SHA-256 | Role |
| --- | --- | --- |
| `src/opennt/base/mvdm/dos/v86/cmd/cmddisp.c` | `2A906E7100DA9DB6F16BAB91C767B149F28E63A0DB2803297812A8E86D2B19D6` | Original `cmdReturnExitCode` selector and result contract. |
| `src/bx-vdm/bx_ntvdm_cmd_get_next_service.c` | `65869373D6865815CDF81AB130CDE98DB360897441ACCE179B1C9CFB375D4221` | One-shot state, original `54:01` delivery and `54:0B` return provider. |
| `src/bx-vdm/bx_ntvdm_cmd_get_next_service.h` | `DA14B310E645B8B7C121B21D8BD0A68A835D05565E7DDA2FF6CA78E9F1632D27` | Versioned copied terminal-record ABI. |
| `src/bx-vdm/bx_ntvdm_adapter_runtime.c` | `24AA4D9AA70D60556D8CDEFCE5A8FFF650D6F0F7CE9C5B33D4D375DFB2FBF504` | Installed exact `54:0B` route gate. |
| `tests/bx-vdm/bx_ntvdm_cmd_get_next_service_test.c` | `E4A9CBB098B4CA634FC882345641747F0AB06FF61FD2833A52B78ED1F6847710` | Package regression. |

## Recovered contract

The installed route had incorrectly asked the COMMAND gate for `54:11`, the
original non-callable dispatcher sentinel. It now asks for `54:0B`; the
provider independently requires the exact four-byte BOP form.

Each committed `54:01` delivery clears the preceding return marker and any
terminal record. Thus a two-slot session may return from its first child with
CF set, deliver the next declared slot, and return again without a test or
caller mutating private state. A duplicate return before the next committed
delivery remains refused.

At the final return only, `54:0B` resumes at `fault_rip + 4`, clears CF,
writes AX from the low byte of DX, and creates this copied record:

| Field | Meaning |
| --- | --- |
| `magic`, `abi_version`, `struct_bytes` | Exact ABI identity (`BXCT`, v1, 32 bytes). |
| `present` | One only after declared-plan exhaustion. |
| `reason` | `DECLARED_PLAN_EXHAUSTED` only. |
| `dos_exit_code` | Low DX byte only (`0..255`). |
| reserved fields | Required zero. |

The copy API refuses an absent record. The record has no CPU state, guest
address, guest pointer, host handle, BOP identity or host-child result.
`54:11`, an early return, a duplicate return, malformed input and every
unrelated terminal outcome create no record.

## Verification

Fresh source closure:

```text
powershell.exe -ExecutionPolicy Bypass -File tools\Invoke-T198S16BootNamespaceVdmX64Probe.ps1
  -RepositoryRoot O:\repos.hobby\ntdos64
  -BuildRoot O:\repos.hobby\ntdos64\artifacts\build\t206-s2-command-terminal-r2
  -Fixture command-entry -HostArchitecture x64
```

The script compiled, linked and ran the complete declared COMMAND-entry
closure using MSVC x64 `/MT /W4 /WX`. Its result JSON records `linkExitCode`
and `runExitCode` zero; its runner printed:

```text
bx-ntvdm COMMAND CMDINFO delivery, environment retry and terminal lifecycles verified
```

The regression covers absent record, `54:01`, first-return re-entry, automatic
return-state reset on the second committed `54:01`, final `54:0B`, a nonzero
`DX=0x1234` low-byte result (`AX=0x34` and copied `0x34`), duplicate return,
and rejected `54:11`.

Separately, the changed installed runtime route compiled with MSVC x64
`/MT /W4 /WX` into
`artifacts/build/t206-s2-runtime-route-compile-r1/bx_ntvdm_adapter_runtime.obj`.

## Boundary

This closes only the adapter-local COMMAND package handoff. It does not make
`54:0B` a CPU stop, does not publish a normal completion through the mantle or
engine, does not choose a CLI process exit, and does not restore the original
BaseSrv/CSR command broker. Those require separately admitted engine and CLI
ABI work.
