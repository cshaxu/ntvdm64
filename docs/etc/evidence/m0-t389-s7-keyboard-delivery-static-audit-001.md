# M0 T389 S7 — CPU40/SoftPC keyboard-delivery static audit

## Question

Does the accepted Console key in the S6 probe prove a broken keyboard path
which S7 may repair by changing interrupt state, or does original source
require a more precise owner before any change?

## Source comparison

The selected original sources are
`O:\repos.external\opennt\base\mvdm\softpc.new\base\{keymouse\keyba.c,ccpu386\c_main.c}`.
The current mirror was compared against them at the complete functions reached
by the fixed probe.

* `nt_event.c::nt_key_down_action` retains its original
  `KeyMsgToKeyCode -> host_key_down_fn_ptr` call.  The only added statement is
  a default-off scalar diagnostic immediately before the unchanged callback.
* `keyba.c::filtered_host_key_down`, `codes_to_translate`,
  `continue_output`, `KbdIntDelay`, `KbdEOIHook`, `do_q_int` and `kbd_inb`
  retain their original branch/order semantics.  Their effective differences
  are default-off scalar observation calls.  The sole non-observer nearby
  difference is the unrelated CPU-reset binding, not the keyboard IRQ path.
* The original keyboard sequence is:

```text
Console key -> filtered_host_key_down -> 6805 queue
            -> continue_output -> do_q_int
            -> output_full = TRUE -> KbdIntDelay
            -> ica_hw_interrupt(adapter 0, line 1, request 1)
            -> CPU_HW_INT_MASK -> CPU40 GET_IF gate
            -> ica_intack -> guest IRQ1 frame -> port 60 read -> EOI hook
```

* `c_main.c` retains the original service predicate:
  `GET_IF() && CPU_HW_INT_MASK`.  The selected mirror changes its shared event
  bitmap to atomic snapshot/raise/consume operations, but does not substitute
  `GET_IF`, clear an interrupt while `IF` is clear, choose an ICA vector, or
  change the original interrupt-frame call.

## Fixed observation reconciliation

The S6 probe proves the path through `do_q_int` and `ica_hw_interrupt`:

```text
MVDM-KBD-OFFER accepted=1
MVDM-KBD-GATE stage=2 eoi=0 int=1 full=1 disabled=0
MVDM-KBD-ICA-REQUEST irr=03 isr=00 imr=18
MVDM-CPU-HW-INT-DEFERRED if=0
```

It has no `MVDM-KBD-PORT60` record.  That absence is consistent with the
unaltered original rule: port 60 is read from the guest IRQ1 path only after
CPU40 can acknowledge the pending interrupt.  It is not evidence that the
observer may set IF, manually read port 60, add a synthetic IRQ, or write a
DOS keyboard buffer.

The prior stopped-frame sample was also reconciled against the source map.
Its host frames are the original CPU40 and SoftPC idle path.  The guest
`FE2E:` offsets seen around the deferred events correspond by NTDOS map offset
class to normal NTDOS file/path routines (`GetCharType`, device open, `$READ`,
and pathname processing), not to a proved `COMMAND.COM` prompt frame.  The
runtime relocation is not reconstructed from this snapshot, so this is a
classification result rather than a symbol-level claim.

## Disposition

S7 must not implement a keyboard/CPU semantic change from the current trace.
The key is accepted and the original machine has correctly retained it pending
the source CPU IF condition.  Before a repair is admissible, S7 must establish
which source-owned NTDOS/COMMAND continuation is holding IF clear and whether
that state is transient, an original lifecycle precondition, or a CPU40
instruction/frame defect.  The next evidence must therefore connect the
post-`54:04` NTDOS continuation to the first relevant IF transition; it cannot
be selected from the keyboard marker alone.
