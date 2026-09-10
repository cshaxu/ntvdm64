# M0 T396 S28 — current WOWEXEC pre-WOW32 live frontier 001

## Fixed run

The current-source x86 Console observer ran the fixed S20 parent and S25
provider for 120,469 ms with the declared command
`-f -o --command system32\\WOWEXEC.EXE`.  It used the existing default-off
reports only.  The product remained live, had 57 loaded modules, and had not
loaded `WOW32.DLL`.

The terminal report records the completed original COMMAND environment request:

```text
MVDM-BOP-DISPATCH 54:0F
MVDM-BOP-RETURN 54:0F cs=05D8 ip=03CF ax=0683 cf=0 if=1
MVDM-CPU-ARPL cs=05D8 ip=1F8F pe=0 vm=0
MVDM-CPU-RM-INT vector=06 source=05D8:1F8F ... target=F000:FF30
```

The no-diagnostics control has the same no-WOW32 live state and host-frame
shape.  Therefore the current frontier is not produced by the default-off
reports.

## Source attribution

Original CPU40 `c_main.c` dispatches opcode `63h` (`ARPL`) to `Int6()` when
`GET_PE()==0`.  Original `c_xcptn.c::Int6` calls
`benign_exception(I6_INT_NR, INTERNAL, -1)`, which selects the real-mode
vector 6 target.  The reported `F000:FF30` target is the BIOS illegal-opcode
BOP path; `bios.c` binds BOP 06 to original `illegal_op_int`.

Original `illegalp.c::illegal_op_int` calls `host_error(EG_BAD_OP,
ERR_QU_CO_RE, ...)`, then, after the panel has returned, advances the saved
fault IP by one byte and calls `unexpected_int()`.  Thus it is an original
visible error-dialog/user-decision boundary.  It is neither an ARPL
implementation failure nor authorization to force protected mode, alter guest
instructions, or invent a BOP result.

## Transfer

S28 closes its observation-only scope at the complete original
`ARPL -> Int6 -> vector 6 -> BOP 06 -> illegal_op_int -> host_error` chain.
S29 may make one predicate-bounded, user-equivalent selection of the existing
original panel's Continue/Ignore control, using the current observer source.
It must prove the exact matched panel and resulting source-owned continuation;
it may not click another dialog, synthesize CPU state, or claim WOW32 load
unless it observes it.
