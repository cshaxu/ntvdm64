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

## Reproducible runtime recheck (2026-08-29)

After the S8 owner-contract subdivision was admitted, the already-generated
formal Ninja fixtures were run again outside the sandbox on both supported
host architectures:

```text
build/M0-T310/S8/ccpu-execution/x64/ccpu-bounded-execution.exe
build/M0-T310/S8/ccpu-execution/x86/ccpu-bounded-execution.exe
```

Both returned zero and printed the sequence above.  The fixture deliberately
enters through original `host_start_cpu`, re-enters through original
`host_simulate`, and executes the original guest byte sequence `B0 5A A2
00 80 D9 E8 D9 1E 04 80 D6 FE`: an ordinary RAM write followed by `fld1` and
`fstp dword [8004h]`, then the original bounded unsimulate exit.  Its RAM
assertions verify the `1.0f` result (`00 00 80 3F`) at guest address `8004h`.
Thus this is a real, bounded CCPU/FPU execution confirmation on x86 and x64,
not merely a link check.  It remains intentionally narrower than a complete
machine-profile result.

## Current S8 functional recheck (2026-08-29)

The S8 plan now requires every owner-contract packet to carry focused behavior
evidence in addition to its x86/x64 compile and link evidence.  The fixture
was therefore regenerated from the current sources and rerun from isolated
build roots:

```text
build/M0-T310/S8/execution-20260829/x64/ccpu-bounded-execution.exe
build/M0-T310/S8/execution-20260829/x86/ccpu-bounded-execution.exe
```

Both returned zero and emitted the same startup/re-entry sequence.  Their
assertions again observed the `0x5A` guest-RAM store and the x87 `1.0f`
four-byte result at `0x8004`.  This is the focused positive behavior result
for `SPC-CCPU-EXECUTOR-DISPATCH`; it does not close the later access-table,
SAS-vector, device-controller, firmware, or whole-machine packets.

## Exception-frame failure contract (2026-08-29)

The original `ccpu386ThrdExptnPtr()` returns `jmp_buf *`, but its `BADID` and
missing-TLS branches used a bare `return`.  That leaves a caller with an
undefined native pointer.  The direct sibling `ccpu386SimulatePtr()` already
establishes the source-family failure direction: log the same class of
diagnostic and return a typed null `jmp_buf *`.

`MVDM-HOST-DIV-128` changes only those two failure returns to the typed null
form.  It retains the existing diagnostics, TLS lookup, normal `excepts[]`
selection, exception nesting, and `longjmp` control flow.  The bounded CCPU
fixture now calls the accessor before initialization and asserts a null result;
it then performs the existing RAM/x87/re-entry sequence.  Fresh MSVC `/MT`
x86 and x64 fixture builds and runs both returned zero.  The full-tree ledger
will be cold-refreshed after the remaining S8 executor-contract batch, rather
than falsely treating this focused result as a whole-tree baseline.

The same reread also found three declaration-only, source-shaped executor
contracts.  `MVDM-HOST-DIV-129` publishes `a3_cpu_interrupt` as its existing
`VOID(int, IU16)` form; `MVDM-HOST-DIV-130` narrows the difference of the
private eight-entry FPU stack only at its original fixed-width register-index
boundary; and `MVDM-HOST-DIV-131` gives the reserved-NPX dispatcher its
existing `VOID ZFRSRVD(IU32)` prototype.  None creates an adapter, a mapping
token, or a new machine behavior.  They remain covered by the same bounded
x86/x64 executor fixture and await the next cold full-tree refresh for their
compiler-ledger confirmation.
