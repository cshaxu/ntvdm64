# S11 V86 software INT with a segment override

The selected CCPU adapter accepted `CD nn` for its existing V86 software INT
reflector only when the decoded instruction occupied exactly two bytes. NT4
`OpcodeGenericPrefixV86` dispatches segment overrides before
`OpcodeINTnnV86`, whose return IP includes the decoded prefix length. A single
segment override does not require a 32-bit operand frame.

A real selected-CCPU fixture executes `26 CD 31` with a mapped IVT target and
the actual DPMI software hook. The pre-change fixture terminated with
`0xc0000005`; this controlled failure is not a demonstrated installed-guest
crash. The candidate accepts exactly one of ES/CS/SS/DS/FS/GS overrides
when CCPU selected a 16-bit operand, then uses the existing V86 reflector.
All six cases reach the target and preserve the expected post-instruction
return address and frame; a seventh case crosses the 16-bit IP wrap. The full
selected boundary gate has 165 passing cases. The change does not extend 32-bit operand, address-size, repeat, lock,
multiple-prefix, hooked-vector, virtual-IF or memory-failure behavior.

The same ten-component package passed the Full 16-gate suite, Winmine, SOL,
WRITE, four SOL Options rounds and the four Win3.1 mode/DOSONLY combinations.
The 86 checked guest binaries were unchanged in both original and isolated
trees. Both publication targets matched all 20 component hashes. Evidence is
in [s11-v86-prefix-evidence](s11-v86-prefix-evidence/). The source exception
is `MVDM-HOST-DIV-490`; the controlled CCPU test does not prove that an
installed guest used this prefixed instruction.

The mirror ledger continues to classify `c_main.c` as partial, with 99.03%
original code lines matched by its text estimator. The current C/H inventory
check found no unlisted or stale mirror differences; this is not behavioral
coverage or resolution of the remaining source-role questions.
