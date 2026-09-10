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
one or more selected real runtime workloads part of its own closure. A link,
BOP ingress or host-only fixture is never guest-execution proof. CPU30 and
Bochs are not runtime alternatives; guest media stays immutable. CSRSS/kernel-
VDM recreation is excluded: only bounded public Win32/session equivalents
retaining original caller results are admissible.

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

### Current candidate packages

The queue does not allocate a standalone single-process compatibility-matrix
package. A matrix run before its owners have runtime closure duplicates the
admission workload while providing no new recovery boundary; a matrix run
afterward belongs to the final release package. Each owner package below
instead declares its selected real workload, original return path and failure
classification as closure evidence.

1. **Closed — CPU40 DPMI32 provider/bootstrap:** M0 T395 formally composes the
   complete 15-body CPU40 cohort, accounts for `53:00`–`53:18`, and proves the
   original DOSX bootstrap through its reached DPMI path.  Its explicit
   limitation is that available original clients require WOW32; the closure
   record transfers that lifecycle proof rather than claiming it.
2. **WOW32 provider admission, bootstrap and WOW16 NE lifecycle activation:**
   bring all 77 active WOW32 provider bodies, 105 declaration carriers, their
   original tables/resources and one original-shaped provider image into the
   formal graph. Recover `51h -> W32Init -> W32Dispatch`, WOWEXEC routing,
   non-fast callback/re-entry and task lifetime; prove selected Win16 NE
   start/exit workloads. A private system call is retained as an explicit
   unavailable boundary rather than a reason to omit its original caller.
3. **WOW16 service, UI and host integration activation:** recover selected
   original WOW32 USER, GDI, shell, multimedia and network call forms through
   public APIs and prove them with selected Win16 workloads. Preserve exact
   USER/CSRSS/Win32k private-boundary failures; do not synthesize `W32Init`, a
   partial callback table or a private system shell.
4. **x64 production retirement and x86 source restoration:** retire native-x64
   product compilation, linking, staging and runtime acceptance before
   cross-process work. Restore every source divergence whose only purpose is
   native-x64 compilation to its selected original/x86 form. The shared
   session mapping-manager contract remains the explicit architecture-neutral
   exception; host x64 Windows continues to run the x86 worker through WOW64.
5. **Single-process WOW16 application lifecycle recovery:** after x64
   retirement and before broker work, recover the immediate original owner
   that prevents the selected Win16 workload from starting, running and
   returning in one x86 worker. It must retain the source-owned guest error
   route until a direct original owner is proven; it may not substitute a
   private USER/CSRSS shell, broker delivery, or CPU-trace inference.
6. **Cross-process broker closure:** after the applicable one-process owner
   contracts close, recover only required BaseSrv-style registration,
   command-queue, notification and disconnect-cleanup semantics through
   public IPC, with selected brokered lifecycle rows. It cannot own guest,
   COMMAND, SoftPC or WOW behavior.
7. **Multi-process matrix and product release:** run the cross-owner
   DOS/Win16 compatibility matrix, prove the brokered multi-process x86-worker
   profile on supported x64 Windows hosts, then publish the portable media
   layout and release package.

### Predecessor capability context

1. **Pure DOS CPU40/SoftPC startup and COMMAND baseline:** freeze one
   console-owning, non-debug, executable-relative media container.  Follow the
   original CPU40/SoftPC boot path through NTIO/NTDOS and COMMAND, recovering
   only the first complete original host/device/console/timer owner that
   prevents it.  Prove a declared original COMMAND built-in completes; do not
   synthesize guest input, a DOS loader or a machine result.
