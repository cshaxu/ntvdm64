# T416 S1 footprint baseline and ownership ledger

## Question

What can T416 remove from the selected MVDM/OpenNT-host mirrors, their private
overlays and their adapters without counting a relocation as a reduction or
breaking the proven three-program DOS path?

## Fixed inputs and method

- Baseline commit: `47521444c2b5f212ba98d36757cc3075ebfece3a`.
- Selected MVDM provenance: the pinned union ledger
  `artifacts/documentation-archive/20260910/etc/operations/ledgers/m0-t274-s1-non-guest-mvdm-union-ledger.tsv`
  (SHA-256 `AD3C456725415AD24FB1316FFC03F627D16FE4F41C8C342164822C08BF74BC36`).
- Non-MVDM original owner: `O:\repos.external\OpenNT`; `netapi/` retains its
  registered `ds/netapi/` upstream mapping.
- Run `powershell -NoProfile -ExecutionPolicy Bypass -File
  tools/audit/Measure-T416Footprint.ps1 -RepositoryRoot (Get-Location).Path
  -OutputDirectory build/M0-T416/S1/footprint-baseline-002`.

The tool uses `git diff --no-index --ignore-cr-at-eol --numstat` only for
canonical paired files.  It retains unpaired paths and binary differences as
explicit non-line-metric rows.  Thus it cannot hide a removed file, switch
editions, or call a historical guest-only path a recovered host file.

## Baseline measurements

| Surface | Classification | Files | Added | Deleted | Distance |
| --- | --- | ---: | ---: | ---: | ---: |
| `mvdm` | text different | 156 | 3,662 | 1,033 | 4,695 |
| `mvdm` | byte equal / CR-only | 3,286 / 38 | 0 | 0 | 0 |
| `mvdm` | unpaired selected-union gap | 1,137 | — | — | — |
| `opennt-host` | text different | 17 | 719 | 13,691 | 14,410 |
| `opennt-host` | byte equal | 51 | 0 | 0 | 0 |
| `opennt-host` | unpaired | 5 | — | — | — |

The large `opennt-host` deletion count is mostly intentional, finite imported
SDK/RTL/USER slices.  It is not a T416 restoration candidate: replacing a
small composable slice with whole NT headers or CSRSS-era implementation would
increase the standalone closure and violate the source-policy stopping
boundary.

Private overlay, measured as whole physical implementation/header/fragment
lines, is MVDM **7 files / 266 lines** and OpenNT-host **1 fragment / 30
lines**.  Raw adapter inventory is **97 files / 10,219 lines** for
`adapter-mvdm-host-out`, and **36 files / 3,677 lines** for
`adapter-opennt-host`; this is deliberately not called autonomous code.  S1
must first classify original-shaped bindings separately from project policy.

## Bounded candidate ledger

| Cohort | Original owner and reached consumer | S1 disposition | Reason and receiver |
| --- | --- | --- | --- |
| WOW `-a` fallback in `cmdmisc.c` | Original COMMAND parses its worker command line; `run16_entry.c` now obtains it through original `BaseGetVdmConfigInfo`. | Retain for S2 test, then delete only if real separate/shared WOW reaches the original `-a` carrier. | The fallback is only selected when OS command line lacks ` -a `; current DOS acceptance cannot prove this WOW condition.  `base_config.c` is a finite registry binding, not a second classifier. |
| COMMAND table tracing in `cmddisp.c` | Original `CmdDispatch`; adapter termination recorder only. | Delete as one diagnostic family in S4. | The +29-line mirror hunk changes no COMMAND result.  Its public recorder declarations/definitions must be removed with callers and fixtures, not left as dead adapter code. |
| `emm_fncs.c` observation candidate | No paired original `dos/dem/emm_fncs.c` exists at the proposed primary location. | No deletion; stale planning observation closed. | The current canonical union does not select that upstream path, so it cannot substantiate a mirror restoration or a saving. |
| EOI bridge | Original `SoftPcEoi(int,int*)` caller and typed selected `ica_eoi(ULONG,LONG*,int)` leaf. | Retain in paired MVDM overlay. | A private 32-bit temporary preserves both input/output carrier semantics and avoids an unsafe incompatible pointer cast.  It is too material for an original insertion and cannot move to an adapter, which is not a legal overlay caller. |
| `localfm.c` state carrier | Original CCPU `localfm.c`; CCPU40 `ccpusas4.c` supplies the selected `Sas`. | Retain 21-line overlay subset. | Compiling the full original carrier would produce a second `Sas` state object.  The retained `Gdp`, `Cpu`, `Video` declarations preserve original owner names and no algorithm. |
| `sas_overwrite_memory` facade | Exact original `ntstubs.c` fragment; direct SIM32/EMS/`nt_mem` callers. | Retain 17-line overlay subset. | Whole `ntstubs.c` conflicts with selected CCPU40 global owners.  The body is the original leaf and calls original `c_sas_overwrite_memory`; no mapping or replacement algorithm exists. |
| C-VID binder | Original CCPU/C-VID slot metadata and `ev_glue.c` composition point. | Retain 81-line body + 10-line private header; audit generator in S3. | The historical generated binder is absent.  The current body supplies only original selected providers and explicit unavailable slots; it is not adapter policy.  S3 may reduce generated/source cost only after preserving the complete vector layout fixture. |
| Pending-worker wait query | Original BaseSrv record/critical-section owner in `srvvdm.c`; `base_service.c` consumer. | Retain 30-line private `opennt-host-overlay` fragment. | `VDM_READY` does not establish that a resident COMMAND worker is blocked in `GetNextVDMCommand`; the query prevents the proven `run16 mem` mutual wait.  No original equivalent is present. |
| `nt_sound.c` declarations | Original sound caller; selected host lock/unlock leaves. | Retain six declaration lines, review only as declaration consolidation in S4. | Including `nt_eoi.h` imports unrelated monitor-only state.  These two exact void declarations add no behavior. |
| Printer carrier | Extracted original `monitor.c` printer fragment; formal `kernel-vdm-printer.lib` consumer. | Retain 107-line MVDM overlay. | It is not a complete MVDM translation unit and therefore cannot be represented as an OpenNT-host import or moved to an adapter. |

## Adapter classification rule for later S packets

S2--S4 will count a function as autonomous only if it owns project policy or
state not present in the named original caller.  The following do **not** count
as autonomous merely because they live under an adapter: an original-shaped
ABI marshal, checked guest-memory lease, authenticated OS resource attachment,
or direct public-Win32 call whose unavailable NT4 dependency is named.  Each
deletion must remove its corresponding declarations, build selection and tests
or be reported as zero net reduction.

## Interpretation and follow-up

S1 establishes a complete disposition for every proposal cohort: two are
bounded deletion candidates (the WOW fallback conditional on its source-shaped
test and the COMMAND diagnostic family), seven are required original-owner or
standalone boundary seams, and one stale candidate is closed without action.
This does not assert that the entire 4,695/14,410 root distance is removable.

S2 first exercises the WOW command-line contract.  S3 handles the overlay
groups as coherent ownership units.  S4 removes the verified diagnostic chain
and declaration/configuration dead support.  Each later S recomputes this
same baseline and reports before-minus-after deltas; no relocation is a
reduction.
