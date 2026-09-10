# M0 T270 S2 — Opennt-guest mirror closure

## Selected-source identity

The component contains 732 tracked files including its README. The 730
production files have one selected, byte-identical OpenNT origin:

- `dos-v86/`: 676 files, each equal to the same relative path in
  `OpenNT/base/mvdm/dos/v86` and/or
  `OpenNT-4.5/nt/private/mvdm/dos/v86`.
- `wow16/inc/`: 2 files, each equal to the same relative OpenNT WOW16 source.
- `wow16/fonts/`: 42 system-font files, each equal to the selected
  `OpenNT/base/win32/winnls/fontsup/system` source (also equal to the matching
  OpenNT-4.5 system-font source).
- `bin86/`: 10 files, each equal to the selected
  `OpenNT/base/mvdm/bin86` source.

There is no `opennt-guest-overlay` root, no locally authored production file,
and no local `DIVERGENCE(...)` marker. The component README's zero-entry
intrusion register is therefore correct.

## Artifact and staging result

The current guest-bundle staging path passed outside the sandbox in a fresh
disposable root:

```text
Stage-T235S5GuestBundle.ps1 ... build/M0-T270-S2/r001
Staged 113 guest artifacts ... (68 original, 0 source-built)

Test-T235S5GuestBundle.ps1 ... build/M0-T270-S2/r001
Verified 113 staged guest artifacts with complete origin records.
```

The selected binaries, libraries, objects, symbols and maps retained under
`dos-v86/` remain immutable original package/build inputs; their detailed
per-file ledger is `m0-t262-s3-opennt-guest-binary-import-001.md`. This is
not a claim that every retained object is a runtime payload.

## Separate recipe limitation

Three retained historical source-build recipes
(`Build-T225S8PrimaryNtio.ps1`, `Build-T225S8PrimaryNtdos.ps1`, and
`Build-T225S8PrimaryCommand.ps1`, plus the SHARE recipe) still name the
pre-T260 `src/opennt-guest/base/mvdm` root. That old root included shared
`base/mvdm/inc` host/CCPU declarations which do not belong in the guest mirror
under the current component boundary. The recipes are not current formal
Ninja or guest-staging consumers, and this S neither revives them by copying
host headers into `opennt-guest` nor alters their compilation semantics.

Their cross-component source-build closure is a later tool/recovery package;
it is explicitly not an `opennt-guest` source divergence or an artifact
provenance failure.

## S2 result

All selected production guest files and the current staged guest bundle meet
the mirror/provenance rule. No source move or production-code change was
necessary.
