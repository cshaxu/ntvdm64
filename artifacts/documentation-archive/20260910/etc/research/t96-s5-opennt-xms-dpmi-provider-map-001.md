# M0 T96 S5 OpenNT XMS/DPMI Provider Map 001

## Boundary decision

`xmsdisp.c` and `dpmi32.c` are source evidence, not adapter link targets.
They both directly use the historical `softpc.h` register and address model;
the DPMI dispatcher additionally reads the service byte at CS:IP and advances
IP itself.  Calling either after the common four-byte copied ingress would
duplicate the dispatch/continuation contract and reintroduce SAS/CCPU/NT-host
dependencies.  The adapter therefore records their table structure only.

Pinned inputs (SHA-256): `xmsdisp.c`
`24B0FA2FE32BD7A94606D424A10B0961A9911190CB2684456AF04034E04BFF01`;
`xmsa20.c` `7261B7064A089B655B8D41D3E4EF6887BE869AF2B7B62970389AEEE9A8ACC94`;
`xmsblock.c` `BAD1DE87B499D3052E757D858B39437232F5653610DC5560C06AFCC5E05FD391`;
`xmsmisc.c` `10AD205D9DEDE714749A9746DD99B3389546B8AAF3A63DD0236386A9D4CE5245`;
`xmsumb.c` `1608B70F239570F2CB5BEEAEEACA63065CC9550BD6BAD15B47F4160307B33DDC`;
`xms.c` `592F4A063AD84C21890CDC15E56D4900E6669224C7C34C829C09D25DFF9BED75`;
`dpmi32.c` `C271DAF86BDFA846C9602F74BD2A4059B98C2AD1F12676E966BCFD427BA93DCA`;
`dpmiselr.c` `9BDD58D63029DE9438633A62934AF2B2B868206CB7F06CC868F537E030FBD234`;
`dpmiint.c` `D0607F266B8C6EE0556AF13FF9260C7C07DF8C0DC05AB25B37567430FB28A4AA`;
`xmem.c` `50BABB31BCD739318E189E88567ED27D6C89AF908B8B590BBEB3E8F1377D13D5`;
`dpmimemr.c` `A21E43830622D5D04BFCD018C6751CDC7A3DE54C166621731494363C8D5A60AE`;
`dpmimscr.c` `41494B9F44E53863D31527CA082DFC286F34E2B4CACE556BAF501274D3EBA061`;
`int21map.c` `B0A8632DDB1BB0ED609115BCB113C7A43A0B042930CD281D333540089589C7E1`;
`i386/dpmi386.c` `E47BD6F7A1F48BA94022C9D5ED78714C269F751737F8F69711FC44C06523F8F0`;
and `vxd.c` `C42DB46A3E7F985B83988668778613BA497B0A940F2AEF4E0A2E082E54D8DD3A`.

## Complete XMS dispatcher disposition

| Svc | Original handler/module | Native owner and blocker | Current disposition |
| ---: | --- | --- | --- |
| 00 | `xmsA20`, `xmsa20.c` | Bochs memory A20/wrap state; original calls SAS wrapping functions. | Deferred. |
| 01--05, 10--11 | `xmsMoveBlock`/block allocator/query handlers, `xmsblock.c`, `xmsmisc.c` | Bochs RAM plus an XMS allocator; original uses `GetVDMAddr`, `SA*`, and NT page size. | Deferred as one extended-memory component. |
| 06--08 | UMB handlers, `xmsumb.c` | Bochs memory map plus guest/UMB allocation policy; original uses `ReserveUMB` and raw guest pointer. | Deferred as one UMB component. |
| 09 | `xmsNotifyHookI15`, `xmsmisc.c` | Bochs interrupt/firmware path; original invokes `UpdateKbdInt15`. | Deferred. |

The table covers all twelve XMS values.  It does not authorize an adapter A20
toggle, a host allocation, or a fabricated register result.

## Complete DPMI dispatcher disposition

| Svc | Original handler group | Native owner and blocker | Current disposition |
| ---: | --- | --- | --- |
| 00,01,03--05,0F,11 | descriptor/mode/init table | Bochs CPU mode, LDT and protected-mode stacks; original uses selector tables and raw VDM mappings. | Deferred. |
| 02,0A,14--18 | PM interrupt/fault/IRET table | Bochs CPU exception, interrupt and stack state. | Deferred. |
| 06 | `DpmiXlatInt21Call`, `int21map.c` | DPMI-to-DOS bridge and protected/real-mode shared buffers. | Deferred. |
| 07--09,0B,10,13 | extended-memory table | Bochs RAM/allocator plus DPMI allocation and application cleanup state. | Deferred. |
| 0C--0D | DPMI in-use state | Composition/session state, including historical VDM state. | Deferred. |
| 0E | debug-register table | Bochs CPU debug-register state. | Deferred. |
| 12 | `DpmiVcdPmSvcCall32`, `vxd.c` | Historical VCD queries the Windows registry for serial ports. | Explicitly unavailable in the first CLI profile. |

This covers all 25 DPMI values.  `bx_ntvdm_xms_dpmi_plane_v1` is consequently
a fixed-width classification record, not a provider callback.  It accepts
only common-ingress/registry-selected XMS or DPMI records, rejects selectors'
sentinels, and exposes no raw guest pointer, Bochs API, SAS/CCPU register
access, or success transition.

## Consequence for future implementation

Any future XMS or DPMI operation must first demonstrate the corresponding
Bochs-native mechanism and a copied composition ABI.  A source-derived
provider may then preserve the original handler's register/error contract;
until then every deferred record stops before execution.  The VCD item must
remain unavailable unless a separately approved non-intrusive CLI serial
capability is designed and tested.
