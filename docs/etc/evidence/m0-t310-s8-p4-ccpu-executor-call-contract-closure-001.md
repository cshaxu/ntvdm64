# M0 T310 S8 P4 — CCPU executor call-contract closure

The selected original CCPU executor, `c_main.c`, relied on historical
implicit declarations for the existing address, segment, PIC, timer, debug
and NPX bodies. On a modern compiler an implicit declaration manufactures a
host `int` contract, which is invalid for the original fixed-width `IU32`
return and can misrepresent a `void` operation on either supported host
architecture.

`MVDM-HOST-DIV-072` includes only the existing owner headers for the address,
segment, PIC and timer contracts, and publishes exact `void` declarations for
the two selected same-package bodies without public headers. The original
callees, ordering, controller ownership and guest state are untouched.

`MVDM-HOST-DIV-073` fixes the distinct TLS jump-frame mismatch: the original
helpers return `jmp_buf *`, while modern MSVC `setjmp` consumes the referred-to
`jmp_buf` array. Each selected call now dereferences that returned pointer,
preserving the same TLS frame and `longjmp` target rather than adding a new
execution mechanism.

Clean formal MSVC `/MT` Ninja rebuilds of `obj/ccpu/c_main.obj` completed on
x86 and x64. They emit no implicit-call diagnostic for `force_yoda`, `d_mem`,
`limit_check`, `address_add`, `TakeNpxExceptionInt`, `host_timer_event`,
`ica_intack` or `load_pseudo_descr`, and no C4024/C4047 diagnostic for the
three selected `setjmp` calls. `EDL_fast_bop` remains a separate, genuinely
undefined source boundary. Fixed-width guest scalar and historical source-style
warnings remain visible for later semantic review; this closure neither
suppresses nor reclassifies them as safe.

## Bounded execution confirmation

The formal bounded-execution graph initially omitted the already-required GDP
slot overlay generation used by `cpu4gen.h`.  The graph generator now runs
`Generate-T310GdpSlots.mjs`, exposes its generated include directory, and
links the existing `mvdm_gdp_state.c` overlay.  This is build-graph recovery,
not a SoftPC algorithm change.

Fresh MSVC `/MT`, CPU_40 selected-profile builds completed the full 145-step
`ccpu-bounded-execution` graph on both x86 and x64 with no compiler or linker
errors.  On each host architecture the executable returned zero after the
same observable sequence:

```text
sas-init -> cpu-init -> access-init -> seed -> start -> returned-start
-> reenter -> returned-recursive
```

This confirms the limited original `c_cpu_simulate` executor path and its
existing GDP overlay binding.  It does not claim all executor dispatch forms,
the successor access/SAS-vector contracts, controller operation, or whole
machine/DOS execution are closed.
