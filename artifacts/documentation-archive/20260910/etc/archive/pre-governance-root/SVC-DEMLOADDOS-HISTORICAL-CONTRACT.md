# SVC_DEMLOADDOS Historical Contract

Status: M16 source/artifact evidence record, 2026-08-08.

## Scope

This record fixes the first NTIO-to-host request of the selected source-built
NT4 EN-US profile. It is a historical behavior contract, not an authorization
to add a BOP, DEM, or DOS service handler to `ntdos64`.

The fixed artifact is:

```text
artifacts/toolchain-runs/ntio-tools16-opennt-v1/
  base/mvdm/dos/v86/doskrnl/bios/NTIO.SYS
length: 33792 bytes (0x8400)
SHA-256: CFC8BE16576BB6ACD16BB8FC9B2D9A080F544BBFDD2D2D2DF07ED908B3AB4937
```

The same byte count and hash are present in the recorded fixed upstream input.
No image bytes are exported by this document.

## Historical Call Chain

`msinit.asm` places `dos_load_seg` in `DI`, then emits
`SVC SVC_DEMLOADDOS`. `dossvc.h` defines that service as `0x11`; the built
image contains `C4 C4 50 11` at the first observed request site.

```text
NTIO init
  -> C4 C4 50 11
  -> original monitor BOP decode (not runnable in this workspace)
  -> BIOS[0x50] in softpc.new/base/bios/bios.c
  -> original MS_bop_0 in softpc.new/host/src/nt_bop.c
  -> original DemDispatch(0x11) in dos/dem/demdisp.c
  -> apfnSVC[0x11] = demLoadDos in dos/dem/demmisc.c
```

`BIOS[0x50]` is `MS_bop_0` under the historical `NTVDM` build condition.
`MS_bop_0` reads exactly one byte at the current `CS:IP`, calls
`DemDispatch`, then increments `IP` by one. Thus its local ownership is the
trailing service byte, not a DOS-wide instruction interpretation.

The missing generated CCPU input prevents a live proof of the monitor's exact
pre-dispatch `IP`. The source chain nevertheless proves the intended division:
the monitor consumes/selects BOP `0x50`; `MS_bop_0` consumes the one-byte DEM
selector. A future generic transition engine must report physical fetch bytes
and apply an explicitly validated instruction-pointer disposition atomically;
it must not expose an unconstrained BOP or mode-switch callback.

## Service Contract

`demLoadDos` documents and implements one guest input: `DI`, the real-mode
load segment. It resolves `GetVDMAddr(DI, 0)`, opens the ambient historical
`ntdos.sys` path, and reads that file in 16 KiB chunks into the resulting
guest address. An open or read failure calls `TerminateVDM`; it is not a DOS
carry-flag error return. On success the routine changes no general-purpose
register or flag itself. `MS_bop_0` advances IP after `DemDispatch` returns.

For the fixed artifact, `dos_load_seg` is a paragraph-aligned linker marker
immediately after the `0x8400`-byte NTIO image. At the conventional NTIO load
physical address `0x700`, its currently observed value is `0x08B0`, hence the
first NTDOS byte is requested at physical `0x8B00`.

This value is an artifact-layout fact, not an ABI constant. `msinit.asm`
states `mov di,dos_load_seg`; it does not encode `08B0h` in source.

## Modern Boundary Consequence

The modern, BYOB-compatible replacement may replace only the historical host
file dependency:

```text
validated selected NTDOS bytes
  -> preflight complete ordinary-RAM destination at DI:0000
  -> atomic guest-memory transaction
  -> validated IP advance / resume, or terminal stopped failure
```

It must preserve these observed semantics:

- The profile determines whether the physical transition bytes denote this
  request; generic core code does not name BOP, SVC, DEM, or `NTDOS.SYS`.
- The requested destination comes from the live guest `DI`, not an adapter
  constant.
- The complete image interval is checked before the first write. The original
  chunked `ReadFile` loop is historical behavior, not a safe modern commit
  model.
- Missing, mismatched, unreadable, or out-of-range BYOB content terminates the
  bounded session with a redacted diagnostic. It may not synthesize a DOS
  success response or inspect an ambient system directory.
- The request must occur after NTIO begins execution. Preloading NTDOS in an
  initial-entry transaction cannot be treated as equivalent historical
  execution.

`IDLE_disk()` after non-date/time services is a separate historical idleness
dependency. It is not evidence that the first loader transaction needs a
timer, PIC, disk controller, or generic device model.

## Non-Claims

This does not make the historical CCPU path runnable, implement DEM, recover
the full BOP decoder, or admit a normal `ntvdm.exe` runtime. It identifies the
first concrete post-transition transaction/resume requirement for a future
neutral core/mantle boundary.
