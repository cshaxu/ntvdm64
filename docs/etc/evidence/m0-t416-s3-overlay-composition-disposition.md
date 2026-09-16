# T416 S3 — private overlay composition disposition

## Question

Can any selected private MVDM/OpenNT-host overlay body be deleted, returned to
an original complete translation unit, or moved to an adapter without changing
the established standalone x86 ownership and failure contracts?

## Inputs and method

- Active source revision: `c59b1008a`, plus the T414 canonical `src/mvdm`
  re-root.
- Pinned `O:\repos.external\OpenNT` and `OpenNT-4.5` MVDM sources, the
  selected formal generator, direct callers and focused fixture sources.
- Every tracked overlay file was compared with its complete original source
  where one exists.  The audit checked definition/caller/build edges with
  `rg`, generated the C-VID table from `c2cpusad.h`, and ran
  `node tools/audit/Verify-T411CvidcVectorContract.mjs` using Node 22.

The current physical implementation inventory is seven MVDM source/header
files / **221 lines** and one OpenNT-host fragment / **27 lines** (README files
excluded).  The C-VID generated include is a disposable build product: **213
lines**, derived from the original 154 public and 55 private slot names.

## Source-first disposition

| Carrier | Original owner and direct consumer | Result | Why it cannot be removed or moved |
| --- | --- | --- | --- |
| `ccpu386/localfm.c` (19) | Original `localfm.c`; CCPU/C-VID vector globals. | Retain private subset. | The original complete file defines `Gdp`, `Cpu`, `Sas`, and `Video`; selected `ccpusas4.c` already defines the authoritative `Sas`.  Linking both creates two SAS state objects.  The subset retains only the other three original names and no algorithm.  Putting its material include/layout selection in the mirror would exceed the minimal mirror-diff rule. |
| `ccpu386/sas_overwrite_memory.c` (14) | Exact `ntstubs.c` wrapper; SIM32, `nt_mem`, XMS and EMS reach the public function. | Retain private original subset. | The full original `ntstubs.c` also defines incompatible alternate CPU/GDP/Video/FPU state.  The retained wrapper is the exact one-call original leaf to `c_sas_overwrite_memory`, has no mapping/state/policy, and has no legal adapter caller. |
| C-VID binder/header (77) and generated table | Original C-VID metadata and `ev_glue.c::setup_vga_globals`; early video bind is used before `config()`. | Retain generated-binding seam. | The historical generated assembly/binding unit is absent from both pinned trees.  The source provides original slots only, explicitly leaves unavailable slots null, and publishes one `Cpu`/`Sas`/`Video` relationship.  An adapter cannot own CCPU vector state.  The checked contract confirms 154/55 slots, original direct effective-address route, the early bind and no autonomous accessor rebinding. |
| EOI bridge/header (21) | Original `nt_eoi.c::SoftPcEoi(int,int*)`; selected PIC `ica_eoi(ULONG,LONG*,int)`. | Retain typed ABI seam. | It preserves the original input/output `int*` carrier using a local `LONG`; replacing it with a cast makes the pointer contract unchecked.  Moving it to an adapter violates the mirror-only overlay rule. |
| printer carrier (90) | Exact functions from original `v86/monitor/i386/monitor.c`; `nt_lpt.c` calls `MonitorInitializePrinterInfo`. | Retain extracted original subset. | The source is a finite kernel-VDM semantic carrier, not an entire composable monitor translation unit.  It operates only on the original-layout bound `VDM_TIB`; the policy explicitly places such an MVDM-owned extraction in `mvdm-overlay`, never an adapter or `opennt-host`. |
| `dos_worker_wait.inc` (27) | Original BaseSrv record lock/event state in `srvvdm.c`; `base_service.c` needs a reuse decision. | Retain mirror-private fragment. | `VDM_READY` does not show that a resident COMMAND is waiting in `GetNextVDMCommand`.  The query prevents the demonstrated `run16 mem` mutual wait.  No original equivalent exists; moving lookup/locking policy to an adapter would make it own BaseSrv state. |

## Verification

1. `Verify-T411CvidcVectorContract.mjs` was mechanically updated from the
   retired T414 path `src/mvdm-host` to canonical `src/mvdm`, then passed with
   154 public slots, 55 private slots, direct CCPU effective addressing,
   early video binding, setup binding and explicit null `ClearHwInt`.
2. The prior fresh S2 formal x86 graph at
   `build/M0-T416/S2/formal-x86-002` remains the applicable full composition
   proof: all 478 sequential commands passed with these same source inputs.
3. A direct Ninja fixture rebuild was attempted without the MSVC environment
   and correctly failed to locate `cl.exe`; a subsequent VS-environment Ninja
   invocation did not return and was terminated.  This is an unavailable
   rerun, not a passing fixture result.  No production source changed during
   S3, so the prior formal proof is retained rather than overstated.

## Conclusion and follow-up

This S finds **zero safe overlay deletions or relocations**.  That is a useful
negative result: deleting or moving any listed carrier would either duplicate
selected original state, introduce unchecked ABI behavior, recreate missing
generator output, or transfer original MVDM/BaseSrv policy into an adapter.

The only source reduction made by S3 is the **mechanical 9-path stale test
repair** in `Verify-T411CvidcVectorContract.mjs`; it restores its ability to
validate the canonical tree and adds no runtime code.  The user-modified
candidate proposal that demands zero overlay roots is not included in this
delivery: its premise conflicts with this source/caller/build evidence and
needs revision before any future admission.
