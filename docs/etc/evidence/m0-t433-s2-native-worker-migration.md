# M0 T433 S2 single native-worker migration

## Scope, source and design handoff

Owner accepts the presented NTVWM migration and authorizes S2 implementation.
S1 reaches a bounded design handoff, not four-component runtime completion;
later frontend/monitor/launcher native probes remain those stages' obligations.
S2 keeps one ntvwm.exe, changes its carrier to AMD64 and retains x86 NTSRV,
NTCON, run16, NTVDM, WOW32/VDMREDIR and Hook32. Hook64 remains AMD64.
Application identity advances to0.0.433; RPC41/I/O25 and task semantics stay.

Baseline: T432 production b97041e74, sealed S6/r048-runtime and publication
r029. Current x86 formal cache is M0-T427/S2/r001; S2 native root is
build/M0-T433/S2/r001, Hook64 root r002-hook64. No old dual-worker source or
artifact enters production. Original mirror bodies remain unchanged.

Recovery ladder: reuse current worker/common/worker-base and the actually
pulled source ledger. Original RTL error.c is compiled unchanged through
the existing native support/TLS facade already used by Hook64. No original
Base VDM client, CSR capture, RTL environment or MVDM execution enters this
worker closure. Source-native Win32 resource/path adaptation stays in common
and the existing requester/launch primitive, not an original mirror.

## Build and ABI

New-NativeWorkerNinja.ps1 consumes the current formal graph and matched
NTVWM map. It rebuilds45 selected source edges, architecture-local static
libraries and MIDL x64 clients, pruning unused service/provider members from
the Base binding archive. Source manifests carry input/graph hashes. Native
flags use MSVC /MT and fail implicit-function or pointer-truncation diagnostics;
source-specific Detours optimization is retained. Machine flags/forced MVDM
headers never leak into the native worker. The historical Base ABI header is
limited to the client that actually consumes it.

Initial generator failures (path regex and unused archive edge), missing
declaration includes, an overbroad forced-header leave macro collision and
test main/wmain mismatch are retained in build/build2/build3/build4 logs.
They are build failures, not product passes. build5 and strict-native-build
successfully link the AMD64 worker and existing native test sources.

Initial native RPC fixture:220 checks/zero failures/handle delta0. Existing
NTVWM lifetime fixture:1084 checks/zero failures/48 completions/16 cancellations,
target survival and remaining handles0. These are unit/mock contracts, not
proof of the actual service or normal process retirement.

## Cross-width selected-file boundary

r003 candidate's actual native32 projection passes. Its native64 request
fails with exit3 before a target: x86 run16 passed Sysnative to a now-x64
worker, where that WOW64-only spelling does not identify the file.
This is a project-added cross-process path boundary, not a guest or original
OpenNT defect. Do not change search, argv, CLI or globally disable redirection.

The requester now opens its already selected native application and obtains
Windows' final DOS/UNC name before copied submission. The x64 worker creates
that actual file while retaining the original command string. This also
preserves an x86 caller's System32 view rather than reselecting it in64 bits.
The new common/native_path.c has only this Win32 file-identity mechanism;
it is separate from section metadata so lightweight request clients do not
acquire original RTL/NT-section dependencies. No second search/classifier.

An initial native-device spelling was readable for metadata but failed the
real CreateProcess fixture with87; it is not the delivered path mechanism.
The final DOS/UNC spelling passes file-ID equivalence, actual machine,
invalid/null/empty/small-buffer output clearing and handle cleanup. Existing
metadata assertions grow from402 to424, zero failures/steady-state growth.
The client fixture retains actual exit37, final-I/O gating, broker failure,
authority stripping and no-target resume assertions. Native launch capability
text uses full-width hexadecimal rather than unsigned-long truncation.

r006 candidate proves actual AMD64 direct target kind3, real output/completion
and order through x86 NTSRV/NTCON/run16. This does not alone certify the final
package; final strict flags/source inputs seal r008-runtime, ten images with
only NTVWM and Hook64 AMD64. New explicit NativeWorker staging/verification
inputs retain exact hash/machine assertions, not inferred relaxed checks.

## Verification still running

r009-full runs the retained serial Full suite on r008, with explicit native
worker and Hook64 input identities, previous independent WOW frontiers,
Console17/Window17, RPC/GUI/version negatives, strict DIR, modern EDIT, cooked
return, relaunch/isolation, retirement and final Window handoff. No global
product matrices run in parallel; only independent unit fixtures may overlap.
Further actual both-width legacy/reentry/handoff and publication gates remain.
No S2 closure, production commit/push or new O:/winnt publication is claimed.

