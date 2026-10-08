# M0 T438 S2 — Win31 path tool split

## Delivered boundary

`tools/win31-path/APPLY.CMD` now discovers and prints the installed old/new
roots, enumerates root-level `*.INI`, `*.PIF`, and `*.GRP` files, creates a
same-directory `.BAK` only immediately before a real change, and records
changed names in `PATCH\PATH-REPAIR.MANIFEST`. `UNAPPLY.CMD` consumes that
list to restore only the matching adjacent backups.

No fixed release hash is used for path-dependent files. The old and new
contents necessarily vary with the selected installed location.

| Format | Owner | Scope |
| --- | --- | --- |
| INI | `APPLY.CMD` | Exact canonical-root substitutions in root-level INIs; refuses `!` or a case-only/unrewritable detected reference rather than silently changing formatting. |
| PIF | released `PIF.EXE` | `replace-root` updates standard program/directory and NT config/autoexec fields. |
| GRP | released `GRP.EXE` | `show` and `replace-root --root`; parses supported PMCC items, redirects only targets resolving in the selected tree, and recomputes the documented checksum. |
| discovery | `WIN31PATH.EXE` | `--discover-root` only; it no longer has a settings-rewrite command. |

## Build and tests

Built AMD64 utilities below `build/M0-T438/S2`:

```text
cmd /c src\addon\win31-launch\BUILD-PATH.CMD build\M0-T438\S2\r8\WIN31PATH.EXE
cmd /c src\addon\pif\BUILD.CMD build\M0-T438\S2\r1\PIF.EXE
cmd /c src\addon\grp\BUILD.CMD build\M0-T438\S2\r1\GRP.EXE
```

Focused results:

```text
PASS GRP structured root replacement and PMCC rejection
PASS PIF structured root update
PASS PIF create/show/update and HASH single-file output
PASS Win31 path apply/repeat/unapply with adjacent backups
```

The path fixture proves real CMD orchestration, root discovery, INI repair,
PMCC group repair, repeat Apply, adjacent-backup recovery, and preservation of
unknown `PATCH` content. PIF structured replacement is exercised separately
with paths small enough for legacy PIF field capacities. A target whose new
path exceeds a PIF field is a safe failure, preserving the newly created
adjacent backup for recovery.
