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

## Native launch-input binding

The native engine now uses `bx_ntvdm_command_v2_runtime_session_bind_from_startup`,
not the test-only empty-session bind.  That entry copies the already admitted
launch descriptor and profile-declared `TARGET.COM` or `TARGET.EXE` identity
from `dem_v2_startup_composition` into the existing OpenNT-shaped
`GetNextVDMCommand` session.  It retains the original `cmdGetNextCmd` owner
body and keeps the mantle selector- and COMMAND-blind.

This seam intentionally transfers only a bounded OEM application name, ASCII
tail, profile drive index and the initial DOS code page 437.  It transfers no
host path, guest pointer, host handle, ambient command line or legacy v1
session.  The three changed source units compile under the formal MSVC x64
`/W4 /WX /MT` configuration.  A native observation that reaches `54:01`
remains pending the earlier machine-continuity boundary; this compile witness
does not claim that observation.

## Formal graph status

The formal Ninja graph was freshly generated in
`build/M0-T231-S10/formal-r2`; its manifest includes the v2 route sources,
`cmddisp.c`, and no `bop-v1` source.  The generator's `/FI` argument was
corrected to MSVC's attached form (`/FI"path"`) after reproducing the prior
space-separated form as an invalid forced-include argument.

The installed Ninja 1.13.2 runners still become idle without executing child
actions in this environment.  This record therefore does **not** call the
formal graph a passing build.  S10 remains active pending a runner resolution,
the full registry-capable matrix and a bounded native guest observation.

## COMMAND v1 direct implementation retirement

The 51 tracked `bop-v1/bx_ntvdm_cmd_*` and `bop-v1/bx_ntvdm_command_*`
implementation/header files have been physically removed.  The 18
COMMAND-specific v1 fixtures that invoked those owners have also been removed
from the tree, and the formal Ninja manifest contains no `bop-v1` source or
COMMAND-v1 fixture input.  The active source-built route is therefore the
OpenNT mirror plus neutral v2 shims; v1 is not an alternate product path.

Fresh `build/M0-T231-S10/formal-r5` generation accepted that manifest and
`ninja -n all` enumerated its complete 378-edge module/fixture/CLI graph,
including the final `ntdos64-native.exe` link, without a missing v1 source.
This is graph-admission evidence only: the runner's actual action-dispatch
stall remains separately recorded above.

The remaining pre-governance boot-namespace/adapter-runtime composition slice
was subsequently removed as a whole, together with the seven fixtures that
exercised it.  It directly included the removed COMMAND v1 headers and was
therefore neither an independent non-COMMAND owner nor a viable product/build
route.  A focused post-removal search finds no `bx_ntvdm_cmd_*` or
`bx_ntvdm_command_*` v1 symbol in `src/bx-vdm/bop-v1` or its legacy fixtures.
The retained historical build/probe scripts still name their old inputs only
as evidence of their historic experiment; they are not formal-manifest inputs.

The first generator invocation after this removal correctly rejected the
stale `t225-s6-four-image-profile` fixture admission.  Its manifest entry was
removed with the fixture; regenerated `formal-r4` then enumerated the complete
graph successfully.  This makes the formal graph reject a reintroduction of
the removed v1 fixture by normal input validation rather than leaving a stale
path hidden in a historical build list.

The final command-specific legacy leaf, `bx_ntvdm_cmdinfo_v1`, was also
removed with its header, dedicated fixture, and the historical native-boundary
fixture that consumed its self-authored wire codec.  `54:01` is consequently
represented only by the imported OpenNT `cmdGetNextCmd` implementation and its
v2 session/checked-RAM seam; the project no longer retains a parallel local
CMDINFO provider as a fallback.

After the CMDINFO fixture removal, `formal-r5` generation and its complete
378-edge `ninja -n all` graph succeeded.  Its admitted COMMAND sources are
the eight original-mirror files plus the explicitly listed v2 bridges and
neutral shims; `bop-v1` is absent from both modules and fixtures.

## Registry-success replay

The scoped S4 `54:0E` registry-success fixture was replayed outside the
restricted runner.  It exited zero and printed:

```text
T231 S4 direct OpenNT console, keyboard fallback/success, and standard-handle token ABI verified
```

The fixture uses only its process-local HKLM override, disposable KB16/keyboard
inputs and temporary `HKCU\\Software\\ntdos64-t231-kbd` key, then removes all
three before exit.  This closes the former host-permission gap without adding
a fallback or changing the imported `cmdkeyb.c` behavior.

## Current-source compilation replay

Because the installed Ninja runners still stall before creating any child
process, the exact MSVC commands emitted by `formal-r5` were invoked from its
own build root for the following fifteen admitted COMMAND units.  All returned
zero under `/W4 /WX /MT`: the eight imported OpenNT translation units
(`cmdmisc`, `cmddisp`, `cmdexit`, `cmdconf`, `cmdenv`, `cmdexec`, `cmdkeyb`,
`cmdredir`), the two COMMAND v2 bridges/session units, and all five neutral
COMMAND shims.  This is a current-source compilation witness derived from the
formal graph, not a claim that the Ninja executor itself has passed.
