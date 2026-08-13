# M0 T194 S32: BOP-Register Derivative Build

Packet: M0 T194 S32  
Disposition: source-built diagnostic derivative complete; engine not started.

## Build

The S31 generator created
`artifacts/build/current/t194-s32-bop-register-observation-v6-r1`; the
documented MSVC x86 `nmake /f ntdos64-t98-current-adapter.mak
ntdos64-t98-current-adapter.exe` then completed successfully from that root.
It compiled the declared `cpu/exception.o`, 68 adapter objects and five CLI
objects before the existing link vector. The only compiler warning is existing
source conversion warning C4244 at `exception.cc:1562`.

## Identity And Boundary

| Item | SHA-256 |
| --- | --- |
| Engine | `49D950AE537B0F9343F2B07E0A2B92DD6B18EB612D572A459559EC0FB3B2AB5D` |
| Rebuilt `cpu\\exception.o` | `AE901987786C9C8743170F4319943AE0A97C4B8EDAC94C98BB9E64CB4FF4786D` |

The manifest records one Bochs replacement, only `cpu\\exception.o`. The map
resolves `bx_ntvdm_bop_catalog_listener` from that object. `main.o` and the
CPU/iodev/memory/gui archives retain the S26 hashes; no archive, device,
configuration, firmware, Bochs source or adapter ABI changed. The generated
compile line contains only the four S31 listener/bridge macros.

## Limitation

This establishes the diagnostic binary only. It does not execute it or prove
the reached IOCTL AL. A later observation must first resolve the existing
controller process-tree termination permission issue and must use one fresh,
no-retry packet.
