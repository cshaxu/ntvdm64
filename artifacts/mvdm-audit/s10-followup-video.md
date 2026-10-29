# S10 C-VID pending physical-source follow-up

## Question and scope

Reconcile the 38 currently pending `src/mvdm/softpc.new/base/cvidc` C/H physical paths against both original audit catalogues and the selected NT x86 CCPU40/C-VID graph. The table is limited to the physical file and captured configuration; neither a file-role exclusion nor identical bytes proves or removes the underlying CPU, video or SAS capability. No product, guest, shared ledger, build graph or cached object was changed.

## Evidence and procedure

The [replayable lexical observer](s10-followup-video-scan.py) reads the then-current host inventory and frozen per-file identity table, both explicitly named read-only audit catalogues, and 2,581 current `src/mvdm` C/H/INC/ASM files; [its snapshot](s10-followup-video-scan.json) retains full source/catalog hashes and per-symbol reference paths. All 38 current hashes equal both recorded OpenNT originals. Each has one DOS inventory row marked `not_reviewed` and two WOW inventory rows marked `pending_boundary_scan`; these are census labels, not functional dispositions. The scan searches literal source include names and `#define` identifiers, including disabled code and comments. It does not expand preprocessor conditions, map enum/struct identifiers, scan every external generator/profile, or establish reachability. A zero-symbol row can still contain enums, types, initializers or executable code.

The separately retained [pending-header dependency snapshot](host-ch-scope/ccpu-cvidc-pending-header-evidence.json) recorded no exact selected-worker dependency for any then-pending C-VID header, with the x86 worker graph SHA-256 `835D8D25248FFBE2C4B4B147C056A43EF7C2F3624A85E09F141258F45FA0410C`. This is a captured snapshot, not a new Ninja invocation, proof of all build configurations, or runtime evidence. `base/cvidc/sources` separately lists original generated C-VID compilation units. The current graph selects `ccpu386/sasCdef.c` for its SAS-vector initializer; compare the actual graph and [SAS vector audit](shard-3/sas4gen-vector-audit.md), not file-name similarity.

## Bounded conclusions for three exact physical paths

| Physical source | Current observed NT x86 profile | Decision and retained boundary |
|---|---|---|
| `sas4gen.h` | The basename is included by `base/inc/sas.h`, but captured dependency records and include search order select the retained `src/ntvdm-exe/softpc/include/generated/x86/prod/sas4gen.h`, not this C-VID path. The selected vector has additional slots; the two layouts are not interchangeable. | Recommend **excluded physical C-VID header in this selected profile**. S11 retains vector ABI, fallback/breakpoint reachability, initialization, and lifetime; never exclude SAS behavior or claim full tail restoration. |
| `sascdef.c` | Original C-VID `sources` does not list this physical file; captured graph selects `ccpu386/sasCdef.c`, whose own initializer has additional vector entries. Original `ccpusas4.c::sas_init` calls `SasSetPointers(&cSasPtrs)`. | Recommend **excluded physical C-VID initializer in this selected profile**, not duplicate-linking it. S11 owns the actual selected initializer, vector compatibility and failure/teardown; other builds remain unproved. |
| `vglob_c.h` | Its three function-count identifiers (`NUM_WRITE_FUNCS`, `NUM_MARK_FUNCS`, `NUM_READ_FUNCS`) occur in scanned project sources only as its definitions and the historical MIPS `evptrs_c.h` definitions; no direct named include appears in `src/mvdm` C/H/INC/ASM or `src`/`tools` C/H/INC/ASM/PS1/CMake searches. Captured selected dependencies have no exact hit; physical C-VID `vglob.c` is empty. | Recommend **excluded physical count-only header in this selected profile**, with generated/alternate profiles expressly outside the evidence. S13 still must validate the selected video provider, vector counts, mode transitions, plane/latch and reset behavior; no graphic functionality is ruled out. |

The first two recommendations consolidate earlier source-role findings; the new scan additionally checks exact current catalog entries and literal identifier references for all pending paths. The coordinator must decide whether a limited physical-profile exclusion is expressible in the shared ledger. If `excluded` is interpreted globally across all historical profiles, these three must remain pending until that broader applicability decision; no mirror-diff source may be closed by this shortcut.

