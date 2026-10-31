# S11 V86 software INT with REP prefixes

The preceding DIV-490 baseline excluded `F2` and `F3` prefixes from V86
`INT imm8`, although NT4's `OpcodeGenericPrefixV86` dispatches REP prefixes
to `OpcodeINTnnV86` without repeating the interrupt or changing its frame.
A native x86 control on this host independently observes breakpoint exceptions
for `CD 03`, `F2 CD 03`, `F3 CD 03`, and `67 F3 CD 03`; `F0 CD 03` instead
raises illegal instruction. Windows reports the breakpoint context one byte
before the post-INT IP in this control. This protected-mode native comparison
does not by itself prove a V86 frame or another processor's behavior.
An initial control incorrectly expected the context to contain post-INT IP;
its exception-code results matched but all four breakpoint offsets were one
byte earlier. The control was corrected to assert the observed Windows
breakpoint convention, without changing any product assertion.

An actual selected-CCPU fixture using the current DPMI hook terminated with
`0xc0000005` on `F2 CD 31` before the candidate. The candidate removes the
extra REP rejection from the existing 16-bit direct-IF reflector. It still
rejects LOCK, F1, operand-size and overlength cases; it changes neither the
guest nor the original fallback. Selected CCPU controls cover F2/F3,
segment/address/REP combinations, maximum length and 16-bit IP wrap. Negative
controls include LOCK/F1/operand-size combined with REP and a valid IVT target.
The 183-case CCPU suite and the native x86 control pass. No claim is made that
an installed guest ran the instruction or that V86 virtual IF, paging,
hooked-vector and failure delivery are complete.

The same ten-component package passed the Full 16-gate suite, Winmine, SOL,
WRITE, four SOL Options rounds and the Win3.1 standard/enhanced × DOSONLY 0/1
matrix. The 86 checked guest binaries were unchanged in both original and
isolated trees. Both publication destinations matched all 20 tested hashes.
[Evidence](s11-v86-rep-evidence/). These checks do not establish installed
guest use of the prefixed instruction. Mirror exception `MVDM-HOST-DIV-490`
records its scope.

The mirror ledger keeps `c_main.c` classified partial at 99.03% matched
original code lines. The C/H inventory check found no unlisted or stale
mirror estimates. This textual figure is not runtime coverage.
