# M0 T271 S5 P10 — OpenNT BOP mirror re-audit and disposition

## Question

After P9, does the current `opennt-bop` production root meet the mirror
standard, and do all previously extracted bodies now have a correct owner?

## Inputs and procedure

- Pinned `O:\repos.external\opennt` and `opennt-4.5` trees.
- `src/opennt-bop`, `src/opennt-bop-overlay`, all production consumers and
  `tools/build/t260-s8-component-manifest.json`.
- Filename/hash comparison against both OpenNT trees; local divergence-marker
  scan; overlay-consumer scan; and source-owner review of every non-baseline
  production filename.

## Observations

- Before this P, 110 BOP `.c`, `.h` and `.inc` files consisted of 18
  byte-identical files, 47 original-name files with a content difference, and
  45 non-OpenNT filenames. Every changed original-name file has at least one
  local `DIVERGENCE(BOP-DIV-...)` marker, but that coarse check does not prove
  that every changed line is minimal.
- All 19 overlay bodies are private: no `app`, `session`, `adapter-bop`,
  `adapter-softpc` or `adapter-win32` source includes the overlay root.
  `opennt-bop` is their sole direct include consumer. The earlier P9 wording
  that called this "13 overlay owner bodies" is stale; 13 is a historical
  grouped ledger count, not the current source-body count.
- The DPMI experiment was a dormant package despite being compiled into the
  formal library. Its sources and two fixtures are now archived outside every
  production/test build root. This is a removal of an invalid production
  claim, not a loss of source evidence.
- After the DPMI archive, DEM CCPU/SAS extraction, wrapper fold and COMMAND
  overlay-boundary recovery, the live root contains 76 C/H/INC inputs: 59
  original-name inputs and 17 remaining project-named compatibility headers.
  This is a reduction, not a closure claim: the 17 inputs are the concrete
  residual owner ledger below.

## Focused regression after the corrective moves

- A clean generated formal Ninja graph at `build/M0-T271-S5/r003` compiled
  and linked the DEM and COMMAND native-session source-path fixtures.
- `t230-s10-dem-native-session-direct-fixture.exe` passed, reporting copied
  `#UD -> MS_bop_0 -> DemDispatch`.
- `t231-s10-command-native-session-fixture.exe` passed, reporting native
  ownership of `54:00` with no v1 fallback.
- After the COMMAND boundary fold, the same two fixtures were rebuilt and
  executed from the refreshed formal `r004` graph with the same passing
  results.
- The XMS source-mirror and A20 source-mirror fixtures were also rebuilt and
  executed from `r004` after the `xms.c` boundary fold; both passed.
- After the DEM body folds, refreshed `r004` formal linking and execution of
  the DEM/COMMAND native-session fixtures passed unchanged.
- Folding the two wrapper bodies initially exposed a real macro-scope collision
  in `softpc/nt_bop.c`; the formal compiler caught it and the file now keeps
  one definition each of `MS_bop_0` and `MS_bop_4`.  This validation proves
  only the moved paths, not whole-root mirror closure.

## Remaining non-mirror disposition

The following are production files with no original OpenNT filename. They do
not pass the mirror test merely because a README row exists.

- **COMMAND source subsets:** the cropped `command/nt_bop_command.{c,h}`
  dispatcher entry has been folded into the matching original
  `softpc/nt_bop.c` mirror boundary; the superseded wrapper is retained only
  as indexed legacy evidence. The five former
  `opennt_command_*_composition`/`opennt_pif_composition` include stubs have
  now been folded into original `cmd.c`, `cmdmisc.c`, `cmdconf.c`, `cmdenv.c`
  and `cmdpif.c` boundaries. Their overlay bodies remain private; the large
  non-original compatibility headers are still residual owner work.
- **DEM CCPU/SAS bridge:** `dem_direct_context.{c,h}` and
  `opennt_dem_ccpu_sas_facade.{c,h}` are project-authored copied-frame,
  guest-pointer, handle and register machinery. The generic CCPU/SAS portion
  has the unique owner `adapter-softpc`; any DEM-only call sequencing must be
  reduced to source-local boundaries rather than moved wholesale as a second
  generic facade.
