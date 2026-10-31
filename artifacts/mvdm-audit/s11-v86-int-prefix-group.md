# S11 V86 INT segment/address-prefix group

The previous `DIV-490` delivery admitted one segment override but declined
address-size and multiple segment/address overrides. NT4
`OpcodeGenericPrefixV86` dispatches these prefixes before `OpcodeINTnnV86`;
the selected direct-IF, 16-bit reflector's implicit SS:SP frame does not use
the explicit address-size override.

In the selected CCPU fixture, each of five previously declined cases ended
with `0xc0000005`: address-size, two segment overrides, mixed address/segment,
the 15-byte instruction boundary, and a multiple-prefix IP wrap. These are
controlled baseline failures, not evidence of an installed guest failure.
The candidate tracks the otherwise discarded LOCK/F1 prefix presence and
accepts segment/address combinations only when the decoded operand is 16-bit,
the total length is at most 15, and no LOCK/F1 or REP prefix is present.
Existing IOPL, hook identity, IVT and failure ownership remain in the selected
reflector. Operands with `66`, repeat/lock/F1 or overlength retain the old
fallback. Separate negative cases install both a DPMI software hook and a
valid IVT target, then verify that each rejected form reaches the old fault
owner instead of the new reflection path.

The real-CCPU gate passes 176 cases including all five positive and six
negative controls. It checks actual target, return frame and IP wrap, not real
guest incidence or the unsupported virtual-IF, hooked-vector, paging, or
exception-delivery paths. The tracked source exception is `DIV-490`.

The same ten-component package passed the Full 16-gate suite, Winmine, SOL,
WRITE, four SOL Options rounds and all four Win3.1 mode/DOSONLY combinations.
All 86 checked guest binaries were unchanged in both original and isolated
trees. Both publication destinations matched all 20 tested component hashes.
[Evidence](s11-v86-prefix-group-evidence/). These regressions do not prove
that an installed guest exercised the prefixed instruction.

The updated mirror inventory continues to classify `c_main.c` as partial;
its original-line text match is 99.03%. The inventory check found no
unlisted or stale C/H diff estimates. This metric is not runtime coverage.
