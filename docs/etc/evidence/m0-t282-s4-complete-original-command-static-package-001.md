# M0 T282 S4 — complete original COMMAND static-package evidence

## Question

Can all translation units selected by the original COMMAND `sources` file be
compiled and archived as one source-identity-preserving package on both target
host architectures, without enabling COMMAND runtime behavior?

## Inputs and procedure

- Used the exact 11-unit `dos/command/sources` list and the T282 S1 source
  ledger.
- Regenerated `New-T282OriginalCommandSurfaceNinja.ps1` under the independent
  `build/M0-T282/S1/x64` and `build/M0-T282/S1/x86` roots.
- Outside the sandbox, loaded the matching MSVC developer environment and ran
  `ninja -v` for each graph.
- Listed both `original-command-surface.lib` archives with `lib /list` and
  checked their graph inputs against the complete original list and forbidden
  runtime/machine inputs.

## Observations

- x64 and x86 each produced `original-command-surface.lib` with exactly:
  `cmd`, `cmddata`, `cmddisp`, `cmdexec`, `cmdexit`, `cmdmisc`, `cmdpif`,
  `cmdredir`, `cmdconf`, `cmdkeyb`, and `cmdenv`.
- The x86 rebuild emitted pre-existing source warnings for old declarations,
  CRT calls, widths and uninitialized local analysis; it compiled and archived
  all eleven units. The x64 graph had already rebuilt the focused COMMAND
  units under the same flags and then archived the full member set.
- The generated graph contains neither `adapter-bop`, `bochs-core`,
  `adapter-bochs`, `nt_bop.c` nor an application executable input.
- Static archiving does not resolve or call historical external functions.

## Interpretation and follow-up

This closes original COMMAND source composition, not COMMAND execution. The
selected archive contains unchanged source bodies and declaration bindings
only. `GetNextVDMCommand`/monitor, child lifecycle, stream/Redirector, PIF,
keyboard, registry, checked guest-memory and session policy remain the named
callable-boundary packages already recorded in the S1 ledgers. No service is
routed and no guest trace is claimed.
