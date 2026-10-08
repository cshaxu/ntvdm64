# M0 T438 S1 — Win31 path adjacent recovery

## Scope

`win31-path` may mutate only the installed-root textual settings selected by
the existing relocation engine and root-level PMCC Program Manager groups.
It does not alter guest binaries, product runtime components, or arbitrary
files below the selected tree.

## Recovery rule

Immediately before replacing an actually changed file, `WIN31PATH.EXE` copies
it beside itself as `<file>.BAK`. Existing backups are a stop condition and
are never overwritten. `APPLY.CMD` records only the changed filenames in
`PATCH\PATH-REPAIR.MANIFEST`; `UNAPPLY.CMD` moves just those adjacent backups
back. The wrapper intentionally has no release identity or fixed hash rule:
the original and repaired content naturally differs for every installed root.

## Focused evidence

Built AMD64 `WIN31PATH.EXE` with:

```text
cmd /c src\addon\win31-launch\BUILD-PATH.CMD build\M0-T438\S1\r1\WIN31PATH.EXE
```

Ran against a copied historical path fixture:

```text
powershell -NoProfile -ExecutionPolicy Bypass -File tests\component-integration\win31_path_recovery_test.ps1 \
  -InstallRoot build\M0-T438\S1\r2\path-fixture \
  -Apply tools\win31-path\APPLY.CMD \
  -Unapply tools\win31-path\UNAPPLY.CMD \
  -BuildRoot build\M0-T438\S1\r2\test
```

Result: `PASS Win31 path apply/repeat/unapply with adjacent backups`.
The test proves first Apply, idempotent repeat Apply, exact byte restoration
from adjacent backups, consumption of those backups, and preservation of an
unknown `PATCH\KEEP.TXT` owner file.

## Follow-up boundary

The owner subsequently requested a tool-boundary refactor: CMD must own INI
rewrite orchestration; a dedicated `GRP.EXE` must own the structured PMCC
group transformation; existing `PIF.EXE` must handle any discovered PIF root
fields. That is admitted as T438 S2 and is not claimed by this S1 baseline.
