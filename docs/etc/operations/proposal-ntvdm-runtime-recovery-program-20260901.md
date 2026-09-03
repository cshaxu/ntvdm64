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
2. **Single-process interactive COMMAND console:** recover original
   interactive `COMMAND.COM` bootstrap without `/C`, not an app command
   parser. Preserve the original Base VDM `VDMINFO` initial-DOS-record and
   `cmdGetNextCmd`/`cmdGetStartInfo` distinction between existing and new
   console. Then recover the public-Win32 console-input to original SoftPC
   keyboard-controller/IRQ and guest DOS `CON` path. Prove one prompt, `dir`,
   line editing, Ctrl+C and `exit` in one fixed package. Do not make every
   prompt line a `GetNextVDMCommand` record, modify `COMMAND.COM`, or recreate
   CSRSS/BaseSrv transport; cross-process delivery remains package 11.

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
   real extender workload return.
6. **Redirector and host-resource execution:** T372 is the closed
   source/binding baseline only: it did not ship `VDMREDIR.dll`.  The next
   activation package must compose that original DLL product first, then prove
   local VDMREDIR and bounded public NetAPI behavior; legacy
   NetBIOS/DLC/private-RPC remains explicit.
7. **COMMAND native-child local lifecycle:** reached `54:08/0A/0B`, public
   `CreateProcess` child lifecycle, original re-entry/event/return and
   explicit unavailable CSRSS-era branches. This remains inside one process;
   it does not create or depend on a broker.
8. **Single-process NTVDM compatibility matrix:** selected DOS, extender,
   Redirector and COMMAND-native-child workloads execute against their
   recovered owner contracts; a failure reopens only its immediate original
   owner. Win16 is deliberately absent from this first matrix because its
   original owner packages are still queued below; their rows join the same
   matrix only after packages 9 and 10 close.
9. **WOW16 bootstrap, callback and Win16 NE lifecycle:** original WOWEXEC
   command route, task/callback/monitor, single-session UI and NE start/exit.
10. **WOW16 service, UI and host integration matrix:** selected WOW32 provider
   packages through public APIs, preserving private-shell failures.
11. **Cross-process control-broker closure:** after the single-process matrix,
   recover BaseSrv-style multi-process registration, command delivery,
   notification and cleanup through public IPC only.
12. **Multi-process matrix, x64 compatibility and product release:** selected
   multi-process DOS/Win16 matrix, paired x86/x64 behavior where runnable,
   unpack-and-run manifests and release packaging.
