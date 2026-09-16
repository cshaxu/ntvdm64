# T417 S1 private-component ownership freeze

## Question

Can the three temporary roots be removed without concealing a source-owner or
runtime-contract change, and what is the smallest safe implementation order?

## Inputs and procedure

- T416's formal x86 graph generator:
  `tools/build/New-T310OriginalSoftpcNinja.ps1`.
- The selected `src/mvdm` and `src/opennt-host` callers, plus the pinned
  `O:/repos.external/OpenNT/base/mvdm` counterparts.
- A tracked-path sweep of the three roots, include roots, object/archive/link
  rules, generated binding input, and production callers.  Counts below are
  physical lines, including comments; README and patch provenance are called
  out separately.

## Frozen inventory and disposition

| Root/body | Lines | Reached graph/caller | Original-source finding | S2--S4 disposition |
| --- | ---: | --- | --- | --- |
| `mvdm-softpc-patch/x86/prod/{PigReg_c,sas4gen,gdpvar}.h` | 1,654 | First SoftPC include root; CCPU, BIOS and host callers | x86 `prod` generator output is absent from all three pinned OpenNT trees.  Other generated carriers exist, but are not byte-equivalent selected x86 output. | S2: recover the missing x86 generator/input or put this exact selected generated output under the canonical `mvdm/softpc.new/host/inc/x86/prod` owner.  This is relocation, not a code reduction. |
| `mvdm-softpc-patch/patches/common/fmstubs.c` selected `EDL_fast_bop`, `c_sas_touch`, `c_VirtualiseInstruction` | 27 selected lines | CCPU `c_main.c` and `sascdef.c`; force-linked as the default trap provider | The selected OpenNT union declares the symbols but has no body.  The only selected behavior is immediate debugger break; no normal product run may reach it. | S2: split the three exact trap defaults into the canonical CCPU owner and retain the negative trap fixture.  Delete the unselected placeholder body. |
| same `fmstubs.c`, selected `ActivityCheckAfterTimeSlice` | 4 selected lines | `nt_unix.c` and `nt_eoi.c` | Selected original callers exist; no provider body is present.  The imported no-op is a deliberate non-policy fallback, not a scheduler. | S2: place the exact empty callback with the CCPU/host owner, after proving the selected generated table's provider contract.  Do not invent a yield or worker lifecycle policy. |
| `callconv.patch`, `ccpu-exception-scope.patch`, and unselected `fmstubs` evidence | 142+ provenance/unused lines | No production symbol is linked from the patch excerpts | Their effects already live as registered small mirror diffs or are not selected. | S2: move the small provenance excerpts to indexed evidence, delete the production-root copies and update audit scripts to consume the evidence record.  This is genuine deletion from the build tree. |
| `mvdm-overlay/.../ccpu386/localfm.c` | 20 | replaces original `localfm.c` object | Original `localfm.c` owns `Gdp`, `Cpu`, `Sas`, `Video`; selected `ccpusas4.c` already owns `Sas`, so the original whole carrier duplicates state. | S3: recover a canonical CCPU-local carrier with original globals and the one documented omitted `Sas` definition.  Validate single state ownership and C-VID layout; no adapter is appropriate. |
| `mvdm-overlay/.../ccpu386/sas_overwrite_memory.c` | 16 | direct CCPU facade object | Its function is byte-for-byte the `ntstubs.c` original wrapper, but full `ntstubs.c` conflicts with selected CCPU state owners. | S3: first attempt composition of the original wrapper in its canonical CCPU source selection; otherwise keep the exact subset as a canonical owner-local source, never an adapter. |
| `mvdm-overlay/.../cvidc/mvdm_cvidc_vector_binding.[ch]` | 89 | `support/main.c`, `cvidc/ev_glue.c`, generated `cvidc_cpu_binding.inc` | The selected CCPU/C-VID manifests omit their historical generated binder and timing provider.  Existing C-VID shims alone recurse if used as providers. | S3: retain the bounded binding semantics but relocate it as an owner-local generated artifact under canonical C-VID, with generator input and full vector fixture.  It is required current behavior, not removable autonomous code. |
| `mvdm-overlay/.../mvdm_ica_eoi_bridge.[ch]` | 28 | `nt_eoi.c::SoftPcEoi` | Original `nt_eoi.c` directly calls `ica_eoi(Adapter, Line, 0)`.  Current bridge only expresses the modern `LONG *` declaration boundary on x86. | S3: restore the original source shape in `nt_eoi.c` using one local, fixed-width ABI conversion, then delete the bridge.  This should be a genuine overlay/body deletion. |
| `mvdm-overlay/v86/monitor/i386/monitor_printer.c` | 106 | `nt_lpt.c`, `kernel-vdm-printer.lib` | Exact three-function subset of upstream `v86/monitor/i386/monitor.c`; it operates on the finite worker `VDM_TIB`.  Full monitor compilation requires unavailable kernel monitor machinery. | S3: relocate the exact original subset under canonical `mvdm/v86/monitor/i386` with its finite context binding.  It cannot move to an adapter: it is original kernel-VDM semantics, not an OS ABI call. |
| `opennt-host-overlay/.../dos_worker_wait.inc` | 29 | textually included only by `srvvdm.c::BaseSrvDOSWorkerWaitPending`; called by broker service reservation | No upstream equivalent: it distinguishes a merely ready resident COMMAND record from a worker actually waiting in `GetNextVDMCommand`.  Record lookup and lock remain original BaseSrv state. | S4: inline the same 29-line body in `opennt-host/base/win32/server/srvvdm.c`, retain existing pending/resident and disconnect tests.  This removes the overlay directory but is zero semantic and near-zero line reduction. |

