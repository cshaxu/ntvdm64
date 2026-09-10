# M0 T280 S8 — exact DEM platform declaration carriers

S8 performs the first usable recovery rung selected by S7: import the three byte-identical OpenNT declaration carriers that original `dem.h` and its consumers require.
It is deliberately limited to source supply and include selection; it does not enable a DEM body or synthesize a modern provider.

## Selected exact carriers

- `public/ddk/inc/ntdddisk.h` and its original nested `ntddstor.h` provide the historical storage/geometry form selected by `dem.h`.
- `public/sdk/inc/ntioapi.h` provides original file/directory/object record declarations reached by `demsrch.c`, `demerror.c` and `demgset.c`.

All three exist in the pinned `O:\repos.external\opennt` source and are imported below `opennt-platform-abi/source/opennt/...` without modification.
The manifest records source path, hash, destination and consumer evidence.

## Boundary

The carrier import does not make old NT system calls available. A later same-shaped adapter-win32 binding decides each reached call.
The import does not give `adapter-win32` or a DEM body a guest pointer: all future `GetVDMAddr`/`Sim32*` operations remain in the one session `guest_memory` mapping-manager path behind `adapter-softpc`.

## Exit criteria

The three destination files are hash-identical to their exact source paths; the manifest and source policy record their provenance; all current declaration consumers have an explicit source root.
The packet stops before adding an adapter binding or compiling/linking a DEM provider.
