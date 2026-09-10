# M0 T271 S5 P7 — family compatibility and private-overlay ledger

## Question

After removal of `opennt-bop/ingress`, does every remaining live BOP input
meet the mirror-component rule, and may S5 therefore close?

## Inputs

- The `opennt-bop` formal-manifest source list and all 115 production files
  below `src/opennt-bop`.
- All 14 files below `src/opennt-bop-overlay` (13 source bodies plus its
  private-root README).
- The local BOP divergence register and the pinned OpenNT comparison paths
  cited by that register.
- A recursive consumer scan for `opennt-bop-overlay`.

## Procedure

1. Grouped every remaining non-obvious mirror input by owner family and
   compared its role with the applicable registered source path.
2. Counted each private-overlay include boundary and searched all non-mirror
   production components for an external include.
3. Compared all local `DIVERGENCE` markers with the BOP README register.
4. Distinguished an admitted source-facing compilation facade from a
   project-authored provider, route, observation, or session implementation.

## Observations

- The removed `opennt-bop/ingress` path has no live source/build consumer.
  Its textual occurrences are retained historical evidence only.
- There are 13 private source bodies.  Each has one matching mirror include
  boundary: six COMMAND (`BOP-DIV-032`, `102`--`106`), one DEM (`090`), three
  SoftPC (`023`, `080`), two VDMREDIR (`056`, `091`), and one XMS (`107`).
  No `app`, `session`, or adapter production file includes that private root.
- COMMAND's remaining compatibility surface is source-facing: the original
  `cmd*.c`/`nt_bop_command.c` bodies retain their names and service order;
  `opennt_command_*_composition` boundaries isolate the registered bounded
  call, configuration, environment, binary-address and PIF-layout seams.
  The source-derived bodies are already private overlays, rather than a new
  dispatcher.
- DEM's remaining `opennt_dem_*` units are family-local compilation and
  source-call facades: the scoped CCPU/SAS facade, VDMREDIR import binding,
  OEM/file/search state and DASD/IOCTL product seams.  They are required by
  directly imported DEM provider bodies and do not select a BOP service or
  route a copied CPU frame.  Their current dispositions are registered by
  `BOP-DIV-038`--`055`, `066`, `087`, `089`, `090`, `092`, and `094`--`097`.
- XMS's `opennt_xms_compat.h` and its sole composition boundary are the
  existing source-facing include/lifecycle closure for the original table and
  service files; they are registered by `BOP-DIV-024`--`031` and `107`.
- VDMREDIR's `vrdisp_compat.h`/`vrmslot_compat.h` retain source-shaped record
  and CCPU/SAS calls.  They are not an app route and are registered by
  `BOP-DIV-056`--`065` and `091`.
- DPMI's three `*_source_shim`/`*_session_shim` pairs remain explicit,
  source-shaped deferred owner-package material (`BOP-DIV-067`--`079`).
  This packet neither enables `53:xx` nor claims these shims are a recovered
  DPMI provider.
- The scan found a material register-quality gap: several old task-labelled
  comments (`DIVERGENCE (T236...)`, `T237...`, `T241...`, `T243...`, and
  `T245...`) remain in imported COMMAND, XMS, SoftPC and overlay bodies.
  They describe real departures but are not locally indexed by a stable
  `BOP-DIV-*` identifier.  This violates the mirror review standard even
  though the underlying component boundary is known.
- Four source-fragment SoftPC filenames (`mouse_install1_headless.c`,
  `printer_openclose.c`, `spckbd_handoff.c`, `update_kbd_int15.c`) and
  `opennt_int06_provider.{c,h}` are source-derived fragments, not exact
  OpenNT filenames.  They retain individual historic source references, but
  they remain in the mirror root rather than behind a matching original-name
  mirror boundary.  That is not admissible as final S5 closure.

## Interpretation and confidence

The ingress removal and overlay privacy conditions are satisfied with high
confidence.  The remaining owner-family facades are not a hidden generic
route: they preserve named source call shapes needed to compile imported
provider bodies.  However, S5 cannot yet close.  Stable per-site divergence
registration and the source-fragment placement rule are both mandatory, not
cosmetic, requirements of the mirror-component standard.

## Follow-up

1. Replace each task-labelled divergence marker with a registered stable
   `BOP-DIV-*` marker and add any missing README rows.
2. Move each non-original SoftPC fragment body to `opennt-bop-overlay` and
   leave only a minimal original-name mirror boundary in `opennt-bop`, or
   transfer the body to its admitted non-mirror owner if a source-shaped
   boundary cannot be stated.
3. Repeat this ledger/consumer scan and then run the focused formal family
   matrix before considering S5 closure.