2. **Single-process interactive COMMAND console:** recover original
   interactive `COMMAND.COM` bootstrap without `/C`, not an app command
   parser. Preserve the original Base VDM `VDMINFO` initial-DOS-record and
   `cmdGetNextCmd`/`cmdGetStartInfo` distinction between existing and new
   console. Then recover the public-Win32 console-input to original SoftPC
   keyboard-controller/IRQ and guest DOS `CON` path. Prove one prompt, `dir`,
   line editing, Ctrl+C and `exit` in one fixed package. Do not make every
   prompt line a `GetNextVDMCommand` record, modify `COMMAND.COM`, or recreate
   CSRSS/BaseSrv transport; cross-process delivery remains package 12.

   The product display contract has one active guest surface at a time.
   Normal character-mode DOS begins in the process Console: that Console
   receives guest-visible output and input, while app does not pre-open a
   presentation window. Original SoftPC graphics/fullscreen state, original
   PIF fullscreen disposition, or explicit Alt+Enter transfers the active
   guest surface to the app presentation window; its keyboard route then
   becomes active and the Console is only the dormant host cursor/wait
   surface. Alt+Enter from that window closes the window and restores the
   Console route; it does not change the guest video mode or fabricate a text
   rendering. Consequently, a graphics-only/window-only program can correctly
   leave only the Console cursor/wait surface after that return. A program
   whose original guest video state is text-capable resumes normal Console
   presentation. App obtains these selections through original
   SoftPC/COMMAND/PIF state and the existing machine-facing boundary, not by
   independently guessing an executable type. Project diagnostics remain
   default-off explicit report files or debugger output; they never share
   guest Console output.

   **Planned S sequence after admission:**

   The complete candidate execution contract is maintained in
   [single-process interactive COMMAND console recovery](proposal-single-process-interactive-command-console-recovery-001.md).

   1. **S1 — Original interactive startup-contract audit.** Map
      `BaseCheckVDM`, `VDMINFO`, `srvvdm`, `cmdGetNextCmd` and
      `cmdGetStartInfo`; record every field difference between an interactive
      initial DOS record and the existing `/C` record. No source change.
   2. **S2 — Interactive initial DOS-record binding.** Publish the exact
      source-shaped no-argument record through the existing local Base VDM
      route; retain positional input as `COMMAND.COM /C <command>`. Verify
      copy, first-call, re-entry and failure results locally.
   3. **S3 — Console-first startup and diagnostic isolation.** Do not create a
      presentation window for normal character mode; recover the original
      SoftPC Console/stream ordering and move project observation output to
      explicit report files or debugger output.
   4. **S4 — Console-input to SoftPC keyboard/IRQ1.** Implement the session
      input pump using public Console events and the source-shaped SoftPC
      keyboard path. Verify ordinary keys, Enter, Backspace, Ctrl+C, repeat,
      and cancellation without app-level DOS parsing.
   5. **S5 — Display-backend arbitration.** Recover the one-active-surface
      contract for text, graphics, Alt+Enter and return-to-Console, including
      output/focus ownership and no fabricated text rendering.
   6. **S6 — Original PIF fullscreen disposition.** Audit and bind the original
      PIF Console/fullscreen fields; verify text, fullscreen and source-shaped
      failure results without extension-based program classification.
   7. **S7 — Fixed interactive runtime matrix.** In one fixed Console
      container prove prompt, `ver`, `dir`, editing, Ctrl+C and `exit`. Run
      `ntvdm.exe edit.com` and prompt-local `edit.com` as integration rows;
      attribute any external-EXEC failure to its immediate original owner
      rather than treating it as Console completion.
   8. **S8 — Closure and publication.** Audit all routes and default
      diagnostics, run the formal Ninja and fixed-container checks, and
      publish only a verified improvement to both `build/output` and the
      runnable `O:\\ntvdm64` root.
3. **Pure DOS `.COM` and MZ `.EXE` execution and parent return:** original
   `$Exec`, file/JFN/SFT, PSP/arena/environment, relocation, child exit and
   COMMAND continuation using immutable low-dependency workloads.  The proof
   requires both a `.COM` and MZ program, each returning by the original guest
   parent/COMMAND path.
4. **Pure DOS device, file and graphics workload closure:** original keyboard,
   console, file/seek/IOCTL, timer/IRQ and SoftPC video paths under selected
   DOS workloads.  The package starts from the proven program-execution
   baseline and admits only the original owner immediately blocking its
   workload.
5. **DOS extender execution:** original XMS/A20/UMB/INT15 and
   DPMI/DPMI32/DOSX runtime paths with mapping-manager identity boundaries and
   real extender workload return. Its selected workload is owner-package
   closure evidence, not a row deferred to a later generic matrix.
