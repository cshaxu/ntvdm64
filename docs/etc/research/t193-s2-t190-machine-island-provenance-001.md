# T193 S2 T190 machine-island provenance 001

Date: 2026-08-12  
Packet: M0 T193 S2  
Disposition: frozen-image provenance reconciled; no build or execution occurred.

## Exact frozen image

The T190 S3 makefile compiles `cpu/exception.cc` with
`BX_NTVDM_ENABLE_MACHINE_COMPOSITION=1`.  Its one final link includes exactly
the previously admitted machine objects:

| Object/symbol set | Meaning |
| --- | --- |
| `machine/bx_ntvdm_machine_bop_v1.obj` | selector-blind seam and the fixed mechanics context |
| `machine/unexp_nt.c.obj`, `_unexpected_int` | unchanged original handler for selector `02` |
| `machine/illegalp.c.obj`, `_illegal_op_int` | unchanged original handler for selector `06` |
| `cpu/exception.o`, `bx_ntvdm_machine_composition_probe` | generic #UD-to-machine seam; Bochs continues to own decode, exception delivery and resume |

The engine hash is
`03C8FFD23FA015B480C02B42250511527450ADE5DCC51F289D4249D617439BCD`.
The bundle manifest in `t190-command-bootstrap-bundle-r1` names that same
hash, and an independent file-hash comparison of the derivative executable
and bundled `ntdos64-bochs.exe` matches it exactly.  Therefore the valid T190
observation used an image that *contains* both islands.

Presence is not a selection claim.  The T190 observation does not contain a
machine-composition outcome record for a `02` or `06` selector, so it proves
neither island ran in that particular bounded command-bootstrap sequence.

## Default surface

`bx_ntvdm_machine_composition_v1_probe` accepts only a valid #UD window and
then selects `02` or `06`.  All other selector values return `DECLINED`.
The exception code performs the generic three-byte resume only for
`HANDLED_RESUME`; a decline returns to native Bochs exception processing.
Thus S1's `12/15/17/5E/5F/66/C8/FE` rows cannot become handler invocations
merely because the two island objects are linked.

The T190 link line also retains legacy adapter objects whose names resemble
several selector domains (`bios_memory_service`, `emm_unavailable_service`,
`printer_unavailable_service`, `config_done_service`, `spckbd_init_service`,
and `controlled_stop_service`).  Object presence is not an alternate machine
route: the S1 collision and owner rules still require ingress/plane identity,
and the machine probe itself exposes no such branch.  No conclusion here
authorizes those objects to handle a bare machine BOP.

## Next S

S3 is a closure-only decision register.  It must combine S1/S2 into the T193
handoff: retain selector `06` as the exceptional already-linked original
machine island, retain all other reached machine rows as native/collision/
unavailable/engine-control dispositions, and state whether any newly admitted
implementation slice exists.  It may change documentation only.
