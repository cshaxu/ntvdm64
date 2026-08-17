# M0 T225 S19 DEM Full-Package Route Reconciliation 001

## Question

What must change for the full 73-slot OpenNT DEM package to become the one
Direct/Readonly host-service owner, rather than a collection of historical
endpoint shims and fixture-only profile routes?

## Source and current-route basis

`src/opennt/base/mvdm/dos/dem/demdisp.c` is the sole callable-table authority:
`apfnSVC[0..72]` corresponds to `50:00..48`; `50:49` is the non-callable
sentinel. The package source is the original DEM directory, grouped by the
existing [T199 S37 matrix](t199-s37-dem-package-provider-matrix-001.md):
namespace/handle, FCB/search, GSET/state, raw-media/IOCTL, error/lock,
misc/lifecycle, original no-op, and the separately registered FastRead
compatibility exception.

The current generated [T221 route audit](t221-s3-t200-current-bound-route-audit-001.json)
contains exactly 73 DEM records. Its static reconciliation reports:

| T200 relation | Slots | Meaning for S19 |
| --- | ---: | --- |
| `changed` | 53 | Later package/session or compatibility seam exists; it must be retained, migrated, replaced, or explicitly rejected by the final package owner. |
| `confirmed` | 19 | The old T200 route still matches the source-visible current route; this is not automatically a compatibility claim. |
| `confirmed-with-gap` | 1 (`50:3C`) | Historical PDB ownership needed a source-order review; P4 corrects it to per-PSP search cleanup only, with Direct token release retained at guest-selected `50:02`. |

The same audit assigns 31 slots `migrate-to-shared-profile-abi`, 11 slots
`already-consumes-shared-profile-abi; remove remaining snapshot/synthetic
fallbacks`, one PDB-owner-dependent migration, 18 not-applicable machine/VDD
or original-terminal dispositions, and 12 owner-admission reviews. This
covers every callable slot without claiming all services work at runtime.

## Reconciliation findings

1. **A typed all-slot route is not a whole-provider result.**
   `tests/bx-vdm/bx_ntvdm_dem_package_family_v1_test.c` deliberately proves a
   typed session route for every `50:00..48` identity. Its own header says it
   is not complete historical-provider evidence. It currently initializes an
   Overlay mutation profile to exercise extension mechanics, so it cannot be
   the Direct/Readonly product acceptance test required by S19.

2. **The durable ownership boundary already exists.**
   `bx_ntvdm_bop_ingress_v1` -> provider registry ->
   `bx_ntvdm_dem_package_session_v1` is the only acceptable normal route.
   The current source still contains legacy exact-window paths in
   `bx_ntvdm_adapter_runtime.c` for bootstrap/GSET/COMMAND-adjacent work.
   S19 may preserve them temporarily only behind their package session and
   must remove their bypass role after an equivalent package regression.

3. **Direct and Readonly are the product modes.**
   Direct uses permitted normal Win32 host capability through the original
   DEM owner; Readonly returns the original/source-derived mutation failure.
   Overlay has a retained ABI and existing research code but is unselected;
   it must neither become a fixture's default product proof nor fall through
   to Direct. Virtual is retired and may not receive new behavior.

4. **The highest-risk gap is process ownership, not a trace frontier.**
   Original `demTerminatePDB` (`50:3C`) pairs with `demEntryDosApp` (`50:36`)
   but does not derive file/search ownership from the latter notification.
   The current session lifecycle releases only known private resources. The
   source-order review is now complete: guest DOS retains JFT/SFT ownership and
   emits `50:02` after `50:3C`; the adapter must not infer a second cleanup
   authority from a BOP number or host handle.

## Package execution order

The following stages remain one DEM package delivery; none is an independent
trace-selected BOP task.

1. **Canonical regression and profile gate.** Parameterize the existing
   all-73 route fixture for Direct and Readonly, assert every original
   no-op/failure terminal, and retain only a negative ABI-extension check for
   Overlay. Audit every adapter-runtime legacy branch against the session.
2. **Namespace/handle and FCB/search convergence.** Move the 31
   shared-profile migration routes through the whole-provider/session path;
   preserve DOS layouts, DTA/FCB continuation, share, mutation and failure
   behavior. Direct may perform the owner-approved host operations; Readonly
   must refuse all mutation uniformly.
3. **GSET/state and error/lock convergence.** Replace snapshot/synthetic
   fallbacks only where original `demgset.c`/`demerror.c` permits a contained
   capability; maintain source-shaped clock, drive, current-directory, DPB,
   hard-error and lock outcomes.
4. **Raw-media, misc/lifecycle and PDB closure.** Keep device/firmware work
   outside bx-vdm; give every raw-media/IOCTL/misc entry its original failure,
   no-op or explicit deferred rule. P4 proves that `50:3C` is deliberately not
   a Direct token-cleanup path; normal guest-selected `50:02` owns it.
5. **Single all-family regression, then one native validation.** The native
   run is only a post-package integration check. It must never create a new
   singleton implementation item.

## Source-recovery requirements

Each retained or replaced route records the mandatory order:

1. independently composable original OpenNT routine;
2. same routine behind the smallest CLI capability/build shim;
3. registered external-code intrusion only for an essential machine boundary;
4. new behavior only when the preceding route is unavailable.

`DemDispatch` itself cannot be linked as a shortcut because it is tied to
CCPU/SAS register and memory access. A source-derived DEM rehost therefore
belongs inside the package session, must preserve handler ordering/layout and
failure behavior, and must name the uncomposable historical dependency.

## First implementation admission

Stage 1 is immediately admissible: it changes no guest, Bochs, host mutation
policy, or BOP provider semantics. It establishes Direct/Readonly all-family
regression and a complete legacy-route inventory. Stages 2--4 require their
own source/ABI decisions within this same S19 packet; an unresolved subfamily
is recorded as deferred while independent DEM subfamilies continue.