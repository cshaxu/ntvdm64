# CCPU fixture PIC readiness counterexample and control

One x86 standalone fixture binary was run twice, without modifying product or guest. Its derived source retains the original CCPU bounded fixture's PIC setup/assertion, adds diagnostic register reads and an environment-gated test control which sets `DelayIrqLine=0` before the IRQ request. The selected `ica.obj` owns the real PIC; the map identifies `DelayIrqLine` as `original-softpc-host-fixture-roots:nt_eoi.obj`, and `WOWIdle` / `host_EOI_hook` as the existing bounded fixture seams. The inherited link uses `/force:multiple`, and this isolated version also adds the existing fail-closed BaseClient seam object. The forced link and seams are a fixture boundary, not an installed-product comparison.

* Default: before request IRR=00, IMR=00 and `DelayIrqLine=FFFFFFFF`; after request IRR=01, IMR=00. INTACK returns -1 with EOI=0/WOWIdle=0, process exit 1. [Captured output](ccpu-pic-blocked-observation.txt), SHA-256 `80F3141AAA9DAC1CC036EF2C91E5D6EBF70806865587E142A59CA6AB8D3E8E3C`.
* Ready control: same executable and input, only environment enables test assignment `DelayIrqLine=0`. IRR=01 and IMR=00 after request; INTACK returns 8 with EOI=0/WOWIdle=1, process exit 0. [Captured output](ccpu-pic-ready-observation.txt), SHA-256 `23A84CA585978E82A212A50E78B58C338191C5D7E04D21093C2F9B1C37A91379`.

EXE SHA-256 for both runs: `8F08B09D9E698B01D96A54E15245FA4C4BCB47D194570507F2A50644920E186C`. Derived source: `1B6A06037D15F898A89D537958AF18CA8FC48AE40BEE69DFEDB50D356447088F`. `nt_eoi.c:49` initializes the original variable to all blocked until SPCKBD loads; `ica.c:488` incorporates it into the scanned interrupt mask; `nt_bop.c:874` clears it after keyboard/IVT readiness and restarts interrupts. Thus the earlier PIC self-check failed because this isolated fixture asked for IRQ0 before modeling that readiness boundary, while IRR retained the request. The counterfactual write is a focused fixture control, **not** a proposal to clear or bypass production readiness.

The older T423 S9 log records an exit-zero original fixture under an earlier build, but does not establish the then-linked variable owner or explain the change. This A/B alone does not prove why that previous build differed, assert current installed product behavior, or validate PIC reset/EOI regressions. It does show that the observed vector rejection follows the selected initial mask, not loss of the queued IRR bit.

Replay from the repository root with existing x86 worker libraries and VS build tools:

```powershell
python artifacts/mvdm-audit/host-ch-scope/ccpu-pic-fixture-derive.py
python artifacts/mvdm-audit/host-ch-scope/ccpu-pic-fixture-run.py
python artifacts/mvdm-audit/host-ch-scope/ccpu-pic-fixture-compare.py
```

The runner stores compilation, map, executable and per-case full outputs under `build/t473-s10/bitwrap-probe`; the compare script executes the same built EXE for each case. Log hashes may vary with diagnostic thread IDs. These scripts and the two compact captures are evidence only; no shared ledger or guest was changed.
