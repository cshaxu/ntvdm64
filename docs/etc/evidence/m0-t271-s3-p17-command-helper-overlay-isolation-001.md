# M0 T271 S3 P17 — COMMAND helper-body overlay isolation

Three remaining `opennt-bop/command` translation units were project-authored
product composition rather than original OpenNT bodies:

- `opennt_command_binary_composition.c` projects source `SCSINFO` into the
  bounded guest address contract;
- `opennt_command_config_composition.c` binds one-session CONFIG/AUTOEXEC and
  bootstrap input; and
- `opennt_command_environment_composition.c` creates the session-owned
  environment snapshot/UI facade used by imported `cmdenv.c`.

P17 moves all three bodies by `git mv` to matching `opennt-bop-overlay`
paths.  The mirror paths retain only registered one-linkage `BOP-DIV-104`,
`BOP-DIV-105` and `BOP-DIV-106` boundaries.  No component or test includes
these overlay bodies directly; original COMMAND provider files continue to
use their unchanged declared source-facing functions.

The related PIF parser composition is deliberately excluded.  `nt_pif.c` is
an independently identifiable OpenNT host capability, not generic COMMAND
glue.  It requires a later whole-package move to `opennt-host`; relocating it
as a local overlay would obscure that owner correction.

Verification was performed outside the sandbox after a formal Ninja relink
in `build/M0-T271-S3/r003`:

- `t231-s5-command-config-direct-fixture.exe` exited zero.
- `t234-s2-command-dynamic-environment-fixture.exe` exited zero.
- `t231-s8-command-lifecycle-direct-fixture.exe` exited zero.