## Build edges

The formal graph has six production edges that must change together: its
SoftPC include root for the three generated headers; two `fmstubs` selected
objects; CCPU `localfm`/SAS objects; C-VID generated-binding object; host EOI
object; monitor-printer archive; and the BaseSrv textual include.  The older
`New-T313CcpuLifecycleNinja.ps1` also names the patch root, but it is a
historical focused generator and must be either updated or retired as part of
S2; it is not a product route.

## Implementation groups and acceptance

1. **S2, patch-root exit.**  Generated x86 CCPU carrier recovery; selected
   trap/no-op provider separation; provenance exit.  Build the formal x86
   graph, run CCPU default-provider negative tests, and prove no live path
   names `mvdm-softpc-patch`.
2. **S3, MVDM owner-local recovery.**  First complete the CCPU/SAS/localfm
   single-owner composition, then C-VID generated binding, EOI restoration
   and printer relocation.  Run full C-VID vector fixture, CCPU exceptions,
   printer fixture and COMMAND/MEM/EDIT regression after each coherent group.
3. **S4, OpenNT BaseSrv overlay exit.**  Inline the bounded record query in
   `srvvdm.c`; run resident-worker, pending-command and broker-death tests.
4. **S5, non-mutating component relationship report.**  Audit `app`,
   `broker`, `session` and each adapter's actual public boundary.  Publish a
   separate proposal only; do not use this cleanup to reorganize those
   components silently.

## Accounting baseline

There are 12 production C/H/INC bodies across the roots: 1,654 generated
declaration lines; 294 MVDM-overlay lines (the C/H bodies; the existing T416
metric counted 266 C/H physical lines under its narrower cohort); and 29
OpenNT-host-overlay lines.  Patch evidence and READMEs are separately
non-production.  S1 identifies one direct semantic deletion candidate (EOI
bridge), production-root/provenance deletion, and otherwise relocation or
required-source recovery.  It therefore does **not** promise a misleading
large line reduction merely by renaming files.

## Conclusion

The requested zero-private-root topology is feasible.  It is not evidence
that all current private bodies are needless: C-VID binding and the finite
printer carrier have reached, source-shaped responsibilities; their owner is
`mvdm`, not an adapter.  The plan preserves that distinction, deletes only
actual redundancy/evidence, and moves all remaining behavior to canonical
owners with focused regression gates.
