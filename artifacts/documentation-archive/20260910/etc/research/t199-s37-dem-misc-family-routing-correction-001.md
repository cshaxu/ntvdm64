# T199 S37 DEM MISC Family Routing Correction 001

## Finding

The initial MISC-family regression exposed a classification error, not a
missing V86 implementation.  `50:1D` is `demSetV86KernelAddr` in OpenNT
`src/opennt/base/mvdm/dos/dem/demdisp.c`.  Its entire historical body is a
comment for a future debugger callout followed by an ordinary return.  It
does not manipulate a V86 kernel, memory, interrupt state, or a device.

The adapter had classified decimal service 29 (`50:1D`) as
`BX_NTVDM_DEM_COMPONENT_V86_BRIDGE`.  Since no such capability is admitted
to the CLI profile, it fell through to the generic AX=5/CF unavailable
result.  That was contrary to the original provider's visible contract.

## Correction and boundary

`src/bx-vdm/bx_ntvdm_dem_plane_v1.c` now assigns service 29 to the DEM MISC
component.  `bx_ntvdm_dem_misc_plane_v1` already supplies its source-derived
ordinary return: resume after the four-byte BOP with no register or flags
delta.  No V86 implementation, Bochs modification, DOS semantic, or host
debugger capability was added.

The same no-debug/no-VDD/no-WOW/no-device profile family is regressed for
`50:1D`, `23`, `34-3A`, `3C`, `3E`, and `45`.  `50:3B demIsDebug` is checked
separately because OpenNT calls `setAL(0)` on the no-debug branch, preserving
AH.  `50:3D demExitVDM` remains separately tested as the typed controlled
stop; it is not a no-op.

## Evidence

`artifacts/build/t199-s37-dem-package-r71-misc-routing/` source-builds,
links, and runs the complete DEM `50:00..48` x64 `/MT` fixture successfully.
The fixture contains no bx-core, bx-mantle, Bochs, native guest trace, or
ambient host debugger/device input.  This closes the recorded MISC routing
error and its contained family regression only; it does not close DEM S37.
