# M0 T225 — Guest memory ownership and lifecycle map 001

| Lifecycle phase | Original owner | Address rule | Mutation authority |
| --- | --- | --- | --- |
| NTIO bootstrap | NTIO/BIOS guest | initial resident NTIO interval | NTIO/BIOS guest only |
| `50:11` | DEM source-derived narrow seam | copied NTDOS bytes to initial `EDI << 4` | one checked mechanical copy only |
| sysinit temporary image | NTDOS `sysinit1.asm` | temporary `T`; DOSCODE `T-0x225` | original NTDOS relocation |
| DosInit | NTDOS `msinit.asm` | DOSDATA copied/migrated to low memory | original NTDOS |
| charinit/device chain | NTDOS + NTIO/BIOS guest driver | DS:SI header and ES:BX request packet | original guest device owner |
| COMMAND/PSP/arena | COMMAND + NTDOS `$Exec` | allocation-selected segments, not fixed profile addresses | original guest process/arena owner |
| host BOP result | bx-vdm/provider | only copied fixed-width result and checked RAM action | declared provider only |

The initial `50:11` EDI address is not a permanent code-location authority.
A later CS:IP may be relocated NTDOS, DOSDATA-adjacent guest code, a device
image, COMMAND, PSP/arena content, or a corrupted transfer. Attribution needs
the phase's original relocation/allocator facts; a static image subtraction is
invalid. This map rejects adapter-owned layout synthesis and selector-driven
Bochs behavior.
