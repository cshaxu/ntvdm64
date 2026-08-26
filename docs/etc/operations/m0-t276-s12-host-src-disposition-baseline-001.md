# M0 T276 S12 P2: `softpc.new/host/src` source boundary baseline

## Scope

The original `softpc.new/host/src/sources` description declares an `spchost`
library. Its source directory contains 56 selected C translation units. This
ledger classifies all 56 by original boundary family and literal original build
membership; it does not import a body into a build.

## Result

- One original BOP dispatcher: `nt_bop.c`. It remains source-owned by the
  MVDM mirror; `adapter-bop` transports only the fixed-width ingress/completion
  mechanics and never absorbs selector/service logic.
- Ten original CCPU/SAS/memory facade units: `copy_fnc.c`, `cpucstbs.c`,
  `nt_cpu.c`, `nt_emm.c`, `nt_mem.c`, `nt_sas.c`, `nt_umb.c`, `sim32.c`,
  `stubs.c`, and `x86_emm.c`. They require original-shape
  `adapter-softpc -> adapter-bochs` review. No source unit directly includes
  or calls a Bochs object.
- Four VDM monitor/reset/event units: `nt_eoi.c`, `nt_event.c`, `nt_inthk.c`,
  and `nt_reset.c`; and one worker unit, `nt_thred.c`. They are separate
  monitor/session contracts, not an invitation to recreate NT4's global worker
  shell.
- One cross-owner startup composition unit: `nt_msscs.c`. Its original body
  joins COMMAND, XMS, debugger/VDD, machine and Win32 startup paths. It remains
  an exact mirror and a composition prerequisite; it is not a monitor provider
  that can be enabled independently.
- Nine Win32/NTDLL host-capability units: configuration, diagnostics, OEM/NLS,
  PIF/security and related host functions. Their original algorithm/error order
  stays in mirror source; `adapter-win32` is the only prospective binding owner.
- Two specialist product bridge units: `nt_vdd.c` and `nt_yoda.c`, assigned to
  the existing VDD/debugger adapters rather than a generic host shim.
- Eighteen device bridge units (display, serial/parallel, input, disk/floppy,
  timer, sound and fullscreen paths). These require a later per-device choice
  between Bochs-native ownership and an individually proved same-shaped public
  host binding; they cannot enter merely because they belong to `host/src`.
- Ten auxiliary sources are absent from the literal historical `SOURCES` list.
  They remain exact mirror evidence with `profile/build-unobserved` state; no
  runtime reachability is inferred from filename or source presence.

## Consequence

`softpc.new/host/src` is not one recoverable library. Its only immediate
recovery candidates are the complete, adapter-owned groups above, after their
function-level interface and failure contracts are recorded. CPU/device sources
remain excluded or gated until Bochs ownership is proved. This preserves the
OpenNT control structure while preventing a second emulator from reappearing.

## Verification

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass `
  -File tools/governance/Audit-T276S12HostSrcDisposition.ps1 `
  -RepositoryRoot O:\repos.hobby\ntvdm64

powershell.exe -NoProfile -ExecutionPolicy Bypass `
  -File tools/governance/Verify-T276S12HostSrcDisposition.ps1 `
  -RepositoryRoot O:\repos.hobby\ntvdm64
```

Expected result: `T276 S12 host/src disposition verification passed: 56 source rows.`
