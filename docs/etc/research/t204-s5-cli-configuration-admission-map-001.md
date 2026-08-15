# T204 S5 P2 — explicit CLI configuration admission map

## Admitted direction

The default synthetic configuration route can be removed only through one
paired, explicit input contract:

```text
ntdos64-run --config-source <path> --autoexec-source <path>
  -> child-only NTDOS64_STARTUP_CONFIG_SOURCE / _AUTOEXEC_SOURCE
  -> engine adapter installation
  -> bounded read + OEM copied values + T204 S2 policy
  -> T204 S4 provider
  -> S5 namespace/COMMAND binding
```

The two options are a pair: a DOS engine invocation supplies both or neither;
the production profile rejects neither.  A named deterministic fixture may
remain only through an explicit fixture profile, never as the default route.

## Ownership and API facts

- `src/cli/ntdos64_run.c` already parses runner options and constructs a
  child-only environment for `CreateProcessW`.  It must normalize and relay
  paths, but must not open, hash, retain or transform configuration bytes.
- `src/bx-vdm/bx_ntvdm_adapter_runtime.c::install` already receives profile/
  root and reads child environment at installation.  It owns the bounded
  `CreateFileW`/`ReadFile` input step, `WideCharToMultiByte(CP_OEMCP)` copy,
  copied root/locale/OEM facts, T204 policy build and provider bind.
- `byob_profile` currently exposes only synthetic boot-file materialization;
  it is not extended to contain arbitrary host paths or content.

`CreateFileW`, `GetFileSizeEx`, `ReadFile`, `GetWindowsDirectoryW`,
`GetLocaleInfoW`, `GetOEMCP`, `GetFullPathNameW`, and
`WideCharToMultiByte` are normal modern user-mode APIs.  Their use neither
requires a host rebuild nor modifies an installation.  Failures are explicit
adapter installation rejections; the provider never opens an ambient path at
BOP time.

## Required implementation ordering

1. add paired CLI grammar, normalization and child-only environment entries;
2. add installation-time bounded copied source reader and exact diagnostic;
3. build a direct-mode T204 policy from the current first profile's shared
   session policy, then bind the provider;
4. reject production installation without the pair and remove its synthetic
   boot initialization; retain a separately declared fixture-only path;
5. exercise pair/missing/unreadable/oversize/OEM conversion and one complete
   provider/COMMAND composition regression.

No BOP, native trace, temporary file, source mutation, registry operation or
Bochs change is part of this work.
