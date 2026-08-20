# M0 T231 COMMAND owner-package plan

## Scope

T231 recovers all seventeen OpenNT COMMAND BOP services, `54:00` through
`54:10`, as one Direct-only owner package.  It preserves the original
`CmdDispatch` ordering, original source-file ownership, guest layouts and
documented success/failure behavior.  Retained `bop-v1` code is comparison
material only and cannot be a product route.

## Ordered subtasks

| S | Owner-package work | Completion evidence |
| --- | --- | --- |
| S1 | Import/mirror manifest; full service, ABI, host-dependency and v1 map. | All 17 rows classified; source/shim/deletion plan reviewed by the formal source-first ladder. |
| S2 | Simple query/bootstrap group: `54:02`, `54:04`. | Original-shaped Direct local tests for success and original failure. |
| S3 | COMMAND registration/context group: `54:03`, `54:05`. | `SAVEWORLD` and `CMDINFO` layouts round-trip only through checked guest-memory and fixed-width records. |
| S4 | Handle/console/layout group: `54:06`, `54:09`, `54:0E`. | Host-handle token, console and keyboard-layout contracts pass x86/x64-safe local fixtures. |
| S5 | Environment/startup/configuration group: `54:0C`, `54:0D`, `54:0F`, `54:10`. | Original CONFIG/AUTOEXEC preprocessor, environment and startup transforms, bounds and failures are exercised. |
| S6 | Binary-admission group: `54:07`. | Original extension/path/error distinctions and profile disposition are exercised. |
| S7 | Command-source/session group: `54:01`. | CLI command source provides the recovered original command-queue/re-entry contract. |
| S8 | Host-child lifecycle group: `54:08`, `54:0A`, `54:0B`. | Launch, wait, exit code, standard-handle restoration and guest resume form one tested state machine. |
| S9 | Controlled session exit: `54:00`. | Original exit/cleanup semantics map to the declared CLI controlled-stop contract. |
| S10 | Whole-package closure. | All 17 local regressions, no product COMMAND-v1 reference, formal source build and one bounded native observation. |

## Import rule

The exact OpenNT COMMAND source files are staged under
`src/bx-vdm/bop/opennt/command/` before use.  They remain normal source files,
not a large commented-out fork.  A file joins the formal build only when its
smallest required neutral shim exists.  Every required edit in an imported
translation unit carries a `DIVERGENCE:` comment explaining the unavailable
historical composition dependency and the preserving shim.

## Atomic clusters

`54:08`, `54:0A` and `54:0B` are one lifecycle state machine and must not be
implemented as disconnected service patches.  Likewise, `54:03` and `54:05`
share the COMMAND guest-context registration boundary.  The S grouping is
therefore the minimum coherent unit, not a trace-determined leaf backlog.

`54:0C` and `54:0D` are not simple pathname queries: original `cmdconf.c`
reads PIF-configured inputs, creates transformed temporary files and extracts
environment data.  They remain coupled to environment/startup recovery in S5.
