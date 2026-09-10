# M0 T387 S1 — positional COMMAND declaration

## Question

Can ordinary host argv declare one DOS command without exposing the app's
previous `--command` validation spelling, while retaining the original
BaseClient/BaseSrv/COMMAND command-record path?

## Rule

The first non-SoftPC option and every following argv item are the declared DOS
command. Earlier `-` or `/` options remain original SoftPC argv. A sole
position argument is copied as its declared text; multiple items are joined
with DOS quotes around whitespace-bearing arguments. Embedded quote or newline
items are rejected rather than parsed by app. `--command <text>` remains an
explicit equivalent spelling and cannot be mixed with a positional command.

Because original `nt_reset.c::host_applInit` accepts normal NTVDM launch only
when argv contains `-f`, app creates a private forwarded argv vector and adds
that original foreground marker exactly once if absent. This restores the
historical BaseCheckVDM/CreateProcess launch precondition without changing the
original SoftPC parser or requiring a user-facing switch.

## Verification

- The focused x86 `declared_dos_record_fixture.exe` passed. It verifies
  `fixture -o echo "hello world"` becomes the declaration
  `echo "hello world"`, while only `fixture -o -f` reaches SoftPC.
- The formal CPU40/x86 `original-softpc-process.exe` linked.
- The fixed console-owning stage ran the positional-only invocation
  `-f -o "echo M0T387 > M0388.TXT"`. It exited zero and wrote
  `O:\\ntvdm64\\M0388.TXT` with `M0T387 <space>\\r\\n`.
- Its BOP sidecar reached original `50:03`, `50:43`, `50:1E`, and `50:02`.

## Scope

This is a CLI declaration translation only. It does not make app a DOS parser,
does not implement an interactive no-argument session, and does not prove
external DOS/host/Win16 execution.
