# T95 S6 MSVC configuration projection 002

## Result

The one admitted actual projection succeeded.  It produced only
`config.h` and `projection-report.json` in
`artifacts/analysis/t95-s6-msvc-x86-config-projection-002-20260811-001`.
No Bochs tree was copied and no compiler, `/Zs`, linker, executable,
configure script, VS project, `bochs.exe`, or `all` invocation occurred.

## Exact input, command, and output

The pinned source before and after the one invocation is SHA-256
`A1FADC25502FB0A4644917B6F3A62987C125CC7335457F9C59E7CE7E62A45AC8`, equal to
the manifest's `sourceConfigSha256`.  The executed command was:

```text
powershell -ExecutionPolicy Bypass -File tools\build\Project-BochsConfig.ps1 \
  -ManifestPath tools\build\t95-s6-msvc-config-projection-manifest.json \
  -AnalysisRoot artifacts\analysis\t95-s6-msvc-x86-config-projection-002-20260811-001 \
  -InputConfigPath refs\bochs\config.h
```

The projected `config.h` SHA-256 is
`A77D063C5960C9825F218253085BA7EEE288E2653E2CC07D568259CDCF0C9A11` (28,105
bytes).  The report is 16,276 bytes.  These are the only retained files.

## Independent report review

A separate PowerShell read of the manifest, report, output hash, and pinned
input verified:

| Check | Result |
| --- | --- |
| Source input unchanged and report input hash matches manifest | pass |
| Report output hash matches retained `config.h` | pass |
| Every manifest replacement/addition occurs exactly once | 46 / 46 |
| Every manifest derived assertion occurs exactly once | 2 / 2 |
| Extra report changes/assertions | none |
| Retained files | only `config.h`, `projection-report.json` |

The result preserves original typedef/format/extrema/derived validation
machinery while applying only the explicit manifest overlay.  It is a
configuration projection artifact, not a selected product configuration and
not syntax, object, link, reset, firmware, guest, or runtime evidence.

## Next gate

`/Zs` is not admitted by this result.  The next gate is coordinator review of
this projection evidence and a separately scoped decision whether to admit one
syntax-only observation.
