# T95 S6 retention-proof protocol design

## Purpose and inputs

This is a protocol for a future, separately admitted empirical object/COMDAT/
link-retention investigation. It is not a build recipe or admission. It is
derived only from the accepted minimal configuration/link contract and the
accepted CPU, non-CPU, and source-level object/link ledgers.

The question it would answer is narrow: after one owner-selected profile has
been made effective for every candidate translation unit, do the retained
sections and unresolved externals contain only the ledger's required original
Bochs mechanics, the selected CRT, and explicitly admitted system imports?

## Prerequisites before admission

All of the following must be recorded before any compile or link is allowed:

1. Owner selects exactly one profile; CPU3 and CPU5 below are alternatives,
   not defaults. The selection must name a reached caller and whether stock
   firmware is in scope.
2. A complete effective configuration is generated from a reviewed projection,
   with bytes/hash and every ledger-relevant macro fixed. No broad historical
   `config.h` may be inherited as a feature grant.
3. The exact non-product fixture entry source, source hash, architecture,
   MSVC x86 identity, CRT model, compile flags, linker flags, artifact root,
   and cleanup/preservation policy are pre-registered. The fixture may only
   construct/clean up the accepted minimal composition; it must not execute a
   CPU loop, firmware, adapter, OpenNT, CLI, or guest code.
4. Candidate inputs are declared as source families, not a claimed target
   recipe. No historical VS2008 project, full `bochs.exe` aggregate, product
   device archive, or adapter/OpenNT object is an input.
5. For each rejected edge below, the admission names the exact map/symbol
   predicate that would prove absence. If an input needs a Bochs source,
   configuration, or exception change, stop before it and obtain a new owner
   admission.

## Candidate family manifest for observation

The future investigation starts from these ledger families. They are a
measurement manifest, not an object command line.

| Family | Ledger status | Measurement purpose |
| --- | --- | --- |
| Composition/minimal SIM | required | Resolve composition globals, logging accessors, and the original SIM/parameter definitions. |
| CPU native family | required candidate | Measure the complete original handler-provider closure from `ia_opcodes.h` and `init_FetchDecodeTables`, without stubbing or omitting instruction semantics. |
| Memory / empty port family | required | Resolve no-SIM allocation/cleanup and default port arrays only. |
| PC-system/log/SIM families | required candidate | Measure constructor/logging/SIM retention separately from their UI/debug/timer members. |
| CPU loop/event | default-not-required | Prove its members are absent if the selected fixture never calls a loop. |
| CMOS, DMA, timer, VGA, full devices/plugins, GUI/config UI, ROM loader | rejected | Prove no surviving section/undefined/import reaches their owners. |
| `main.cc`, `config.cc`, adapter/OpenNT/CLI, VS2008/full aggregates | rejected | Prove they are absent from input, map, undefined list, and imports. |

## Required evidence artifacts and metrics

The future investigation must retain immutable artifacts below one fresh
analysis root:

| Artifact | Required observation |
| --- | --- |
| Configuration and manifest | Full effective config bytes/hash; selected profile label; candidate-family manifest; source hashes. |
| Per-source `/Gy` objects | Object hash, section/COMDAT inventory, defined-symbol list, undefined-symbol list, and source-to-object mapping. No object is promoted as a product library. |
| Link diagnostics | One exact linker command, map file, verbose library search/import diagnostics, full unresolved-symbol list, exit code, and final input list. `/OPT:REF` and `/INCREMENTAL:NO` must be recorded. |
| Image metadata, if link reaches an image | PE import table and final map retained for inspection only. The image is never run in this protocol. |
| Negative evidence | A machine-readable or reviewable list mapping every denied owner to zero retained sections, zero unresolved references, and zero imports. |

Metrics are counts and identities, not merely successful exit status: number of
candidate objects; defined/undefined symbols per object; retained versus
discarded COMDAT sections; map ownership of each retained external; and the
set of PE imports. A successful link with an unexplained retained external is
a failure of closure.

## Decision rules

| Result | Required proof |
| --- | --- |
| Required closure passes | Every retained external resolves to a required ledger owner, selected CRT, or pre-admitted MSVC system import; every CPU handler target has an original provider; required SIM/log/PC/memory/empty-port symbols are present. |
| Denied member is dead-stripped | Its owning source object may exist, but the map/COMDAT evidence shows no retained section references its denied symbol and no unresolved/import edge reaches its denied family. Examples: `DEV_cmos_get_reg`, `DEV_dma_raise_hlda`, `DEV_vga_mem_read/write/refresh`, GUI/configuration callbacks, plugin/device initialization, `bx_pc_system.Reset/initialize`, and CPU-loop/event-only members. |
| Closure fails: required unresolved | A required handler, SIM/parameter, logging, PC constructor, no-SIM memory, or empty-port symbol remains unresolved. Do not hand-stub, remove instruction semantics, or move it to the adapter. Return to source-owner analysis. |
| Closure fails: rejected retained | Any retained/external/import edge reaches CMOS, DMA, timer, VGA, full devices/plugins, GUI/config UI, firmware/ROM loader, `main.cc`, `config.cc`, adapter/OpenNT/CLI, or a non-admitted system library. Stop; do not cure it by enabling the feature. |
| External/toolchain failure | Retain exact command/log/environment, classify externally blocked, and do not retry with another compiler or broaden inputs without a new admission. |

## CPU-profile substitution point (no selection)

Exactly one row may be substituted into a future admission:

| If owner selects | Required protocol substitution |
| --- | --- |
| CPU3 minimal mechanics profile | Use the accepted CPU3 projection family and its source-derived `FPU=0`, `MSR=0`, `APIC=0`, `SMP=0`, x86-32 constraints. Measure the CPU3 native handler/fallback closure, including `FPU_ESC`; do not infer stock-firmware compatibility. |
| CPU5 stock-firmware-in-scope profile | Replace the CPU3 configuration facts only after a new complete projection/ledger review names CPU5's FPU/APIC/MSR and related derived constraints. Re-enumerate handler and non-CPU candidate families before compiling; historical CPU5 firmware evidence is not an object-closure waiver. |

No matrix or dual-profile link is allowed: comparing both profiles is a separate
owner admission, because they have different effective declarations and
retention surfaces.

## Stop conditions and non-claims

Stop before or during the future investigation if profile authority is absent,
configuration values are unresolved, source provenance is missing, an input
requires device/GUI/adapter semantics, a forbidden owner is retained, or the
fixture would be run. The protocol never authorizes source/config/exception
changes, device enablement, external adapter behavior, OpenNT behavior, or a
CPU semantic reduction.

Even a passing empirical closure establishes only object/link retention for a
single non-product fixture and selected profile. It makes no executable,
runtime, reset, bounded-stop, firmware, guest, DOS, WOW, adapter, or product
claim. A runtime observation would require a separate task admission and
verification plan.

## Next gate

The coordinator may review this protocol while CPU-profile choice remains
pending. The only next authority-changing action is the owner decision naming
one profile and reached caller; only then can a new controlled closure
investigation be proposed.
