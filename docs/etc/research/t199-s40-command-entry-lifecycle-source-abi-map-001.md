# T199 S40: COMMAND entry lifecycle source and ABI map

## Original control plane

`src/opennt/base/mvdm/inc/cmdsvc.h` defines the seventeen `54:00..10`
services.  The entry lifecycle is not a single `54:01` endpoint:

| Service | Original function | Source role |
| --- | --- | --- |
| `54:02` | `cmdComSpec` (`cmdmisc.c`) | COMSPEC bootstrap input |
| `54:0F` | initial-environment provider (`cmdenv.c`) | environment construction |
| `54:05` | `cmdSetInfo` (`cmdmisc.c`) | records SCS/DOSDATA locators for later DOS/SCS coordination |
| `54:01` | `cmdGetNextCmd` (`cmdmisc.c`) | reads packed CMDINFO, obtains next command, returns command/environment/path information |
| `54:0B` | `cmdReturnExitCode` | return / next-command lifecycle |

`cmdGetNextCmd` itself reads CMDINFO from `DS:DX`, and on its first call
records environment facts from that CMDINFO.  `cmdSetInfo` independently
records `SCS_ToSync`, DOS-binary and FD-access locations.  Therefore
registration is a package state requirement, but treating its absence as a
generic bridge decline is not an original host outcome.

## Current adapter mismatch

`bx_ntvdm_command_package_session_v1_get_next` currently requires
`s->launch.valid` before even gathering CMDINFO.  Its complete transaction
unconditionally writes `reg->scs_to_sync` and `reg->is_dos_binary`.
Consequently a native startup reaching `54:01` before an admitted `54:05`
registration escapes the COMMAND package as a generic decline.

The existing bound regression proves only the registered order (`54:05` then
`54:01`), in `tests/bx-vdm/bx_ntvdm_boot_namespace_composition_v1_test.c`.
It does not cover the absent-registration entry boundary observed by S39.

## S40 implementation rule

Recover the lifecycle as one package transaction:

1. preserve the copied CMDINFO gather/validation and immutable launch plan;
2. make registration-dependent writes conditional on a validated copied
   registration rather than using a raw/zero locator;
3. define the source-derived pre-registration result for `54:01` explicitly
   (resume with the safe CMDINFO result or a documented COMMAND failure), not
   generic decline;
4. cover both orderings, registration locators, CMDINFO aperture failure,
   bootstrap/environment order and return lifecycle in one source-built
   package regression.

This is adapter-only COMMAND composition work.  It adds no Bochs selector
knowledge, host process, console, handle, DOS kernel or guest-pointer
retention.

## Current implementation and regression

The package now permits `54:01` before `54:05`: it executes the same checked
CMDINFO gather and immutable launch transaction, but omits the two writes that
belong specifically to a validated copied registration.  Once `54:05` has
registered its locators, those writes remain part of the normal transaction.

Fresh source-built MSVC x64 `/MT` evidence:

- `artifacts/build/t199-s40-command-entry-r3/` verifies the command helper
  with a null registration and transaction preflight.
- `artifacts/build/t199-s40-command-session-r2/` verifies the bound ingress →
  composition → checked-RAM session route first without registration, resets
  the test transaction state, then retains the registered `54:05 → 54:01`
  lifecycle regression.
