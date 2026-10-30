# Selected DPMI 32-bit interrupt return

The selected-source DPMI fixture executes the actual
`DpmiIntHandlerIret32` body with controlled register, descriptor and memory
boundaries. Across 16-bit and 32-bit stack descriptors, nested return retains
the locked stack; the final return restores the application's stack and
writes the complete 32-bit EIP, CS and EFLAGS frame. All four combinations
passed, alongside the existing 32-bit fault-return and software-interrupt
controls. Inputs and output are retained here.

The fixture and production worker maps select `dpmiint.obj`; the tested
function's 343 object instruction bytes match the production object, with
unresolved relocation placeholders. The retained producer worker hash matches
the current release manifest. This controlled check does not establish a live
CODE32 guest execution, linked relocation equivalence, all interrupt sources,
or a real stack-fault failure. No guest or production source was changed.
