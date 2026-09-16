# Retire `mvdm-softpc-patch`

## Objective

Eliminate `src/mvdm-softpc-patch` as a production component without losing any
currently selected Win32/x86 CCPU40 behavior. Every remaining source carrier
must take its final owner from the original SoftPC caller:

- a directly composable declaration or same-shaped minimal correction belongs
  in `src/mvdm` at its original-relative owner path;
- a material non-original provider that cannot fit the mirror insertion rules
  belongs in the matching private `src/mvdm-overlay` path; and
- provenance-only patch excerpts leave the production source tree for indexed
  evidence, retaining source path, hash, selected hunk and removal rationale.

No code is to be moved into an adapter merely to delete the directory.

## Bounded inventory

The current formal graph still uses these inputs from the retiring component:

| Carrier | Current role | Required disposition before deletion |
| --- | --- | --- |
| `x86/prod/PigReg_c.h`, `sas4gen.h`, `gdpvar.h` | Missing x86 generated CCPU/SAS/GDP declaration carrier. | Locate the original include-owner path and recover a byte-identical selected input if available; otherwise retain the smallest declaration-only source-shaped carrier under `mvdm`, with a registered divergence. |
| `patches/common/fmstubs.c` under `MVDM_SOFTPC_PATCH_CCPU_VECTOR_DEFAULTS_ONLY` | Selected CCPU vector-default failure bodies. | Audit each symbol and reachability. Delete unreachable defaults; place each required non-original executable body in `mvdm-overlay` beside the original caller, preserving its exact failure contract. |
| `patches/common/fmstubs.c` under `MVDM_SOFTPC_PATCH_ACTIVITY_CHECK_ONLY` | Reached `ActivityCheckAfterTimeSlice` no-op provider. | First locate an original provider. If unavailable, put only the source-shaped `VOID` provider in `mvdm-overlay`, retaining the established no-scheduler/no-yield behavior and test it on the COMMAND idle path. |
| `patches/minnt/callconv.patch`, `patches/common/ccpu-exception-scope.patch` | Provenance excerpts for corrections already applied inside `mvdm`. | Move the necessary attribution/hash/hunk evidence into an indexed evidence record; do not retain a production patch component for text that is not compiled. |

`MVDM-SOFTPC-PATCH-002` evidence-only compilation must be separately checked:
it is not a product dependency, so it must either become an indexed evidence
fixture outside production selection or be removed after preserving its exact
source/hash record.

## Planned S packets

### S1 — ownership and graph freeze

Record every formal include, object, library and link edge from the component;
map each exported symbol to its original caller, original source availability,
call reachability and current tests. Prove the final target path of each
declaration carrier before moving it.

Exit: no unclassified patch file, symbol or graph edge; fixed before metrics.

### S2 — declaration and mirror-correction recovery

Recover/move the generated declaration carriers into their proper `mvdm`
owners and retain the existing `WINAPI` and exception-scope corrections as
minimal registered mirror diffs. Remove obsolete production includes and prove
the formal x86 graph compiles without the old include root.

Exit: all declaration inputs are owned by `mvdm`; no patch-directory include
path remains.

### S3 — executable provider disposal

For vector defaults and activity checking, prefer direct original provider
recovery; otherwise put only a necessary material provider in `mvdm-overlay`.
Do not transfer a whole `fmstubs.c` body. Delete every unselected placeholder
and the evidence-only product archive edge.

Exit: each retained provider has original caller, source-first ladder,
reachability evidence and focused positive/negative tests.

### S4 — deletion and closure

Remove the component directory and all build/document architecture references
that describe it as a component. Preserve final provenance in indexed evidence,
regenerate formal x86 build, run affected CCPU fixture plus COMMAND/MEM/EDIT
regression, and report net mirror/overlay additions and deleted component lines
separately.

## Acceptance

Closure requires zero tracked paths under `src/mvdm-softpc-patch`, zero formal
include/object/library/link references to it, a clean x86 CCPU40 formal build,
and evidence that no retained provider has been rehomed into an adapter. The
final report distinguishes original recovery, minimal mirror divergence,
private overlay addition, provenance-only archival, and true deleted code.
