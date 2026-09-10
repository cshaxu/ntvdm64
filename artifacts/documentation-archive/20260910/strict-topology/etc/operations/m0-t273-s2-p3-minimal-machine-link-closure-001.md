# M0 T273 S2 P3: minimal machine link closure

## Objective and method

Prove the narrow Bochs-only path
`construct -> hardware reset -> opaque real-mode bytes -> finite timer budget
-> controlled stop -> cleanup` without configuring or linking the Bochs product
shell.  The object list was derived from the actual reset/decode/link path;
the linker, rather than a trace hit or a permissive stub, decided each added
CPU unit.

## Closed source inputs

- `adapter-bochs/bounded_run.{h,cc}` is the sole new project-owned mechanical
  request: fixed copied bytes, real-mode entry, timer budget and typed result.
  It has no BOP, OpenNT, DOS, SoftPC/CCPU, session or host-service vocabulary.
- `tests/adapter-bochs/m0_t273_s2_bounded_run_fixture.cc` supplies only an
  opaque `EB FE` two-byte loop for the timer test.
- Initial compilation exposed two omitted, transitive native headers used by
  the retained `iodev` declarations: `iodev/virt_timer.h` and
  `iodev/slowdown_timer.h`.  Both were copied byte-identically from the pinned
  Bochs 2.6 baseline.  They introduce no divergence or behavior.
- The current mirror comparison is 207 baseline-relative files: 178 exact,
  29 registered differences and no missing baseline-relative path.

## Object-closure disposition

- The final closure contains 149 x64 objects and the same 149 x86 objects:
  CPU decode/execute, FPU/MMX support, RAM, logging, original parameter tree,
  PIC/keyboard support, private CPU/RAM/port-space overlay and the seven
  `adapter-bochs` / fixture units.
- `fetchdecode.cc` holds static references across the CPU instruction handler
  family.  CPU5 configuration therefore still requires the whole original CPU
  instruction-object group as a link closure; this does **not** enable a newer
  CPU profile or product device.
- `iodev/devices.cc` is deliberately absent.  It is the old full device/plugin
  product shell and duplicates the private `minimal_port_space.cc` members.
  Its absence eliminates all product plugin/device requirements.  The native
  `pic.cc` and `keyboard.cc` stay only because the admitted minimal PIC and
  optional headless 8042 use their original types and lifecycle.
- No GUI implementation, `main.cc`, plugin registry, configured device
  archive, firmware image, OpenNT source, guest binary, `src.old/` input or
  `bochs.exe` participates in this closure.

## Verification

Both build outputs are disposable and reside below `build/M0-T273-S2/`.

- MSVC x64 `/MT`: 149-object link succeeded; the fixture emitted native CPU
  hardware-reset/CPU5 CPUID/RAM-allocation diagnostics and exited `0`.
- MSVC Win32/x86 `/MT`: the same 149-object closure linked; the same fixture
  emitted the same reset diagnostics and exited `0`.

The run therefore proves only the stated finite mechanical operation.  It does
not load a guest image, run a BOP, enable a historical host provider, compose
BIOS product behavior or claim general runtime continuity.

## Next boundary

S2/P4 must expose a reusable opaque lifecycle state that can stop and resume
without recreating the machine, then verify that no caller outside
`adapter-bochs` receives a Bochs type.  This P3 fixture intentionally destroys
its one machine after the budget result, so it is not evidence for resume.
