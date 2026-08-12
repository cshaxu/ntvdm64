# M0 T96 S6 OpenNT System Provider Map 001

## Scope and boundary

This is the complete remaining host-provider disposition after DEM, COMMAND,
XMS and DPMI. It covers every Redirector service, every debugger service, and
the non-family `nt_bop.c` selectors that are host composition rather than
Bochs-native BIOS/device mechanics. It is not an authorization to link
`nt_bop.c`, `vrdisp.c`, or `dbg.c`.

Pinned SHA-256 inputs: `vdmredir/vrdisp.c`
`030A3A2F2BAAEB36BB58EB459CC91A0F6BF15BFDEC777FC2998E3C3D69B2A389`;
`dbg/dbg.c` `5A3A207EFF1570908E270C737D12FB40FE66097895063136D17FE6B971494B90`;
`softpc.new/host/src/nt_bop.c`
`378A34807645277090BF4B38556EA210502F79A898855B649B39D6C4CCC47F66`;
`softpc.new/host/src/nt_mouse.c`
`A73C0572FA1E44326D26B2F4D47C15BFF768B031FCA16DCFD789A35E65E0E48B`;
and `softpc.new/base/bios/bios.c`
`A8345E88DB25332BDF008BDC730FFA15BDC6110ECF1A6C54ED7E59C95CA7085B`.

## Redirector: complete service disposition

`VrDispatchTable` is the authoritative contiguous 00--30 table. `MS_bop_7`
first attempts to load `VDMREDIR`; failure sets CF and `AX=ERROR_INVALID_FUNCTION`
before advancing IP past the service byte. Its source pulls in NT/Win32,
network, named-pipe, mailslot, NetAPI, DLC and raw VDM-pointer bindings.
The first CLI profile deliberately supplies no VDMREDIR capability: all 49
services are therefore **explicitly unavailable**, not individually rehosted.

| Services | Original handler family | System binding |
| --- | --- | --- |
| 00,01,0F | initialize/uninitialize/DOS-process termination | VDMREDIR DLL and session lifecycle |
| 02--08,23,24 | named-pipe information, state, synchronous and async I/O | Win32 named pipes, handles and async completion |
| 09--0E | mailslot operations | Win32 mailslots and host buffers |
| 10--1A,22,2E | NetAPI/server/workstation/message/service calls | host network/domain/service APIs |
| 1B--21 | identity, assignment and handle information | host identity, mapped drives and handles |
| 25--28,2F,30 | NetBIOS, DLC, window/interrupt completion | network device/DLC and VDM interrupt state |
| 29--2D | assignment modes and macro compatibility | redirector session state |

The machine record classifies every numeric value 00--30. The grouped table
is only a readable index; `vrdisp.c` table order plus the immutable provider
map preserve the individual original handler name for each value.

## Debugger: complete service disposition

`MS_bop_6` invokes `DBGDispatch`, whose 00--0F switch reads structured data
from the guest stack with `Sim32GetVDMPointer`, alters register state for some
events, and calls historical debugger/module facilities. No debugger backend
is admitted in the first CLI profile. All 16 values remain **deferred**:

| Services | Original event group | Blocker |
| --- | --- | --- |
| 00--04 | segment/module load, move and free | raw guest stack/pointers and debug-module state |
| 05--09 | single-step, break, GP/divide/instruction faults | CPU exception/register/debugger integration |
| 0A--0D | task and DLL lifecycle | WOW task/module lifecycle |
| 0E--0F | attach and ToolHelp | historical debugger attach/tooling APIs |

## Selector-level disposition

| Selector | Historical entry | Current owner/disposition |
| ---: | --- | --- |
| 51 | WOW32 lazy loader/dispatcher | Separate WOW composition, deferred; not an adapter DLL loader. |
| 55 | `ms_bop()` SoftPC entry (not named in `bop.h`) | SoftPC/Bochs-native compatibility investigation; not an adapter provider. |
| 58 | ISV register/deregister/dispatch | Dynamic arbitrary DLL composition; explicitly unavailable. |
| 59 | direct-access error | Historical host warning/UI; explicitly unavailable. |
| 5A | VDD idle control | CLI scheduler policy is not a VDD; deferred without a result transition. |
| 5B | host debugger breakpoint | Explicitly unavailable. |
| 5C,5D | temporary keyboard/video | Bochs device ownership; deferred. |
| 5E | CONFIG completion notification | DEM/UMB/DASD composition event; deferred pending its native prerequisites. |
| 5F | unimplemented interrupt | Historical host diagnostic; explicitly unavailable. |
| FD,FE | switch-to-real-mode / unsimulate | CPU execution lifecycle; Bochs owner, not adapter. |
| BIOS mouse rows | mouse install, INT 0A/33 and callbacks | Bochs input/video/interrupt device owner; no adapter mouse semantics. |

`bx_ntvdm_system_plane_v1` accepts only registry-selected 56:00--0F or
57:00--30 records. It has no callback, raw pointer, host handle, CPU result,
or top-level selector execution. Redirector is classified unavailable because
the original entry has a directly observed failure contract; debugger is
deferred because its correct failure/continuation contract requires a future
complete debugger component.
