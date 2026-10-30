# S11 remaining CCPU diagnostic read boundary

Source inspection of the currently selected CALLF diagnostics found a
separate, explicitly enabled read path from the already removed DIV-306/308
argument reads. `call.c` invokes `mvdm_softpc_report_wow_far_call_attempt`
before the normal real/protected-mode branch. The reporter returns before
its reads unless both `MVDM_WOW_CALLBACK_CPU_TRACE_PATH` and matching
`MVDM_WOW_CALLBACK_CPU_SELECTOR` are present. When enabled, it copies caller
LDT data, translates a derived instruction address with `xtrn2phy`, then
passes either the translated physical value or the original linear value to
`c_sas_loads`. The latter calls `c_sas_hw_at`, whose `bios_read_byte` treats
its input as linear and may perform another paging translation and eventually
`biosDoInst`. Thus an enabled witness does not establish side-effect-free
physical reads, and the selected translated address can be translated again.
This is a source-derived diagnostic limitation, not an observed production
failure; with the environment disabled those internal reads are not reached.

The separately enabled descriptor-store witness similarly calls `c_sas_loads`
from five physical store paths after its address/filter test. This may observe
memory while in a store callback and potentially reenter emulation. No actual
failure, reentry, wrong result or device side effect has been demonstrated.
Existing selected-object far-exception tests prove the removed unconditional
DIV-306/308 reads no longer change the tested exception order; they do not
exercise a matching DIV-305 enabled trace at its CALLF entry, or the separate
store witness. Do not claim the enabled trace preserves all guest semantics,
nor conflate this opt-in concern with the normal product path. The source
registration already records that enabled behavior needs separate verification.
No source change or original 486 repair follows from inspection alone.
