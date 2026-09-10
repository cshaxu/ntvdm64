# True Zero Ledger 1 (`zero-ledger1.tsv`)

`zero-ledger1.tsv` is the runtime-oriented successor to `zero-ledger0.tsv`.
It retains the true `mvdm-host` source set except the portions of
`softpc.new` that would reintroduce a second, incomplete SoftPC executor or
are firmware/tool-only inputs.

The original source is not deleted. Every reviewed SoftPC definition remains
in `zero-ledger1-softpc-disposition-ledger.tsv`; extracted definitions also
remain in `zero-ledger1-softpc-exclusion-ledger.tsv` with the exact final file
disposition and replacement owner.

## SoftPC boundary

- `base/ccpu386`, `base/cvidc`, `base/cpu`, and `base/c_regs.c` are the old
  CPU/FPU/decode/register executor. `bochs-core`, reached only through
  `adapter-bochs`, is the sole equivalent executor.
- `base/system`, `video`, `disks`, `comms`, `keymouse`, and `dos` are the old
  PC-device executor. Bochs owns PIC/PIT/CMOS/DMA/video/disk/serial/input and
  EMS mechanics; endpoint/provider logic is retained only where its original
  file disposition is adapter-backed or overlay-required.
- Original firmware is an immutable `mvdm-softpc-firmware` input, never an
  MVDM host library. Historical tools stay under `mvdm-tools`.
- `obj.vdm/c_regs.c` and `obj.vdm/ntvdm.c` are the historical standalone
  product entry/register shell; `app` owns final composition, so they do not
  remain in the host closure either.
- The only retained SoftPC function bodies are the 361 `adapter-backed` and
  91 `overlay-required` host-control/monitor/capability algorithms, including
  the narrow source-shaped transport overlays for `nt_cpu`, `nt_emm`, `nt_mem`,
  `nt_umb`, `sim32`, and `x86_emm`.

Generation:

```text
node tools/governance/export-t301-s1-true-zero-ledger1.mjs .
```
