# M0 T313 S2 P1 — Original SoftPC process-entry binding

Date: 2026-08-30  
Status: partial S2 evidence; not a machine-run claim

## Decision

`app` no longer treats `host_cpu_init()` as the machine initialization
operation.  In the selected `CPU_40_STYLE`/`CCPU` source, that original host
hook is an intentional empty compatibility body.  The selected process
lifecycle instead remains the original body in
`mvdm-host/softpc.new/obj.vdm/ntvdm.c`:

`main -> TimerInit -> CpuEnvInit -> nls_init -> host_main ->
InitialiseDosEmulation -> host_start_cpu`.

The formal build compiles that source with only `main` renamed to
`mvdm_softpc_original_entry`.  `adapter-mvdm-host-out/softpc` binds a session
before calling this original entry and records only its typed return; it does
not reproduce or reorder the original initialization path.  `app` owns the
top-level product entry and delegates the selected machine lifecycle to that
binding.

## Source and divergence evidence

- `ntvdm.c` retains its original entry body.  Its one selected source change is
  `MVDM-HOST-DIV-144`: spelling the already-public `DbgBreakPoint` calling
  convention as `NTAPI` to match the x86 declaration.
- `adapter-mvdm-host-out/win32/include/nt.h` supplies
  `ADAPTER-WIN32-035`, declaration-only NT4 record/selector/call forms needed
  by the original CPU-environment registry enumeration.  It does not replace
  the original open/enumerate/failure flow.
- `app/machine_shell.c` now arms the session in `open()` and invokes the
  source-shaped entry through `run()`; it no longer invokes the empty CPU hook
  or invents a reset sequence.

## Formal compile evidence

Fresh selected x86 CCPU40 graph root:

`build/M0-T313/S2/r1`

The following formal targets completed after the entry binding was selected:

```text
ninja -C build/M0-T313/S2/r1 \
  obj/host/ntvdm_entry.obj \
  obj/adapter-softpc/mvdm_softpc_execution.obj \
  obj/app/machine_shell.obj
```

The original entry no longer has incompatible `DbgBreakPoint`, missing
`KEY_VALUE_FULL_INFORMATION`, `KeyValueFullInformation`, `NtOpenKey`, or
`NtEnumerateValueKey` declarations.  Remaining diagnostics are visible
original-source warnings and are not suppressed by this packet.

## Limit

This is a source/compile boundary only.  It does not claim that the complete
candidate graph or a real guest run has completed; S2 still requires the
original CPU/SAS, firmware/CMOS/ROM/reset lifecycle and bounded behavior
evidence.
