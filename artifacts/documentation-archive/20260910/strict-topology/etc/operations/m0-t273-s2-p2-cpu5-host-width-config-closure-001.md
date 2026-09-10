# M0 T273 S2 P2: CPU5 and host-width configuration closure

## Scope

Close the first configuration prerequisite for the smallest Bochs-only
machine: the admitted CPU5/Pentium-MMX guest profile must compile on both
Win32/x86 and Win64/x64 hosts without truncating native Bochs pointers.
This is a configuration recovery checkpoint, not a machine link or runtime
claim.

## Findings and disposition

- The recovered generated Bochs configuration selected `BX_CPU_LEVEL == 6`
  and `BX_SUPPORT_X86_64 == 1`.  That contradicts the owner-approved CPU5 /
  Pentium-MMX guest target and makes `minimal_sim` reject the machine before
  initialization.
- The same configuration hard-coded `SIZEOF_INT_P == 4`.  On an x64 host it
  made `bx_ptr_equiv_t` and `bx_hostpageaddr_t` 32-bit, producing pointer
  truncation warnings in native CPU memory access code.  A mapping-manager or
  adapter cannot safely repair that internal Bochs representation.
- `bochs-core/config.h` now has two registered, local configuration changes:
  `BX-CONFIG-001` selects CPU5/non-x86-64 guest execution, and
  `BX-CONFIG-002` chooses `SIZEOF_INT_P` from `_WIN64`.  The latter retains a
  32-bit layout for the Win32 build and an 8-byte pointer representation for
  the Win64 build.  Neither change changes the required dual-host product
  policy.

## Verification

All probes used only new-root sources, `WIN32`, MSVC `/MT`, and disposable
paths below `build/M0-T273-S2/`:

- x64: `adapter-bochs/minimal_sim.cc` compiled with the CPU5 configuration.
- x64: `bochs-core/cpu/access32.cc` compiled after `BX-CONFIG-002`; the prior
  `C4311` / `C4312` native-pointer truncation diagnostics were absent.
- x86: the same `access32.cc` and `minimal_sim.cc` compiled successfully.

The current mirror comparison is 205 baseline-relative source files: 176
byte-identical, 29 registered differences, and no absent baseline path.  The
additional changed file is the registered `config.h` configuration boundary;
the earlier S2/P1 record remains an accurate pre-P2 recovery snapshot.

## Non-claims and next boundary

This checkpoint does not link a minimal machine, start a Bochs product shell,
load firmware or a guest image, decode BOP, or introduce OpenNT/SoftPC/Win32
service semantics.  S2/P3 must select a non-overlapping CPU/RAM/port-space
object list, link the opaque lifecycle fixture, and report unresolved native
object edges exactly.
