# Interrupt diagnostic divergence registration

Compared with pinned OpenNT, selected `softpc.new/host/src/nt_inthk.c` adds
CPU register access and three reporting calls before/after the original
software or exception hook. DIV-307 records INT 2Ah's original fast-LDT
signature and scalar arguments; DIV-309 records INT 2Ah register scalars;
DIV-308 records exception number, error and handler result. The implementation
in `mvdm_softpc_termination.c` checks `MVDM_WOW_CALLBACK_CPU_TRACE_PATH`,
appends a bounded-format record and restores the host last-error. Original
handler decisions remain in source. Enabled logging has I/O and timing effects
that have not been demonstrated equivalent to the uninstrumented path.

All three were missing from the divergence registry and are now explicitly
registered. DIV-308 is also used by a separate far-call destination diagnostic
in `base/ccpu386/c_xfer.c`; the latter executes additional descriptor reads
and address translation before the log check. Its actual default-off side
effects and potential exception-order change require separate validation.
The source trace is evidence, not a guest behavior or fault-injection test;
both mirror-different files remain partial. No runtime source was changed.
