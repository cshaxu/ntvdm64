# M0 T282 S7 — original COMMAND initialization composition

Recover the first directly composable COMMAND owner slice as the exact
original `dos/command/cmd.c` and `cmddata.c` bodies. The slice validates the
original `CMDInit(argc, argv)` call and original global initial state on both
host architectures; it does not enable a BOP route or claim that the whole
COMMAND lifecycle is running.

## Inputs

- `O:\repos.external\OpenNT\base\mvdm\dos\command\cmd.c`, SHA-256
  `9d0a773fcc8d7765b0fe3fe3750b5c9a29a596362f78a221161318fd83cc0262`.
- `O:\repos.external\OpenNT\base\mvdm\dos\command\cmddata.c`, SHA-256
  `937c134e46b5c714b9106d85fd0250b88fca5958e69a1e60fdeab558d4f4f8d2`.
- T282/S1 complete original COMMAND archive and T282/S6 local command
  acquisition closure.

## Scope

- Compile and link the exact mirrored `cmd.c` and `cmddata.c` bodies against
  the existing declaration carriers and public `GetSystemDirectory` API.
- Verify original global initial values and the `CMDInit` current system-drive
  assignment under x86 `/MT` and x64 `/MT`.
- Record source identity and prove no mirror-body divergence.

## Non-goals

- No BOP/`54:xx` ingress, guest trace, command dispatch, `cmdGetNextCmd`,
  process launch, console, PIF, broker, WOW, SoftPC executor or Bochs input.
- No modification of either original mirror body, no new adapter, and no
  environment or current-directory policy beyond the original `CMDInit` body.

## Exit criteria

The focused x86/x64 Ninja fixture links and executes the two exact original
objects, checks their source hashes and source-visible state, accepts only the
documented exact-source K&R/unused-parameter warnings from `cmd.c`, and passes
documentation governance plus `git diff --check`.
