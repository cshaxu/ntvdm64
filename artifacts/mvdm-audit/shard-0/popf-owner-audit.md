# CCPU POPF original-owner boundary

The frozen OpenNT `popf.c` SHA-256 is
`66C4FCAF0F3329DC6D7021A0AD8BA910FA775E84D63AD35404EE2A3134DD46C3`;
the selected mirror SHA-256 is
`C1CA0F0273BB6B41051CD25C4AC39A85E1967D6B0DB9EF8ECE246C71F0294F72`.
The complete raw comparison has one addition: the registered DIV-076 comment
and `<stdio.h>` declaration. All 41 original nonblank uncommented code lines
match; this is source retention, not proof of complete behavioral coverage.
No guest or product code is changed by this review.

The cached x86 worker map SHA-256
`641E5C33430445ECC73885176250194BE936549654E14864074BD0B7D92D3CE2`
attributes `_POPF` and `_POPFD` to `original-ccpu386:popf.obj` for worker
SHA-256 `D3437091D920A74FF72FDC8ED023C6207642682A7739C45751E4F03A8C1D53E5`.
This map is from the cached worker, not proof of an independently tested
current deployment. `c_main.c` decodes opcode 9D, raises the original VM/IOPL
access exception when applicable, and selects POPF or POPFD by operand size.
Each validates a stack operand before `spop`; stack access/limit/page checks
are owned by `c_stack.c`. `spop` reads word or dword and advances the selected
stack pointer. `c_bsic.c::setFLAGS` owns privilege-dependent IF and IOPL,
while POPFD preserves VM and RF around `c_setEFLAGS`. Thus this function is
not an independent flag authority. Shared decoder, stack and flag helpers
must each be reviewed separately; this source match does not prove their
fault ordering or runtime identity.

The historical T420 S13 narrative says the implicit declaration was
restored, while commit `a2ec4d531` actually retains the stdio declaration
in this file and the current DIV-076 registry records it. Treat the source
and commit as authoritative for the observed file; do not infer a later
removal from the narrative. Existing integration probes contain POPF/POPFD
instructions but their mere presence does not establish that every privilege,
stack-fault, flag-preservation or diagnostic path was asserted. The row
remains partial because the mirror differs. Guest execution and negative
privilege/page-fault tests remain unverified here.
