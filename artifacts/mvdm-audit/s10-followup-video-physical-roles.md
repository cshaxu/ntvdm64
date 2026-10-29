# S10 C-VID physical roles: selected-profile deeper reconciliation

Scope is the 38 entries frozen by [previous inventory](s10-followup-video-scan.json), not all C-VID. SHA equality to both original catalogues and lexical scans are recorded there. [Selected x86 Ninja dependency snapshot](host-ch-scope/ccpu-cvidc-pending-header-evidence.json) records each listed C-VID header with zero selected object dependencies, 0 stale dependency blocks, plus the graph and dependency hashes. The original C-VID `sources` does not name `sascdef.c`; selected SAS is `ccpu386/sasCdef.c`. Source lexical named-include scan found no include of these C-VID physical names except `sas4gen.h`, which resolves to the distinct generated x86/prod header. This is a selected-graph and physical-file conclusion only; it is not proof these algorithms are unneeded, runtime-safe, or absent in alternative/debug profiles. Generated artefacts or external tools outside the observed graph have not been investigated.

Recommended physical disposition for every row below is **excluded for the captured NT x86 worker build only**. If the ledger cannot encode this explicit limitation, retain pending rather than state globally excluded. This proposal never deletes files; S11/S13/S17 retain functional auditing, adaptation and testing of corresponding behavior. No binary or product source modified.

The detail column states observable declarations, not a claim of a live call path. Dependency observation of absence is not an invocation or functional acceptance test.

| Source | Original physical role | Observed exact selected object dependencies | Functional work retained |
|---|---|---:|---|
| `acc_efi.h` | access-table initializer | 0 (captured graph) | S11 CPU/SAS |
| `access_c.h` | operand access shapes and checks | 0 (captured graph) | S11 CPU/SAS |
| `adjust_c.h` | flag adjustment masks | 0 (captured graph) | S11 CPU/SAS |
| `bldr_c.h` | operand-size constants | 0 (captured graph) | S11 CPU/SAS |
| `bpi_c.h` | BPI instruction enumeration | 0 (captured graph) | S11 CPU/SAS |
| `buffer_c.h` | fragment and instruction-buffer records | 0 (captured graph) | S11 CPU/SAS |
| `chain4_c.h` | chain-four constant | 0 (captured graph) | S13 video/I/O; S11 CPU where shared |
| `contxt_c.h` | CPU context record and mask | 0 (captured graph) | S11 CPU/SAS |
| `cpnote_c.h` | constraint-change enumeration | 0 (captured graph) | S11 CPU/SAS |
| `cpureg_c.h` | Intel register identifiers | 0 (captured graph) | S11 CPU/SAS |
| `ea_c.h` | effective-address scaling | 0 (captured graph) | S11 CPU/SAS |
| `flags_c.h` | flag widths and masks | 0 (captured graph) | S11 CPU/SAS |
| `flagtb_c.h` | flag table constants | 0 (captured graph) | S11 CPU/SAS |
| `frag_c.h` | fragment, registers, flags and instruction shapes | 0 (captured graph) | S11 CPU/SAS |
| `gdebug.h` | generated debug declarations | 0 (captured graph) | S17 debug/profiling |
| `gldc_c.h` | descriptor and selector cache records | 0 (captured graph) | S11 CPU/SAS |
| `inst_c.h` | numeric-processor operand enumeration | 0 (captured graph) | S11 CPU/SAS |
| `io_c.h` | I/O port table record | 0 (captured graph) | S13 video/I/O; S11 CPU where shared |
| `lc_c.h` | fragment, entry-point and copier records | 0 (captured graph) | S11 CPU/SAS |
| `muldiv_c.h` | multiply/divide flags | 0 (captured graph) | S11 CPU/SAS |
| `nano_c.h` | instruction and block records/constants | 0 (captured graph) | S11 CPU/SAS |
| `page_c.h` | page virtualisation metadata | 0 (captured graph) | S11 CPU/SAS |
| `parser.h` | generated instruction/access parser codes | 0 (captured graph) | S11 CPU/SAS |
| `ports_c.h` | EGA port constant | 0 (captured graph) | S13 video/I/O; S11 CPU where shared |
| `prof_c.h` | profiling and EOI constants | 0 (captured graph) | S17 debug/profiling |
| `prot_c.h` | translation cache and protection records | 0 (captured graph) | S11 CPU/SAS |
| `pxcptn_c.h` | protected-mode exception constants | 0 (captured graph) | S11 CPU/SAS |
| `rdwr_c.h` | read/write traversal constants | 0 (captured graph) | S11 CPU/SAS |
| `sas4gen.h` | SAS vector layout and accessors | 0 (captured graph) | S11 CPU/SAS |
| `sascdef.c` | C SAS vector initializer | not selected (alternate initializer) | S11 CPU/SAS |
| `segreg_c.h` | segment descriptor and selector constants | 0 (captured graph) | S11 CPU/SAS |
| `sf_c.h` | host and special-function access constants | 0 (captured graph) | S13 video/I/O; S11 CPU where shared |
| `stack_c.h` | stack safety and item constants | 0 (captured graph) | S11 CPU/SAS |
| `strint_c.h` | string direction constants | 0 (captured graph) | S11 CPU/SAS |
| `task_c.h` | task-state-segment constants | 0 (captured graph) | S11 CPU/SAS |
| `trace_c.h` | trace ring and bit flags | 0 (captured graph) | S17 debug/profiling |
| `vglob_c.h` | video dispatch vector counts | 0 (captured graph) | S13 video/I/O; S11 CPU where shared |
| `xfer_c.h` | protected-mode transfer constants | 0 (captured graph) | S11 CPU/SAS |
