# Single-process interactive COMMAND console recovery

## Purpose

Recover the original interactive `COMMAND.COM` experience in one
`ntvdm.exe` process without altering guest media.  A no-argument invocation
must start an original interactive COMMAND session, not an application-owned
command parser.  A positional program remains an original
`COMMAND.COM /C <program>` launch.  The product must retain the distinction
between an initial DOS command record delivered by the host and ordinary
keyboard input subsequently consumed through DOS `CON`.

This is the first candidate after the active M0 T387 package.  It receives an
M0 T number only when it is admitted into `docs/STATUS.md`.

## Source and ownership model

- Original MVDM/Base VDM code owns initial-record and re-entry semantics:
  `BaseCheckVDM`, `VDMINFO`, `srvvdm`, `cmdGetNextCmd` and
  `cmdGetStartInfo` are the primary source evidence.
- Original SoftPC owns guest keyboard-controller, interrupt and guest-memory
  behavior.  The host must reach it through the same-shaped machine boundary;
  `app` must not parse DOS commands or write guest input buffers.
- `app` owns process startup, public Console handles, a presentation-window
  host surface and product diagnostics.
- `session` owns the one-process lifecycle, cancellation, host-resource
  identity and input-pump lifetime.
- Private BaseSrv/CSRSS transport, cross-process delivery and WOWEXEC
  notification are excluded.  They remain the later broker/WOW work, not a
  reason to invent a second interactive parser here.

## Product contract

1. `ntvdm.exe` with no declared program publishes the original source-shaped
   interactive initial DOS record and waits for the original COMMAND prompt.
2. `ntvdm.exe edit.com` publishes the original `/C edit.com` declaration.
   Running `edit.com` after reaching the interactive prompt is ordinary DOS
   keyboard/COMMAND behavior; it must return to that prompt through original
   guest execution paths.
3. Normal character-mode DOS has exactly one guest-facing surface: the
   process Console.  Project diagnostics never share it.
4. A presentation window is created only when the original machine/PIF state
   selects graphics/fullscreen or the user requests Alt+Enter.  Alt+Enter back
   closes that window and restores Console routing without inventing a text
   frame for a graphics-only guest.
5. Public Win32 Console input is converted into source-shaped SoftPC keyboard
   operations and IRQ1.  The application does not translate input into
   COMMAND text, BOP records or guest-memory writes.

## Planned S sequence

### S1 — Original interactive startup-contract audit

Map the complete original `BaseCheckVDM`/`VDMINFO`/`srvvdm` and
`cmdGetNextCmd`/`cmdGetStartInfo` sequence.  Record every field and result
that distinguishes an initial interactive record from a `/C` record, including
new versus existing console, current directory, environment, re-entry and
failure behavior.

Exit: a source-path/field/result ledger identifies one owner for every reached
field; no implementation change or synthetic record exists.

### S2 — Interactive initial DOS-record binding

Bind the audited record to the existing local Base VDM route.  Preserve the
current positional declaration path as `/C`; do not reinterpret prompt lines
as `GetNextVDMCommand` records.

Exit: local positive, repeat/re-entry and unavailable tests prove copied
record lifetime and failure behavior.  No guest binary is changed.

### S3 — Console-first startup and diagnostic isolation

Recover the original SoftPC stream/Console ordering needed by the interactive
route.  Default character mode must not create the presentation window.  Move
project diagnostics to explicit report files or debugger output so Console
input/output remain guest-owned.

Exit: source/build evidence and a bounded process check prove a single
guest-facing Console before graphics/fullscreen transfer; diagnostics do not
pollute it.

### S4 — Console input through SoftPC keyboard and IRQ1

Recover and verify the original SoftPC public-Console event worker, which
already owns `ReadConsoleInput`-style acquisition, scan-code conversion,
keyboard-controller insertion and IRQ1 delivery.  Its worker must carry a
bounded session binding and must be alertable and joined during teardown.
`app` must not create a second input pump, parse DOS lines, synthesize BOP
records or write guest input buffers.  Use the session mapping manager only
where an identity actually crosses a guest or original-ABI boundary; a
process-local Console worker HANDLE is not a guest handle.

Exit: source and focused checks cover ordinary keys, Enter, Backspace, Ctrl+C,
repeats, alertable cancellation and joined teardown; the app contains no
DOS-line parsing, direct guest-buffer write or parallel input pump.

### S5 — Display-backend arbitration

Implement the single-active-surface state machine.  It selects Console for
text and the app presentation window only for original graphics/fullscreen,
PIF fullscreen or Alt+Enter.  It transfers focus and keyboard ownership and
returns to Console exactly as stated in the product contract.

Exit: local state-transition tests cover text, graphics, Alt+Enter in both
directions, a window-only result and close/cancellation behavior.  File-name
or extension guesses are prohibited.

### S6 — Original PIF fullscreen disposition

Audit and bind the original PIF fields and failure ordering relevant to
Console/fullscreen choice.  Any unavailable private-shell state must retain a
source-shaped unavailable result rather than silently selecting a window.

Exit: text/fullscreen/unavailable inputs have an original-owner disposition
and focused tests; no generic PIF rewrite is introduced.

### S7 — Fixed interactive runtime matrix

Use one formal EXE and one immutable runnable root to observe a single
interactive COMMAND session: prompt, `ver`, `dir`, editing, Ctrl+C and
`exit`.  `ntvdm.exe edit.com` and prompt-local `edit.com` are integration rows
only; an external execution failure reopens its immediate original owner, not
the Console package by default.

Exit: each row has product/media identity, bounded command procedure and
exact observed owner result.  No debugger-dependent or changed-container
result is accepted.

### S8 — Closure and publication

Audit all routes and default diagnostic paths, run the admitted formal Ninja
targets and fixed-container checks, then publish only a verified improvement
to both `build/output` and `O:\\ntvdm64` with hashes and retained media
identity.

Exit: no-argument interactive launch, positional `/C` launch and input/display
ownership are all runtime-proven or explicitly source-shaped unavailable.  An
unverified or regressed executable is not published.

## Explicit exclusions

- No modification to `COMMAND.COM`, NTDOS, NTIO or any other guest media.
- No app-owned DOS parser, synthetic DOS loader, artificial BOP result or
  direct guest-memory input injection.
- No CSRSS/BaseSrv transport reconstruction, cross-process broker, WOWEXEC
  command delivery, kernel VDM or private USER/GDI implementation.
- No executable-type heuristic as a replacement for original video/PIF state.

## Dependencies and handoff

This package follows T387 only after its positional CLI publication has an
honest direct-runtime result.  It precedes `cmdExec32` stage-to-return
recovery.  Redirector, native-child and WOW work remain separate owner
packages; they may supply later integration workloads but do not block the
source and local closures above.
