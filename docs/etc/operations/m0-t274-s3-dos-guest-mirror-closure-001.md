# M0 T274 S3 DOS guest mirror closure

## Inputs

- `src.old/opennt-guest/dos-v86`, the owner-authorized direct recovery source;
- [S1 union ledger](m0-t274-s1-non-guest-mvdm-union-ledger.tsv);
- [source policy](policy/source-policy.md) and the admitted S3 Status brief.

## Procedure

`Recover-T274GuestMirror.ps1` enumerated the local DOS/V86 source tree,
copied each file only if no conflicting destination existed, and wrote one
source-path/SHA-256/size row to the S3 manifest. A later run must accept only
the identical destinations, making the recovery repeatable without overwrites.

## Observations

- The S1 union ledger has 676 `opennt-guest-dos` rows; all are classified
  `carry-forward-local-guest-mirror`.
- The direct source contains 676 files totaling 11,358,880 bytes. It includes
  source (`.asm`, `.inc`, `.c`, `.h`), build descriptions, objects, libraries,
  executable/com/sys products and other guest inputs.
- The destination manifest has the same 676 rows and a complete SHA-256 audit
  reports zero mismatches.
- `build/output/dos` and `build/output/wow16` remain at their existing paths.
  S3 neither copied, moved, rebuilt nor made them a host source/link input.

## Interpretation

`opennt-guest-dos` is now a complete carried local guest mirror, not a second
external OpenNT import and not a host-side static library. Its only later
product role is immutable image selection/loading through the declared machine
composition boundary.

## Follow-up

S4 repeats this direct, hash-checked route for the already-established WOW16
guest mirror. A later explicitly admitted guest-build/package task may decide
how a released bundle consumes the retained build outputs; this source-supply
task does not make that decision.
