# T217 S12 — COMMAND Shell Capability Seam Result

## Implementation

The existing source-derived COMMAND configuration provider now carries one
installation-time copied capability enum:

- `ORIGINAL_HOST`: retain OpenNT `cmdconf.c`'s generated
  `%SystemRoot%\System32\command.com` `SHELL=` line.
- `DECLARED_GUEST`: retain CONFIG filtering, country generation, AUTOEXEC
  transform and merge behavior, but omit only the unavailable generated
  `SHELL=` override. Original SYSINIT consequently retains its own
  `\\COMMAND.COM` default, which is already a declared immutable guest input.

The environment source checks the ordinary user-mode existence of the named
historical host command at installation time and copies the resulting enum;
no BOP rechecks process state or accesses a host path. The input ABI advances
to version 2 because the capability is explicit rather than implicit.

## Verification

Two focused MSVC x64 `/MT` closures pass in
`artifacts/build/t217-s12-command-shell-provider-r1`:

1. the configuration provider covers both dispositions, including exact
   original-host output and the declared-guest output with no `SHELL=` line;
2. the installation-source closure compiles and executes with the current
   ordinary user-mode host source path.

The pre-existing broad boot-namespace probe stops at link because its source
list omits three long-standing observation translation units
(`namespace_identity`, BOP sequence and DEM Open). This is retained evidence
of that harness closure gap, not an S12 provider failure.

One fresh full-current-source native integration at
`artifacts/build/t217-s12-command-shell-native-r2` no longer reaches the
former `50:3D` controlled terminal. It reaches 33 BOPs and then stops at the
new `50:36` boundary. The old fixture intentionally asserts the former
`50:3D` terminal, so its nonzero exit is the expected assertion obsolescence;
the build/link and native log are retained. No claim is made for the meaning
or implementation of `50:36`.

## Evidence Correction

S10's default-off observer records the **second ordinal** normal DEM Open.
It continues to show its previous 32-byte/three-component identity in the
new run. That observer has never established that this ordinal Open is the
final command-interpreter load. It is therefore not evidence against S12 and
must not be used to name the final command path. S12's direct integration
evidence is limited to removal of the old `50:3D` branch and arrival at a new
owner boundary.

## Recovery Ladder

| Rung | Disposition |
| --- | --- |
| Original OpenNT source | `cmdconf.c` generation and BIOS/SYSINIT `SHELL=` consumption remain the owner contract. |
| Smallest seam | Retained: copied host-command capability selects whether the original host-only override can be generated; absent mode falls back to the original SYSINIT default. |
| External intrusion | Rejected. |
| Newly authored behavior | Rejected: no BOP/DOS result or substitute command implementation was created. |

The next task is not a leaf `50:36` implementation. It is a complete
DOS-loader/EXEC owner-package map which includes that observed call only as
an integration ordering fact.
