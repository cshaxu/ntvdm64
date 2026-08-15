# T204 S2 — startup-configuration policy ABI

## Result

S2 establishes the configuration package's admission record without changing
startup behavior.  It is a fixed-width copied `bx-vdm` value; it is neither a
configuration provider nor a BOP implementation.

`bx_ntvdm_startup_configuration_policy_v1` contains only:

| Field | Meaning |
| --- | --- |
| ABI header | Magic, version and exact structure size. |
| `source` | One provenance category: contained fixture, explicitly selected host source, or virtual volume. |
| `mutation_mode` | The already-authorized T201 session mode, copied by value. |
| `permitted_mode_mask` | The fixed mode compatibility of that provenance category. |
| `reserved0` | Must be zero. |

No path, file contents, host handle, guest address, BOP selector, callback,
Bochs object or raw pointer is admitted.  Initialization can only obtain the
mode through T201's `COMMAND` / `SESSION_CONTEXT` registration, then drops the
profile pointer.  Thus a future provider cannot choose a different mutation
mode at a service call.

Contained-fixture and explicit-host provenance are policy-neutral: all four
shared modes are representable because they describe the provider's future
session storage behavior, not an I/O action.  Virtual-volume provenance is
valid only with the virtual mode.  This does **not** authorize a host read or
write for any mode.

## Files and owner boundary

- `src/bx-vdm/bx_ntvdm_startup_configuration_policy_v1.h`
- `src/bx-vdm/bx_ntvdm_startup_configuration_policy_v1.c`
- `tests/bx-vdm/bx_ntvdm_startup_configuration_policy_v1_test.c`

The record belongs to the startup-configuration package in `bx-vdm`.  It
does not move COMMAND, DEM, UMB/DASD, DOS, or Bochs ownership.  In particular,
`54:0C`, `54:0D`, `5E`, guest RAM, `cmdconf.c` transformation and the current
boot namespace are deliberately untouched.

## Reproduction

The following ran from the repository root on 2026-08-15 and completed with
exit code zero:

```powershell
cmd.exe /c 'call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat" -arch=x64 -host_arch=x64 && cl.exe /nologo /TC /std:c11 /W4 /WX /MT /I src\bx-vdm /Fe:artifacts\build\t204-s2-startup-configuration-policy-r1\bx_ntvdm_startup_configuration_policy_v1_test.exe src\bx-vdm\bx_ntvdm_mutation_profile_v1.c src\bx-vdm\bx_ntvdm_startup_configuration_policy_v1.c tests\bx-vdm\bx_ntvdm_startup_configuration_policy_v1_test.c && artifacts\build\t204-s2-startup-configuration-policy-r1\bx_ntvdm_startup_configuration_policy_v1_test.exe'
```

The test covers direct, readonly, overlay and virtual copied mode selection;
the virtual-volume/direct mismatch; a corrupted reserved field; clear-state
rejection; and missing COMMAND registration.  It contains no host I/O,
guest-memory access, BOP ingress, profile parser, CLI or Bochs dependency.

## Limitation and next boundary

S2 proves only that later work has one shared configuration-source admission
record.  It does not prove the historical configuration transform or a normal
guest startup.  S3 must recover the whole `cmdconf.c` input/transform/error
contract and contained provider design before any existing synthetic boot
bytes or `54:0C/0D` handoff is replaced.
