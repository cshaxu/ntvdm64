# COMMAND standard-stream and pipe recovery — merged into M0 T420 S34

> **Status (2026-09-18):** the owner merged this candidate into active
> `M0 T420 S34`.  It is no longer a queue entry or separately admissible T
> package.  Its design and acceptance rows are retained as S34 scope.

## Objective

Recover the original standard-stream delivery and COMMAND redirection contract
for the standalone three-program product.  The product must support host
shell-provided files and pipes when `run16.exe` is launched directly, and DOS
COMMAND-owned file and pipe redirection after entering one or more nested
`COMMAND.COM` sessions.  This is a COMMAND/BaseClient/BaseSrv resource-binding
recovery, not a VDMREDIR network capability.

## Required behavior

The admitted task proves all of the following on the published x86 package:

1. A host shell parses `run16.exe COMMAND.COM /C VER >> O:\out.txt`; `run16`
   receives an inherited stdout file attachment, the worker starts with its
   Console identity intact, and the expected text reaches the file.
2. A host pipe such as `run16.exe COMMAND.COM /C VER | find "MS-DOS"` transfers
   bytes through the ordinary inherited stdout pipe without turning the pipe
   into a Console handle.
3. In an interactive first COMMAND session, `VER >> O:\out.txt` and a bounded
   COMMAND pipe execute through the original guest COMMAND parser and DOS
   handle rules.
4. The same rows remain correct after a second and then a third nested
   `COMMAND.COM` invocation; each child returns to its still-usable parent
   prompt.

The exact COMMAND syntax may use the guest media's documented grammar.  The
task records the selected commands and expected byte/file results rather than
inventing a parser in `run16`.

## Source-first design boundary

`VDMREDIR.DLL` owns BOP 7 network redirector services (named pipes, mailslots,
NetBIOS and RAP-facing interfaces).  It does not own the `>` or `|` grammar.
The latter remains with the original COMMAND/DOS handle implementation.

The recovery must preserve this split:

| Owner | Required responsibility | Excluded responsibility |
| --- | --- | --- |
| `run16.exe` | Capture inherited standard-handle attachments and the caller's Console identity before submitting the launch. | Parsing DOS redirection or pipe grammar. |
| `basesrv.exe` | Carry authenticated, typed stream attachments with the original command record and duplicate them only at the source-shaped update boundary. | Treating a file/pipe as a Console or owning COMMAND policy. |
| `ntvdm.exe` | Bind its process-local Console input/event context separately from the three standard streams, then enter original worker/COMMAND code. | Reconstructing host handles from raw values or implementing a second shell. |
| Original COMMAND/DOS sources | Parse `>`, `>>`, `<` and `|`, create/duplicate/restore DOS handles and report original failures. | Broker protocol or host process policy. |

No native handle, guest pointer or Console pseudo-handle may appear in broker
records.  A file or pipe crosses only through the existing authenticated
OS-managed attachment mechanism.  Console membership is an independently
captured identity/binding: redirecting stdout or stderr must never cause the
worker to lose input, screen/event access, or its parent-session association.

## S decomposition

| S | Whole unit | Exit evidence |
| --- | --- | --- |
| S1 | Trace and freeze the existing run16/BaseSrv/worker standard-handle and Console-membership route, including direct and nested COMMAND consumers. | Exact original-owner/binding ledger; reproduce the current direct-host failure without changing behavior. |
| S2 | Recover independent typed stream attachment versus Console binding across launch, registration, update, failure and disconnect. | Focused file/pipe/Console positive and negative tests; no raw handle crosses IPC. |
| S3 | Recover the reached original COMMAND/DOS redirection and pipe lifecycle, including nested COMMAND return and DOS-handle restoration. | Guest file and pipe rows pass at first, second and third COMMAND depth. |
| S4 | Integrate and close. | Fresh formal x86 product; direct and interactive COMMAND/MEM/EDIT regressions; all required direct/nested stream rows; deployed package evidence and clean P. |

## Non-goals

- No VDMREDIR network/RAP enablement or new redirector protocol.
- No guest-media patch, custom `cmd.exe` implementation, generic handle
  forwarding service, CPU30 monitor, WOW workload repair or shell syntax
  invention.
- No claim that host `cmd.exe` grammar belongs to guest COMMAND: the task tests
  each layer at its actual parser boundary.

## Admission and closure conditions

The preceding admission timing applied while this was a queue candidate.  It
was superseded by the owner's 2026-09-18 merge into active `M0 T420 S34`.
Before implementation,
record the current exact hang/zero-byte observation separately from successful
rows.  Closure requires source/diff review, x86 compilation, focused
attachment/COMMAND tests, the established direct and nested COMMAND/MEM/EDIT
matrix, deployment to `O:\winnt`, governance checks, commit, push and a clean
worktree.
