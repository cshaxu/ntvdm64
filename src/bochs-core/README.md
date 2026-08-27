# bochs-core

Pinned Bochs 2.6 original mirror. Only `adapter-bochs` may call this
component. Any later divergence follows the mirror audit rules and is
registered here.

## M0 T273 S1 recovery register

The old tree is comparison-only until its individual files are copied into
this root by later admitted packets.  Its 205 upstream-relative candidates
compare to the pinned Bochs 2.6 baseline as 177 exact and 28 registered
differences; none lacks an upstream counterpart.  The supporting
[recovery audit](../../docs/etc/operations/m0-t273-s1-bochs-recovery-audit-001.md)
records the complete changed-file set and routes all new executable bodies to
a future private `bochs-core-overlay` implementation boundary.

The 28 difference candidates are not authorized merely by this register.
Their prior local divergence identifiers and detailed evidence must be
revalidated at copy time.  `bochs-core` has no service source yet, and no
formal build may read `src.old/bochs-core`.

## M0 T273 S2 configuration exception

- `BX-CONFIG-001` — upstream generated `config.h` selected CPU6/x86-64 guest
  execution.  The admitted bare machine is CPU5/Pentium-MMX, so the two guest
  profile macros are set to `5` and `0`.  This changes no host-width contract;
  x86 and x64 host binaries remain required.  The local lines carry matching
  `DIVERGENCE(BX-CONFIG-001)` comments.  File: `config.h`.
- `BX-CONFIG-002` — upstream generated `config.h` hard-coded its configure
  host's four-byte pointer width.  The local conditional selects four bytes
  for Win32/x86 and eight for Win64/x64, preserving one CPU5 guest profile
  while preventing native host-pointer truncation.  The local lines carry a
  matching `DIVERGENCE(BX-CONFIG-002)` comment.  File: `config.h`.

## M0 T273 S4 reconciled divergence index

Every non-identical mirror file is registered below. Private executable bodies
live only in `bochs-core-overlay`; these mirror files retain original subsets,
declarations or minimal call sites.

- `BX-BUILD-002` — historical build-tree/compiler spelling is adjusted only
  to preserve the same declarations. Files: `cpu/i387.h`,
  `gui/paramtree.cc`, `gui/siminterface.h`.
- `BX-BUILD-003` — root-relative include spelling is redirected to this
  mirror root only. Files: `bochs.h`, `gui/siminterface.h`.
- `BX-MACH-023` — reset/exit is reduced from the full product shell to its
  reset-window subset. The mirror crops the product bodies; the private
  Bochs-method replacements live in `bochs-core-overlay/pc_system_lifecycle.cc`.
  File: `pc_system.cc`.
- `BX-CORE-DIV-001` and `BX-EXEC-016` — private profile/entry state supports
  minimal predicates and calls. Files: `cpu/access.cc`, `access32.cc`,
  `cpu.cc`, `cpu.h`, `ctrl_xfer16.cc`, `ctrl_xfer32.cc`, `exception.cc`,
  `icache.cc`, `init.cc`, `io.cc`, `stack.cc`, `string.cc`.
- `BX-UD-001` and `BX-UD-002` — a selector-blind opaque event hook is private
  overlay code. Files: `cpu/cpu.cc`, `cpu/cpu.h`, `cpu/exception.cc`.
- `BX-IO-025`, `BX-IO-029`, `BX-IO-030`, `BX-MACH-024`, `BX-MACH-025`,
  `BX-MACH-082` — full device/plugin composition is reduced to private minimal
  port-space/PIC lifecycle bodies. Files: `iodev/devices.cc`, `iodev/iodev.h`,
  `iodev/pic.cc`, `iodev/pic.h`.
- `BX-MACH-026`, `BX-MACH-027`, `BX-MACHINE-091` — original machine/keyboard
  product lifecycles are reduced to private native boundaries with no GUI or
  host-input composition. Files: `iodev/iodev.h`, `iodev/keyboard.cc`,
  `iodev/keyboard.h`, `memory/memory.h`.
- `BX-MEM-007`, `BX-MEM-010`, `BX-MEM-020`, `BX-MEM-024`,
  `BX-CORE-DIV-006` — SIM-dependent RAM setup is replaced by private checked
  ordinary-RAM allocation/copy bodies; mirror files keep declarations. Files:
  `memory/memory.h`, `memory/misc_mem.cc`.
- `BX-DPMI-MECH-001` — a selector-blind copied protected-frame declaration
  allows a private overlay to prevalidate and apply a same-privilege segment
  transition. No MVDM, BOP or service identity enters the mirror. Files:
  `cpu/cpu.h`, `../bochs-core-overlay/cpu/protected_transition.cc`.
- `BX-DPMI-MECH-002` — a selector-blind copied real-mode frame declaration
  allows the private overlay to atomically apply validated CS/SS/SP/IP/FLAGS
  after a returned CPU loop. It has no interrupt, DPMI, BOP or guest-memory
  meaning. Files: `cpu/cpu.h`,
  `../bochs-core-overlay/cpu/realmode_profile.cc`.

## M0 T286 final minimization measure

The current pinned comparison has 27 non-identical upstream-relative mirror
files (393 insertions / 356 deletions) and no project-authored production file
under this root.  The ten private implementation bodies (1,067 physical
lines) remain in `../bochs-core-overlay` and are indexed by its README.  The
final evidence and focused x86/x64 Ninja validation are recorded in
[`m0-t286-s6-final-minimization-validation-001.md`](../../docs/etc/evidence/m0-t286-s6-final-minimization-validation-001.md).
