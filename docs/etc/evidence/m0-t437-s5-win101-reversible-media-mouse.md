# M0 T437 S5 — Win1.01 reversible media mouse replacement

## Question

Can a selected Win1.01 installation-media directory receive the approved
mouse driver directly, while retaining an exact local recovery copy and a
minimal `PATCH` directory?

## Contract

`APPLY.CMD` owns the selected user media only. It moves the root-level
`MOUSE.DRV` to `MOUSE.DRV.BAK`, then copies the released replacement to the
same original root path. It never puts a replacement driver in `PATCH`.

Media `PATCH` contains exactly these new helper files:

```text
SETUP.CMD
PIF.EXE
HASH.EXE
SETVER.EXE
```

`SETUP.CMD` creates launch PIF/configuration files only while it runs, deletes
them on both normal and negative return, and invokes original Setup against the
prepared root media. Original Setup remains the owner of `WIN100.BIN` assembly.

## Identity and repeat behavior

The current media `MOUSE.DRV` must match the released driver when a
`MOUSE.DRV.BAK` already exists; otherwise application stops without replacing
anything. A repeat application leaves the backup untouched. A superseded
S4-created `PATCH\MOUSE.DRV` is removed only if it matches the released
replacement; an unrecognised file stops the operation rather than being
deleted.

## Focused verification

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File tests\component-integration\win101_setup_cmd_test.ps1 -ToolRoot tools\win101-setup -BuildRoot q
```

Passed. The disposable flat-media fixture proves first application, byte-for-
byte root backup, replacement identity, exact media-PATCH contents, repeat
application preservation, normal/negative Setup-launch cleanup, generated
installed profile, and conflicting-backup refusal without overwriting the
original root driver.

The test intentionally uses a compact physical `q` path because legacy PIF
configuration fields are bounded. It does not prove that a specific original
Setup guest run completes; that remains manual acceptance.

## Scope boundary

This S changes only Win1.01. Win3.1 has a separate pre-Setup
`KRNL386/WIN386` transition contract and must not be changed by assuming its
package can contain the same four-file helper set.
