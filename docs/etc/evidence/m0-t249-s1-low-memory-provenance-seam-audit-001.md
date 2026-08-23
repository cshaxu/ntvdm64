# M0 T249 S1 — low-memory provenance source and seam audit

## Question

Can an existing selector-blind observation identify the writer of the
runtime-modified instruction at physical `0x0A87` and the pointer consumed by
its subsequent far indirect call, without changing guest execution?

## Inputs

- Source-built `build/output/dos/NTIO.SYS` and its primary build map
  `build/M0-T225-S8/ntio-primary-source-build-r8/.../bios/ntio.map`.
- T248 S4's bounded native transition: `0000:0A87 -> 5859:5F04`.
- Original `sysinit1.asm` and current `bx-core`/`bx-mantle` RAM and diagnostic
  surfaces.

## Source and layout map

`ntio.map` places `SYSINITSEG` at image `0x09D0..0x415F`; physical `0x0A87`
is consequently source logical `009D:00B7`, within that original sysinit
image, not an IVT/BDA/firmware hole.  The staged bytes at image offset
`0x387` begin `9E 58 1F ...`; T248 observes runtime `FF 9F 26 03 ...`.

The original source has multiple legitimate runtime relocation paths:

- `sysinit1.asm:goinit` copies `sysinitgrp` with `rep movsw` to a computed
  segment and reaches it through `retf`;
- `sysinit1.asm` chooses `MovDOSLo` or `dos_segreinit`/a low-memory stub;
- `MovDOS` copies DOS with `rep movsb` and records its relocated segment.

This proves that an image-byte mismatch can arise in guest bootstrap
relocation. It does **not** identify which original writer created the
observed `FF 9F 26 03` call sequence or published the pointer at
`DS:BX+0x326`.

## Existing seam audit

| Surface | What it observes | Sufficiency |
| --- | --- | --- |
| Instruction history | fixed CPU positions, last 16 entries and CS transitions | Does not retain general register operands or RAM writes. |
| CS provenance | copies 15 predecessor/successor bytes and 10 stack bytes only when CS changes | Proves the far call and return pair, but only after the unknown write. |
| Terminal provenance / finite capture | copies a bounded terminal instruction/stack window | Read-only terminal fact; cannot identify an earlier writer. |
| IVT watch | reads a declared IVT word before/after use | Vector-specific and read-only; `0x0A87` is neither an IVT slot nor its writer. |
| Checked RAM / mechanical actions | caller-directed bounded copies | These are explicit composition operations, not CPU-write observation. |
| `BX_MEM_C::writePhysicalPage` | common adopted CPU physical-write path | It currently dispatches device memory handlers or writes ordinary RAM directly. No default-off, bounded mantle callback or provenance record exists for ordinary CPU writes. |

## Result and disposition

No existing observation can identify the first ordinary-RAM write to a fixed
physical range, record its immediately preceding CPU position, and retain no
guest pointer. S2 is therefore not applicable. S3 is admitted only to design
and validate the smallest default-off selector-blind physical-write observer:
fixed watched range, copied physical address/byte span/pre-write CPU position,
no guest semantic decoding, no host pointer exposure, and no execution or
write-result change. Because its hook point is adopted `BX_MEM_C`, any source
change must be registered as a Bochs intrusion before implementation.
