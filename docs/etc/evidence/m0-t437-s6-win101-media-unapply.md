# M0 T437 S6 — Win1.01 prepared-media recovery

## Question

Can a prepared Windows 1.01 media directory be restored without guessing at
its current contents or deleting owner files from `PATCH`?

## Contract

`tools/win101-setup/UNAPPLY.CMD` prompts for the prepared media root. It
requires both root `MOUSE.DRV` and same-directory `MOUSE.DRV.BAK`, and first
requires the current root driver to byte-match the released replacement. Only
then does it copy the backup back to the original path and consume the backup.

If `PATCH` exists, each of the four tool helpers is compared byte-for-byte
with the tool package before deletion: `SETUP.CMD`, `PIF.EXE`, `HASH.EXE`, and
`SETVER.EXE`. Other files and directories are not deleted; `PATCH` is removed
only if it becomes empty. The recovery tool itself is never copied to media.

## Focused verification

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File tests\component-integration\win101_setup_cmd_test.ps1 -ToolRoot tools\win101-setup -BuildRoot build\x
```

Passed. The compact `build\x` fixture is required by the historical PIF field-size
limit. It proves apply-to-unapply byte restoration, backup consumption,
verified helper removal, preservation of an unknown `PATCH\KEEP.TXT`,
repeat/unprepared rejection, and the pre-existing normal/negative Setup
cleanup/profile assertions.

## Boundary

This is a selected-media operation only. It neither changes an installed
Windows tree nor modifies the distinct Windows 3.1 packaging contract.
