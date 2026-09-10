# T95 S6 MSVC native-container feature manifest

## Scope

This is a source/metadata feature manifest for the future MSVC/x86 native
Bochs container observation. It is not a build recipe, link list or runtime
claim. It separates VS2008 solution build order from actual executable link
membership and from the CPU3 projected runtime feature surface.

## Facts

- `bochs.vcproj` Release|Win32 explicitly names only `winmm.lib`,
  `comctl32.lib` and `wsock32.lib` as linker dependencies. The `.sln` lists
  sixteen project-order dependencies but does not expose their final object or
  library membership in that project file.
- The accepted CPU3 projection has `BX_WITH_NOGUI=1`, `BX_WITH_WIN32=0`,
  `BX_DEBUGGER=0`, `BX_INSTRUMENTATION=0`, `BX_SUPPORT_X86_64=0`,
  `BX_SUPPORT_FPU=0`, `BX_SUPPORT_PCI=0`, `BX_SUPPORT_SMP=0`,
  `BX_NETWORKING=0`, `BX_SUPPORT_SOUNDLOW=0`, USB controller support `0`,
  `BX_SUPPORT_VMX=0` and `BX_SUPPORT_SVM=0`.
- `bximage`, `bxcommit` and `niclist` are separate executable utility
  projects. Their appearance in solution ordering is not container admission.

## Project classification

| Original project / source owner | Build-graph fact | CPU3/profile status | Native-container disposition |
| --- | --- | --- | --- |
| `bochs` executable (`main`, `config`, `plugin`, `logio`, `pc_system`, etc.) | Original native lifecycle owner | Required lifecycle | Required; retain original ownership. |
| `cpu` | Original 93-unit project; S6 object-proven | CPU3 execution profile | Required, native CPU island. |
| `memory` | Original project | RAM/ROM mechanics required | Required, native mechanics. |
| `iodev` | Original project includes stock manager/core | Static presence is not feature enablement | Required lifecycle owner, but profile must record retained core effects. |
| `gui` | Owns SIM/parameter tree and original `nogui` backend | `nogui` only; no project GUI | Required lifecycle support; GUI frontends remain unavailable. |
| `stubs` | Original static project | Required by native device-manager declarations | Required support; not a device enablement grant. |
| `cpudb` | Original CPU database project | CPU model/profile dependency not yet link-proven | Compiled support candidate; no model feature claim. |
| `fpu`, `disasm`, `bx_debug` | Original static projects | FPU and debugger disabled; disassembly/debug paths not admitted | Compiled-but-runtime-disabled unless the original link trace proves a required lifecycle reference. |
| `iodev_hdimage` | Original storage helper project | No guest disk admitted in first reset profile | Compiled-but-runtime-disabled; no storage profile admission. |
| `iodev_network`, `iodev_sound`, `iodev_usb` | Original optional device projects | Networking, sound and USB disabled | Compiled-but-runtime-disabled; no device admission. |
| `bximage`, `bxcommit`, `niclist` | Standalone utility executables | Outside container profile | Explicitly unavailable to the container; solution-order only. |

## Required profile inputs

The first native-container profile may name only original configuration facts:

- original `display_library: nogui`;
- CPU3 and a declared RAM size;
- approved original BIOS/VGA ROM fixture inputs where a reset fixture needs
  them; and
- the existing single `plugin_ctrl` denial declaration for optional default
  plugins (`unmapped`, `biosdev`, `speaker`, `extfpuirq`, `parallel`,
  `serial`).

It must not introduce a project-owned device selector, runtime object
construction, GUI replacement, network/USB/sound/storage enablement, BOP/DOS
branch, or adapter C++ object crossing.

## Remaining build-graph gate

The historical VS project files do not reveal the final ordered static-library
arguments for `bochs.exe`. A first MSVC container observation must therefore
derive a fresh, source-to-object/link manifest from the original project
metadata and retained object evidence before invoking `link.exe`. It may not
infer a link list from solution order or silently add every project output.