## Final-path representation and renewed gates

r009-full passes all three independent WOW frontiers, then fails Console
native-zero's unchanged version-output assertion despite exit0. The page
contains CMD's message-resource lookup failure2350, not the expected version.
The final-file query returned an extended namespace prefix; using ordinary
short DOS/UNC spelling retains file identity and restores the strict output.
This is namespace representation, not a Sysnative string-replacement rule,
new resolver, global redirection disable or guest change. Extended long-path
behavior remains outside this repair's known supported contract.

r011 targeted probe incorrectly uses the long physical build path and fails
before DOS with1067; the owner-known long-path limit remains unmodified.
r012 uses the required owned SUBST Z: alias, removes it in finally and passes
the same strict native-zero marker/exit assertions. Failure logs remain saved.
Short-path client and metadata fixtures still pass,424 checks/no growth.

r010-runtime seals the corrected actual files. r013-full currently passes
Console17/Window17 and independent WOW frontiers, and continues serial Control
gates; it is not yet a full pass. Current strict-native RPC220/lifetime1084
and matching/cross-width Hook147/147 fixtures pass without weakened sources.
test-native-worker-package-inputs.ps1 proves rejection of I386 worker input,
missing Hook64 and an outside-build worker path in r014; these are file-only
staging negatives, not process/lifecycle proof. Governance/link/diff pass.

r013-full completes with exit0 in398706ms, all14 group rows true. This is a
full retained gate pass on r010, not reclassification of r009's failure.
Final actual both-width legacy/reentry/GUI certification is running in
r015/r016/r017. The reentry probe additionally pins the snapshot-selected
carrier and verifies its actual module path/hash/AMD64 image, not merely a
kind label or planned package width. I386-parent reentry already proves
the same carrierPID12328 and correct child/parent output/exit23.
No publication or S2 closure is yet claimed.

## Final certification, source selection and publication

r015-hook-I386/AMD64 each pass ten real legacy routes plus visible WINMINE
startup (not gameplay). r016 reciprocal reentry passes actual child/parent
kinds, output order, exit23 and the same carrier PID (12328/15292).
carrier-*.json pins the actual ntvwm.exe module/hash and machine8664 in both
phases. r017 actual64 GUI passes startup-only receipt, detached projection
and authenticated close. No bitness-only second worker is created.

Formal x86 graph no longer links x86 ntvwm.exe. It retains a declarative
source closure for native construction, and imports only an explicitly
selected, machine/subsystem/DLL-bit/hash-checked AMD64 worker into the single
product slot. Without that input it fails, never falls back to x86. Product
staging requires the native worker. The finite five client-binding source
members come from S1's image-matched ledger; an optimized native map may
discard the entire unused legacy dispatch branch and is not a recipe source.
The no-fallback recipe regenerates45 source edges and Ninja reports no work;
both cache copies remain exactly hash5101F09FEC7A2030EA28EDC308AEBC957D31D4BBED0BB54FBE05C41E408348BC.

Failed cleanup attempts remain logs: Windows PowerShell could not load
Get-FileHash under the inherited module path, so the import uses base CLR
SHA256; reading only the optimized map produced an empty member set; the
initial source-closure rewrite omitted the common libraries normally added
by link postprocessing. Those are corrected build-recipe errors, not runtime
passes. A failed linker deleted its mutable cache EXE; restoration uses the
sealed tested identical-source image before regeneration, never rewrites a
sealed package. Final source graph/client closure and artifact identity pass.
r020 repeats staging negatives and additionally rejects I386 formal import
before output: four cases pass. No process or lifecycle claim is attached.

r018-publication preserves the coherent T432 ten images, MIT notice and current
configuration, replaces only verified product binaries and checks all hashes.
r019 deployed DOS MEM plus actual32/64 CMD VER/echo/direct receipts pass.
All ten deployed images and notice still match; no guest/configuration was
replaced. Explicit test-owned process cleanup is not normal retirement proof.
Native build/source/ABI checks and final mirror diff against the preceding
production commit remain clean: no src/mvdm or src/opennt-host body change.
Review/governance/commit/push conclude delivery; S3 remains unadmitted.
