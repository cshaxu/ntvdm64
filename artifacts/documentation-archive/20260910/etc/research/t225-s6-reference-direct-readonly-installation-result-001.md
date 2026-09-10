# T225 S6 Reference Direct/Readonly Installation Result

## Scope

This record closes only the approved-reference-image installation witness for
M0 T225 S6. It does not claim guest instruction execution, NTDOS `$Exec`,
COMMAND lifecycle, child return, or a source-build result.

## Input Classification

The run used only the owner-approved secondary reference sequence in
`artifacts/research/t225-s6-reference-inputs-r4`, governed by
`docs/etc/evidence/t225-s6-reference-guest-input-admission-001.md`.
Its hashes match the locked source-built artifact identities, but its
provenance remains `approved-reference-binary-research-input`.

## Build And Run

The fixture reused the completed current-source CPU5/P bx-core, bx-mantle and
bx-vdm closure in `build/t225-s6/four-image-r12`, under MSVC x64 `/MT`.
The changed runtime and fixture objects were recompiled and the executable was
freshly relinked. The fixture exited zero.

| Mode | Result | Evidence |
| --- | --- | --- |
| Direct | accepted | fixture installation and NTIO startup-plan preparation succeeded |
| Readonly | accepted | same installation and startup-plan preparation succeeded without acquiring Direct-only CLI stream handles |
| Overlay | rejected | explicit unselected/deferred mode; installation returned `-1` |
| Virtual | rejected | explicit retired mode; installation returned `-1` |

## Corrected Invariant

COMMAND profile binding itself was valid. The prior `-58` report was a
misattributed later failure: `admit_command_cli_streams` was executed for all
modes, although it is a Direct host-stream capability. Runtime composition now
admits those handles only for Direct. Readonly still installs the same COMMAND
package and retains its source-shaped readonly provider dispositions; it never
receives a hidden Direct fallback.

## Boundary

This is an installation-level profile witness. It is not a native guest trace
and does not authorize trace-led BOP work. The primary original-toolchain
source-built image sequence remains a separate required cross-validation input
when physically restored.