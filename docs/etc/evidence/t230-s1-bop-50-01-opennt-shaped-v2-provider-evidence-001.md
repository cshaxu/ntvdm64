# M0 T230 S1 — BOP 50:01 OpenNT-shaped v2 Provider

Original owner: `demChMod` in `src/opennt/base/mvdm/dos/dem/demfile.c:437-506`, dispatched by `demdisp.c` as `50:01`.

`src/bx-vdm/bop/demfile.c` mirrors the original function's get/set split, `AL`/`CL` ABI, `FILE_ATTRIBUTE_NORMAL` conversion, DOS attribute mask, and register preservation: get writes only `CX`; set writes no GPR; errors write `AX` and CF.

The original translation unit is not independently composable because `GetVDMAddr`, CCPU register macros and historical INT24 composition bind it to the retired host shell. The required divergences are limited to checked gathered pathname input, host-namespace APIs, typed CPU results, and the current typed error seam. Direct performs actual host metadata calls; Readonly refuses metadata mutation before the host call; Overlay remains behind the existing profile seam.

Fresh formal Ninja graph `build/M0-T230-S1/50-01-demfile-v2-r1` linked the 160-edge DEM family fixture. Both `direct` and `readonly` exited zero. This establishes code completion/local profile regression only; native guest observation remains unverified.