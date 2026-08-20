# T231 S10 — COMMAND v2 route admission (interim)

## Result

The active external generic-UD composition now recognizes bounded
`C4 C4 54 00..10` windows in `src/bx-vdm/bop/command_v2_generic_ud_bridge.c`.
It dispatches them only through a bound `command_native_session_shim` into the
already admitted OpenNT COMMAND mirrors.  A recognized COMMAND window with no
bound v2 session declines to the CPU exception path; it never falls through to
`bop-v1`.

`src/bx-vdm/bop/dem_v2_composition_bridge.c` remains the single opaque mantle
entry.  The mantle receives no BOP selector, service number or COMMAND type.
The adapter owns selector recognition and copied-state conversion; the new
session owns only checked-RAM callbacks and the existing fixed-width COMMAND
call ABI.

## Original dispatcher fidelity correction

The imported OpenNT `base/mvdm/dos/command/cmddisp.c` is now a formal `bx-vdm`
source and `command_misc_shim.c` calls its original `CmdDispatch` table.  The
former local `if/else` service selection was removed.  The only source edit in
`cmddisp.c` is a `DIVERGENCE:` include substitution for the unavailable NT4
CCPU/SAS product-host headers; the 17 function-pointer order is unchanged.

## Local source-built evidence

`build/M0-T231-S10/direct-r2/command-v2.lib` was rebuilt from all admitted
COMMAND mirrors, the original dispatcher, neutral COMMAND shims and fixed
width CPU ABI sources with MSVC x64 `/W4 /WX /MT`.

The rebuilt focused fixtures exited zero for S2, S3, S5, S6, S7, S8, S9 and
the new v2 native-session route fixture.  The new fixture reports:

```text
T231 v2 COMMAND session owns 54:00 without a v1 fallback
```

It proves that a bound `54:00` enters original `cmdExitVDM` and produces the
typed controlled stop; `54:11` is rejected as out of range; and an unbound
`54:00` declines.  It does not claim a real Bochs machine-stage RAM test.

The S4 fallback/pipe portions also rebuild.  Its registry-backed keyboard
success branch could not run in this execution environment because creation
of `HKCU\\Software\\ntdos64-t231-kbd` was denied.  This is an external
host-permission limitation, not a substituted or skipped product branch; the
fixture retains that original public-Win32 registry test unchanged.

## Formal graph status

The formal Ninja graph was freshly generated in
`build/M0-T231-S10/formal-r2`; its manifest includes the v2 route sources,
`cmddisp.c`, and no `bop-v1` source.  The generator's `/FI` argument was
corrected to MSVC's attached form (`/FI"path"`) after reproducing the prior
space-separated form as an invalid forced-include argument.

The installed Ninja 1.13.2 runners still become idle without executing child
actions in this environment.  This record therefore does **not** call the
formal graph a passing build.  S10 remains active pending a runner resolution,
the full registry-capable matrix, final product-v1 deletion and a bounded
native guest observation.
