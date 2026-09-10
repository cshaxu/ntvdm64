# M0 T310 S3: selected-backend lifecycle evidence

## Scope

This record covers only the copied selection/lifecycle binding. It does not
claim that original SoftPC machine composition is runnable; that is T310 S4.

## Implemented contract

- A ready `session` accepts one of two fixed-width identities: SoftPC or
  Bochs. A second selection and every selection after activation fail.
- `app_machine_shell_select_backend(..., NONE)` is the product default and
  selects SoftPC. Bochs requires an explicit application choice.
- `app_machine_shell_open` creates only an explicitly selected Bochs machine.
  A SoftPC-selected session returns `APP_MACHINE_SHELL_BACKEND_UNAVAILABLE`;
  it creates no Bochs machine.
- The source-shaped `host_simulate(void)` uses the Bochs typed finite-resume
  facade only for a Bochs-selected session. A SoftPC-selected session records
  `SESSION_MECHANICAL_STATUS_BACKEND_UNAVAILABLE` and never falls back.

No native pointer, host handle, Bochs object, CCPU state or callback crosses
the session ABI. This selection carries no guest-facing identity, so the
mapping manager is not involved.

## Formal verification

The following commands were run outside the sandbox. Each Ninja graph is a
disposable `build/M0-T310/S3/backend-selection/<arch>` root and uses MSVC
`/MT`.

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools/build/New-T310SelectedBackendNinja.ps1 -Architecture x64
ninja -C build/M0-T310/S3/backend-selection/x64 -j8 all-verify
powershell.exe -ExecutionPolicy Bypass -File tools/build/New-T310SelectedBackendNinja.ps1 -Architecture x86
ninja -C build/M0-T310/S3/backend-selection/x86 -j8 all-verify
```

All six executables completed with exit code zero:

- `session-backend-selection-test.exe`: exactly-one selection and no late
  switch on each architecture.
- `app-backend-selection-test.exe`: explicit Bochs opens exactly one fake
  mechanical lifecycle; omitted selection chooses SoftPC and opens none.
- `softpc-backend-selection-test.exe`: `host_simulate(void)` invokes one fake
  typed resume for Bochs and invokes none for SoftPC, which records the
  unavailable status.

The fake mechanical endpoints are fixture-only boundary probes. They neither
replace `adapter-bochs` nor claim an original SoftPC executor.

## Source-owner scan

The current MVDM mirror has original `host_simulate` call sites and historical
SoftPC definitions, but no `adapter-bochs` or `bochs-core` include/type/global
reference. The only selected binding is outside the mirror at
`adapter-mvdm-host-out/softpc/mvdm_vdm_stack.c`.

## Deferred work

S4 owns original SoftPC create/reset/firmware/device composition. Until then,
the default SoftPC selection remains an explicit unavailable state. Every
later functional verification must retain the x86/x64 × SoftPC/Bochs matrix.
