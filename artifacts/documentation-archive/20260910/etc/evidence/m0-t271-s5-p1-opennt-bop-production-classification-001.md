# M0 T271 S5 P1 — `opennt-bop` production classification

## Scope

Inspected every live source under `src/opennt-bop` and
`src/opennt-bop-overlay`, plus its formal-manifest inputs and include
consumers.

## Classified source

- **Original/provider mirror families:** `command/`, `dem/`, `xms/`, selected
  `softpc/`, and `vdmredir/` contain the admitted OpenNT functions, source
  subsets and their registered include modifications. Existing private bodies
  are confined to the corresponding `opennt-bop-overlay/<family>/` path.
- **Private overlays:** 13 body files are present: six COMMAND, one DEM,
  three SoftPC, two VDMREDIR and one XMS. Each has a matching mirror include
  boundary and no adapter/app/session production include consumer.
- **Non-mirror ingress:** 22 `ingress/*.c` inputs remain project-authored
  transport/route/session code. They are not silently acceptable as mirrors.
  Their exact owner disposition is:
  - `opennt_bop_route` and the DEM/COMMAND/XMS generic/native bridges must be
    replaced by their already-admitted source-shaped `nt_bop.c` entry paths;
    they may not move to `adapter-bop`, which is selector-blind.
  - `dem_runtime_session`/`dem_direct_session` and app-created package bind
    state are final composition candidates, but must move only after their
    provider calls no longer retain BOP vocabulary.
  - `softpc_*`, `bios_selector_map`, and `startup_machine_interrupt` are
    OpenNT SoftPC/firmware owner-package work, not generic BOP ingress.
  - `redir_*` belongs to the deferred Redirector owner package and must not be
    moved to a generic adapter or reported source-complete.
  - `config_complete`, `spckbd_handoff`, and `top_level_nosupport` are
    separate top-level/machine policy paths; their source owner is not the
    DEM/COMMAND/XMS dispatcher packages.
- **Header debt:** XMS's current compatibility header and DEM/COMMAND
  composition headers are reached compilation closures, not original units.
  They must be either reduced to original headers plus same-shaped adapters or
  made mirror-private overlays; tests must not include overlays directly.

## Decision

S5 will not perform a name-only relocation of selectors into `adapter-bop` or
app. The next implementation parts remove only a bridge when its original
source dispatcher has a complete replacement. Deferred owner packages retain
an explicit source/failure disposition until their own admission.
