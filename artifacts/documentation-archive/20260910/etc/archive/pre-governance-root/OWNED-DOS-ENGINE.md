# Owned WHPX DOS Engine

## Purpose

`owned-dos-engine.exe` is the first executable CPU-backed implementation of
the public BYOB engine command line contract. It is intentionally self-owned:
its input is only a selected COM or real-mode MZ file and it neither discovers nor consumes a
Microsoft DOS runtime. The implementation relies solely on documented Windows
Hypervisor Platform APIs and fresh process-local guest RAM.

## Invocation

```text
ntdos64-run.exe --engine owned-dos-engine.exe target [arguments...]
```

The dispatcher supplies `owned-dos-engine.exe -- target [arguments...]`.
The current engine rejects extra target arguments, which is a known limitation
rather than silently inventing a PSP command tail policy.

## Guest Contract

- One MiB of newly allocated RAM is mapped read/write/execute into a fresh
  WHPX partition.
- A minimal PSP starts at segment `1000h`; a COM image begins at `1000:0100`.
  A real-mode MZ header is validated for declared page length and relocation
  containment; its module is loaded at `1010:0000`, relocations receive that
  module-segment delta, and its declared entry and stack are used.
- An owned IVT entry for `INT 21h` enters a host-observed I/O trap.
- `AH=02h` emits `DL`; `AH=09h` emits the byte sequence at `DS:DX` through the
  first `$`; and `AH=4Ch` supplies the host process exit code.
- For a returning service, the adapter reads the real-mode INT frame that the
  CPU created, verifies its bounds, then atomically restores only its saved
  `IP`, `CS`, `FLAGS`, and post-frame `SP`. This is a narrow, owned service
  return operation. Ordinary `IRET` behavior is independently exercised by
  `whpx-int`.

## Explicit Non-Goals

No BIOS, BDA, PIC, PIT, keyboard, files, DOS allocator, process
tree, device namespace, protected mode, V86, DPMI, NTDOS bootstrap, or Win16
environment exists here. Unsupported `INT 21h` functions terminate with a
diagnostic. The engine must not be selected as an NTDOS adapter merely because
it can execute this small COM fixture.

## Verification

`whpx-com-terminate` creates a fully owned COM file. It uses both output
services, returns `7` via `AH=4Ch`, and is invoked through
`ntdos64-run --engine`; the CTest assertion checks the observed `Hi there`
output and exit code. No Microsoft byte or runtime file participates.
