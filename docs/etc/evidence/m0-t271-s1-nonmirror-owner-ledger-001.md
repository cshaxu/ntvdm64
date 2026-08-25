# M0 T271 S1 — non-mirror BOP owner ledger

## Purpose

This is the S1 source-of-truth disposition for project-authored production
code discovered below `src/opennt-bop`.  It does not call such code an OpenNT
mirror merely because it is currently linked into `opennt-bop`.

`adapter-bop` is the mechanical entrance into the BOP plane: it copies and
validates the event/result ABI, then enters `opennt-bop`.  It does not
interpret selector/service bytes.  OpenNT selector and provider ownership is
therefore retained by the original dispatcher recovery work, not moved to an
adapter.

## Completed extraction in this packet

- `observation/{bop_sequence,command_bootstrap,command_current_dir,dem_open,`
  `generic_ud_sequence,ntdos_exec,ntdos_exec_entry}.{c,h}` moved to
  `src/app/observation/`.  They are default-off CLI diagnostics; the app
  observes an already-returned typed outcome and cannot alter dispatch.
- `observation/demfile_create_observation.{c,h}` moved to
  `src/adapter-win32/facade/`, beside the OEM file facade that emits it.
- `ingress/dem_startup_composition.{c,h}` moved to
  `app/startup_composition.{c,h}`.  The app now supplies its admitted launch
  declaration to the temporary COMMAND session through an explicit argument,
  so `opennt-bop` no longer imports `app` configuration/image code.
- `app/bop_composition.c` now wraps the route after dispatch. Consequently,
  production source in `opennt-bop` has no `app/` include.

The moved observation fixtures are current production-boundary fixtures, not
historical evidence.  They retain their `tests/component-integration` owner.

## Retained temporary boundary inventory

Each following file is project-authored and remains temporarily only so that
the existing product route keeps its current behavior until the named original
package replaces it.  Retention is not a mirror conformance claim.

### Original dispatcher replacement boundary

- `ingress/opennt_bop_route.{c,h}` — temporary selector route.  Replacement
  source: `base/mvdm/softpc.new/host/src/nt_bop.c`; the owner tables and
  service/IP ordering are recovered in T271 S2--S4.  It must not acquire more
  provider behavior before then.

### DEM boundary, to be replaced by T271 S2

- `ingress/dem_generic_ud_bridge.{c,h}` — current copied-frame entry bridge;
  replace with `MS_bop_0` → original `DemDispatch` sequencing.
- `ingress/dem_ingress_shim.{c,h}` — staged DEM frame bridge; replace only
  through a same-shaped CCPU/SAS facade demanded by the imported DEM body.
- `ingress/dem_direct_session.{c,h}`, `dem_native_session.{c,h}`, and
  `dem_runtime_session.{c,h}` — project session/configuration composition;
  replace with original `dem.c`, `demdata.c`, `demdisp.c` and reached provider
  global/session ordering.

### COMMAND boundary, to be replaced by T271 S3

- `ingress/command_generic_ud_bridge.{c,h}` — current copied-frame entry;
  replace with `MS_bop_4` and the original COMMAND dispatcher call ordering.
- `ingress/command_native_session.{c,h}` and
  `command_runtime_session.{c,h}` — project child/session composition;
  replace with original `cmd.c`, `cmddata.c`, `cmddisp.c` and reached provider
  state.  Any uncomposable old host call remains a same-shaped
  adapter/session seam, not a replacement dispatcher.

### XMS boundary, to be replaced by T271 S4

- `ingress/xms_generic_ud_bridge.{c,h}` — current copied-frame entry;
  replace with `MS_bop_2` and original XMS dispatch ordering.
- `ingress/xms_native_session.{c,h}` and `xms_runtime_session.{c,h}` —
  current staged-memory/session composition; replace with original `xms.c`,
  `xmsdisp.c`, `xmsmemr.c` and only their required same-shaped mechanical
  facades.

### Deferred original owner packages

- `ingress/redir_generic_ud_bridge.{c,h}` and
  `redir_native_session.{c,h}` — project VDMREDIR route.  Original source
  boundary: `nt_bop.c` `MS_bop_7` plus VDMREDIR provider sources.  Redirector
  recovery is explicitly outside T271; these files are frozen pending its
  separately admitted owner package and may not grow new service semantics.