6. **Redirector and host-resource execution:** T372 is the closed
   source/binding baseline only: it did not ship `VDMREDIR.dll`.  The next
   activation package must compose that original DLL product first, then prove
   local VDMREDIR and bounded public NetAPI behavior; legacy
   NetBIOS/DLC/private-RPC remains explicit. Its selected resource workload
   and original return/failure path are closure evidence for that owner.
7. **COMMAND native-child local lifecycle:** reached `54:08/0A/0B`, invokes
   `run16.exe` through the public child-process lifecycle rather than
   `COMSPEC`/`cmd.exe`, then preserves original re-entry/event/return and
   explicit unavailable CSRSS-era branches. This remains inside one process;
   it does not create or depend on a broker. Its selected native-child
   workload and original return/failure path are closure evidence for that
   owner.
8. **WOW16 bootstrap, callback and Win16 NE lifecycle:** original WOWEXEC
   command route, task/callback/monitor, single-session UI and NE start/exit.
9. **WOW16 service, UI and host integration:** selected WOW32 provider
   packages through public APIs, preserving private-shell failures.
10. **x64 production retirement and x86 source restoration:** retire the
   native-x64 worker/product route before cross-process recovery. Audit every
   x64-introduced source, build, test, staging and publication divergence;
   restore the selected original/x86 form when x64 compilation is its sole
   reason. The shared mapping-manager remains an explicit retained exception.
11. **Cross-process control-broker closure:** after the applicable
   single-process owner contracts close,
   recover BaseSrv-style multi-process registration, command delivery,
   notification and cleanup through public IPC only. The approved
   launcher--worker topology and its exclusions are specified below.
12. **Multi-process matrix and product release:** selected multi-process
   DOS/Win16 matrix with the x86 worker on supported x64 Windows hosts,
   unpack-and-run manifests and release packaging.

### Approved x64 production retirement and x86 source restoration

This is the detailed design direction for candidate 10, not an active packet
or authorization to alter the current WOW runtime work. It precedes the
launcher--worker broker topology because a single x86 worker is the selected
product runtime for both 32-bit and supported 64-bit Windows hosts. WOW64 is
the host operating system's x86-process facility; it is not a second project
NTVDM implementation.

The candidate removes the native-x64 product route: x64 host compilation and
link graphs, the architecture-named `ntvdm32.exe`/`ntvdm64.exe`
publication/staging slots, x64 runtime fixtures and the paired-x86/x64 release
requirement. It replaces their product contract with one x86 background
worker, `ntvdm.exe`, and one x86 WOW provider, `wow32.dll`. An x86 foreground
launcher may still use public `CreateProcess` for a native x64 PE target; that
normal host-process handoff does not require an x64 VDM worker or launcher.

Before any source restoration, the candidate produces a complete
divergence-to-rationale ledger for every source, generated binding, build
manifest, test and document change attributed to native-x64 compilation. A
row is restored only when native-x64 compilation is its sole retained reason.
The selected original/x86 expression, declaration, layout, calling convention
and failure order are then restored. Changes that independently preserve the
x86 runtime's source contract, memory safety, build reproducibility or public
Windows boundary remain only with their own non-x64 rationale; they are not
silently reverted by a filename or type-based sweep.

The sole declared non-reversion is the shared session mapping-manager and its
typed lease boundaries. It remains architecture-neutral: per-session,
fixed-width at component boundaries, and the only permitted compatibility path
for reached guest-address mappings. This exception does not preserve generic
x64 pointers, x64 worker ABI branches, cross-architecture object graphs or a
native-x64 runtime claim.

The candidate updates the product-goal, architecture, execution/publication,
build and staging authorities together. It removes every requirement to
compile, link, stage or test an x64 worker, replaces paired-product release
claims with x86-worker-on-x64-Windows acceptance, and retains historical x64
records only as indexed evidence. It must prove one formal x86 product and one
fixed x86 workload on a supported x64 Windows host before declaring the
retirement closed. It does not modify guest media, alter original DOS/WOW
semantics, create a second x86 implementation, or begin broker IPC work.

### Approved launcher--worker broker topology

