# opennt-bop-overlay

Private source bodies for registered OpenNT BOP mirror divergences. This root
has no public ABI and is included only by the matching `opennt-bop` mirror
translation unit; no adapter, host, session or app source may include it.

| ID | Original definition purpose | Private divergence | Mirror boundary |
| --- | --- | --- | --- |
| `BOP-DIV-023` | `memory_size` reads the mutable BIOS memory-size word. | The reached one-function source subset is private because the complete SoftPC file is not admitted and the subset exceeds the mirror threshold. | `softpc/mem_size.c` |
| `BOP-DIV-080` | `cassette_io` and `emm_init` are historical SoftPC BIOS entry points. | The admitted real-mode AH=88h and no-EMS source subsets are private because their full product files are not admitted and the subsets exceed the mirror threshold. | `softpc/tape_io.c`, `softpc/emm_fncs.c` |
| `BOP-DIV-056` | `vrdisp.c` dispatches the VDMREDIR service table through the historical CCPU frame. | The copied-frame dispatcher is a source-derived rehost, not a retained OpenNT mirror body. | `vdmredir/vrdisp.c` |
