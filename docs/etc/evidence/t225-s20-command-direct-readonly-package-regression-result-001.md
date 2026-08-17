# T225 S20 COMMAND Direct/Readonly Package Regression Result

## Scope

This record verifies the one source-owned COMMAND package session for every
callable original `cmddisp.c` slot `54:00..10`.  It is family-regression
evidence only; it does not claim native guest execution or completion of the
COMMAND owner package.

## Build Closure

A fresh disposable MSVC x64 `/MT` Ninja graph was generated at
`build/M0-T225-S20/command-family-r3` from
`tools/build/t225-s7-full-module-manifest.json`.

- pinned CPU5/P configuration projection SHA-256:
  `DDB0D13E35C2FD9E7EC29896E6296F83ECE7F6BF5D2AD474E8937D389F97872F`;
- `bx-vdm.lib` and `cli.lib` were built as separate archives;
- the fixture declares its launch-plan serialization dependency on `cli`, so
  the link closure does not rely on an implicit object cache;
- after success, `ninja -n` reported `no work to do`.

## Result

`tests/bx-vdm/t225_s20_command_package_family_fixture.c` passed in both
supported modes.

The fixture drives each selector/service pair through
`bx_ntvdm_boot_namespace_composition_v1_handle`:

- `54:00` and `54:10` retain their source-shaped stop terminals;
- `54:01..0F` return the checked resume contract;
- Direct receives only opaque standard-handle and child-backend paths;
- Readonly does not call the child backend and retains the typed unavailable
  standard-handle result;
- the COMMAND environment fixture uses the source-defined double-NUL block
  layout rather than a fixed 4023-byte assumption.

The fixture setup also records a source-defined ordering fact: the DEM mutation
profile must be installed before its host namespace, because construction of
the whole provider consumes the profile.  This is setup evidence, not a new
provider behavior.

## Limits

This result is not a native trace.  It neither claims that every COMMAND
subfamily is closed nor admits trace-led service work.  The next S20 step is
source/ABI reconciliation and targeted provider convergence for the remaining
owner-package gaps; any bounded native trace remains package validation only.