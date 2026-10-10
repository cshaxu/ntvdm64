# T442 S1 mysmb64 Console-identity transition

## Question

Does the owner-provided `mysmb64.exe` runtime target only request visibility
for an inherited carrier Console, or does it change its own Console identity
before it shows that Console?

## Inputs

- Owner-supplied runtime target: `mysmb64.exe` (AMD64 PE32+).
- Its owner-maintained Win32 source modules `main_win32.c`, `text_console.c`,
  and `launch.c`, consulted only as read-only behavioral evidence.
- T442's existing ownership audit and packet stop condition.

No external source was imported, linked, built, or made a product/runtime
dependency.

## Procedure

1. Confirm the supplied binary's AMD64 PE identity and enumerate its relevant
   imported Console/window API names.
2. Trace the matching source's graphical startup, text-mode transition and
   text-mode close paths.
3. Compare that sequence to T442's explicit identity-transition stop
   condition.

## Observations

- The binary imports `AllocConsole`, `AttachConsole`, `FreeConsole`,
  `GetConsoleWindow`, `ShowWindow`, and `SetForegroundWindow`.
- `main_win32.c` frees its Console during graphical startup when it does not
  start in text mode.
- `text_console.c` uses `AttachConsole(ATTACH_PARENT_PROCESS)` when a text
  startup has no current Console; it otherwise may use `AllocConsole()`.
- The text-mode presenter gets the attached/allocated Console HWND, hides the
  graphical HWND, then shows and foregrounds that Console HWND.
- Normal return to graphical mode closes the text Console with `FreeConsole()`
  before re-showing the graphical HWND.

## Interpretation and confidence

High confidence: this is not a request to reveal the original NTVWM carrier.
It is a target-owned Console detach/attach/allocate lifecycle followed by a
visibility request for the newly selected Console. Direct CMD works because
the target's OS parent is attached to the actual outer Console. In the current
worker topology, `ATTACH_PARENT_PROCESS` instead reaches NTVWM's hidden
carrier, which explains why the target exposes that carrier.

The original T442 candidate can correctly route a carrier-visibility intent,
but it cannot safely substitute for this identity transition. Any solution
must be separately admitted with a defined rule for the target's parent/root
Console identity, attachment authorization, failure passthrough, and teardown.

## Follow-up

Do not modify hook, NTSRV, NTVWM, or NTCON code under the visibility-only
packet. Obtain owner approval for a narrowed Console-identity design, then
evaluate only authenticated service-mediated alternatives; in particular, do
not add direct hook-to-NTCON control, target-name branches, HWND/PID inventory,
or Console emulation.
