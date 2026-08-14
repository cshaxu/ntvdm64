# T199 S49 — Machine and BIOS BOP package revalidation

## Question

Does the current CLI composition keep machine/BIOS BOP selection in `bx-vdm`
while retaining CPU, memory, interrupt, monitor and device mechanics below the
selector-blind bx-core/bx-mantle boundary?

## Source-derived owner/disposition map

| Selector/form | Original evidence | Current owner/disposition |
| --- | --- | --- |
| `12` | historical BIOS table `memory_size` | declared bare-machine profile data: real-mode AX=`027F`, `RIP+3` |
| `15:AH=88` | BIOS `cassette_io` route and DOS callers | declared bare-machine extended-memory profile data: AX=`0C00`, `RIP+3` |
| `15`, other AH | no admitted capability contract | rejected; no fabricated BIOS result |
| `5C` | active NTVDM BIOS table maps `MS_bop_C` to `illegal_bop`; header variant conflicts | machine/variant deferred typed stop |
| `5D` | `MS_bop_D` is monitor IRET-hook control | monitor deferred typed stop |
| `5F` | `MS_bop_F` runs `kb_setup_vectors`, then interrupt lifecycle | admitted checked RAM action handoff; mantle owns mechanical execution |
| `FD` | monitor-only `switch_to_real_mode`; DPMI caller in `dxutil.asm` | mode-transition deferred typed stop |

The `12` and `15:88` values are profile declarations rather than direct Bochs
or CMOS queries. `5F` requests a typed selector-blind mechanical action and
does not transfer keyboard, PIC, BIOS or interrupt semantics to `bx-vdm`.
None of `5C`, `5D`, or `FD` selects a historical `BIOS[]` branch.

## Verification

Fresh MSVC x64 `/MT` runs passed:

1. `artifacts/build/t199-s49-machine-facade-r1/` checks all six selector
   classifications.  It verifies that memory/handoff routes receive no
   facade-invented result and that `5C`, `5D`, `FD` stop at the typed boundary.
2. `artifacts/build/t199-s49-machine-package-r1/` builds and runs the bound
   boot-namespace composition.  It verifies actual profile results for `12`
   and `15:88`, the checked `5F` composition action, and the three deferred
   outcomes.

## Interpretation and limit

This package closes current ownership and disposition, not a full PC BIOS,
monitor, IRET-hook, protected-mode transition, keyboard, video, PIC or device
implementation.  The explicit deferred results preserve that distinction and
prevent a selected machine BOP from becoming raw `#UD`.  No native trace ran.
