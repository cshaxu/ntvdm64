# M0 T404 S3 P13 — CPU40 BOP entry-contract audit

## Decision

Do **not** restore a two-byte BOP prefix before either the mouse callback or
the DOSX real-mode continuations.  A BOP is a three-byte host trap
(`C4 C4 <number>`), while all three examined CPU40 entries are addresses of
ordinary guest instructions.  P12 already changed the mouse route to enter
its registered address directly.  The two remaining CPU40 consumers in
`reset.c` are proposed direct entries for a separately admitted, bounded
follow-up; this audit changes no product code.

## Method and CPU40 contract

The selected production source was searched for `HOST_BOP_IP_FUDGE` and its
definition.  The sole definition is `-2` in
`src/mvdm-host/softpc.new/host/inc/host_def.h`.  The complete production
inventory contains that definition and the three entries below; no selected
CPU40 caller was omitted.

`src/mvdm-host/inc/bop.h` defines a BOP as `C4 C4 <number>` and defines its
size as three bytes.  CPU40 enters emulation at the live `CS:EIP` value
(`SETUP_HOST_IP` in `base/ccpu386/c_main.c`); it does not apply an implicit
two-byte adjustment.  Consequently, an adjustment is valid only when the
supplied address has been demonstrated to name the third byte of a
host-constructed BOP/trampoline.  A direct address of normal guest code must
be entered unchanged.

## Inventory and disposition

| Route | Supplied address provenance | Entry bytes / source proof | CPU40 disposition |
| --- | --- | --- | --- |
| INT 33h mouse callback, `base/keymouse/mouse_io.c` | Guest callback registered by `EDIT.COM` | P12 captured callback `371A:0223`; `0221` is `02 00` while `0223` begins `06 1E` (`PUSH ES; PUSH DS`). | **Direct, delivered in P12.** CPU40 now uses `setIP(offset)`. The historical fallback remains only outside CPU30/CPU40. |
| CMOS shutdown code 9, `base/bios/reset.c:620` | The BIOS data area gives a saved `SS:SP`; the saved IRET frame supplies its IP. | `dpmi/dxutil.asm` deliberately pushes `segDXCode` and `offset enrm50`; its own comments say code 9 reloads `SS:SP`, restores registers, then IRETs. `enrm50` begins `FCLI`, ordinary DXCODE, not a BOP. | **Direct proposed.** For CPU40, do not subtract two before `setIP`. Requires a new admission and DOSX regression proof. |
| CMOS shutdown code 0Ah, `base/bios/reset.c:684` | `dpmi/dxutil.asm` writes `IO_ROM_SEG = segDXCode` and `IO_ROM_INIT = offset DXCODE:enrm45`. | The same source says code 0Ah loads `CS:IP` and jumps to it. `enrm45` begins `FCLI`, ordinary DXCODE, not a BOP. | **Direct proposed.** For CPU40, do not subtract two before `setIP`. Requires a new admission and DOSX regression proof. |

The source tree contains actual BOP users (for example, the `BOP` macro and
the BIOS `BOP_UNSIMULATE` route).  They construct/target the real three-byte
trap directly and do not consume `HOST_BOP_IP_FUDGE`; they are not evidence
that these three guest continuations need a prefix.

## Consequence and bounded follow-up

There is no selected CPU40 production consumer proved to require `-2` after
P12.  The historical `reset.c` adjustment predates the CPU40 direct-entry
contract and is a high-confidence explanation for failures that begin in the
DOSX reset/mode-switch chain.  That is a correlation, not runtime proof.

The safe follow-up is a new packet that changes only the CPU40 condition in
the two `reset.c` sites, preserving the historical non-CPU40 branch, then
builds and runs focused DOSX/WOW regressions.  It must not remove the global
definition or rewrite real BOP stubs until all non-selected profiles have
been separately inventoried.  Inserting two BOP bytes before `enrm45` or
`enrm50` is rejected: it would alter original DOSX code layout and would not
match the direct continuation addresses written by that source.

## Scope result

P13 is a read-only source audit.  It adds no runtime instrumentation, guest
media, build artifact, or product behavior.  The proposed follow-up remains
unadmitted.
