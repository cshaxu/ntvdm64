# T205 S2 P2 — XMS copied move bridge

## Question

Can OpenNT `xmsMoveBlock` be recovered without a guest pointer or an XMS-aware
Bochs change?

## Design and implementation

OpenNT reads three DWORDs immediately below `SS:BP`: word count, source
linear address and destination linear address. The XMS session now forms one
existing `bx_ntvdm_mechanical_action_v1` ordinary-RAM read at `SS:BP-12` and
receives twelve copied bytes. It decodes the fields, multiplies word count by
two, and submits a fixed `MOVE_PHYSICAL` request to mantle.

The mantle operation is selector-blind. It validates both backing-RAM ranges
and uses its existing bounded chunked copy routine, including overlap order.
It has no OpenNT, DOS, XMS or BOP vocabulary. The adapter receives only the
fixed result and restores AX to `1` or `0`; it carries no mapping or host
pointer. A zero word-count returns success without a physical copy.

## Evidence and limitation

The strict MSVC x64 `/MT /W4 /WX` package regression constructs the 12-byte
record and verifies source `0x2000`, destination `0x3000`, four-byte count and
AX success through common ingress. The mantle fixture and static boundary
script cover physical-copy request fields and ordinary RAM copy behavior.

The full minimal-machine recipe still has its independently recorded missing
CPU/decode object closure at final link. This P does not claim a native XMS
move run. It also does not claim an exhaustive BL error-code table: the
inspected OpenNT handler documents BL on failure but its shown branch has no
concrete BL assignment, so failure preserves BL and sets AX to zero.
