# M0 T276 S12 P8: Device-bridge interface baseline

## Question

How do the eighteen original `softpc.new/host/src` device bridge files divide
between Bochs-native machine ownership, a bounded public host endpoint and
historical product shells that cannot enter the current profile?

## Inputs and method

All eighteen literal `spchost` source inputs were read as complete device
families, using original source spans and the P2/P3 source/build record. The
ledger assigns one future owner set and prerequisite gate per file. It records
no host device binding, source edit, adapter ABI or build edge.

## Findings

- `nt_cga.c`, `nt_ega.c`, `nt_vga.c` and `nt_munge.c` are the historical
  SoftPC display-device/painter body. They are explicit Bochs-replacement
  exclusions, not reusable rendering helpers.
- `nt_graph.c`, `nt_det.c`, `nt_fulsc.c` and `nt_hosts.c` are the old NT4
  presentation shell. A later public presentation endpoint may exist, but it
  must consume Bochs-owned device state through `adapter-bochs`; no direct
  VGA synchronization, console-global mutation or raw VRAM path is admitted.
- `nt_input.c` and `nt_mouse.c` translate host input into historical machine
  actions. `adapter-vdm-monitor` owns bounded event delivery,
  `adapter-win32` may own public input acquisition, and `adapter-bochs` is
  the sole machine operation boundary.
- `nt_com.c`, `nt_wcom.c` and `nt_lpt.c` mix historical serial/printer
  endpoints with device registers and global native handles. Bochs owns
  UART/LPT semantics; any public endpoint must use session-owned opaque
  resources and typed bridge operations. WOW direct port access remains a
  WOW-owner prerequisite.
- `nt_fdisk.c` and `nt_rflop.c` implement SoftPC storage/FDC/DMA/IRQ behavior
  beside raw NT device access. Their controller implementations are excluded;
  a future explicitly admitted host-media endpoint remains a typed Bochs
  backend and must separately audit raw-device policy and public API errors.
- `nt_sound.c` and `nt_timer.c` contain speaker/PPI and timer/PIC device
  semantics. Bochs owns both; session may schedule bounded execution but may
  not synthesize machine timer behavior.
- `nt_rez.c` is the only small host-resource helper. It remains source-owned
  and can be considered through `adapter-win32` only when app's immutable
  resource manifest names the input; it cannot perform ambient discovery.

## Decision

No generic device adapter is created. Bochs remains the only PC device
implementation. Public host device, console, display, audio or storage APIs
may appear only as bounded endpoints behind the existing named adapters and
session resource mapping; historical fullscreen, direct hardware and WOW
product shells remain profile-gated.

## Verification

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass `
  -File tools/governance/Verify-T276S12DeviceBridgeInterfaces.ps1 `
  -RepositoryRoot O:\repos.hobby\ntvdm64
```

Expected result: `T276 S12 device-bridge interface verification passed: 18 rows.`
