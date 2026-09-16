# T413 S6 — Terminal resize and parent-CMD handoff investigation

## Owner cancellation

The owner reports the symptom can no longer be reproduced and explicitly
cancels the task, requesting a clean workspace. S6 is cancelled, not repaired
or accepted. Its temporary observer changes are removed back to 58c535e2e;
no product source or deployed EXE was changed. The earlier procedure below is
historical and can be recovered from e40b7a812 if the symptom returns.

## Request and baseline

The requested defect is Windows Terminal maximize/resize during the sequence
run16 COMMAND.COM, EXIT, native CMD DIR, then cooked typing. The owner reports
typed characters jumping approximately three rows above the bottom prompt.
This is not the already-closed divide exception repair.

Baseline 58c535e2e; published S5 EXE hashes match the S5 record. No production
source or executable is changed in this investigation. Installed Terminal is
Microsoft.WindowsTerminal 1.24.11911.0; system conhost file version is
10.0.26100.1. Native Terminal UI observation is unavailable in this session.

## Source checks

Original nt_event.c restores saved input/output modes. ResetConsoleState in
nt_fulsc.c normally leaves window dimensions alone; its old unconditional
startup-size restoration is compiled out. A separate stream-transition branch
can restore geometry, but this audit has not proved it causes the reported
defect. Existing console_compat.c implements the registered cell-grid resize
boundary. Do not add a fixed row correction or assume old geometry is restored.

The neighboring SoftPC Console broker is read-only comparison: it keeps
separate raw/cooked buffers and restores buffer metadata at selection. That
architecture is not imported into this project and does not establish the
cause of this particular CMD/Terminal symptom.

## Procedure and observations

BuildConsoleRegression.cmd build/M0-T413/S6/integration compiles the existing
observer with a new --handoff scenario. It starts a private ConPTY and native
CMD, executes run16 COMMAND, resizes, exits the guest, runs native DIR, pauses
with a partially typed echo command, then exits CMD. A job owns only this
test process tree; no user window is manipulated. Native cell journals and
raw VT output are retained below O:\winnt\logs.

Four baseline runs completed with parent exit zero:

| Log stem | Resize point and observations |
| --- | --- |
| m0-t413-s6-baseline | 80x25 to 120x40 during guest COMMAND; after DIR, cursor (5,39); partial input (18,39). |
| m0-t413-s6-baseline-host | 80x27 to 120x40 after guest exit; after DIR, cursor (5,39); partial input (18,39). |
| m0-t413-s6-baseline-input | Resize while parent CMD waits after DIR; prompt remains on row 26 and typed input is also on row 26. This is not a bottom-prompt/upper-input mismatch. |
| m0-t413-s6-baseline-history | Sixty long history lines before guest startup, resize during guest; after DIR cursor (5,39), partial input (18,39). |

Each stem has .raw and .raw.cells.txt captures. Output mode is 7. These are
negative reproduction results, not repair acceptance. The observer uses system
CreatePseudoConsole and records the server's cells, not Windows Terminal's
frontend screen or its packaged OpenConsole runtime. Its pre-existing fixed
cursor-position response is also not a complete terminal emulator.

## Disposition

Before cancellation, S6 stopped at the reproduction gate. The hypothesis of stale
restored dimensions is unproved, and no speculative product patch is made.
Need the real Terminal frontend before/after-typing state and matching server
state, or a reproducer using that exact frontend/runtime. Owner screenshots
were requested without asking for unrelated EDIT tests. No fresh product
build/publication or completed-repair claim is justified by these observations.
