# T105 S1 post-POST CLI continuation observation 001

## Procedure and immutable result

The unchanged T104 binary
`9433196B03A10E31C85AB90C2BB33D221A51B957098E48BE32B85B839B49D3C9` ran once
with its unchanged deferred profile
`3606D8D0570F4D7C42B0DA181EE81A31F18D92E2377396CFE2AFF65BA771D982`, the
same optional-ROM identity, `-benchmark 10`, and a 30-second watchdog.  The
immutable record is
`artifacts/analysis/t105-s1-post-post-cli-continuation-001`.

The run reached native POST, the opaque handoff, NTIO's ordinary-RAM NTDOS
transaction and the later real DOS/COMMAND path.  It then ended only because
the watchdog stopped it (`exitCode: 1`, `watchdogTerminated: true`), not by a
controlled guest exit or the earlier video-aperture panic.  It recorded
674,104 BOP observations and four bounded commits.

## First stable boundary

The terminal stable identity is guest INT 6 BOP selector `06`, observed
674,072 times at `073B:0740`.  Its stack pointer falls by six bytes per entry;
the copied fault frame continues to identify BOP-shaped bytes.  This is a
re-entry loop, not forward CLI execution.

Existing source evidence already classifies selector `06`: OpenNT's
`spckbd.asm` installs it as the guest INT 6 vector, and SoftPC's original
`illegal_op_int` reads the real-mode fault frame before reporting an invalid
instruction.  Therefore selector `06` is not a service family for which the
adapter may invent a result.  The retained COMMAND bootstrap evidence shows
the same post-`50:36` route can reach `54:02 SVC_CMDCOMSPEC`, whose legitimate
provider begins with a bounded guest gather read.  This one observation does
not itself prove that `54:02` is the first current faulting opcode; the
listener's final report is already the INT 6 handler, so the predecessor must
be established from the common ingress/action control flow rather than guessed
from the selector.

## Decision

The next package is not an INT 6 or BOP-06 handler.  T106 S1 will audit the
existing two-phase action bridge as one shared composition surface: BOP
ingress/registry/plane selection, provider `NEED_READ` and multi-write action
production, ordinary-RAM execution, checked resume, refusal and fall-through.
It will map every currently admitted provider that depends on that bridge,
with COMMAND COMSPEC/bootstrap as its source-correlated exemplar.  No provider
or Bochs behavior is changed until that common bridge is proven absent or
miswired.

