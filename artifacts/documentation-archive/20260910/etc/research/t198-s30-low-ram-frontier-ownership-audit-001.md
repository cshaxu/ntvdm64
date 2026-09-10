# T198 S30 Low-RAM Frontier Ownership Audit 001

## Question

Who owns the first-generic-fault range `0000:0A84..0AD2`, and does its all-FF
window justify initializing an IVT/BDA, firmware, or device?

## Confirmed Layout

The admitted NTIO pre-entry contract copies exactly `0x8400` source-built bytes
to physical `0x0700` and enters at `0070:0000`.  Therefore the loaded image
occupies physical `0x0700..0x8AFF`; physical `0x0A84` lies inside it.

A read-only extraction of the exact fixture's `ntio_bytes.cc` at image offset
`0x0A84 - 0x0700 = 0x0384` starts with non-FF bytes:

```
14 03 ff fd 9e 58 1f eb 03 fa eb f8 33 c0 8e d8 ...
```

`bx_ntvdm_minimal_machine_c::initialize` creates memory, empty port space,
performs CPU reset, and then finite-run copies the NTIO image.  It has no
post-copy low-RAM/IVT/BDA or firmware initializer.  Thus neither the original
fixture image nor the minimal mantle's normal post-copy lifecycle explains an
all-FF window at that physical location.

## Source Context

OpenNT `sysinit1.asm` documents that NTIO relocates BIOS and DOS structures,
sets up a BIOS communication block, then calls DOS initialization. `msinit.asm`
receives the BIOS communication pointer and continues through the DEM/CMD
initialization services.  These are guest-controlled relocations and state
transfers; they are not an instruction to synthesize an unrelated PC firmware
or video device.

Earlier retained traces also identify `0000:0A84` as the downstream state
after an unserved startup boundary, rather than a direct initial NTIO entry.
S29 is stronger on the current branch: it proves a `50:0F` resume, then an
event with `CS=0000`, `EIP=0A84`, and an all-FF copied window.  The initial
entry CS is `0070`, so the bad state includes a guest-visible segment transfer
in addition to the changed bytes.

## Owner Matrix

| Candidate | Evidence | Disposition |
| --- | --- | --- |
| Bochs VGA/device | `0x0A84` is ordinary low RAM and belongs to the loaded NTIO aperture. | Rejected. |
| Mantle default IVT/BDA/firmware write | The result would overwrite or mask a source-built image range, and no source contract admits it. | Rejected. |
| `bx-vdm` BOP provider result | A provider might indirectly cause a wrong guest control flow only through an incorrect original contract. | Requires predecessor evidence; not yet implicated. |
| Guest NTIO/NTDOS relocation or a preceding guest write/control transfer | CS changed from `0070` to `0000` and the in-aperture bytes changed after initial copy. | Primary evidence target. |

## Conclusion

The all-FF event is not a missing low-memory initialization request.  The next
task must capture the predecessor event state and a bounded before/after
ordinary-RAM snapshot around `0x0A84`, then map the first mutation or transfer
to original NTIO/NTDOS code.  The snapshot must be an opaque mantle-owned
mechanical read; `bx-vdm` must not interpret or modify it.
