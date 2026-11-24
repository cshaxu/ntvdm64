# T484 S4 physical runtime staging through CMD

## Change and scope

P6 completes the shared physical preparation behind New-PhysicalRuntimeStage.
Its PowerShell facade delegates allocation, copying, optional identity refresh
and ownership marker generation to the native-configured stage-runtime.cmd.
Cleanup remains the existing identity-checked test runner operation.

CMD accepts only build-owned sources, explicit runtime image names and direct
include leaves. It preserves hidden files and empty directories, excludes the
existing builds/logs/logs2 directories, rejects mixed flat/system32 images and
creates a short direct child of O:/tmp. Only successful creation establishes
cleanup ownership. The marker is completed last; failure removes only that
owned child. The existing native package tool emits the UTF8 JSON header.
Fresh identity refresh now also preserves the existing flat-layout contract.
No original guest or product implementation changes.

## Verification

Evidence root: build/t484-s4-physical-stage.

- Contract PID25464 completed successfully on 2026-11-24,16:15:05Z-16:15:25Z.
  Expanded Test-PackageIdentityTool covers selected/duplicate includes, hidden
  files, empty directories, exclusions, invalid leaves, mixed-layout rejection,
  legacy flat refresh, actual Unicode source paths and cleanup. Earlier UUID,
  malformed manifest, full-image preflight and source preservation tests remain.
- Delivery PID3812 completed cached full build and all package contracts, but
  returned1 because three inherited-environment application runs failed.
  SOL gameplay returned4 without an owned application window; Options also
  found no Solitaire window; WRITE timed out. These are retained failures.
- Control PID18532 used the HEAD PowerShell physical-copy implementation with
  the same package, observer and gameplay runner. It also returned4 without
  an owned Solitaire window (16:26:42Z-16:27:11Z). Thus failure is not unique
  to the new CMD provider; this does not diagnose a particular environment key.
- Userenv PID14272 used the existing native CreateEnvironmentBlock reader,
  current token, inherit=false, without an environment allowlist. All three
  unchanged private-desktop runners passed (16:28:19Z-16:29:25Z): SOL actual
  Deal/stock drawing and normal launcher completion, Options modal routing,
  WRITE editing/saving with content verification and normal close over two
  clean starts. WRITE recorded640 saved bytes and its marker in both rounds.
  This proves the supported logon-environment cases, not the failing inherited
  environment case. Logs, screenshots and launcher reports remain per case.
- publication.json records all11 raw host inputs unchanged and all22 published
  hashes matching assets/release and O:/winnt/system32 at16:29:56Z. No changed
  product binary required copying. No new Full runtime pass is claimed.

## Remaining work

Counts remain327/355 disposed and28 pending, with24 overlapping preparation
chains. Shared physical preparation is migrated; consumer-specific generation
and staging still require audit. In particular Invoke-WowAppProbeBuild still
copies its built probe in PowerShell, so the three application callers are not
counted as fully migrated. Independent test assertions and cleanup are outside
automatic language conversion. S4 remains active; clean interpreter-free
end-to-end acceptance remains S5.
