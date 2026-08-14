# T200 S21 — initial-state publication closure

## Question

Can the profile-selected, opaque bx-vdm initial-state record reach an
initialized minimal Bochs machine without granting the mantle any BOP,
OpenNT, DOS, firmware-feature, or profile interpretation?

## Inputs

- T200 S20's admitted, copied `bx_ntvdm_initial_state_v1` record.
- The existing selector-blind `bx_ntvdm_mechanical_action_v1` mantle ABI.
- T198 S74's retained full CPU5 native-core object closure and its MSVC x64
  `/MT` configuration projection.

## Procedure

`bx_ntvdm_initial_state_action_v1_prepare` converts only admitted physical
ranges and copied bytes to one generic mantle write action. The focused
fixture initializes the minimal machine, applies a two-range action, and reads
both ranges through ordinary RAM. It then makes the second range inaccessible
and proves the first later destination was untouched, demonstrating the
mantle's complete range-accessibility preflight occurs before copying.

The reproducible command was:

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools\Invoke-T200S21InitialStatePublicationProbe.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\artifacts\build\t200-s21-initial-state-publication-r10
```

Its retained JSON record reports x64 `/MT`, initialized machine, no guest
execution, no CPU loop, and exit code zero.

## Result

The direct composition now publishes its installed record as a typed generic
action, and reset makes that publication unavailable. The source manifest
includes the bx-vdm conversion unit. The mantle receives only action kind,
physical ranges, and copied bytes; it remains selector-blind and has no host
service or profile dependency.

This does not claim a native CLI engine has yet applied the action before the
NTIO startup plan, nor does it claim guest execution, BIOS/POST equivalence,
or a BOP closure. Those are the next lifecycle-composition question.
