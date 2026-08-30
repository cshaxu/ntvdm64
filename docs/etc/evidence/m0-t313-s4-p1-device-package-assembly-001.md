# M0 T313 S4 P1 — Original Device Package Assembly

## Original package selection

The selected x86 CCPU40 formal graph reads the original `sources` manifests
and archives these complete packages directly:

- `softpc.new/base/system`: PIT/PIC/CMOS/ROM and controller source;
- `softpc.new/base/video`: EGA/VGA/CGA state and port/memory handling;
- `softpc.new/base/cvidc`: CCPU C-video memory access backend;
- `softpc.new/base/disks`: floppy/disk BIOS and FLA/GFI forms;
- `softpc.new/base/comms`: serial, printer and screen-print forms.

No S4 device algorithm is implemented in an adapter.  The only selected
overlay relevant to this machine set is the existing CCPU GDP mechanical
state; it is not a video, disk or communications provider.

## Forced-link audit

The current x86 forced-link audit was run against the complete original
archives.  It reports no unresolved symbol owned by the four S4 device
libraries.  Its unresolved symbols are pre-existing cross-package boundaries:

- CCPU EMS writes: `LIM_b_write`, `LIM_w_write`;
- DEM module/debug forms: `ModuleLoad`, `ModuleFree`, `ModuleSegmentMove`,
  `DbgPrompt`;
- host UI/debug/private NT forms: `ShowStartGlass`, `DBGDispatch`, `DBGInit`,
  `NtRaiseHardError`, `NtAllocateVirtualMemory`, `NtFreeVirtualMemory`.

The audit also exposes duplicate exports between original CCPU and `cvidc`
archive members.  It uses `/FORCE` solely to expose boundary evidence; the
resulting DLL is explicitly non-runnable and is not a product link method.

## S4 implication

S4 starts from complete original device packages, not a list of artificial
missing symbols.  The next work is source-first selection of each original
device startup path and its bounded endpoint.  It must retain the original
CCPU/C-video division and cannot cure archive collisions by choosing a second
executor or an adapter video backend.
