# OpenNT NTVDM Product-Composition Triage

Status: G0.4 host-output ownership record, 2026-08-07.

## Imported Reference Inputs

The 75-row descriptor inventory exposed three separate historical declarations
named `TARGETNAME=ntvdm`. They were not part of the original focused CCPU/BOP/
DEM import, so the fixed OpenNT source was extended with exact upstream copies
while retaining their original paths:

```text
base/mvdm/softpc.new/obj.vdm/   9 files
base/mvdm/v86/scaffold/         21 files
base/mvdm/ieuvddex/             23 files
```

`src/opennt/PRODUCT-HOST-IMPORT-MANIFEST.json` records all 53 files, source
revision, length, and SHA-256. It is intentionally separate from the original
focused-input manifest rather than silently broadening its old claim. The
import does not add a CMake target, source glob, default build dependency, or
runtime dependency.

## The Three Declarations Are Not One Product

| Descriptor | Historical target shape | Current research classification | Reason |
| --- | --- | --- | --- |
| `softpc.new/obj.vdm/sources` | `TARGETTYPE=PROGLIB`; links SoftPC system, BIOS, video, DOS, comms, debug, disks, keymouse, monitor, DEM, command, DPMI, XMS, suballoc, OEM libraries, and legacy Win32 product libraries | VDM host/product composition, `blocked` | This is the broad historical user-mode product linker recipe. Its selected CPU path and old process/UI integration are not a standalone x64 research runner. |
| `v86/scaffold/sources` | `TARGETTYPE=PROGLIB`; links monitor/DEM/XMS/debug/command and optional network layers; source list begins with `vdm.rc` | V86 host/product composition, `sample/development-only` for the owned route | It is tied to the historical V86 monitor/scaffold model. The research engine is software-emulated and must not reintroduce V86 as its execution dependency. |
| `ieuvddex/sources` | `TARGETTYPE=DYNLINK`; source group includes process, protected-mode, system, trace, help, and VDM DLL resources | VDM support DLL, `blocked` | The shared target name conceals a dynamic-link component, not the product executable. Its output identity must be resolved through historical build rules before any use. |

## Consequence

The inventory now distinguishes three different historical product surfaces
instead of treating `TARGETNAME=ntvdm` as a single missing executable. The
first execution route remains the owned user-mode monitor. These imported
sources are evidence for host capability, lifecycle, and compatibility
requirements only; none authorizes a V86 path, a legacy product-shell shim, or
a claim that an original `ntvdm.exe` has been rebuilt.