This is the detailed design direction for candidate 11, not an active packet
or an authorization to interrupt the current single-process WOW work.  It is
admitted only after the applicable DOS and WOW single-process owner contracts
have their selected runtime closures.

The published product has two roles:

```text
run16.exe (x86)                        foreground launcher
  -> public native CreateProcess       native PE target, including x64 PE
  -> broker session + command record   DOS, PIF or Win16 target
       -> ntvdm.exe                    background x86 VDM worker
            -> original COMMAND or WOWEXEC/WOW32 task path
```

The launcher is the product-facing command classifier and process parent.  It
does not become a DOS shell, guest loader, WOW scheduler or replacement
BaseSrv.  Native PE targets use ordinary public `CreateProcess`; DOS, PIF and
NE targets are copied into a broker-owned fixed-width command record before a
worker is created.  The worker remains the owner of original host startup and
of every subsequent original COMMAND/WOW task transition.

`ntvdm.exe` is a pure background worker, never a user-facing launcher or
command interpreter. Its command line contains only original-shaped worker
bootstrap information: the required `-f` creation marker, the applicable
WOW/session switches, the bootstrap-kernel carrier where required, and a
broker bootstrap reference. The selected application path, command tail,
environment, PIF data and standard-stream metadata do not move into worker
arguments. They remain in the copied BaseVDM-style record and are consumed
through the original `GetNextVDMCommand` call shape. In particular, a WOW
`-a` carrier identifies the bootstrap kernel, not the user-selected NE
application. `run16.exe` alone owns product-facing classification, worker
creation and user-visible process results.

The original COMMAND `54:08`/`cmdExec32` native-child route remains owned by
COMMAND and does not invoke `COMSPEC` or `cmd.exe`. It creates `run16.exe` as
the product-facing child with the original copied application, command,
environment and standard-stream contract. `run16.exe` then classifies that
target: native PE continues through public `CreateProcess`; DOS, PIF and NE
targets enter the broker-record then `ntvdm.exe` worker route. This preserves
the existing COMMAND re-entry, event and exit ordering without introducing a
replacement COMMAND parser or generic child scheduler.

The broker is the bounded replacement for the observable BaseSrv coordination
slice, not for CSRSS or the NT kernel.  It owns per-user worker registration,
opaque stable session identity, copied command-record queues, availability
notification, launcher waiting, disconnect detection and cleanup.  The
bootstrap session identifier preserves any source-required worker/session
meaning but is not itself an IPC authority: launcher and worker authenticate a
versioned broker endpoint with a distinct unguessable session capability.
Broker messages contain only fixed-width copied data and opaque broker IDs;
they never carry native HANDLEs, pointers, guest addresses, mapping tokens or
CPU objects.

The transport must retain the two source-visible acquisition behaviors:

- A DOS/PermCom `GetNextVDMCommand` request waits for a later record.  A
  broker notification reaches a worker-local client, which signals only that
  worker's existing local wake object; the original client retries its request.
- An empty WOW request remains nonblocking and returns the original empty
  result.  After original `RegisterWowExec` has identified the active
  WOWEXEC boundary, a newly queued WOW record causes only the source-audited
  availability notification there; WOWEXEC itself asks again through
  `GetNextVDMCommand`.  The broker does not run a generic USER message pump or
  dispatch a task directly.

Worker exit is a real worker-process boundary.  The broker reports the
source-shaped completion/disconnect outcome to the launcher and releases the
session record; it does not convert a guest/task exit into launcher process
termination.  This permits removal of same-process composition escapes that
currently exist only because `app` and the MVDM host share one executable.
The intended result is fewer project-authored architectural divergences around
the original `ntvdm.exe` entry, environment and termination boundaries.  It
does not justify hiding necessary source divergences, recreating private NT4
services, or claiming byte-identical NT4 system architecture.

Candidate 11 must close in these order-preserving increments: freeze the
launcher--worker bootstrap ABI and broker capability rules; establish worker
registration and initial-record delivery; prove DOS record wait/wake and
worker exit; then prove the separate WOWEXEC registration/notification and
second-task route. Candidate 12 alone owns multi-worker matrix coverage,
release claims and portable-package publication.
