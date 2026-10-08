# M0 T437 S9 — Win1.01 released driver source

## Question

Can the Win1.01 media preparation package use the one released
`MOUSE101.DRV` without retaining a duplicate driver under
`tools/win101-setup`?

## Change

- Removed `tools/win101-setup/MOUSE.DRV`.
- Both tool and add-on-source APPLY/UNAPPLY paths now require and authenticate
  `assets/release/MOUSE101.DRV` together with the released `PIF.EXE` and
  `HASH.EXE`.
- APPLY installs that released driver at the original media-root path and
  retains the existing `.BAK` identity checks. It still removes a legacy
  `PATCH/MOUSE.DRV` only after proving it matches the released driver.
- The focused fixture now fails if the tool package contains a mouse driver
  and verifies the prepared media root against the released asset directly.

The media `PATCH` content is unchanged: `SETUP.CMD`, `SETVER.EXE`, `PIF.EXE`,
and `HASH.EXE`. The original setup medium remains responsible for assembling
its guest image from its media-root `MOUSE.DRV`.

## Verification

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File tests\component-integration\win101_setup_cmd_test.ps1 `
  -ToolRoot tools\win101-setup -BuildRoot build\t437s9\r1
```

Passed. The fixture exercises first apply, repeat apply, successful generated
setup profile, failed setup cleanup, authenticated unapply with retained owner
content, and a conflicting-backup negative case.

## Boundary

This is package-source cleanup only. It neither changes released asset bytes
nor proves original graphical Setup completion; that retained guest-runtime
boundary is outside S9.
