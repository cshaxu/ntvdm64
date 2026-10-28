# C-VID CPU vector metadata: selected generation and bounded fixture

`softpc.new/base/cvidc/c2cpusad.h` is an original generated metadata input,
identical in current tree and both compared OpenNT editions. It is not directly
named by the current Ninja graph or compiler dependency database. Instead,
`New-T310OriginalSoftpcNinja.ps1` invokes `GenerateCvidcCpuBinding.mjs` while
the graph is generated; that script reads this file's `CpuVectorNames` and
`CpuPrivateVectorNames` and generates `cvidc_cpu_binding.inc`. The selected
`mvdm_cvidc_vector_binding.c` includes it and its object is a member of
`original-softpc-cvidc.lib`. Original `ev_glue.c::setup_vga_globals` invokes
the binding adapter. This is a project integration path, not evidence that
the original product generated the adapter in the same manner.

The [observer](cvidc-metadata-observe.py) and [receipt](cvidc-metadata-evidence.json)
bind source, generator, builder, selected generated include, binding source
and graph hashes. In the recorded tree the original input defines 154 public
and 55 private names; all 209 names occur in the same order in the generated
include. Replay with `python artifacts/mvdm-audit/host-ch-scope/cvidc-metadata-observe.py`.
This confirms name and order only: it does not independently validate the
selected providers, intended null slots, ABI layout or dispatch behavior.

Existing [fixture evidence](../cvidc-vector-fixture-r74-evidence.json) records
a successful selected-object test of `setup_vga_globals`; fixture source
checks `Cpu.Video`, `Cpu.Sas`, `Cpu.Private`, several intended null values,
and calls `SetSTATUS`, selected getters and jump restart functions. That is
bounded evidence of some binding behavior, not execution of all 209 entries.
The fixture used test-owned host seams and the existing `/force:multiple`
link; it does not prove the deployed product exercised this path or that all
providers, callbacks, mode changes, failures, concurrency or teardown match
the original. The selected generated mapping contains explicit zero-valued
providers; their correctness must not be inferred from successful binding.

Recommended disposition: **partial**, with the source text retained at
100% against both compared originals; no claim is made about a numerical
behavioral completion percentage. Original slot semantics, adapter mapping,
current-image identity and runtime lifecycle remain open to the corresponding
implementation and integration stages. No source, ledger, guest, product or
build was modified for this observation.
