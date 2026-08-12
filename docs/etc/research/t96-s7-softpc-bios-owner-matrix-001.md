# M0 T96 S7 SoftPC BIOS Owner Matrix 001

## Result

The immutable BOP inventory contains 41 named `BIOS_*` selector definitions
and 349 explicitly annotated alternative `BIOS[]` table rows. S7 assigns each
row one of three dispositions; none is an adapter implementation or a runtime
choice of a historical table branch.

| Disposition | Rows | Meaning |
| --- | ---: | --- |
| Historical unavailable | 227 | `illegal_bop` table form. No adapter replacement or success result. |
| Host-selector collision | 25 | Selector also has a named OpenNT host BOP identity. Alternative historical rows do not select an owner from three instruction bytes. |
| Bochs-native candidate | 97 | Historical non-`illegal_bop`, non-host-selector table forms. They are evidence for a Bochs machine domain, not proof that the current minimal Bochs profile enables the feature. |

The counts are mechanically verified by
`tests/bx-ntvdm-adapter/Test-SoftpcBiosOwnerMatrix.ps1` against the immutable
inventory; it rejects missing table forms and any unclassified row.

## Collision rule

The collision forms are selectors `50,51,52,53,54,56,57,59,5A,5B,5C,5D,5E,
5F,FD,FE`. They include 16 OpenNT top-level identifiers and the 25 annotated
SoftPC alternatives. For 50--5F the historical `NTVDM` table contains
`MS_bop_0` through `MS_bop_F`, while other compile-time branches contain
`illegal_bop`, `smeg_*`, `Cpu_hook_bop`, or `romMessageAddress`. FD/FE have
CPU lifecycle alternatives. Therefore a copied BOP selector cannot choose a
SoftPC `BIOS[]` owner; common ingress/host planes decide only their already
admitted OpenNT contracts, and all other forms remain non-executable evidence.

## Native-machine domains

The 97 native candidates are historical evidence for the following Bochs
domains: standard BIOS interrupt compatibility (23), bootstrap (3), floppy
(3), disk/mouse (9), host-mouse alternatives (2), extension/device facilities
(5), CPU lifecycle (6), and other machine/device compatibility (46). The
last category is intentionally not split into adapter pseudo-services.

Bochs remains owner of CPU, reset, memory, interrupt/PIC, timer, keyboard,
mouse, video, disk and device mechanics. The current minimal profile may
defer any domain not proven needed by the reachable guest path. An enabled
domain requires original Bochs-native evidence and, if repository code must
change, an entry in `adapter-external-intrusion-exceptions.md` first.

## Evidence

The inventory JSON SHA-256 is
`893EB8D98A476A4D8A7AE996635B2B2BD24706594DE3666C2E664A1FB97E5C28`.
The historical `bios.c` SHA-256 is
`A8345E88DB25332BDF008BDC730FFA15BDC6110ECF1A6C54ED7E59C95CA7085B`.
