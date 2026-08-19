# M0 T230 S1 — BOP 50:02 OpenNT-shaped v2 Provider

Original owner: `demClose` in `src/opennt/base/mvdm/dos/dem/demhndl.c:28-90`, dispatched by `demdisp.c` as `50:02`.

`src/bx-vdm/bop/demhndl.c` now mirrors the original null-handle success, `CX:DX = FFFFFFFF` no-seek sentinel, signed 32-bit seek, `CloseHandle` success/failure ordering, and AX/CF-only failure ABI. Direct uses the actual session-owned host handle. Readonly continues through its existing readonly-file provider and closes its token without a host namespace mutation. Overlay remains an ABI seam and is not enabled.

The original translation unit cannot be independently composed because it uses CCPU register macros, a guest-register host HANDLE, historical `demClientError` INT24 composition, and optional Redirector named-pipe bookkeeping. The documented divergences are: bounded opaque AX:BP tokens, typed copied CPU delta, session-owned close plus token retirement, current typed error seam, and no fabricated Redirector provider. The direct route now claims stale tokens so `demClose` itself returns its OpenNT-shaped invalid-handle failure rather than a generic package terminal.

Formal Ninja graph `build/M0-T230-S1/50-01-demfile-v2-r1` rebuilt and linked the DEM family fixture. `direct`, `readonly`, and focused `direct pdb-lifecycle` exited zero. The regression covers the no-seek sentinel, CF-only success result, opaque-token retirement, and a stale token's AX=ERROR_INVALID_HANDLE/CF failure. This is code completion/local profile regression; native guest observation remains unverified.
