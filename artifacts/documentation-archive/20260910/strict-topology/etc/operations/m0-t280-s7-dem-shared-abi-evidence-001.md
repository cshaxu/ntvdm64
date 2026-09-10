# M0 T280 S7 — DEM shared ABI recovery evidence

## Original build evidence

The exact original `base/mvdm/dos/dem/sources` selects all sixteen bodies and sets `C_DEFINES=-DWIN_32`.
Its `INCLUDES` reaches MVDM `inc`, the Windows tree, the MVDM root, and original SoftPC base/host declarations.
The debug macro is not invented: OpenNT-4.5 `nt/public/oak/bin/makefile.def` records `TARGET_DBG_DEFINES=-DDEVL=1`.
`dem.h` and `demexp.h` show that this condition exposes `demDebugBuffer`, `fShowSVCMsg`, and the DEM trace masks.

## Source-form probe

The x64 MSVC syntax probe was repeated with `/DWIN_32 /DDEVL=1`, the original component include roots and the current adapter-win32 facade.
Fifteen bodies then stopped at the same first error in the modern `ntddstor.h`: `DEVICE_TYPE` is unavailable to that modern header selection.
`demmsg.c` remains warning-only. This changes the interpretation of the S6 bare probe: the apparent per-body missing diagnostics were a missing historical build form, while the first real package prerequisite is the historical storage declaration pair selected by original `dem.h`.

The result does **not** authorize copying a modern SDK header into a mirror or rewriting DEM.
The exact source-rung candidate is the OpenNT-4.5 public SDK `ntdddisk.h` plus its original `ntddstor.h` include.
S7's ledger separately records the remaining `ntioapi.h`, name-collision, and SoftPC mapping families so that the next S can decide them as one source-shaped compatibility package.

## Mapping-manager review

The source call sweep finds `GetVDMAddr` or `Sim32*VDMPointer` in every DEM body except `demmsg.c`.
Those numeric segment:offset/linear values are guest identities, so future recovery uses only the existing session `guest_memory` mapping-manager instance through `adapter-softpc`.
`FILE_BOTH_DIR_INFORMATION`, `DISK_GEOMETRY`, `VDMQUERYDIRINFO`, Win32 handles and temporary C pointers are host-local data and are expressly excluded from all mapping managers.

## Dual-role review

The implementation review checked the five ledger rows against original file paths and consumers.
The independent review rejected both tempting shortcuts: adding a per-BOP `GetVDMAddr` adapter and using the current modern `ntdddisk.h` as a substitute for OpenNT's declaration carrier.
The minimal next unit must be a shared source-shaped declaration/binding package.
