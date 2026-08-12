# NTVMDx64 and the Non-Invasive Runner Requirement

## Decision

The checked NTVMDx64 source is valuable evidence for the historical NTVDM
monitor and modern x64 adaptation problem, but it is **not** an acceptable
execution foundation for the research project's non-invasive `nvtdm` CLI.
It must remain a read-only research input. Its CCPU, MINNT, and HAXM builds do
not satisfy the required deployment boundary.

## Evidence

The source's own `readme.txt` documents that its x64 support compensates for
missing/private console and CSRSS facilities by patching system DLLs in memory
through 32-bit and 64-bit AppInit DLLs. It requires process-wide injection,
signature-policy changes, registry settings, placement of `NTVDM.EXE` in
`SYSTEM32` and `SysWOW64`, and console-version registry changes. Its loader is
also coupled to the historical Kernel32/console-server NTVDM launch protocol.

The optional HAXM execution mode is even further outside the boundary. The
project's `doc/haxm.txt` requires a special kernel driver with BOP and address
translation support, Intel VT-x unrestricted guest capability, test-signing,
Hyper-V launch configuration changes, and normally driver service installation.
The document notes that parts of that modified driver are not available as
source in the repository.

MINNT improves historical build ergonomics and language support; it does not
remove the runtime integration assumptions above. CCPU avoids V86 mode but is
still coupled to the same NTVDM process, console, and loader environment.

## What the Source Still Contributes

NTVDMx64 remains authoritative research evidence for:

1. The distinction between CCPU, monitor/V86, and VT-x execution engines.
2. The need for BOP interception and guest-memory address translation.
3. The historical use of BOP, virtual interrupt state, DOS/BIOS services,
   console, redirector, DPMI, and WOW interactions.
4. The reason host process launch and guest DOS execution must be separate
   operations in a clean wrapper design.

It does not define a public, safe host ABI for a new runner.

## Required Direction for `nvtdm`

The non-invasive runner must be a normal user-mode executable under the
research output tree. It must not require any of the following:

* files placed under Windows system directories;
* AppInit or other process-wide injection;
* registry mutation, console-mode mutation, driver/service installation, or
  boot-configuration changes;
* private CSRSS/console-server ABI calls;
* a preinstalled BOP-aware hypervisor driver.

Its eventual architecture therefore needs a user-mode x86 execution backend
(interpreter or independently contained user-mode virtualization facility), an
explicit monitor-service table for BOP and virtual-interrupt behavior, and a
user-supplied profile containing the constructed DOS artifacts. DOS commands
run inside that guest service boundary. PE images are detected before guest
launch and transferred to normal host `CreateProcess` handling, never routed
through DOS startup.

## Verification Gate

Before any executable is called a non-invasive runner, a clean-user test must
prove that it launches from an arbitrary writable directory with no elevation,
does not write registry/service/system locations, runs a selected DOS command
against its profile, and directly transfers a selected 32-bit and 64-bit PE
program to the host process launcher. NTVMDx64 itself cannot satisfy this gate.