## Per-file reconciliation

All non-listed alternatives below remain **pending**, not restored or globally excluded. The numbers are unique macro names (`#define`, excluding include guards) and distinct other MVDM files mentioning any of those names; they are lexical only, may count duplicate definitions or unrelated same-name identifiers, and do not cover enum/type or initializer references. Full hashes and paths are in the linked snapshot. “Candidate” means the three bounded decisions above, not coordinator approval.

| Physical file | Status recommendation | Macros | Other lexical reference files | Named include files |
|---|---|---:|---:|---:|
| `acc_efi.h` | pending | 0 | 0 | 0 |
| `access_c.h` | pending | 0 | 0 | 0 |
| `adjust_c.h` | pending | 4 | 1 | 0 |
| `bldr_c.h` | pending | 4 | 1 | 0 |
| `bpi_c.h` | pending | 0 | 0 | 0 |
| `buffer_c.h` | pending | 19 | 1 | 0 |
| `chain4_c.h` | pending | 1 | 1 | 0 |
| `contxt_c.h` | pending | 8 | 1 | 0 |
| `cpnote_c.h` | pending | 0 | 0 | 0 |
| `cpureg_c.h` | pending | 1 | 1 | 0 |
| `ea_c.h` | pending | 4 | 1 | 0 |
| `flags_c.h` | pending | 16 | 1 | 0 |
| `flagtb_c.h` | pending | 11 | 1 | 0 |
| `frag_c.h` | pending | 32 | 2 | 0 |
| `gdebug.h` | pending | 0 | 0 | 0 |
| `gldc_c.h` | pending | 146 | 8 | 0 |
| `inst_c.h` | pending | 0 | 0 | 0 |
| `io_c.h` | pending | 0 | 0 | 0 |
| `lc_c.h` | pending | 8 | 4 | 0 |
| `muldiv_c.h` | pending | 5 | 1 | 0 |
| `nano_c.h` | pending | 161 | 13 | 0 |
| `page_c.h` | pending | 14 | 1 | 0 |
| `parser.h` | pending | 1003 | 9 | 0 |
| `ports_c.h` | pending | 1 | 4 | 0 |
| `prof_c.h` | pending | 9 | 3 | 0 |
| `prot_c.h` | pending | 66 | 1 | 0 |
| `pxcptn_c.h` | pending | 24 | 1 | 0 |
| `rdwr_c.h` | pending | 3 | 1 | 0 |
| `sas4gen.h` | profile-limited exclusion candidate | 55 | 84 | 1 |
| `sascdef.c` | profile-limited exclusion candidate | 1 | 17 | 0 |
| `segreg_c.h` | pending | 3 | 3 | 0 |
| `sf_c.h` | pending | 39 | 2 | 0 |
| `stack_c.h` | pending | 37 | 5 | 0 |
| `strint_c.h` | pending | 3 | 1 | 0 |
| `task_c.h` | pending | 6 | 1 | 0 |
| `trace_c.h` | pending | 0 | 0 | 0 |
| `vglob_c.h` | profile-limited exclusion candidate | 3 | 1 | 0 |
| `xfer_c.h` | pending | 6 | 1 | 0 |


## Remaining S11/S13 work

- **S11, SAS and CPU execution:** bind the actual generated x86 vector layout to all original caller and provider slots, explicitly test `sas_touch`, `VirtualiseInstruction` and `IsPageInstanceData` reachability and non-success breakpoint/null behavior, and validate address translation, protected-mode transitions, flags, faults and teardown. The diagnostic evidence cited in the prior SAS audit is not a live guest reachability proof.
- **S13, video and lifecycle:** identify actual generator/output provenance and use of the remaining type/enum/initializer carriers, especially `acc_efi.h`, `bpi_c.h`, `cpnote_c.h`, `parser.h`, `trace_c.h` and `gdebug.h`; establish vector ABI across CCPU/C-VID, then verify actual video mode selection, frame/dirty regions, latches/planes, reset and session retirement. Selected helper symbol names or numerical constant equality do not establish equivalent rendering.
- Other MIPS/AXP/PIG/debug configurations and external generators remain **unproved**. Their applicability requires an explicit profile decision; the current project is not required to import an entire other CPU implementation merely to justify a physical path.