- `ingress/{startup_machine_interrupt,config_complete_generic_ud_bridge,`
  `softpc_emm_unavailable_generic_ud_bridge,softpc_machine_interrupt_generic_ud_bridge,`
  `softpc_memory_size_generic_ud_bridge,softpc_mouse_vector_generic_ud_bridge,`
  `softpc_printer_openclose_generic_ud_bridge,softpc_tape_io_generic_ud_bridge,`
  `spckbd_handoff_generic_ud_bridge}.{c,h}` — selector-specific bridges for
  original SoftPC/BIOS entries.  Their source operation bodies are recorded
  under `softpc/`; their machine mechanics require the future Bochs/SoftPC
  owner package.  They are not `adapter-bop` candidates because they decode
  selector/service data.
- `ingress/top_level_nosupport_generic_ud_bridge.{c,h}` — current BOP 59
  bridge.  Original boundary: `nt_bop.c` `MS_bop_9` and
  `host_direct_access_error`; it remains pending the separately tracked
  top-level host-error package.
- `ingress/bios_selector_map.{c,h}` — current source-derived SoftPC selector
  mapping data.  It remains frozen with the above machine bridges, pending
  replacement by the original machine/provider recovery rather than movement
  into a selector-blind adapter.

## Non-ingress composition and seam inventory

The following source-derived support inputs were included in the S1 sweep.
They remain adjacent to their imported source only where an original body
currently includes them. Their named recovery packet, not their current
directory, determines ownership completion.

- COMMAND S3: `command/opennt_command_binary_composition.c`,
  `opennt_command_config_composition.c`,
  `opennt_command_environment_composition.c`,
  `opennt_command_composition.{c,h}`, and
  `opennt_pif_composition.{c,h}`. These are current product-shell bindings
  around original COMMAND bodies. S3 must replace each with the smallest
  same-shaped `adapter-win32`, `adapter-softpc`, `opennt-host` or session
  seam proved necessary by the original caller.
- DEM S2: `dem/opennt_dem_{dispatch,demdasd_ioctl,demdir,demerror_lock,`
  `demfile,demgset,demmisc,demsrch_fcb}_composition.{c,h}` (where present),
  plus `opennt_dem_{common,demdasd_ioctl,demfile,demgset,demmisc,`
  `demsrch_fcb}_compat.h`. They are the existing direct-import boundaries for
  original DEM providers. S2 must either minimize them to a documented
  same-shaped external seam or delete them when the original source closure
  becomes directly composable.
- XMS S4: `xms/opennt_xms_compat.h` and `xms/opennt_xms_composition.c`.
  They are the temporary header/machine binding for original XMS sources;
  S4 owns their direct-source or minimal-facade disposition.
- DPMI deferred package: `dpmi/dpmi_{descriptor_source,startup_source,`
  `startup_session}_shim.{c,h}`. These are source-derived and frozen; this
  T does not claim them as an OpenNT mirror or modify their behavior.
- Machine/SoftPC deferred package:
  `softpc/opennt_config_complete_composition.c`,
  `opennt_int06_provider.{c,h}`, and the private overlay bodies
  `opennt-bop-overlay/softpc/{emm_fncs,mem_size,tape_io}.c`. Their original
  provider names remain at the mirror boundary; machine semantics cannot move
  to `adapter-bop`.
- Redirector deferred package: `vdmredir/{vrdisp,vrmslot}_compat.h` and
  `opennt-bop-overlay/vdmredir/{vrdisp,vrmslot}.c`. They remain private to
  the matching mirror translation unit; no component external to
  `opennt-bop` may include the overlay.

## S1 verification record

- `rg 'app/' src/opennt-bop --glob '*.{c,cc,h,hpp}'` has no match after the
  observation and startup-composition extraction.
- Fresh external formal Ninja graph:
  `build/M0-T271-S1/r003`; the affected product target rebuild produced
  `bin/ntvdm64.exe` after the startup extraction.
- Focused moved-observation fixtures passed:
  `t245-s14-command-bootstrap-observation-fixture`,
  `t245-s17-command-current-dir-observation-fixture`,
  `t245-s18-dem-open-observation-fixture`,
  `t245-s2-ntdos-exec-observation-fixture`, and
  `t247-s2-ntdos-exec-entry-observation-fixture`.
- The affected source-shaped keyboard handoff fixture,
  `t243-s2-spckbd-handoff-fixture`, also passed after its display-state
  storage moved to `adapter-softpc`.

The two first fixtures initially exposed a test-only one-megabyte automatic
array stack overflow (`0xC00000FD`).  Their buffers are now static storage;
no product behavior or observation implementation changed.
