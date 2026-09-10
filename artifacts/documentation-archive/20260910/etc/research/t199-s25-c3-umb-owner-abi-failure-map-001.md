# M0 T199 S25 C3 UMB Owner, ABI, and Failure Map

## Question

Can the no-device minimum Bochs machine truthfully offer a C3 upper-memory
map/reserve/release capability for the complete XMS UMB group without
synthesizing a firmware or product-host layout?

## Inputs

| Input | SHA-256 | Relevant fact |
| --- | --- | --- |
| `src/opennt/base/mvdm/xms.486/xmsumb.c` | `1608B70F239570F2CB5BEEAEEACA63065CC9550BD6BAD15B47F4160307B33DDC` | XMS `06..08` delegates UMB discovery, allocation and release to `ReserveUMB`/`ReleaseUMB`. |
| `src/opennt/base/mvdm/softpc.new/base/inc/umb.h` | `2461D7F74C3317A98D1456C238AD2F14BCDB2CBCD189D39836F24A353893E4B3` | Historical UMB is the `C0000..FFFFF` address space with owner and host-page policy. |
| `src/opennt/base/mvdm/softpc.new/host/src/nt_umb.c` | `2A109F11985507463797E60B8B19FC12441BD18278B1444DDE581CC351DE72ED` | The owner map is a historical NT/SoftPC composition, not a generic allocator. |
| `src/bx-core/memory/misc_mem.cc` | `B0C3F295B3E4DFA4B283420A325F55A457E5B7AEC13742C8DF5788BB68E64531` | The finite machine initializes RAM, blank ROM storage and no handlers, but no queried UMB layout. |
| `src/bx-core/memory/memory.cc` | `A80ECA25F97F81C9354FCCB356DD46A83C84D51097BB6A456C8B507F5814AA72` | Native accesses treat the low-ROM region as special; no-device initialization does not make it writable ordinary RAM. |

## Historical Dependency Map

| Historical operation | Actual dependency | Current owner/disposition |
| --- | --- | --- |
| `xmsInitUMB` (`52:06`) | Repeated `ReserveUMB(UMB_OWNER_XMS)`, a heap-linked segment list, `GetVDMAddr(AX:BX)`, and A20 wrapper state | Original XMS/SoftPC composition; not independently composable. |
| `xmsRequestUMB` (`52:07`) | First-fit, paragraph splitting/coalescing, segment/error register writes | Original XMS provider after a real map exists; deferred. |
| `xmsReleaseUMB` (`52:08`) | Exact segment owner lookup and coalescing | Original XMS provider after a real map exists; deferred. |
| `ReserveUMB` | Historical owner transitions for RAM/XMS/EMM/VDD/ROM, SAS mapping or NT section mapping | Historical product host; rejected as a mantle dependency. |
| `InitUMBList` | On x86, reads NT Hardware Description registry ROM blocks and maps a section; alternate build uses a fixed EGA/BIOS product list | Historical host/product configuration; unavailable to the contained CLI. |
| Minimum Bochs state | `init_memory_without_sim` leaves device handlers empty and ROM backing filled `FF`; it does not load firmware, initialize VGA/option ROMs, or publish a ROM-hole ownership map | Native core/mantle, but insufficient to identify safe allocatable UMB ranges. |

## Failure And Admission Decision

The historical error distinction is meaningful only after a real map: request
returns `BL=B0` with largest free paragraphs, `BL=B1` with no UMB, and release
returns `BL=B2` for an unknown segment.  Inventing a C3 table now would invent
the prerequisite ownership map and therefore also invent those error results.

**C3 is deferred, not implemented.**  The no-device machine deliberately has
no VGA/option-ROM/BIOS or historical NT registry composition.  Its ROM-special
address handling is not an allocation map and cannot be converted into one by
assuming that blank bytes are free.  Enabling a product device or firmware just
to manufacture UMBs would violate the active packet's stop conditions.

The only future admission path is a first-profile source-proven native machine
layout whose regions, reservation owner and reset lifecycle are independently
observable.  Such work must be a new packet and cannot reuse the historical
NT host's section/SAS/VDD allocator.

## Follow-up

Preserve `52:06..08` as a complete deferred XMS subgroup.  Continue with C5's
source/owner mapping; do not use trace to select an individual UMB service.
