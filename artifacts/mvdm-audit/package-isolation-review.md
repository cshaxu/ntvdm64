# Package-scoped NTSRV candidate: independent review in progress

The parallel implementation is still in progress; these are review points
against the listed file hashes, not acceptance of its final code. UUID is an
endpoint partition and package consistency value, not a security credential
against another process in the same logon session. The existing RPC policy
continues to authorize the peer's session, token and real process.

The stage script copies the baseline before stamping ten images one by one.
If a later operation fails, its fresh output can retain copied prior
`product-system32-manifest.json` or partially replaced images. The manifest
is written before the license copy. Review the completion marker and failure
behavior; a partial directory must not be mistaken for a complete package.
Do not delete a user's baseline or unrelated data.

The existing `tests/observation/verify-system32-package-staging.ps1` invokes
the staging script without its mandatory `NativeWorker` and corresponding
Hook inputs, while expecting the older eight-image form. Its normal and
negative cases therefore need alignment with the actual ten-image package
contract, including image identity and failure checks. A mistaken initial
review claim that normal staging could succeed without Hook images was
withdrawn: mandatory NativeWorker implies Hook64, and Hook64 requires Hook32.

The candidate's two-package tests cover RPC clients and controlled DOS/native
CUI routes; source review identified the need to separately verify actual
product mix rejection, x86/x64 injected DLL origin, staging failure and
existing package entrypoints. New results from the parallel session require
independent inspection, and changed file hashes invalidate this snapshot.

## Review follow-up

The staging path now removes a copied old manifest, writes the new manifest
only after all image and license operations, and removes the exact completion
manifest and pending file if staging fails. The output directory itself is
retained for diagnosis. The previously eight-image staging test now supplies
the required worker and both hooks, checks ten manifest identities, and
asserts an overlay-conflict failure leaves no completion manifest.
`build/t473-s10/staging-review-r2/results.json` records its pass against the
S9 baseline and candidate x86/x64 outputs. A first run failed due to a
PowerShell 5 array conversion issue in the new test; it was corrected and the
second run passed. This is not a substitute for the product integration and
full regression gates.

## Independent-test scope

UUID belongs to a staged package, not a calling Codex session or process.
Copying the same package preserves its ID; such copies will intentionally
address the same NTSRV. The existing `New-PhysicalRuntimeStage` copies package
bytes and does not mint an identity, so two tests independently staging the
same baseline cannot claim isolation merely from separate directories. Verify
the final test harness creates a distinct, internally consistent package
identity for each concurrent scope, without changing the installed package,
and verifies that different workers/frontends cannot cross sessions.

## Delivery and retained verification

The parallel implementation and subsequent review fixes were built and
tested as `build/package-uuid/package-c` (package ID
`ad4ac7ac-cffa-4892-8770-e41a5b765e94`). Its ten component hashes match
both `build/t473-s10/package-product-review-r1/runtime-manifest.json` and
`build/t473-s10/package-control-review-r7/runtime-manifest.json`. The Product
gate passed WOW startup comparisons and all 17 Console plus 17 Window routes.
The Control gate passed all 11 selected stages, including real incompatible
version/protocol servers, GUI routing, managed worker/frontend retirement,
relaunch and nested Window handoff. Tests used a desktop-capable token and a
controlled short TEMP; this is not proof of arbitrary inherited environments.

Independent retained probes passed Minesweeper flag/reset/open/close, SOL
deal/draw/close and Options, and WRITE close/restart/close (respectively
`mine-review-r1`, `sol-game-review-r1`, `sol-options-review-r1` and
`write-review-r1` under `build/t473-s10`). A two-stage UUID regression passed
concurrent RPC and DOS sessions (`fresh-stage-review-r1`); same identity was
rejected by the shared test guard and a different verified identity allowed.
The parallel session additionally recorded distinct-package DOS/native CUI,
32/64 Hook observation, mixed-identity and ordinary-copy tests under
`build/package-uuid`. This is a finite regression set, not all historical
tests or T-wide acceptance.

The first control run could not find generated service headers; the second
could not find the PowerShell host after PATH restriction. The third used an
unstamped management fixture; the fourth was stopped by an old global broker
check. These were test-integration failures, retained in `package-control-review-r1`
through `r6`, not product passes. A separate unsanitized nested test entered a
guest illegal-instruction dialog (`handoff-retry`); its controlled-environment
repeat passed (`handoff-controlled`), without proving the original guest
environment defect's root cause. Control r7 passed end-to-end.

After backups and tested-manifest verification, the exact ten package-c
images were synchronized to `assets/release` and `O:/winnt/system32` and
verified there. The recovery copy is
`build/t473-s10/package-isolation-delivery-backup`. `release-manifest.json`
records each published hash and package ID. No guest image was modified.
The wider S10 audit remains open.

## Source identities at this review

- `src/ntsrv-exe/transport/rpc_security.c`: `2EFB9FEF76700ED3D7BE274C4C2D843DF448DFF3B468E5FF70BF7FFFDCE71849`
- `tools/build/Stage-System32ProductPackage.ps1`: `F7487C62F381866F0BCE4FA1BF4C0D95EB5DF9250230D8EB2D5388E6A2FD38F1`
- `tools/build/Set-PackageIdentity.ps1`: `E1498DEF7677DD3A9F63D9B6936749613A11671AEF905BB4E05AC34A23985916`
- `tests/observation/verify-system32-package-staging.ps1`: `7A9E941D8D7DDBDE73D639C3CF6D73FE4CE4B654691D348741F6FC382EE5CCCF`
