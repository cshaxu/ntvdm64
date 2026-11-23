# T484 S3 P9: retired historical entrypoints

Six files are removed after actual input/consumer inspection:

- New-T198S25SourceBuiltNtdosFixtureInput.ps1 and
  New-T198S93SourceBuiltNormalReturnFixtureInputs.ps1 require three pinned
  historical artifacts that are absent from disk and the tracked tree.
  Their array symbols have no product or integration consumers. Their sole
  consumer, verify-historical-c-arrays.ps1, synthesizes fake bytes and replaces
  hashing only to test the two emitters themselves; it and its C fixture are
  removed with those producers. No original guest input is restored/rebuilt.
- New-T359S2SasStoreNinja.ps1 is already an unconditional retired-generator
  error because its configuration-command fixture no longer exists. It has
  no executable caller and produces no graph or coverage.
- Invoke-T310OriginalSoftpcSerial.ps1 has no executable caller. The current
  native CMD runner already accepts MVDM_BUILD_JOBS=1. That entry is tested;
  the old PowerShell command-replay fallback for a broken Ninja dispatcher is
  not reintroduced as a second execution path. Dispatch failures remain errors.

Historical evidence is retained. Current build documentation names the native
serial entry. The retired T290 disabled-ingress entry is not swept away by this
batch: its family-matrix caller still needs separate reconciliation.

PID29580 passed the native serial worker invocation and cached full host
build. All11 raw images and22 published hashes remain unchanged. No product,
mirror, guest or published image changed, so no runtime rerun/republication is
claimed. [Receipt](../../../artifacts/build-migration/t484/s3-retired-inputs/result.json)
records the checks; the containing commit is P9's delivery identity.

A remaining-generator literal scan finds43 Ninja generators,9 with missing
literal source inputs. This is not a live-edge or complete S3 remaining count:
dynamic inputs and inline test compilation still require review. S3 remains
active, with S4 publication and S5 final acceptance outstanding.
