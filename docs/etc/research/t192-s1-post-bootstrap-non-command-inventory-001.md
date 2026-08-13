# T192 S1 post-bootstrap non-COMMAND BOP inventory 001

Date: 2026-08-12  
Packet: M0 T192 S1  
Disposition: reached-family source map complete; no source/build/run change.

The T190 trace reaches DEM selector `50` services
`00/02/0D/0F/11/12/1B/21/32/36/3B/3C/42/46`.  Original `MS_bop_0` reads the
fourth byte and calls `DemDispatch`; `dossvc.h` and `demdisp.c` give the
service names and original table behavior.

| Reached services | Original group | Current whole disposition |
| --- | --- | --- |
| `00`, `02`, `12`, `42` | file seek/close/open/FASTREAD | finite read-only boot-namespace/session provider; the historical FASTREAD table slot is `demNotYetImplemented`, so success is not fabricated outside the explicitly source-shaped finite provider path |
| `0D`, `0F`, `21`, `46` | boot drive, drive list, IOCTL, DPB list | contained CLI drive projection and source-derived metadata; no host filesystem enumeration or raw device access |
| `11`, `36`, `3C` | load DOS, entry DOS app, PDB terminate | startup/session lifecycle composition; lifecycle-bound and not a generic process launcher |
| `1B` | DTA location | bounded DTA registration/complete path |
| `32`, `3B` | hard-error info, debug test | source-backed result/registration semantics without NT UI/debugger activation |

The original DEM table also explicitly marks `42` FASTREAD as
`demNotYetImplemented`; this remains a failure/disposition fact, not authority
to add a general file provider.  All reached DEM services already pass the
common catalogue/ingress/DEM-plane gate and have an owner.  T192 therefore
rejects a new DEM implementation slice.

Other reached selectors (`12/15/17/5E/5F/66/C8`) have no catalogue service
byte and are only SoftPC-BIOS provenance. `06` is the exception/machine
composition path, and `FE` is controlled stop. None belongs to the adapter
host-service plane. The only next coherent question is whether any reached
machine selector has a separately admitted original handler island and Bochs
mechanics closure.
