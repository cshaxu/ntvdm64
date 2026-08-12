# WHPX Backend Research

## Boundary

Windows Hypervisor Platform (WHPX) is a documented user-mode Windows API for
creating a partition, mapping guest physical memory and controlling virtual
processors. It is a potentially compatible CPU execution backend because its
required interface is public Win32/Windows virtualization API, not a private
NTVDM loader or console interface.

`whpx-probe` is intentionally limited to `WHvGetCapability` for
`HypervisorPresent`. `whpx-hlt` is the next owned fixture: it creates one
partition, maps one owned page containing only `HLT`, installs a bounded
real-mode register state, and verifies the documented HLT exit before destroying
all objects. Neither program reads a runtime, changes the optional Windows
feature, or installs a driver.

## Current Local Evidence

- `C:\Windows\System32\WinHvPlatform.dll` and `WinHvEmulation.dll` exist.
- Windows SDK header `WinHvPlatform.h` exists.
- A non-elevated optional-feature query was rejected by DISM, so its enabled
  state is not inferred from DLL presence.
- `whpx-probe` reports whether a hypervisor is actually present to the
  documented API. `whpx-hlt` was built and run successfully on 2026-08-06:
  its owned real-mode `HLT` byte produced the documented HLT exit and all WHPX
  objects were destroyed during teardown.
- `whpx-io` is the next fixture. It runs owned `OUT 0xE9, AL` and validates a
  documented I/O-port exit, retires the known instruction through the documented
  WHPX instruction-emulator callback API, and validates the following HLT exit
  before teardown. It does not emulate or forward a real device port.
- `whpx-int` maps an owned IVT entry, executes owned `INT 60h`, validates the
  `IRET` return to caller HLT and checks the resulting stack frame plus SP and
  FLAGS restoration. It is a CPU/IVT fixture, not host interrupt injection.
- `whpx-com-run` is the first owned DOS-service adapter. It loads an owned COM
  image with a PSP, installs an owned INT 21h trap stub and implements
  `AH=02h` character output, `AH=09h` dollar-terminated `DS:DX` output, plus
  `AH=4Ch` termination. Returning services use a bounded owned transition that
  restores the verified real-mode INT frame atomically, then resumes the
  caller; all other INT 21h functions fail explicitly. The CPU's ordinary
  IRET behavior remains separately verified by `whpx-int`.
- `owned-dos-engine` exposes that bounded adapter through the public
  `engine.exe -- target` contract and is exercised through
  `ntdos64-run --engine`. It accepts COM and validated real-mode MZ images,
  but remains restricted to those three
  services; it does not read or bootstrap any BYOB NTDOS runtime.
- The installed documented SDK (`WinHvPlatformDefs.h`) exposes
  `WHvPartitionPropertyCodeExceptionExitBitmap`,
  `WHvRunVpExitReasonException`, and `WHV_VP_EXCEPTION_CONTEXT` containing
  the faulting instruction bytes and exception type. This is the intended
  potential non-private observation route for the historical `C4 C4 selector`
  BOP byte pattern. The checked real-mode probe instead reaches a
  memory-access exit during normal #UD delivery, rather than an exception exit;
  therefore direct #UD interception is not an available BOP mechanism on this
  backend. `whpx-bop-observe` now instead proves the documented execute-page
  route: a non-executable R/W guest page exits before its `C4 C4 50` bytes run,
  retaining physical RIP and registers. Its one-shot handled-resume proof
  validates only `RIP + 3`; it cannot re-trap later patterns on the same page.
  The next candidate is an opt-in private guest-RAM rewrite to an equal-length
  documented I/O trap, backed by the already verified `whpx-io` emulator path.
  It is documented as a proposal, not an implemented adapter.

## If Available

The first experiment must stay bounded: create one owned partition, map owned
guest RAM, set documented x86 register state, execute a wholly owned halt/IO
fixture, and destroy the partition. It must not load BYOB NTDOS files until
memory exits, I/O exits, interrupt injection and teardown are verified.

## If Unavailable

The executable must report WHPX unavailable and retain the existing loader and
host-capability research path. It must not attempt to enable Hyper-V, invoke
DISM, alter boot configuration, install a driver, or fall back to an intrusive
historical VDM route.

## Sources

- [Windows Hypervisor Platform overview](https://learn.microsoft.com/en-us/virtualization/api/)
- [WHvGetCapability and platform API definitions](https://learn.microsoft.com/en-us/virtualization/api/hypervisor-platform/hypervisor-platform)
- [WHvRunVirtualProcessor](https://learn.microsoft.com/en-us/virtualization/api/hypervisor-platform/funcs/whvrunvirtualprocessor)
