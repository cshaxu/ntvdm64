# T473 planned S12: NTCON input-source handoff

The owner inserts this stage after S11; it is planned, not the active S.
The former S12–S19 become S13–S20. S11 and the remaining T objective retain
their existing scope. Admit this packet in CURRENT only after S11 concludes.

## Observed problem and evidence

In one DOS worker, `run16 command` → Ctrl+Alt+F (CAF) → CAF → CAF can leave
keyboard input ineffective on the second entry to Window. The equivalent
`run16 cmd` sequence is the control. The supplied focused observations report
5/5 normal DOS runs when a complete CAF arrives tightly; delaying the third
CAF's modifier releases by 20 ms reproduces DOS failure 5/5, while CMD works
5/5. Read-only sampling of the test worker showed Ctrl and Alt remaining down
after the delayed releases and clearing after compensating releases. Separate
single-modifier controls also reproduced the corresponding retained state;
their F records still carried full CAF flags. Pre-switch compensating releases
produced DOS and CMD success in 5/5 each. These are simulated-event,
test-specific observations, not production-fix or physical-keyboard proof.

Review the probe construction, worker identity, installed binary and symbol
correspondence in `tests/observation/console_terminal_observer.c`,
`tests/observation/caf_keyboard_probe_hook.c` and
`build/t473-s11/caf-three-diagnosis/` before relying on the evidence. Working
hypothesis: a Console key-down already delivered to the worker can outlive
its source when the key-up arrives after Console → Window; inactive-source
filtering can discard that release. CAF F debounce also needs a complete
source-transition lifecycle. Do not infer that the native CUI consumption path
is immune to all related defects.

## Scope and required contract

Before implementation, trace the exact queue, locks and ownership transitions
through `src/ntcon-exe/frontend_session.c`, `window_keyboard.c`,
`console_channel.c`, `src/ntvdm-exe/win32/console_client.c` and the original
DOS input/scan-code handlers `nt_event.c` and `nt_keycd.c`. Distinguish
observing, enqueuing and delivering a record from Peek, actual consumption,
`ReturnUnusedKeyEvents` and prepend; preserve rollback and order.

Establish one coherent, symmetric Console / Window source-lifecycle contract:

1. Track Console key-downs actually delivered, with correct scan-code and
   extended-key identity, left/right Ctrl and Alt, repeats and records with no
   scan code. Do not synthesize a release for a down never delivered.
2. Between retiring old Console input and admitting Window input, release its
   delivered but still-down keys in order, exactly once: after their downs and
   before new-source input. Do not let late old-source releases affect keys
   belonging to the new source. Handle CAF F debounce, focus loss, failed
   transitions, rollback, owner changes and exit.
3. Apply the corresponding reset/retire contract Window → Console and
   integrate the existing Window key ledger without competing authorities.
4. Preserve `INPUT_RECORD` character, scan code, modifiers, repeat count,
   ordering and non-keyboard events; ordinary Ctrl/Alt combinations must keep
   working. The common frontend contract applies to NTVDM and NTVWM while
   retaining their actual consumption differences. No DOS-, modifier-, or
   CAF-only workaround, guest change or guest rebuild.

As read-only design comparisons, inspect the current SoftPC implementations:
`src/lib/console-broker/win32/console.c`, `src/lib/console-broker/console.c`,
`src/lib/kvm-console/console.c`, `src/emulator/session/control.c`, and
`src/lib/kvm-base/hotkey.c`. Check source policy and actual upstream version;
these are not runtime dependencies or evidence that every upstream timing has
been validated.

## Required validation and delivery

Use isolated, test-owned processes/desktops and probes; no computer use.
Demonstrate the original failure on an identified pre-fix package and bind
post-fix results to the actual tested package. Label simulated input and
physical manual tests separately; do not claim either from the other.

Exercise real DOS `run16 command` and Win32 `run16 cmd` product chains for
three and repeated CAF switches, verifying actual input on first and later
Window entries. Cover modifier releases before, during and after switching;
independent Ctrl and Alt, left/right modifiers and focus loss. Cover ordinary
Ctrl+C, Ctrl+letter, Alt combinations, held normal keys, repeats, ordinary and
mouse input. Verify late and reordered events, unconsumed queues, Peek,
prepend / ReturnUnusedKeyEvents, owner handoff, exit and error rollback.

Complete the repository-required build, appropriate full and focused gates,
coherent publication, review, commit and push. Report unexecuted physical or
other acceptance separately; compile and unit tests alone do not establish a
complete fix. Preserve unrelated work and do not interrupt S11's running work.
