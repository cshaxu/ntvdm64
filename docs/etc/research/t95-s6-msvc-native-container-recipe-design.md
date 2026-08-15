# T95 S6 MSVC native-container recipe design

## Question

What is the smallest evidence-honest MSVC/x86 build shape for an intact Bochs
2.6 `nogui` container after source-object pruning has been rejected?

## Original metadata

The pinned `vs2008/bochs.vcproj` declares a Win32 executable project
(`ConfigurationType=1`). Its original Release|Win32 metadata supplies:

- preprocessor baseline: `WIN32;NDEBUG;_CONSOLE;_CRT_SECURE_NO_WARNINGS`;
- original include roots: Bochs root and `instrument/stubs`; and
- original system libraries: `winmm.lib`, `comctl32.lib`, `wsock32.lib`.

The executable-owned source set is only `config.cc`, `crc.cc`,
`load32bitOShack.cc`, `logio.cc`, `main.cc`, `osdep.cc`, `pc_system.cc`, and
`plugin.cc`. The solution dependency list, however, names sixteen original
projects: `cpu`, `memory`, `fpu`, `disasm`, `gui`, `iodev`, `iodev_hdimage`,
`iodev_network`, `iodev_sound`, `iodev_usb`, `cpudb`, `stubs`, `bx_debug`,
`niclist`, `bximage`, and `bxcommit`.

That graph is historical product-build metadata, not a statement that every
runtime feature is enabled. It proves that a source-file-reduced MSVC link is
not an available native-container implementation path.

## Container recipe shape

The future recipe must be a separately named, fresh-root **native-container
observation target**, not a build of the historical solution and not an
adapter/CLI target. It has these invariants:

1. Use BuildTools MSVC x86 with a recorded version and `/MT` CRT model.
2. Preserve original Bochs executable/lifecycle ownership, including
   `main.cc`, SIM, logging, PC-system and device-manager lifecycle.
3. Derive project membership and compiler/linker baseline from the original
   VS2008 metadata; do not invent a hand-selected core-object vector.
4. Consume a copied, manifest-hashed CPU3/nogui configuration projection in
   the fresh root only. `refs/bochs/config.h` stays immutable.
5. Materialize a declarative first-profile input that selects original
   `nogui`, declared RAM/ROM test inputs, and original optional-plugin denial
   gates. It may not declare arbitrary devices as absent by a project patch.
6. Keep `src/bx-ntvdm-adapter` and `src/cli` outside the first container
   binary. The pre-existing default-off generic #UD seam remains inactive
   unless a later same-island ABI exercise expressly opts in.
7. Require reset and controlled-stop evidence before any guest, BOP, DEM,
   DOS, WOW or CLI runtime claim.

## Explicit exclusions

This is not permission to invoke the VS2008 solution, run `bochs.exe`, or
start a full build now. It does not enable GUI, debugger, network, sound, USB,
optional storage or a new device merely because a historical project exists.
It does not move profile parsing, SIM construction, plugins, timers, memory,
CPU or shutdown into the adapter.

## Next admission gate

Before the first native-container invocation, create a source-to-build
manifest that maps every original project dependency to a projected feature
status: required lifecycle, compiled-but-runtime-disabled, or explicitly
unavailable. The manifest must additionally name the exact original
`plugin_ctrl`/profile configuration input and negative predicates. Only then
may one fresh-root MSVC container build observation be considered.
