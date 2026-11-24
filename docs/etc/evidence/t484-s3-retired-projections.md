# T484 S3 P154 retired projection and one-shot archive tools

## Decisions

B0347-B0349 are frozen Bochs CPU/config projection JSONs. The current CCPU
product does not consume them. The only remaining references to the T95
manifests came from Test-BochsConfigProjection.ps1, whose Project-BochsConfig.ps1
generator is already absent. Remove these three unused data files and that
obsolete generator self-test together; no current compiler inputs change.

B0350 update-guest-archive.py is a fixed-list T447 one-shot archive addition,
not the current product publisher. T447 scope-freeze evidence records its
completed27 additions. The current guest manifest retains those additions
along with later approved entries. The tool rejects existing members, so it
cannot serve as an incremental general updater. Remove this completed,
uncalled tool instead of migrating an archive mutation path without a current
purpose. Preserve assets/winnt.zip, its manifest and all historical evidence.

B0341 Stage-NativeWorkerImage.ps1 is not included in this retirement: it still
has native import-validation callers and needs separate treatment.

## Verification and limits

Read source and searched tools/test/root CMake for the full deleted closure.
Post-deletion search has no remaining references in those current roots.
Git reports no assets change. Five files/394 lines removed. No product or
build graph changed; no new build, runtime test or publication is claimed.
Run current tracker, documentation/link governance and diff checks for this
batch. Four inventory dispositions complete:284 of355,71 remain.
S3 and final S4/S5 delivery obligations remain open.
