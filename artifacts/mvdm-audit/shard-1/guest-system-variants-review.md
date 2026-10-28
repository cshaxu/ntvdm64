# Shard 1: pending Win16 SYSTEM Vectra/ROM declarations

The two scoped files are `待核验` in the current shard ledger and match both frozen OpenNT editions. The observer rechecks original SYSTEM sources and recipe, resolves VALID Ninja dependencies relative to each of four host build trees, and hashes the retained, installed and archived guest SYSTEM.DRV. No guest or product execution is implied by these checks.

| Pending file | Original role and bounded selection evidence |
| --- | --- |
| `wow16/system/equate.inc` | Historical Vectra standard/extended BIOS equates and macros; `envect.asm` directly includes it for Vectra enable/disable support. The checked original SYSTEM makefile/retained `system.lrf` build and link only `system.obj` and `timer.obj`, not `envect.obj`. The consumer is a real original source but its selection in this recipe is not established. |
| `wow16/system/romstuff.inc` | Historical conditional ROM/non-ROM macros for data segment and offset. `system.inc`, which both selected source files include, comments that its `DSEG` and `doffset` definitions were picked from `romstuff.inc`; this is copied text, not a direct include. No selected original consumer of the file itself was established. |

The current four x86 worker/WOW32 and x64 frontend/service Ninja dependency logs contain no exact normalized VALID hit for these two guest declarations. These are candidate historical alternate-platform/declaration file roles relative to the checked original SYSTEM recipe only; the Vectra, ROM, SYSTEM driver, timer and other configurations remain separately scoped.

The retained `src/mvdm/wow16/system/SYSTEM.DRV`, exact installed `O:/winnt/system32/SYSTEM.DRV`, and `WINNT/SYSTEM32/SYSTEM.DRV` in `assets/winnt.zip` have the same SHA-256 `32D88D68FD5DAE0F37766DB04742CAA912C9A4C7193D15A1E99E617FF6F794A3`. This does not prove the retained makefile built that binary or that any alternative platform and timer paths execute correctly. `SYSTEM.RES` was handled separately by `guest-resource-components-review.md` and is not reclassified here.

Replay `python -B artifacts/mvdm-audit/shard-1/guest-system-variants-observer.py` with `PYTHONDONTWRITEBYTECODE=1`. No product, guest, compiler or provider ran; no product, guest, build tree or shared ledger changed.
