# T230 S11 DEM 73-slot local-execution audit 001

## Question

Does the current Direct OpenNT DEM mirror prove local execution of each of the
73 original `demdisp.c` slots, rather than merely prove that `apfnSVC[]` has
73 non-null entries?

## Inputs

- `src/opennt/base/mvdm/dos/dem/demdisp.c` and its direct mirror
  `src/bx-vdm/bop/opennt/dem/demdisp.c`.
- The nine formal T230 Direct fixtures in the Ninja manifest.
- `src/bx-vdm/bop/shim/demdisp_shim.c`.

## Original ownership map

| Owner | Slots | Existing local fixture |
| --- | --- | --- |
| `demhndl.c` | `00,02,08,16,1e,27,47,48` | `t230_s2_demhndl_direct_import_fixture.c` |
| `demfile.c` / `demlabel.c` | `01,03,05,12,17,22,44` | `t230_s3_demfile_direct_import_fixture.c` |
| `demdir.c` | `04,06,13,18` | `t230_s4_demdir_direct_import_fixture.c` |
| `demsrch.c` / `demfcb.c` | `07,09,0a,0b,0c,20,2c,2d,2e,2f,30,31,3c` | `t230_s5_demsrch_fcb_direct_import_fixture.c` |
| `demgset.c` | `0d,0e,0f,10,14,15,19,1a,1b,1c,25,41,46` | `t230_s6_demgset_direct_import_fixture.c` |
| `demdasd.c` / `demioctl.c` | `21,29,2a` | `t230_s7_demdasd_ioctl_direct_import_fixture.c` |
| `demerror.c` / `demlock.c` | `32,33,3f` | `t230_s8_demerror_lock_direct_import_fixture.c` |
| `demmisc.c` | `11,23,34,35,36,37,38,39,3a,3b,3d,3e,45` | `t230_s9_demmisc_direct_import_fixture.c` |
| `demdisp.c` ordinary/no-op bodies | `1d,1f,24,26,28,2b,40,42,43` | `t230_s10_demdisp_direct_import_fixture.c`, plus S2/S10 ingress/session fixtures |

The map covers all 73 slots: 64 source-owner slots plus eight original
`demNotYetImplemented` slots and the original ordinary-return
`demSetV86KernelAddr` slot.

## Observation

Each owner fixture invokes its listed source-owner services through an
owner-specific shim and validates at least one success or original error/no-op
contract. The formal Ninja run `M0-T230-S10/v1-archive-r9` passed all nine
owner/table fixtures plus ingress and native-session fixtures.

This is stronger than table-only evidence, but it does **not** prove that all
64 source-owner slots pass through the one real `DemDispatch` table at local
runtime: the owner fixtures call their per-owner shim entrypoints directly.
Only the original ordinary/no-op path is currently dispatched through
`bx_ntvdm_demdisp_invoke`, and the native-session fixture reaches only one
such bounded route.

## Conclusion

S11 establishes that all 73 identities have imported source ownership and a
local owner-level witness. It does not close whole-table local integration.
S12--S14 must replace or supplement the owner fixtures with one shared
`DemDispatch`-through-shim execution harness covering every listed slot. The
result must keep original `demNotYetImplemented` and ordinary-return behavior
unchanged. S15 and S16 remain required: the native engine currently includes
and installs `bop-v1/bx_ntvdm_composition_runtime_v1`.
