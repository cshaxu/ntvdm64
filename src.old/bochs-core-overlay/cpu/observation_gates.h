/////////////////////////////////////////////////////////////////////////
// DIVERGENCE(BX-CORE-DIV-003,BX-CORE-DIV-004): private default-off
// observation gate scaffolding. Adopted Bochs call sites retain only the
// mechanical observation points.
/////////////////////////////////////////////////////////////////////////

#ifndef BOCHS_CORE_OVERLAY_OBSERVATION_GATES_H
#define BOCHS_CORE_OVERLAY_OBSERVATION_GATES_H

#ifndef RUNTIME_ENABLE_MACHINE_SEGMENT_ACCESS_OBSERVER
#define RUNTIME_ENABLE_MACHINE_SEGMENT_ACCESS_OBSERVER 0
#endif

#ifndef RUNTIME_ENABLE_MACHINE_INTERRUPT_RETURN_OBSERVATION
#define RUNTIME_ENABLE_MACHINE_INTERRUPT_RETURN_OBSERVATION 0
#endif

#ifndef RUNTIME_ENABLE_MACHINE_INSTRUCTION_HISTORY
#define RUNTIME_ENABLE_MACHINE_INSTRUCTION_HISTORY 0
#endif

#ifndef RUNTIME_ENABLE_MACHINE_SOFTWARE_INTERRUPT_OBSERVATION
#define RUNTIME_ENABLE_MACHINE_SOFTWARE_INTERRUPT_OBSERVATION 0
#endif

#ifndef RUNTIME_ENABLE_MACHINE_PHYSICAL_WRITE_OBSERVATION
#define RUNTIME_ENABLE_MACHINE_PHYSICAL_WRITE_OBSERVATION 0
#endif

#if RUNTIME_ENABLE_MACHINE_SEGMENT_ACCESS_OBSERVER
#define RUNTIME_RECORD_SEGMENT_ACCESS(s, seg, off, kind) BX_CPU_THIS_PTR overlay_observe_segment_access((s), (seg), (off), (kind))
#else
#define RUNTIME_RECORD_SEGMENT_ACCESS(s, seg, off, kind) do { } while (0)
#endif

#if RUNTIME_ENABLE_MACHINE_INTERRUPT_RETURN_OBSERVATION
#define RUNTIME_RECORD_INTERRUPT_RETURN(width_value) BX_CPU_THIS_PTR overlay_observe_interrupt_return((width_value))
#else
#define RUNTIME_RECORD_INTERRUPT_RETURN(width_value) do { } while (0)
#endif

#if RUNTIME_ENABLE_MACHINE_INSTRUCTION_HISTORY
#define RUNTIME_RECORD_INSTRUCTION_HISTORY() BX_CPU_THIS_PTR overlay_observe_instruction_history()
#else
#define RUNTIME_RECORD_INSTRUCTION_HISTORY() do { } while (0)
#endif

#if RUNTIME_ENABLE_MACHINE_SOFTWARE_INTERRUPT_OBSERVATION
#define RUNTIME_RECORD_SOFTWARE_INTERRUPT(vector_value) BX_CPU_THIS_PTR overlay_observe_software_interrupt((vector_value))
#else
#define RUNTIME_RECORD_SOFTWARE_INTERRUPT(vector_value) do { } while (0)
#endif

#if RUNTIME_ENABLE_MACHINE_PHYSICAL_WRITE_OBSERVATION
#define RUNTIME_OBSERVE_LIN_MEMORY_WRITE(paddr, size, rw, dataptr) do { if ((rw) == BX_WRITE) BX_CPU_THIS_PTR overlay_observe_physical_write((Bit64u)(paddr), (unsigned)(size), (dataptr)); } while (0)
#define RUNTIME_OBSERVE_PHYSICAL_WRITE(cpu, paddr, size, dataptr) (cpu)->overlay_observe_physical_write((Bit64u)(paddr), (unsigned)(size), (dataptr))
#else
#define RUNTIME_OBSERVE_LIN_MEMORY_WRITE(paddr, size, rw, dataptr) ((void)0)
#define RUNTIME_OBSERVE_PHYSICAL_WRITE(cpu, paddr, size, dataptr) ((void)0)
#endif

#endif
