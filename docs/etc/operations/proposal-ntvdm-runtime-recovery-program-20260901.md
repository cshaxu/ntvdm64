# NTVDM runtime recovery program

## Objective and rules

Recover the original NTVDM design as a CLI-hosted CPU40/SoftPC product. The
primary outcome is one-process functional compatibility: original MVDM source
must be reused with the smallest possible mirror diff/private overlay and the
closest attainable original observable behavior for DOS, DOS extenders, Win16
and the original COMMAND host-child path. A cross-process broker is a later,
separate coordination outcome; it cannot make an incomplete one-process route
appear complete. Every candidate maps its full original owner path, recovers
only original bodies and same-shaped bindings, runs package tests, and makes
one frozen integration observation. A link, BOP ingress or host-only fixture
is never guest-execution proof. CPU30 and Bochs are not runtime alternatives;
guest media stays immutable. CSRSS/kernel-VDM recreation is excluded: only
bounded public Win32/session equivalents retaining original caller results are
admissible.

The ordered list is an **activation frontier**, not a fixed promise to test
every currently linked library before recovery. When the earliest workload is
blocked by a named original owner component missing from the formal graph, the
next T is immediately that component's complete adoption/binding/formal-link/
same-workload-runtime closure. Only then does the predecessor resume. This
prevents a known missing component from being deferred behind unrelated work.

## Ordered capability packages

### Owner-directed single-process compatibility priority — 2026-09-02

Static package closure, BOP ingress, and a fixed observation are not DOS
execution results.  The active WOW16 service package reached a real private
NTUSER/CSRSS registration boundary before the product has proved that
`COMMAND.COM` can execute an external DOS program.  The owner has directed
that this ordering be corrected.  WOW keeps its exact unavailable evidence;
no synthetic `W32Init`, partial handler table or private-shell replacement is
permitted.  It resumes only after the preceding single-process package has
its stated runtime proof.

### Closure vocabulary

- **source/binding baseline** means original source selection, same-shaped
  adapter/overlay disposition, selected build membership and focused local
  evidence. It is an input to later runtime recovery, never a claim that an
  application runs.
- **single-process runtime closure** requires a selected original workload to
  execute in one fixed `ntvdm.exe` process, return through its original owner
  path, and have every reached host/machine boundary classified.
- **cross-process closure** begins only after the relevant single-process
  contract is runtime closed. It recovers only BaseSrv-like registration,
  commands, notification and teardown using public IPC; it does not recreate
  CSRSS, kernel VDM, or a second provider.

1. **Pure DOS CPU40/SoftPC startup and COMMAND baseline:** freeze one
   console-owning, non-debug, executable-relative media container.  Follow the
   original CPU40/SoftPC boot path through NTIO/NTDOS and COMMAND, recovering
   only the first complete original host/device/console/timer owner that
   prevents it.  Prove a declared original COMMAND built-in completes; do not
   synthesize guest input, a DOS loader or a machine result.
2. **Pure DOS `.COM` and MZ `.EXE` execution and parent return:** original
   `$Exec`, file/JFN/SFT, PSP/arena/environment, relocation, child exit and
   COMMAND continuation using immutable low-dependency workloads.  The proof
   requires both a `.COM` and MZ program, each returning by the original guest
   parent/COMMAND path.
3. **Pure DOS device, file and graphics workload closure:** original keyboard,
   console, file/seek/IOCTL, timer/IRQ and SoftPC video paths under selected
   DOS workloads.  The package starts from the proven program-execution
   baseline and admits only the original owner immediately blocking its
   workload.
4. **DOS extender execution:** original XMS/A20/UMB/INT15 and
   DPMI/DPMI32/DOSX runtime paths with mapping-manager identity boundaries and
   real extender workload return.
5. **Redirector and host-resource execution:** local VDMREDIR first, then
   bounded public NetAPI; legacy NetBIOS/DLC/private-RPC remains explicit.
6. **COMMAND native-child local lifecycle:** reached `54:08/0A/0B`, public
   `CreateProcess` child lifecycle, original re-entry/event/return and
   explicit unavailable CSRSS-era branches. This remains inside one process;
   it does not create or depend on a broker.
7. **WOW16 bootstrap, callback and Win16 NE lifecycle:** original WOWEXEC
   command route, task/callback/monitor, single-session UI and NE start/exit.
8. **WOW16 service, UI and host integration matrix:** selected WOW32 provider
   packages through public APIs, preserving private-shell failures.
9. **Single-process NTVDM compatibility matrix:** selected DOS, extender,
   COMMAND-native-child and Win16 workloads execute against their recovered
   owner contracts; a failure reopens only its immediate original owner.
10. **Cross-process control-broker closure:** after the single-process matrix,
    recover BaseSrv-style multi-process registration, command delivery,
    notification and cleanup through public IPC only.
11. **Multi-process matrix, x64 compatibility and product release:** selected
    multi-process DOS/Win16 matrix, paired x86/x64 behavior where runnable,
    unpack-and-run manifests and release packaging.
