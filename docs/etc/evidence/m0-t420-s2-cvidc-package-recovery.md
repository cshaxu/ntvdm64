# M0 T420 S2 — C-VID complete-package recovery

## Decision

The selected C-VID package is now a finite, source-shaped composition. All 39
C sources named by its original `sources` manifest are selected by the formal
x86 graph. The package needs one mirror correction: C-VID's original
`ev_glue.c` is the sole `Gdp` allocation owner. `ccpu386/localfm.c` now only
declares that external state. This removes the former linker-dependent
coalescing of two tentative definitions; it does not introduce a new C-VID
state manager.

The CPU/C-VID binder remains outside the mirror at
`src/ntvdm-exe/softpc/mvdm_cvidc_vector_binding.c`. Searches of the selected
OpenNT mirror, `opennt-src-2`, and OpenNT 4.5 found headers and generated-table
metadata (`c2cpusad.h`), but no historical binder translation unit. The current
binder is therefore the smallest permitted replacement for missing generated
product output: it assigns generated table members only and has no resource,
renderer, command, or lifecycle policy.

## Frozen selection and exclusions

| Source set | Count | Disposition |
| --- | ---: | --- |
| Original C-VID `sources` C members | 39 | All selected into `original-softpc-cvidc.lib`. |
| `ev_stubs.c` on disk, not in the manifest | 1 | Excluded: alternate empty setup providers conflict with the selected real `ev_glue.c`. |
| `sascdef.c` on disk, not in the manifest | 1 | Excluded: alternate SAS carrier; selected CCPU `sasCdef.c` owns `Sas`. |
| C-VID `vglob.c` on disk, not in the manifest | 1 | Excluded: empty alternate; selected original video accessor is `base/cpu/src/evid/vglob.c`. |

The selected C-VID manifest, original generated headers and formal graph are
the complete package boundary. No CPU30 monitor, renderer rewrite, x64/HAXM
provider, WOW, DPMI, or new guest ABI was admitted.

## State, vector and lifecycle ledger

| Item | Sole provider / phase | Proof |
| --- | --- | --- |
| `Gdp` storage and allocation | `cvidc/ev_glue.c`; `setup_global_data_ptr()` | Fresh `ntvdm.exe.map` records `_Gdp` from `original-softpc-cvidc:ev_glue.obj`; contract gate rejects any other owner. |
| `Cpu`, `Video` carrier storage | selected `ccpu386/localfm.c` | They are distinct original carrier globals; C-VID publishes members but never reallocates their carrier structs. |
| `Sas` carrier storage | selected CCPU `sasCdef.c` | The C-VID alternate `sascdef.c` is unselected. |
| early `Video` | original `support/main.c`, before `config()` | Existing early call binds only `Video`; contract gate requires it precede full setup. |
| full `Cpu`/`Sas`/`Video` | original `cvidc/ev_glue.c::setup_vga_globals()` | The existing call into the generated-table binder occurs at this original setup point. |
| repeat publication | same full bind point | Focused fixture republishes after latches/selectors and proves the quick-event restart state remains valid. |
| teardown | worker process termination | `obj.vdm/ntvdm.c` calls `host_main()` once, then worker finish ends the worker process. No selected C-VID source offers a reusable-session teardown routine. |

The fixture also proves 38 video accessor pairs across four values, all 81
`Video` slots, selector/latch preservation, a stale-provider negative control,
and concurrent scratch preservation.

## Intentional unavailable slots

The generated tables contain 154 public and 55 private slots. Twenty-six are
explicitly unavailable in this `SPC386`, non-`PIG`, non-`SPC486` production
profile, rather than silently receiving invented providers:

- Public (14): `ClearHwInt`, `InitIOS`, `DefineInb`, `DefineInw`, `DefineInd`,
  `DefineOutb`, `DefineOutw`, `DefineOutd`, `SetSTATUS`, `SetAC`, `GetAC`,
  `GetET`, `GetNE`, `GetWP`.
