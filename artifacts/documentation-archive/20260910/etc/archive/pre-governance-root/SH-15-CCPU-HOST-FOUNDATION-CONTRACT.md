# SH-15 CCPU Software Host Foundation Contract

Status: proposed implementation contract and trace-baseline boundary. This
document is based on the fixed OpenNT input under `src/opennt/` and applies
only to the default-disabled x86 historical compiler/CRT island. It does not
authorize a default runner,
Microsoft guest-media distribution, V86 execution, `NtVdmControl`, HAXM,
injection, a kernel component, or a host-global configuration change.

## Purpose

The initialized CCPU link experiment proves that the historical monitor and
PC/AT providers are source-closed but need a small modern host surface. SH-15
defines the first portion of that surface without inventing a DOS service,
altering an original BIOS handler, or adding a substitute device model.

The formal execution ownership remains:

```text
CCPU instruction decoder
  -> BOP selector bridge
  -> original BIOS[selector]
  -> original MS_bop_0
  -> original DemDispatch
  -> original DEM service
```

The foundation must not call `DemDispatch` directly, interpret a BOP service
byte, modify DOS registers after a service, initialize an IVT/BDA, or decide
whether DOS startup succeeded.

## Current Implementation Boundary

`local/shim/ccpu_host_foundation.c` and
`local/shim/ccpu_monitor_sm0_foundation.c` are trace fixtures, not an SH-15
implementation. The former supplies an empty local `BIOS[]` only for an
initialization fixture and aborts every unadmitted callback; the latter is a
bounded SM-0 RAM aperture with abort-on-use callbacks. Neither may be linked
by a normal runner or widened into a DOS/BIOS/DEM runtime.

The direct-CCPU full-source closure currently reaches link diagnostics but has
no live original NTIO startup prefix. Consequently no host foundation callback
has yet met the admission rule of being observed on the required historical
startup path. SH-15 remains a contract for the first such callback, not
permission to promote either fixture or to infer IRQ, SAS, ROM, event, or
session behavior from its guards.

## Evidence And Scope

| Historical dependency | Original evidence | Foundation responsibility | Explicit non-responsibility |
| --- | --- | --- | --- |
| `host_sas_init(sys_addr)` / `host_sas_term()` | `ccpu386/ccpusas4.c` asks the host for CCPU private M-area storage; historical `host/src/sim32.c` provides the same contract. | Allocate/release private, zeroed CCPU guest backing storage with checked size and no host file/device access. | ROM bytes, BIOS setup, IVT/BDA creation, guest-media loading, memory translation policy. |
| `EDL_fast_bop(IU32)` | `ccpu386/c_main.c` consumes the complete C4 BOP encoding and calls it only for the `...FE` extended form. | Record an unadmitted extended-BOP callback and stop the current fixture. | Decode C4 bytes, consume/adjust IP, index `BIOS[]`, fetch a DOS service byte, call DEM, or define special selector behavior. |
| `host_swint_hook(IS32)` / `host_exint_hook(IS32, IS32)` | CCPU calls them only in protected mode before continuing its own interrupt/exception handling. | Return `FALSE` in the first non-V86 profile, meaning no host handler claimed the event. | Switch mode, inject an exception, patch CCPU state, emulate DPMI/WOW/VDD fault handlers. |
| `trace_file`, `host_error(...)` | CCPU/SAS diagnostics reference them on failure paths. | Emit bounded, redacted per-session diagnostics; choose a non-interactive failure result. | Show system UI, terminate unrelated processes, record guest binaries or arbitrary host paths. |
| `host_ica_lock` / `host_ica_unlock`, timestamp/calibration calls | Original ICA/QEVNT retain IRQ and quick-event scheduling policy. | Provide an in-process, per-session synchronization/time source only once the required ordering tests exist. | Reimplement PIC priority/masking/EOI, synthesize guest IRQs, or create a background event thread before a bounded test proves it. |

## Required Semantics

### SAS Allocation

- The allocation is per research session and has no shared global guest
  memory. A second initialization before termination must fail or reset only
  after the caller has explicitly terminated the first session.
- `size == 0`, overflow, or a size outside the profile maximum returns null.
  No partial allocation is exposed.
- Returned storage is writable private virtual memory, zero-initialized, and
  remains valid until the matching `host_sas_term`.
- Termination releases only the foundation's own allocation and returns null,
  matching the historical convention used by `sas_term`.
- The implementation must not call `NtVdmControl`, map a physical drive,
  inspect a host drive letter, or allocate executable guest memory.

### CCPU Extended BOP Exclusion

- The first software profile has no sourced owner for `EDL_fast_bop`; the
  foundation records the callback and stops the current fixture.
- It does not alter IP, EIP, CS, FLAGS, general registers, mode bits, memory,
  or any event queue before it stops. CCPU remains the owner of instruction
  consumption.
- No extended selector is mapped to `BIOS[]`, including selector `0x50`; the
  fixture may not claim BOP or DOS SVC support through this extension.

### Non-V86 Hook Policy

- `host_swint_hook` and `host_exint_hook` return `FALSE` without side effect
  in the CCPU-only profile. This preserves the original fall-through path in
  CCPU rather than pretending a missing monitor handler processed it.
- Any future `TRUE` result needs a separately versioned handler registration
  contract and a bounded protected-mode fixture. It is not part of SH-15.

### Diagnostics

- Diagnostics identify the historical callback and numeric parameters, but
  redact host paths and never persist guest memory or user-provided binaries.
- A fatal allocation/bridge error makes only the current research session
  unavailable. It must not call `ExitProcess` from a historical callback.

## Acceptance Fixtures

1. **CPU versus SAS boundary:** original `c_cpu_init` establishes only the
   CCPU thread/reset state; it does not allocate SAS memory. A CPU-reset
   fixture therefore may prove reset register state only. SAS lifecycle is
   exercised later through original `sas_init`/`sas_term`, after the original
   ROM-provider path has been explicitly assembled; a zero-size foundation
   allocation request is rejected before that path is entered.
2. **Extended-BOP exclusion:** an observed `EDL_fast_bop` call records the
   callback name and stops. The fixture must not reach `BIOS[]`, `MS_bop_0`,
   DEM, or guest startup.
3. **Hook fall-through:** protected-mode fixture evidence shows both hooks
   return false and CCPU selects its original handler path. No mode transition
   or injected interrupt is allowed.
4. **Containment audit:** the foundation target is `EXCLUDE_FROM_ALL`, not
   referenced by a default engine, and imports neither `NtVdmControl` nor
   physical-device/file-system APIs.

## Deferred Dependencies

The first foundation implementation intentionally leaves timer delivery,
ICA/QEVNT locking, hardware interrupt notification, mouse installation, VDD,
WOW, redirector, console server behavior, files, media, and child process
handoff unresolved. They require their own source-level contracts and
fixtures. Solving them by making this foundation silently return success would
violate the historical ownership model.
