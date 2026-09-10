# M0 T199 S8 XMS Source, ABI, and Failure Matrix

## Source boundary

`src/opennt/base/mvdm/inc/xmssvc.h` defines twelve services `00..0B`; `xmsdisp.c` is the corresponding ordered table. The original host BOP (`nt_bop.c:281`) reads the service from historical CS:IP, calls `XMSDispatch`, then advances IP. Modern common ingress already owns that copied instruction and continuation contract, so neither original dispatch object may be linked.

Pinned source hashes: `xmsdisp.c` `24B0FA2FE32BD7A94606D424A10B0961A9911190CB2684456AF04034E04BFF01`; `xmsa20.c` `7261B7064A089B655B8D41D3E4EF6887BE869AF2B7B62970389AEEE9A8ACC94`; `xmsblock.c` `BAD1DE87B499D3052E757D858B39437232F5653610DC5560C06AFCC5E05FD391`; `xmsmisc.c` `10AD205D9DEDE714749A9746DD99B3389546B8AAF3A63DD0236386A9D4CE5245`; `xmsumb.c` `1608B70F239570F2CB5BEEAEEACA63065CC9550BD6BAD15B47F4160307B33DDC`; and `xms.c` `592F4A063AD84C21890CDC15E56D4900E6669224C7C34C829C09D25DFF9BED75`.

## Complete table

| Svc | Handler/source | Required ABI/owner | Current disposition |
| ---: | --- | --- | --- |
| `00` | `xmsA20`, `xmsa20.c` | A20 wrapping state; native machine memory owner. | Deferred. |
| `01` | `xmsMoveBlock`, `xmsblock.c` | `EXTMEMMOVE` guest structure; XMS handles plus checked conventional/extended copies. | Deferred. |
| `02` | `xmsAllocBlock`, `xmsblock.c` | XMS allocator and handle table over machine-owned extended RAM. | Deferred. |
| `03` | `xmsFreeBlock`, `xmsblock.c` | Same allocator/handle lifetime. | Deferred. |
| `04` | `xmsSysPageSize`, `xmsmisc.c` | Native page-size result. | Deferred; no profile memory contract. |
| `05` | `xmsQueryExtMem`, `xmsmisc.c` | Native extended-memory size. | Deferred. |
| `06` | `xmsInitUMB`, `xmsumb.c` | UMB reservation/ownership and DOS memory map. | Deferred. |
| `07` | `xmsRequestUMB`, `xmsumb.c` | UMB allocation, guest segment/error result. | Deferred. |
| `08` | `xmsReleaseUMB`, `xmsumb.c` | UMB release/owner state. | Deferred. |
| `09` | `xmsNotifyHookI15`, `xmsmisc.c` | INT 15 hook and keyboard/firmware notification. | Deferred to native machine/firmware lifecycle. |
| `0A` | `xmsQueryFreeExtMem`, `xmsmisc.c` | XMS allocator free-space state. | Deferred. |
| `0B` | `xmsReallocBlock`, `xmsblock.c` | Allocator resize plus handle/lifetime guarantees. | Deferred. |

`EXTMEMMOVE` is a copied guest layout only; its historical `GetVDMAddr`, SAS and NT virtual-memory paths cannot enter the modern adapter. The existing combined XMS/DPMI classifier correctly groups `00` as A20, `01..05,0A,0B` as extended memory, `06..08` as UMB, and `09` as firmware interrupt. Its current “Bochs memory” owner is a design direction, not evidence that a typed mantle capability exists.

## Failure boundary

Historical `XMSDispatch` checks `iSvc >= XMS_LASTSVC`, sets CF and returns false only under `DBG`; release indexes the table unchecked. Thus selector `52:0C` is non-callable and modern ingress must reject it rather than reproduce an out-of-bounds call.

No XMS service is presently implementable by bx-vdm without a preexisting native-machine request/response ABI. S9 must choose one whole-package disposition and explicitly keep all services deferred until bx-core/bx-mantle prove the required memory/firmware mechanisms.
