# T95 S6 MSVC format-token projection v2

## Question and source basis

Projection-baseline syntax observation 001 showed `FMT_LL` is absent from the
projected root config and is token-concatenated by original source at
`gui/siminterface.cc:1290,1302` and `gui/paramtree.cc:244,296,315,458`.
The original MSVC `WIN32` branch in `osdep.h:59` uses the MSVC 64-bit format
family (`FMT_LL` there is `"%I64"`).  This is a format-token platform omission,
not a device, GUI, adapter, or OpenNT behavior choice.

The coordinator admitted a narrow v2 manifest addition only: absent
`FMT_LL` with the specified literal token `"I64"`.  It is inserted into the
projected config as a string-literal macro, so the original sources retain
their adjacent-literal form.  This record makes no claim that it has been
syntax-validated or that any formatting behavior has run.

## Inputs and v1 preservation

`tools/t95-s6-msvc-config-projection-manifest.json` is immutable at SHA-256
`6815D8BD419ED9522C98F9DF55D242FB7037F42125C4504EC5766AE732FB9E28`.
`tools/t95-s6-msvc-config-projection-manifest-v2.json` retains its
`sourceConfigSha256`, replacements, and derived assertions exactly; its only
delta is this additional additions-array record:

```json
{ "name": "FMT_LL", "expectedSource": "<absent>", "newValue": "\"I64\"", "classification": "MSVC format-token platform omission" }
```

The extended synthetic `Test-BochsConfigProjection.ps1` pins the v1 hash,
compares v1/v2 replacements and derived assertions, requires exactly one v2
addition, and confirms that an absent synthetic `FMT_LL` produces exactly one
`#define FMT_LL "I64"` and report addition.  The focused test passed.  It
uses temporary synthetic inputs only.

## Actual projection result

The sole actual v2 command was:

```text
powershell -ExecutionPolicy Bypass -File tools\Project-BochsConfig.ps1 \
  -ManifestPath tools\t95-s6-msvc-config-projection-manifest-v2.json \
  -AnalysisRoot artifacts\analysis\t95-s6-msvc-x86-config-projection-003-20260811-001 \
  -InputConfigPath src\bochs\config.h
```

It succeeded and retained only `config.h` and `projection-report.json` in that
fresh root.  The input remains SHA-256
`A1FADC25502FB0A4644917B6F3A62987C125CC7335457F9C59E7CE7E62A45AC8`; projected
`config.h` is SHA-256
`C47030F79880C894A346F56E3AB3242924308983E00B5845EDCE849450C6C19D`.

Independent report review verified the immutable v1 hash; all 47 v1-plus-v2
replacement/addition records exactly once; `FMT_LL` exactly once as
`add/<absent>/"I64"`; both derived assertions exactly once; matching output
hash; and no retained file beyond the config and report.

## Non-claims and next gate

No Bochs tree was copied for this package and no `/Zs`, compiler, link,
executable, configure, VS project, `bochs.exe`, or `all` command was invoked.
The v2 output is a controlled projection artifact only.  The next gate is
coordinator review before any separately admitted syntax observation.
