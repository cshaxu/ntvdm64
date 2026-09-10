# T198 S45 real-mode IP-wrap source audit

## Original guest requirement

S44's terminal pointer is `BF1F:FFFF`. The original NTIO source defines the
corresponding `strategy` routine in
`src/opennt/base/mvdm/dos/v86/doskrnl/bios/msbio1.asm:220-232`:

```text
mov word ptr cs:[ptrsav], bx
mov word ptr cs:[ptrsav+2], es
ret
```

The first instruction encodes as five bytes (`2E 89 1E 18 00`). The locked
image ledger places the original strategy at `0070:01EC`; `sysinit1.asm`
copies and rebinds BIOS/DOS segments before `DosInit`, while its `seg_reinit`
routine writes relocated code-segment fields into the resident entries. Thus
the observed dynamic target is a legitimate original guest position, not a
host table.

For a 16-bit real-mode instruction beginning at offset `FFFF`, that guest
layout requires the decoder/fetcher to consume the first byte at `FFFF` and
the remaining bytes at offsets `0000..0003`, then continue at `0004`.
This is a generic real-mode code-address mechanic; it contains no DOS, BOP,
device or adapter policy.

## Retained Bochs path

The current core differs from the pinned `O:\repos.external\bochs-2.6-compat\bochs-2.6`
`cpu/cpu.cc` only by the separately registered history calls. The relevant
upstream mechanics are retained unchanged:

| Path | Source fact | Effect at this boundary |
| --- | --- | --- |
| Sequential execution | `cpu.cc:104-106` and `instr.h:74-75` | increment full-width `RIP` by decoded instruction length before dispatch. |
| Prefetch | `cpu.cc:573-583` | preserves 32-bit EIP and raises a fault when `EIP > CS.limit`. |
| Real-mode segment load | `segment_ctrl_pro.cc:178-190` | real-mode CS has its 16-bit compatible limit in the current entry path. |
| Current run | S44 output | reports `prefetch: EIP [00010000] > CS.limit [0000ffff]`. |

There is no conditional 16-bit `RIP` reduction at either listed sequential
execution site. The diagnostic is therefore mechanically consistent with
the observed `BF1F:FFFF` strategy target. It also explains why the later
history cannot be treated as the direct result of the original far call.

## Decision

The evidence warrants a neutral real-mode instruction-fetch-wrap
reproduction, not an immediate compatibility patch. The existing exact NTIO
run proves a caller and the code/source relationship, while its terminal
pointer remains a post-stop observation. A neutral fixture must first isolate
one standard five-byte instruction split across `CS:FFFF`/`CS:0000`, require
the next instruction at `CS:0004`, and contain no OpenNT image, BOP, device,
or adapter service. Only if that fixture fails may a registered, generic core
mechanics repair be admitted.
