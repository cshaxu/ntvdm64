# M0 T339 — Minimal vertical-slice integration closure

## Purpose

Advance the selected original CPU40/MVDM vertical slice from T338's immutable
short-root container. The current observed owner is original
`50:3D -> demExitVDM`; this task must identify its exact guest predecessor
before selecting any recovery. It is not a BOP backlog, an FDC/INT15 task,
a guest-loader rewrite or a new DOS-drive policy.

## Current predecessor

- T337 established one executable-relative `mvdm` package root and preserved
  original `config.nt` processing.
- T338 proved the original `cmdconf.c` 64-byte system-root capacity condition.
  Its byte-identical 38-character stage crosses the configuration cohort and
  reaches `50:3D -> demExitVDM`.
- `demExitVDM` has two direct original guest callsites: initial NTDOS
  `sysinit1.asm::stall`, and COMMAND `init.asm` after its own started-path
  version rejection. `stall` also has original file/EXEC *and* `mem_err`
  predecessors. The current external observation does not distinguish them.

## S plan

### S1 — Original `50:3D` guest-predecessor contract

Build the complete source/ABI/failure map from each original guest caller of
`SVC_DEMEXITVDM` through its immediate NTDOS/COMMAND file, version, memory,
DEM and host-path inputs. Classify which facts can distinguish the callsites
and `stall` predecessor families using an existing source-shaped mechanism,
and name the smallest complete owner cohort. Do not change product, guest
media, BOP routing, devices or host drive policy.

**Closed.** The source/build identity walk rejects a mixed NTDOS/COMMAND
version pair and classifies the additional original `mem_err -> stall` family,
including its BIOS BOP 12h and selected configuration-device inputs. S2 must
obtain a passive source-shaped discriminator before selecting a cohort.

### S2 — Earliest complete original owner recovery

Obtain one passive, source-shaped callsite/family discriminator from the
unchanged short-root container, then select only the earliest S1-proven
incomplete original binding. Prefer an already selected original MVDM/OpenNT
body and a same-shaped adapter. Preserve guest ordering, original failures and
mapping-manager leases. A host namespace or boot-drive change requires its own
source contract; it may not be inferred from the terminal service alone.

**Current observation limit.** The one allowed fixed-container run recorded
original BOP ingress through `54:05` and then timed out; it did not reach
`50:3D`, so its copied register discriminator was not emitted. The resulting
[S2 evidence](../evidence/m0-t339-s2-passive-terminal-discriminator-001.md)
selects no `demExitVDM` recovery cohort. Static source review establishes the
post-`54:05` continuation as original resident DOS-BIOS device initialization:
`charinit -> DEVIOCALL2 -> strategy/interrupt` runs before the next
`SVC_DEMGETDPBLIST` BOP. That is a complete machine/firmware owner transfer,
not a `demExitVDM` inference. **Closed:** the transfer is recorded in the
[T339 closure](../../history/m0-t339-minimal-vertical-slice-integration-closure-20260831.md).

### S3 — Fixed vertical-slice result

Not admitted. S2 selected no recovery change and already established the next
exact source-defined owner transfer. A second fixed-container run would not
meet the one-observation rule and is expressly excluded.

## Exit

Close only with a fixed-container original guest-result boundary or one narrow
source-defined owner transfer. A host-only check, compilation or a BOP
marker alone is not guest-execution proof; x64 link closure is not an x64
guest-runtime claim.
