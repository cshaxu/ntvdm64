# M0 T199 S10 DPMI Source, ABI, and Failure Matrix

`dpmi32.c:44-69` defines the ordered 25-entry `DpmiDispatchTable`; `DpmiDispatch` reads the service byte at historical CS:IP, validates it, invokes the handler, and owns its historical continuation. It cannot be linked after modern copied ingress without duplicating ABI and IP behavior.

Pinned source hashes: `dpmi32.c` `C271DAF86BDFA846C9602F74BD2A4059B98C2AD1F12676E966BCFD427BA93DCA`; `dpmiselr.c` `9BDD58D63029DE9438633A62934AF2B2B868206CB7F06CC868F537E030FBD234`; `dpmiint.c` `D0607F266B8C6EE0556AF13FF9260C7C07DF8C0DC05AB25B37567430FB28A4AA`; `xmem.c` `50BABB31BCD739318E189E88567ED27D6C89AF908B8B590BBEB3E8F1377D13D5`; `dpmimemr.c` `A21E43830622D5D04BFCD018C6751CDC7A3DE54C166621731494363C8D5A60AE`; `dpmimscr.c` `41494B9F44E53863D31527CA082DFC286F34E2B4CACE556BAF501274D3EBA061`; `int21map.c` `B0A8632DDB1BB0ED609115BCB113C7A43A0B042930CD281D333540089589C7E1`; `i386/dpmi386.c` `E47BD6F7A1F48BA94022C9D5ED78714C269F751737F8F69711FC44C06523F8F0`; and `vxd.c` `C42DB46A3E7F985B83988668778613BA497B0A940F2AEF4E0A2E082E54D8DD3A`.

| Services | Original component | Native owner / blocker | Current disposition |
| --- | --- | --- | --- |
| `00,01,03,04,05,0F,11` | Initialization, selector and mode operations | CPU protected mode, LDT/GDT, protected stacks and application state. | Deferred. |
| `02,0A,14,15,16,17,18` | PM interrupt/fault/IRET operations in `dpmiint.c` | CPU exception frames, interrupt controller, IVT/IDT and stack delivery. | Deferred. |
| `06` | `DpmiXlatInt21Call`, `int21map.c` | Protected-to-real DOS bridge, shared buffers and DOS execution contract. | Deferred. |
| `07,08,09,0B,10,13` | Extended-memory operations | Machine memory plus DPMI allocation/application lifetime. | Deferred. |
| `0C,0D` | DPMI in-use/session records | Composition session and historical VDM state. | Deferred. |
| `0E` | Debug-register operations | CPU debug-register state and exception handling. | Deferred. |
| `12` | `DpmiVcdPmSvcCall32`, `vxd.c` | Historical serial/VCD registry and VDD integration. | Explicitly unavailable. |

The table covers all `53:00..18`. `bx_ntvdm_xms_dpmi_plane_v1` has the same grouping and correctly marks only `12` explicitly unavailable. It is an identity/owner record only; it must not be mistaken for a provider.

The DPMI handler ABI is inherently machine-facing: selector descriptors, exception return frames, protected/real stack switches, raw VDM mappings and allocator state. No DPMI service can be source-derived inside bx-vdm today. The only safe current behavior is package classification and no result. There is no sentinel service; `53:19` and above are unknown service and common ingress rejects them.

S11 must choose package disposition together with S9's four XMS machine boundaries. In particular it must not add a second memory allocator, second CPU mode manager, or adapter-owned interrupt delivery.
