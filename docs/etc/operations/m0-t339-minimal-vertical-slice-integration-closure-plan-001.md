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
- `demExitVDM` has two current original guest callers: initial NTDOS
  `sysinit1.asm::stall` after command-file failure, and COMMAND `init.asm`
  after its own started-path version rejection. The current external
  observation does not distinguish them.

## S plan

### S1 — Original `50:3D` guest-predecessor contract

Build the complete source/ABI/failure map from each original guest caller of
`SVC_DEMEXITVDM` through its immediate NTDOS/COMMAND file, version, DEM and
host-path inputs. Classify which facts can distinguish the two callers using
an existing source-shaped mechanism, and name the smallest complete owner
cohort. Do not change product, guest media, BOP routing, devices or host
drive policy.

**Closed.** The source/build identity walk records both guest callers in the
S1 ledger. The staged NTDOS/COMMAND pair matches the retained source-built
hashes and the selected 5.00 source pair, so a binary-version mismatch is not
the admitted recovery cohort. S2 owns the original initial NTDOS command
bootstrap.

### S2 — Earliest complete original owner recovery

Recover only the S1-proven earliest incomplete original binding in the initial
`ProcessConfig -> SHELL -> initial COMMAND EXEC` cohort. Prefer an already
selected original MVDM/OpenNT body and a same-shaped adapter. Preserve guest
file/EXEC ordering, original failures and mapping-manager leases. A host
namespace or boot-drive change requires its own source contract; it may not
be inferred from the terminal service alone.

### S3 — Fixed vertical-slice result

Rebuild/link the selected x86 and x64 formal product graphs. Run at most one
unchanged short-root, console-owning x86 container and record either an
original declared-command/guest boundary or the next exact source-defined
owner transfer.

## Exit

Close only with a fixed-container original guest-result boundary or one narrow
source-defined owner transfer. A host-only check, compilation or a BOP
marker alone is not guest-execution proof; x64 link closure is not an x64
guest-runtime claim.
