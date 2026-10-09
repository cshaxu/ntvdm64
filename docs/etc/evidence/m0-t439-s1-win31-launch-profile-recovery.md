# M0 T439 S1 — Win3.1 installed launch-profile recovery

## Question and bounded outcome

The owner-selected installed Windows 3.1 tree reached the splash screen but
did not reach the Standard-mode desktop after both portable tool Apply
operations. This S repaired the project-owned profile/tool boundary without
changing NTVDM, guest executable bytes, or the ordinary Setup transition.

The owner confirmed that `PATCH\WINSTD.CMD` now reaches the desktop and that
Program Manager accepts the repaired group files.

## Changes

- `tools/win31-launch` regenerates portable Standard/386 profiles, reports
  each actual replacement or addition, and uses the established local
  `PATCH\TEMP` and startup contract.
- The evidence-backed Standard display profile is `VGA.DRV`/`VGA.3GR`,
  `display.drv=vga.drv`, `386grabber=vga.3gr`, and `display=*vddvga`.
  Video7 files are not touched.
- `GRP.EXE` keeps the PMCC pre-tag boundary valid while relocating item paths,
  preserves/recomputes the complete-file checksum, and relocates the
  documented Program Manager working-directory tag when its item moved.
- `PIF.EXE` preserves the explicit CloseOnExit and legacy field contract used
  by the generated one-shot profiles.
- The owner-directed removal of `assets/win101-setup-patched.zip` is included
  in this delivery; current tools consume released add-on artifacts instead.

## Verification

The AMD64 add-on releases were rebuilt/staged and their manifest refreshed.
Focused checks ran from the repository and, where a historical PIF field
requires a short physical path, an owned temporary `O:\` fixture that was
removed after the run:

```text
PASS GRP structured root replacement and PMCC rejection
PASS PIF create/show/update and HASH single-file output
PASS Win1.01 APPLY/UNAPPLY/SETUP CMD orchestration, generated profile,
     recovery, and cleanup
PASS explicit WINSTD/WIN386 PIFs and launchers, shared profile, CMD-owned
     recovery, and no duplicate tool payload
PASS win31-launch adjacent backup, repeat apply, legacy migration, and
     authenticated unapply
PASS Win31 path apply/repeat/unapply with adjacent backups
```

The final direct `MAIN.GRP` group-file check reported:

```text
WORKDIR_TAG=<installed-root>\
PMCC_TAG_LAYOUT_OK
```

The owner then performed the real Standard-mode desktop and Program Manager
acceptance. `git diff --check` passed before closure.

## Boundary and follow-up

This is Standard-mode portable-launch recovery. Enhanced-mode reliability and
the original Windows 3.1 Setup post-copy protected-mode transition remain
outside this S and are retained by the queued ordinary-Setup candidate.
