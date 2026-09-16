# M0 T414 S3 canonical MVDM tree closure

## Closed result

`src/mvdm/` is now the one physical, OpenNT-relative source tree for the
selected `base/mvdm` union. The re-root moved 4,615 original blobs without
changing their contents. `src/mvdm-overlay/` is adjacent but deliberately
outside the mirror; it remains the only MVDM-specific private overlay tree.

The former directory labels survive only where they describe an excluded,
non-MVDM owner (`src/mvdm-host/kernel-vdm/` and `src/mvdm-guest/font16/`) or
historical evidence. They are not active original-MVDM build inputs.

## Final verification record

| Check | Result |
| --- | --- |
| Canonical identity | `Test-T414CanonicalMvdmTree.ps1 -Phase after -BaselineRef 9162b97fe`: PASS; 4,615 destination blobs equal the frozen baseline, with six declared exclusions. |
| Current path ownership | PASS; the formal generator, runtime stager and current architecture/coding/source-policy authorities have no stale selected-root path. |
| Formal product | PASS; fresh MSVC Win32/x86 `/MT` graph linked `run16.exe`, `basesrv.exe` and `ntvdm.exe` from `src/mvdm/...`; `Verify-VdmTibStorage.mjs` passed during worker link. |
| Focused behavior | PASS; BaseSrv reservation, original Check/Update/Get/ExitVDM lifecycle, broker record, CCPU descriptor/HALT-RESET, original external memory, and original C-VID vector fixtures passed. |
| Governance | PASS; documentation topology/link governance and `git diff --check` passed. |
| Product identity | PASS; all three current formal programs compile against `src/app/version.h` with `APP_VERSION 0.0.414` and protocol 3; the IDL remains `version(3.0)`. |

The legacy direct-worker `--runtime --record-manifest` probe exited 161. This
is an expected contract mismatch, not a product regression: T412 intentionally
made `ntvdm.exe` a broker-connected worker rather than the legacy standalone
runtime CLI. It was not used as a passing test and no behavior was changed to
accommodate it.

## Publication decision

No executable was copied to `O:\winnt`: T414 changes source layout and the
required task-version metadata, not runtime behavior. The existing published
T413 package remains undisturbed; any next functional T can publish and run
its own tested T414-version product. All T414 build and temporary package
outputs remain below `build/M0-T414/`.

## Diff accounting

This task intentionally reduces neither source-code semantic diff nor private
overlay logic. It replaces five scattered original-MVDM physical roots with
one canonical mirror root, and renames the private overlay from
`mvdm-host-overlay` to `mvdm-overlay`. The measurable outcome is structural:
4,615 original files become directly comparable at their OpenNT relative paths,
while private changes remain visibly separate.