- **DEM family compositions:**
  `opennt_dem_{dispatch,dasd_ioctl,dir,error_lock,file,gset,misc,srch_fcb}_composition.*`
  and their `*_compat.h` headers mix three owners: original-provider call
  sequencing, historical CCPU/SAS macros, and Win32/host capability calls.
  They require a split, not a path rename: same-shaped CCPU/SAS operations go
  to `adapter-softpc`, unavailable Win32/NTDLL spellings to `adapter-win32`,
  host capabilities to `opennt-host`, and only inseparable source-local
  modified fragments to `opennt-bop-overlay/dem` behind the original DEM
  translation unit. The eight `.c` bodies have now been moved to those private
  overlay boundaries and deleted from the mirror root; their 12 non-original
  compatibility headers remain the active owner-split work.
  A controlled P11 experiment removed the eight `*_composition.c` bodies
  from the production library and supplied them only to direct-import tests.
  Formal linking of the unchanged native `MS_bop_0 -> DemDispatch` fixture
  then failed with 53 unresolved references from the original DEM provider
  table.  The experiment was reverted: the bodies are formal production
  closure dependencies, not dead test helpers.  They must be split by owner;
  they cannot be hidden by relocating them beneath `tests/`.
- **Redirector compatibility headers:** `vdmredir/vrdisp_compat.h` and
  `vrmslot_compat.h` are project headers. Their typed frame/guest-span pieces
  belong to `adapter-softpc`; their named-pipe/mailslot host calls belong to
  `opennt-host` or `adapter-win32`. The original-name `vrdisp.c` and
  `vrmslot.c` may remain mirror boundaries with their registered private
  overlays.
- **XMS compatibility and composition:** `xms/opennt_xms_compat.h` is a
  project compatibility aggregate and must be decomposed into the existing
  same-shaped `adapter-softpc`/`adapter-win32` declarations. Its private
  composition body is now included only at the original `xms.c` boundary;
  `opennt_xms_composition.c` no longer exists as a named custom production
  mirror file.

## Extracted-code placement review

- `adapter-bop` is correct: its current source is selector-blind.
- `adapter-softpc/interrupt06_provider.{c,h}` is correct as typed machine
  mechanics. Its caller `app/bop/startup_machine_interrupt.c`, however,
  recognizes selectors `02h` and `06h`; selector/source routing is not final
  app wiring and must return to the matching OpenNT SoftPC dispatcher boundary.
- `app/bop/route.c` and the current generic-UD recognizers select BOP families
  and services. `app` may bind one opaque route callback, but may not own this
  source semantics. These recognizers must be replaced by the appropriate
  original `nt_bop.c`/family dispatcher paths; they must not move to
  selector-blind `adapter-bop`.
- `app/bop/redir_native_session.c` retains source-derived Redirector service
  policy and a 618-line session/provider implementation. It must be separated
  into `session` neutral state, `opennt-host` capability, same-shaped adapters,
  and original `vdmredir` source/registered private fragments. Leaving it as
  app code is not compliant.

## Conclusion

P9's structural private-overlay scan was useful but insufficient. The mirror
root is **not yet closed**. P10 resolves the dormant DPMI breach and supplies
the authoritative disposition for each remaining class. The first bridge move
is complete: the DEM copied-frame/CCPU-SAS facade and direct context now
belong to `adapter-softpc`; they are compiled there and no longer reside in
the mirror root. Their remaining source-specific span-selection logic is the
next split, rather than a claim that the facade is already fully generic.

## P15 follow-up: DEM-private declaration isolation

Ten project-named DEM declaration surfaces had no production consumer outside
the corresponding DEM mirror units: `opennt_dem_common_compat.h`, the
dispatcher/directory/error/file/GSET/search composition headers, and the
file/GSET/search compatibility headers. They now live in
`opennt-bop-overlay/dem/` with the P14 bodies. Original DEM units include them
only as registered private boundaries; focused direct-import fixtures use them
only to compile the matching mirror source. A formal Ninja rebuild and
execution of the DEM native-session and direct-dispatch fixtures passed.

The residual mirror-root set was initially seven cross-component headers:
COMMAND composition/PIF, DEM DASD/misc compatibility, VDMREDIR
dispatch/mailslot compatibility and XMS compatibility.  P16 removed the two
DEM entries after proving that `app` did not need the DASD surface and could
bind `DemInit`'s original system-directory call through the same-shaped
adapter-win32 facade.  The current residual set is five headers: COMMAND
composition/PIF, VDMREDIR dispatch/mailslot compatibility and XMS
compatibility.  Their required disposition is API ownership recovery, not
another path-only move.
