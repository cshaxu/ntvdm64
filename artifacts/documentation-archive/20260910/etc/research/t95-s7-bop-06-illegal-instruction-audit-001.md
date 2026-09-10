# T95 S7 BOP 06 Illegal-Instruction Audit 001

## Runtime fact

The sole r14 trace (`013`) reaches the admitted `66` endpoint at
`8dc8:79c5`, commits only low `BX=0`, and resumes at `79c8`.  It observes no
`67` or `68`.  The next reached identity is repeated `C4 C4 06` at
`073b:0740`; the watchdog record has 418,779 BOP observations and four prior
transactions.  The binary is SHA-256
`746BF963D2D89E55C6340AC2F7B97656B69A2BEA6657F76669FD41482BA5FB6D`.

## Source correlation

`dos/v86/doskrnl/bios/spckbd.asm:1053-1055` installs the guest INT 6 vector
as BOP `06` followed by `iret_com`.  The catalogue's `illegal_op_int` owner
is therefore exact.  `softpc.new/base/system/illegalp.c:79-125` shows the
historical handler reads the faulting `IP` and `CS` from the real-mode
`SS:SP` frame, reports an invalid instruction (with a 386-prefix distinction),
then guesses a one-byte skip before forwarding to the unexpected-interrupt
path.

## Decision

`06` is evidence of an earlier unhandled CPU instruction, not a host service,
and it must not be resumed as a successful BOP.  Implementing its historical
continue behavior would discard the fault address and silently corrupt guest
control flow.  It also cannot be treated as an EMS continuation: r14 proves
the source-defined `BX=0` route avoids `67` and `68`.

The next diagnostic obligation is to expose the existing copied `SS:SP`
context needed to locate the pre-vector fault frame, then correlate that
address and opcode to OpenNT/NTIO source.  Any memory observation must remain
generic, bounded, and read-only; it must not decode BOP `06` or alter CPU
state in Bochs.  No new service endpoint or machine device is admitted by
this audit.
