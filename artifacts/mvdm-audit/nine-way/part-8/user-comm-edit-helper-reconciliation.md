# Part 8: Win16 USER COMM and EDIT helper source distinctions

Five owned guest USER paths are individually identity-checked in `guest-family-inventory-observations.json`. The original WOW guest `user/makefile:88-91,203` lists `wowcomm.obj`; it does not list `comdev.obj`, `msutil.obj` or `wmsyserr.obj` in those captured lists. This is recipe evidence, not proof those sources are never used in another profile or absent from a packaged image.

`user/comdev.c` implements an older guest communication layer and `comdevi.h` defines its internal state. The selected WOW path instead has `user/wowcomm.c::IOpenComm/ICloseComm`: it allocates the 16:16 event structure, frees on failed open, and obtains/frees it on close. USER ordinal dispatch then reaches original `wow32/wucomm.c`, whose port table owns Win32 handles, events, buffers, writer/modem threads and originating task, as documented in `m0-t469-s1-comm-owner-matrix.md`. Real host serial hardware is an external resource; neither guest helper nor a mock proves CTS/DSR behavior.

The original guest `msutil.c` and `wmsyserr.c` headers identify GetCharDimensions and DrawFrame helpers for the old EDIT family. They must not be conflated with the separate selected NT USER EDIT source and its host dependencies. Inactive `#ifndef WOW` portions cannot be counted as active WOW behavior solely from filename matching.

| Owned file | Classification and limit |
| --- | --- |
| `wow16/user/comdev.c` | Original guest comm implementation; retained alternate/profile source, not the shown WOW makefile provider. Its cleanup semantics are relevant comparison evidence. |
| `wow16/user/comdevi.h` | Original comdev internal data declarations; no permission to substitute them for WOW32's port table. |
| `wow16/user/msutil.c` | Original guest EDIT/messaging helper, historical role. Active guest object and binary provenance unproved. |
| `wow16/user/wmsyserr.c` | Original guest EDIT/system-error drawing helper; active profile and object selection unproved. |
| `wow16/user/readme.txt` | Original build note: NONX86 avoids embedding USER32 code in an x86 guest build. Historical instruction only; do not execute or rebuild guest. |

T469 S1/S2 records the original WOW32 `FreeCommSupportResources` stopping after its first matching port, contrasted with guest comdev cleanup over all ports, and an original bounded writer/modem close wait. Those source audits found the same first-match behavior in several original revisions, with no approved replacement. This is a recorded inherited limitation, not proof a particular user has experienced a leak; it must not silently be declared fixed. T469's COM3 had no CTS/DSR peer and could only witness the original timeout path. No fresh test was executed here.

S14/S15 should preserve actual WOW guest→WU32 ownership while testing success/failure, queue/DEB identity, close and task exit, including multiple ports using controlled endpoints. S13 owns real serial endpoint behavior; report unavailable hardware separately. S15 owns EDIT helper semantics and direct guest/control behavior; S19 binds exact release and guest identity. Preserve original guest and avoid a second independent COMM table.

Replay `rg -n "wowcomm.obj|comdev.obj|msutil.obj|wmsyserr.obj" src/mvdm/wow16/user/makefile`, read `wowcomm.c`, `wucomm.c` and the two T469 records, and run part-8 inventory observer. No guest, product, device or shared ledger change.
