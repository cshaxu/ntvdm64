# M0 T310 S34 P1 — MVDM support-library closure

## Selected original packages

The selected Win32/x86 CCPU40 runtime support libraries are the exact original
`sources` selections, not a project-authored common-support replacement:

- `mvdm-support/suballoc/suballoc.c` is the sole `suballoc.lib` runtime body.
  `tsa.c` is the original `UMTEST` program and remains outside the product
  archive.
- `mvdm-support/oemuni/file.c` and `mvdm-support/oemuni/process.c` are the
  two original `oemuni.lib` runtime bodies. `toemuni.c` is the original
  `UMTEST` program and remains outside the product archive.

The generated S34 graph compiles these exact roots respectively into
`original-mvdm-support-suballoc.lib` and
`original-mvdm-support-oemuni.lib`. Both archives are whole-archive inputs of
the selected CCPU40 forced-link audit. No S34 wrapper, overlay, warning
suppression, Bochs route, MONITOR route, or replacement allocator/OEM service
was introduced.

## Original caller contracts and disposition

- `suballoc.h` declares `SAInitialize`, `SAQueryFree`, `SAAllocate`,
  `SAFree`, and `SAReallocate`. The selected original XMS bodies call these
  routines through `xms.c` and `xmsblock.c`; selected DPMI32 bodies call the
  same routines through `dpmimemr.c`. The allocator keeps its original
  `ULONG` address arithmetic and caller-supplied commit, decommit and move
  callbacks. Those callbacks are the already named XMS/DPMI machine-memory
  boundaries (S15/S37); S34 neither turns a guest address into a host pointer
  nor creates a second mapping manager.
- `oemuni.h` declares the original OEM file, path, directory, environment and
  process helpers. The reached selected callers include DEM's `CreateFileOem`
  file paths and SoftPC `nt_pif.c:GetPIFConfigFiles` through
  `ExpandEnvironmentStringsOem`. `file.c` and `process.c` retain the original
  OEM-to-Unicode and public Win32 call ordering. Their RTL/Win32 declarations
  are supplied by the already selected same-shaped Win32 adapter; no separate
  OEM facade remains.

The retained `inc` headers are declaration carriers for later owner packages
(XMS, DPMI32, Redirector and WOW). They add no independent S34 executable
provider. Their already registered mirror divergences are outside the selected
x86 CCPU40 source bodies and are not reopened here.

## Formal evidence and transfer

`build/M0-T310/S34/formal-support-x86-r1` completed all **381/381** selected
Win32/x86 CCPU40 Ninja edges. It archived the exact three runtime source
bodies above. Its forced-link audit reports no unresolved `SA*`,
`CreateFileOem`, `ExpandEnvironmentStringsOem`, or other support-library
provider edge. The remaining forced-link symbols belong to LIM, DEM monitor
forms, debugger, DPMI, host section/memory, or later host-console owners;
none selects new S34 work.

S34 therefore closes the source-library contract. Real XMS/DPMI allocation
and OEM file behavior are mandatory consumers' integrated-machine evidence at
their named owner packages and at S49; this source packet does not substitute
a new fixture for that later behavior.
