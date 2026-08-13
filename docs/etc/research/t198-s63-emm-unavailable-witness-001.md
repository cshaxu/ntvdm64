# M0 T198 S63: EMS-Unavailable Composition Witness

## Result

The active top-level bx-vdm composition now calls the pre-existing exact
`BOP 66` unavailable provider.  It resumes three bytes later and writes only
`BX=0`, which is the original `InitSpcEmm` no-expanded-memory condition.

No EMS/LIM allocator, page mapper, device, firmware, or host integration was
added.

## Clean source-built witness

The complete MSVC x64 `/MT` source-built run at
`artifacts/build/t198-s63-emm-unavailable-r1` passes the `BOP 66` boundary and
reaches:

```text
next=c8:33 terminal=1:8dc8:78cd
```

This is the OpenNT guest's `BX=0 -> fail -> AX=FFFF` path.  `C8` is the next
three-byte top-level BOP selector; `33` is following guest code, not a BOP
minor selector.

## Next frontier

The retained OpenNT SoftPC BIOS table identifies `C8` as first-phase host
mouse installation.  An existing bx-vdm headless mapping provider exists, but
its exact event state and source contract must be audited before admission.

