# M0 T194 S30: BOP Register-Observation Derivative Audit

Packet: M0 T194 S30  
Disposition: existing diagnostic is bounded, but current generator has no exact-only mode.

## Source Facts

`refs/bochs/cpu/exception.cc` defaults
`BX_NTVDM_ENABLE_BOP_CATALOG_LISTENER` to zero. When enabled, the existing
generic `#UD` listener copies the already captured CPU state and instruction
window, calls the adapter-local catalogue, and appends `fault_rip`, AX, BX,
CS, DS, DX, SI, DI, CX, FLAGS, SS, SP and optional bounded diagnostic bytes to
the one log line. It is void: it receives no adapter result and changes no
CPU, memory, device, exception, or guest state. The `exception()` branch calls
it for every eligible `#UD` before the normal interceptor flow; it contains no
`50:21` or AL-specific branch.

Thus BX-TRACE-043 is already implemented in source and remains consistent with
its register entry. S28 lacks those fields because its retained
`cpu/exception.o` predates this enabled listener form; source presence alone
does not make a retained object observe them.

## Build Closure

The current derivative generator normally retains `cpu/exception.o`. Its
existing `-RealModeVectorDiagnostic` switch replaces exactly that one object
and enables the BOP listener, but also enables an unrelated real-mode-vector
diagnostic. It is therefore not the exact diagnostic derivative needed here.
No current switch rebuilds only `cpu/exception.o` with:

```text
BX_NTVDM_ENABLE_EXCEPTION_INTERCEPT=1
BX_NTVDM_ENABLE_BOP_CATALOG_LISTENER=1
BX_NTVDM_ENABLE_STARTUP_TRANSACTION=0
BX_NTVDM_ENABLE_CPU_RESULT_BRIDGE=0
```

All other current adapter/CLI objects and retained Bochs archives can remain
identical. No archive, device, configuration, firmware, CPU decoder, or
adapter ABI change is required.

## Interpretation

Confidence is high. The next bounded action is a build-tool-only slice that
adds an exact named generator mode for this registered one-object diagnostic,
with a negative check rejecting `main.o`, archive recursion, unrelated vector
diagnostics and source replacement beyond `cpu/exception.cc`. It must not run
the derivative. A later observation is independently blocked until the
controller's process-tree termination permission is resolved without retrying
S28.
