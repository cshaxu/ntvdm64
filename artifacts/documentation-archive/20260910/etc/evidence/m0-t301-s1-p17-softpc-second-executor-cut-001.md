# M0 T301 S1 P17 — SoftPC Second-Executor Cut

## Result

`zero-ledger1.tsv` is derived from `zero-ledger0.tsv` using the complete
per-file final disposition in `mvdm-file-recovery-ledger.tsv`.  It does not
delete the original audit; it classifies the SoftPC portion according to the
already-governed single-machine-executor rule.

- `zero-ledger0.tsv`: 11,949 true component-owned definitions.
- SoftPC review population: 8,681 definitions, all joined by exact original
  path and SHA-256 to a final file-disposition row.
- Retained SoftPC host algorithms: 452 (361 adapter-backed; 91
  overlay-required).
- Extracted SoftPC definitions: 8,229.
- `zero-ledger1.tsv`: 3,720 retained definitions total.

## Extracted classes

- 6,090 old CCPU/CVIDC/CPU/register decoder and FPU definitions are replaced
  by the sole Bochs CPU/FPU executor.
- 949 old SoftPC PC-device definitions (PIC, PIT, CMOS, DMA, video, disk,
  serial, keyboard/mouse and EMS paths) are replaced by Bochs-native machine
  ownership.
- 631 alternate SoftPC host product-control definitions and 53 alternate
  support-shell definitions remain original evidence only; `adapter-bochs`,
  `bochs-core`, and `app` own their final modern roles.
- 19 firmware definitions belong to `mvdm-softpc-firmware`, 440 are tools,
  and 47 are historical standalone `obj.vdm` product-entry/register bodies.

## Retained SoftPC boundary

Retained original host algorithms are only those already classified
`adapter-backed` or `overlay-required`: original host configuration,
monitor/BOP/session/capability endpoints plus the narrow source-shaped
transport overlays (`nt_cpu`, `nt_emm`, `nt_mem`, `nt_umb`, `sim32`,
`x86_emm`). These are not a CPU or device executor and reach mechanics only
through the typed adapter boundary.

No production code, build route, BOP path, or Bochs behavior changes in this
audit.
