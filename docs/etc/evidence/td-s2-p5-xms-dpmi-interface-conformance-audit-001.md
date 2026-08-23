# Td S2 P5 — XMS and DPMI Interface-Conformance Audit

## Question

Which XMS/DPMI source routes preserve their original call form, which use an
already approved generic machine mechanism, and which are either pointer/seam
migration debt or deferred NT4 product composition?

## Inputs and method

- Original and mirrored `xmsdisp.c` 12-slot service table.
- Original and mirrored `dpmi32.c` 25-slot dispatch table.
- XMS `xmsa20.c` pointer/writeback shim, and DPMI startup/descriptor source
  shims.
- The shared-pointer decision in dependency 118 and registered mechanical
  records `BX-ABI-094` and `BX-MANTLE-095..097`.

Every XMS/DPMI entry and direct dependency was classified independently.  A
generic machine exception is never treated as a DPMI provider.  Existing
project-shaped DPMI startup, descriptor and XMEM seams are migration debt,
not silently legitimized by their source-shaped field order.

## Results

The tracker P5 overlay contains all 64 required rows: 12 XMS BOPs, 25 DPMI
BOPs, and dependencies 034--041 plus 059--077.

| Disposition | Count | Consequence |
| --- | ---: | --- |
| `opennt-shaped-facade` | 10 | Retain the source/table through the bounded named facade. |
| `registered-exception` | 7 | Retain only the selector-blind ordinary-RAM mechanics under their existing IDs. |
| `migration-debt` | 17 | Migrate XMS guest-address behavior or DPMI project-shaped seams before source-route expansion. |
| `deferred` | 30 | Wait for the whole DPMI frame/LDT/INT21/VDD owner package; do not activate a leaf. |

In particular, `52:07/08` legitimately retain their original empty-UMB
failure behavior, while `52:06` remains a pointer-facade migration.  The
formal DPMI startup/table work (`53:00`, `04`, `05`, `0F`) is not called
complete: it is classified as migration debt until its historical source
shape can be preserved under the Td rules.  All fault/IRET, process-LDT,
protected-stack, VCD and host-address-return paths are explicitly deferred.

## Verification and follow-up

`tools/governance/Test-TdS2P5XmsDpmiAudit.ps1` proves the 64-row inventory and
all required disposition classes.  P6 audits Redirector, WOW16, Debugger/VDD
and top-level/machine rows.  P7 then reconciles all 321 inventory rows before
T257 S5 can be reconsidered.
