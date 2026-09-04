# M0 T389 S5 — Console worker and guest keyboard source cohort

## Question

The S4 fixed observation timed out before the default-off BIOS keyboard
`waitio` marker.  This packet determines whether the original Console worker
or guest keyboard route is therefore missing.

## Source-shaped lifecycle

The selected original non-WOW host path is complete and remains selected:

1. `softpc.new/host/src/nt_reset.c:host_applInit` runs `init_host_uis` and
   creates the original suspended `ConsoleEventThread` through
   `nt_start_event_thread`.
2. Guest `COMMAND.COM:tcode.asm` unconditionally invokes
   `SVC_CMDINITCONSOLE` after its keyboard-layout work.
3. The mirrored `dos/command/cmddisp.c` dispatches it unchanged to original
   `cmdInitConsole` in `cmdmisc.c`.
4. `cmdInitConsole` invokes original `nt_init_event_thread` in
   `softpc.new/host/src/nt_fulsc.c`; after the original BIOS LED sync it
   resumes `ThreadInfo.EventMgr.Handle` and calls `KbdResume`.
5. The original `ConsoleEventThread → nt_event_loop` uses the real Console
   input wait handle and `ReadConsoleInputExW`, converts `KEY_EVENT` records
   with original `nt_key_down_action`/`nt_key_up_action`, invokes original
   keyboard-controller callbacks, and wakes the original idle state.
6. Original BIOS `keyboard_io` emits the optional `waitio` witness only once
   the guest has already entered its `AH=2` idle/readiness operation.  It is a
   downstream guest-state marker, not the mechanism that starts the worker.

The selected S4 fixed report contains an actual `54:05` dispatch and return,
with original COMMAND call stages.  It also contains the subsequent original
`54:0E` keyboard-layout service.  Thus the product reached the exact original
console-initialization boundary; it did not take an app/session line reader
or a synthetic input injection path.

## Disposition

There is no missing Console-event-worker, keyboard-controller, IRQ1 or
DOS-CON *binding* to recover in this cohort.  The absence of the later
`MVDM-COMMAND-INPUT-READY` marker proves only that the guest has not reached
the second child shell's `Do16BitPrompt → INT 21h/AH=0Ah` path yet.

Adding an app-side Console parser, injecting keys before the original BIOS
wait, or forcing the worker to consume input would violate the selected
source ownership.  S5 is therefore closed as a negative source-cohort result:
the next investigation belongs to the original post-`54:01` parent/child
COMMAND lifecycle before `DoReEnter`, not to input delivery.
