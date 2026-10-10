# M0 T443 S1 — project-owned special-case contract ledger

## Scope and method

This record freezes the first production audit of the ten mechanisms listed in
the admitted [proposal](../../proposals/proposal-project-special-case-contract-repair-001.md).
It is a source-and-build-graph audit, not a claim that every listed risk has
already reproduced at runtime.  The inspected production package baseline is
the published T442 lifecycle release, `44c25e138`.

The governing rule for this packet is deliberately narrow: a source spelling
or an old regression witness is not enough to remove a branch.  Each item must
have a reachable caller, a provider/consumer contract, provenance, and a
positive plus negative witness before a repair is admitted.

## Reachability and disposition

| ID | Production chain and provenance | Current disposition |
| --- | --- | --- |
| H01 | `cmdCreateProcess` reaches the project adapter `opennt_command_create_process_a`; its `COMSPEC /c` branch calls `opennt_command_launch_vdm_child`. `opennt_command_simple_shell_tail` then selects direct `run16` versus an extra DOS COMMAND wrapper by searching `|&<>`. The current form is project-owned; the character heuristic was introduced by `62a3720e77`, not imported COMMAND code. | Coupled with H02/H03. Do not remove independently: it currently preserves DOS COMMAND ownership for composite syntax. No complete quoted-symbol/batch/nested-runtime matrix exists. |
| H02 | The same adapter additionally recognizes only bare `COMMAND.COM /c`. It is in the same project commit and participates in the same routing decision. | Coupled with H01/H03. The actual COMSPEC parser at the entry boundary already compares the configured COMSPEC path; this later bare-name recognition is an inconsistent second identity rule. Requires one explicit internal command-tail contract rather than more aliases. |
| H03 | `run16` removes transport quotes only when the selected file has DOS type, basename `COMMAND.COM`, exactly three argv entries and `/c`. The code is project-owned (`62a3720e77`). | Coupled with H01/H02. Its narrow condition prevents arbitrary programs being rewritten, but it duplicates interpreter identity and argv encoding in a second component. A repair must preserve the original tail and leave shell grammar to COMMAND. |
| H04 | Failed image classification enters the documented `COMSPEC /c` fallback in `run16`; this supplies command built-ins/batch/shell text to the public host shell. | Retain pending error taxonomy proof. This is a real fallback with a stated external shell contract, not yet evidence of a fabricated success. A negative matrix must distinguish missing, access-denied, malformed image and an intentional shell request before changing it. |
| H05 | Both Hook32 and Hook64 compile the same `src/nthook32-dll/create_process.cpp`. `legacy_application` resolves a candidate for classification. When it is *not* legacy, `hooked_w` still passes `resolved` rather than the caller's `application` to the real `CreateProcessW`. | First proven bounded repair candidate. It changes normal native CreateProcess selection from caller/Windows semantics to the project CWD/PATH resolver even though no redirect occurs. The repair is to pass the original application for non-redirect calls and to preserve the original legacy request for `run16`; the resolved candidate remains local classification evidence only. |
| H06 | `command_tail` extracts argv[0] using a small quote/whitespace parser, but only to find a legacy candidate when `application == NULL`, and to construct the already-approved legacy redirection. | Defer with H05's test expansion. It is not currently used for an unredirected native create. A complete Windows command-line parser here would be a new compatibility framework, which this packet forbids. |
| H07 | The adapter dynamically calls `NtAlertThread`; only when that export is absent does it use `QueueUserAPC`. | Retain with explicit limitation. Modern production NTDLL exports `NtAlertThread`, so the fallback is not on the normal package path. A no-export test is needed before deciding whether a non-equivalent fallback must fail instead. |
| H08 | WOW font tracking deliberately uses host Windows font discovery (`GetWindowsDirectoryW`, fonts directory, then `SearchPathW`) before tracking non-local font resources. | Defer to the queued WOW font owner package. This is an adapter for a host-GDI contract, not proven guest-root leakage. No change in this T without a source-backed conflict witness. |
| H09 | `common_resolve_application` is explicitly native-capable (32,768 characters), while the Hook token and `run16` path buffers are `MAX_PATH`. | Defer with H05/H06 boundary testing. The inconsistent capacity is real, but widening the hook token parser before establishing its exact CreateProcess boundary would hide the needed failure behavior. |
| H10 | The NTVDM text publisher converts `ERROR_NOT_READY` to success only after a completed handoff can reject a stale old-owner frame; all other errors propagate. | Retain as a justified stale-publication boundary. It is not receiver-side de-duplication and prevents an old worker from painting a new owner. Existing handoff/final-frame tests are the positive basis; a new distinct real-not-ready negative is required before any change. |

## First implementation slice

H05 is selected for the next implementation slice because it has a local,
observable breach independent of the command-shell redesign:

1. Preserve the caller's `lpApplicationName` exactly on every non-legacy
   Hook32/Hook64 path, including `NULL`.
2. Continue using the shared resolver only to classify a confirmed legacy
   DOS/Win16 request. Pass that original request to `run16`, which repeats the
   same shared resolution rather than consuming Hook-private final-path data.
3. Add a focused Hook regression in which classifier discovery finds a DOS
   candidate but a private launcher verifies that it receives the caller's
   original request, not the Hook's resolved absolute candidate. Native W/A
   identity preservation is checked at the single real-call boundary.
4. Exercise the same source as both `nthook32.dll` and `nthook64.dll`; do not
   fork either Hook implementation.

This repair does not alter guest bytes, original OpenNT code, NTSRV/NTCON
control protocols, or DOS COMMAND semantics. H01--H03 remain a separate,
coupled shell-boundary design problem; treating H05 as a reason to rewrite
them would violate this packet's stop condition.

## Required verification before H05 delivery

- build both Hook images from the one shared source;
- positive legacy-original-request witness and existing suspended-child rollback
  checks;
- unchanged caller flags, directory, environment and process-result handling;
- focused Hook install tests for both bitnesses; and
- an adjacent audit confirming no second non-redirect `resolved` substitution
  remains.

No production code has changed in S1.