- Private (12): `GetSadInfoTable`, `GetCpuState`, `SetCpuState`, `InitNanoCpu`,
  `PrepareBlocksToCompile`, `SetRegConstraint`, `GrowRecPool`, `BpiCompileBPI`,
  `TrashIntelRegisters`, `FmDeleteAllStructures`, `ConstraintsFromUnivEpcPtr`,
  `ConstraintsFromUnivHandle`.

`GetCpuState` and `SetCpuState` were specifically tested as recovery
candidates. Their original `c_getset.c` bodies are guarded by
`#if defined(PIG)`; the selected `SPC386` object exports neither symbol, and
forcing the mapping caused fresh x86 link failure. Keeping both null is the
exact selected-profile behavior. The status-bit slots are `SPC486` only; I/O,
Nano and block-compiler entries belong to CPU30/monitor or PIG profiles absent
from this product.

## Change and footprint accounting

Relative to the S1 delivery `2d128cbaa`, this S adds **+5/-1 mirror lines**
in `cvidc/ev_glue.c` and **+2/-1 CCPU shared-carrier lines** in `localfm.c`.
Those seven added lines replace one ambiguous duplicate-state arrangement; no
mirror file, overlay file, or autonomous runtime implementation was added.

The generated binder, fixture, and verifier were pre-existing S1 inputs. S2
adds one focused negative assertion and extends the verifier by **+19/-2**
lines to require the final map owner and profile-null disposition. Thus the
only added non-mirror logic is audit/test logic, not product behavior.

## Verification

Fresh root: `build/M0-T420/S2/formal-x86-001`.

1. Fresh formal x86 targets `run16.exe`, `basesrv.exe`, `ntvdm.exe`, and
   `dtmgr.exe` linked successfully. The same formal graph was re-linked after
   T419's accepted DTMgr record-projection repair at `eeaebc995`; the final
   product set is therefore the T419+S2 composition, not a stale S2 binary.
2. `cvidc-vector-binding-fixture.exe` passed: `38 pairs x 4 values, 81 slots,
   latches/selectors, republish, old-provider negative control, concurrent
   scratch preservation`.
3. `node tools/audit/Verify-T411CvidcVectorContract.mjs .
   build/M0-T420/S2/formal-x86-001/ntvdm.exe.map` passed and reported the
   154/55 slot counts, direct effective-address selection, required early/full
   bind, and `gdp_owner=cvidc/ev_glue.c` with map verification.
4. `git diff --check` passed. The only emitted notices are repository
   CRLF-normalization warnings; no whitespace error was reported.

The earlier isolated-stage attempt correctly stopped before host startup with
`ERROR_BAD_PATHNAME` (`NTVDM-S3 phase=bootstrap status=000000a1`): its
53-character build root cannot satisfy the unchanged original `cmdconf.c`
64-byte short-root command contract. This is the expected `APP-DIV-014`
product-layout guard, not a C-VID failure and not a reason to relax the
original constraint.

The final four formal EXEs were deliberately copied to `O:\winnt`; source and
deployed SHA-256 values matched for every file. The real Console observer then
passed `direct-mem=0`, `mem-repeat=1`, `nested-mem=1`, `command-c=0`, and
`edit=1`. The nonzero interactive results are the already-recorded original
COMMAND convention. The observer drove the required interactive sequences,
including nested `COMMAND → MEM` and `EDIT → MEM`, and retained raw reports
under `O:\winnt\logs\m0-t420-s2-final-*`.

## Confidence and remaining boundary

Confidence is high for package selection, generated-table provenance,
single-`Gdp` ownership, intentional unavailable profile slots, early/full
publication, repeat publication and fresh x86 linkage. This does not certify
CCPU events, DPMI, host-SAS mappings, or WOW. Those are separately owned by
later package units and are not reclassified as C-VID work here.
