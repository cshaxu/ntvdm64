# Selected DPMI 32-bit fault-return boundary

The existing selected-source DPMI fixture now also executes the actual
`DpmiFaultHandlerIret32` implementation using controlled register, LDT and
guest-memory boundaries. It covers 16-bit and 32-bit stack descriptors at
locked-stack depths one and two, checking the count and full restored EIP,
CS, EFLAGS, ESP and SS. All four cases passed; the original software interrupt
controls also passed. The fixture and input hashes are retained here.

The selected production worker map identifies this function in
`original-mvdm-dpmi32:dpmiint.obj`. Its 174 instruction bytes match the
test object's bytes, with object relocation placeholders unresolved; the
input worker hash matches the current release's producer manifest. This does
not prove identical linked relocations or actual guest execution. In
particular, it does not replace the missing real CODE32 interrupt/fault-return
probe or cover every frame, nesting condition or provider failure. No guest
or production code changed.
