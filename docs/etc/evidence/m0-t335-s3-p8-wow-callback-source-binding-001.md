# M0 T335 S3 P8 — original WOW callback source binding

## Scope

This packet binds only the fixed-frame `CallBack16` cohort in the original
`mvdm-host/wow32/wcall16.c` source body. It retains the original task lookup,
caller-frame read, callback-frame preparation, `SETVDMSTACK`, recursive
`host_simulate`, return-frame read and AX:DX extraction order.

`MVDM-HOST-DIV-178` replaces just the three fixed raw frame-alias intervals
with `wow_callback_frame_lease`. The lease resolves the original packed VP
through the CCPU40 descriptor path, reaches the existing adapter-softpc
bounded guest-memory seam, commits the writable callback frame before the
original recursive call, and leaves no host alias live across it.

## Declaration closure

The reached original include closure requires the byte-identical declaration
carriers `wowuserp.h` and `winuserp.h` in `opennt-abi`. The modern SDK claims
the historic `_WINUSERP_` guard before the original carrier can expose
`PFNW32ET`; `ADAPTER-WIN32-042` therefore provides only that same-shaped
typedef and the public `WCB16_CDECL` flag. It imports no USER/GDI provider.

`MVDM-HOST-DIV-179` redirects two original private include-root spellings to
the corresponding byte-identical ABI carriers. `MVDM-HOST-DIV-180` keeps the
modern SDK's incompatible later `wownt32.h` export declarations out of this
original-provider translation unit. `MVDM-HOST-DIV-181` selects the original
CCPU simulator stack form rather than the SDK host-x86 native-VDM register
alias.

## Verification

The selected original `wcall16.c` translation unit compiles on both profiles
with the same CPU40 source selection:

- x86: `build/M0-T335/S3/P8/wcall16-x86.obj`;
- x64: `build/M0-T335/S3/P8/wcall16-x64.obj`.

The x86 compile contains only retained original non-width warnings. The x64
compile additionally reports original pointer-width warnings in unrelated
public WOW global-memory helper exports earlier in the same translation unit;
they do not occur in the selected fixed-frame `CallBack16` body and remain
explicitly outside this non-GUI cohort. They are not suppressed or treated as
runtime closure.

The existing P6 CCPU40 fixture proves the original recursive CPU return path;
P7 proves the exact two-frame acquire/commit/release shape on both profiles.
This P8 source compile proves the original `CallBack16` body now selects those
same bounded operations. A direct linked `CallBack16 -> host_simulate ->
CallBack16` fixture remains S3 work; this packet does not claim it has run.

## Boundary

No generic WOW pointer binding, GUI/USER/GDI, WOWEXEC, CSRSS/BaseSrv, kernel
VDM, fast WOW, guest-media execution, raw pointer identity or new mapping
manager is enabled.
