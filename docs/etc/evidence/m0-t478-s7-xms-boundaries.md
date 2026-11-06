# T478 S7 — original XMS block boundary correction

## Scope, original contract and disposition

Baseline: 75c8d917e. The owner authorizes actual original recovery, minimal
mirror differences/local implementation and removal of displaced incomplete
code, without extra tolerance or new capability. S7 owns finding 10 only.

This is an inherited original-host defect, not a missing dispatch/provider or
an autonomous replacement to delete. Both OpenNT families' xms.486/xmsblock.c
skip allocation for DX=0 and skip reallocation for unchanged size, but still
read unassigned output locals. The current project retained those statements.
Original xmsdisp.c already connects the services; ordinary allocation/free,
move and growth already work. No additional provider or adapter is needed.

Original contract evidence (read-only OpenNT/base/mvdm and matching OpenNT-4.5):

- himem4.asm::AllocExtMemory calls XMS_ALLOCBLOCK even for zero size, accepts
  nonzero AX as the base, and records a zero-length handle. FreeExtMemory skips
  host freeing when the recorded length is zero. This is already a handle-only
  operation, not a reason to allocate an artificial minimum-sized block.
- XMSInit in xms.486/xms.c fixes the suballocator base at 1 MiB + 64 KiB. Even
  its non-i386 reservation route explicitly rejects a different returned base.
  Use that existing base as the deterministic anchor of a zero-length handle.
  This value is a minimal host correction inferred from the existing pool and
  handle contract, not a recovered original initializer or a new allocation.
  It grants no storage or permission to dereference bytes; later growth uses
  the original allocator and its existing occupancy/failure rules.
- himem4.asm::ReallocExtMemory returns early when lengths match. Independently,
  original SAReallocate explicitly returns OriginalAddress for equal rounded
  sizes. Initializing NewAddress from BaseAddress preserves that original
  no-change result if the host service itself receives an equal-size request.

## Minimal change and rejected alternatives

DIV-870 in xmsblock.c makes only two executable changes: initialize BaseAddress
to the existing XMS pool base, and initialize NewAddress to the supplied base.
The original guards, allocation calls, failure returns and guest register ABI
remain unchanged. Two short local comments and the mirror register explain why.
No guest, dispatcher, allocator, adapter or public API changes.

A zero initialization would make HIMEM reject an existing zero-length handle
operation as out-of-memory. Removing the zero-size guard would instead invoke
allocator bookkeeping/commit logic for an operation intentionally owning no
storage. Neither is necessary. New validation/retries, an allocated dummy block,
a new pool-query API and a second XMS implementation are all avoided. There is
no displaced incomplete local implementation to delete in this boundary; the
independent move-memory adapter is still used and is not S7 cleanup material.
Suballocator callback rollback remains the separate proposed S8.

## Focused verification

`tests/observation/verify-xms-block-boundaries.ps1` compiles the actual selected
xmsblock.c with the product XMS flags and links original-mvdm-host-suballoc.lib.
The fixture substitutes register storage and backing callbacks, not the
allocation/reallocation logic. Unused move/query dependencies resolve only to
an aborting seam; no successful fake lifecycle is supplied. Map checks bind the
three tested original functions to provider.obj.

- Baseline source extracted from HEAD: 139 assertions, 35 failures. Failures
  identify repeated zero-address results and equal-size return values.
- Corrected source: 139 assertions, zero failures. Coverage includes repeated
  zero requests with no allocation/commit, equal-size address/data retention,
  zero-to-one growth while another block occupies the anchor, normal growth,
  oversized failure, free and complete free-byte restoration.
- Initial fixture link lacked unused move/query dependencies; this was a test
  composition failure, resolved by explicit fail-fast out-of-scope aliases.
  It is not recorded as a product defect or passing run.

`tests/observation/verify-xms-block-guest.ps1` uses the existing private-desktop
observer and New-PhysicalRuntimeStage/owned cleanup. Original HIMEM is unmodified.
The independently authored xms_block_boundaries.asm repeats 32 complete zero-size
allocate/lock/unlock/query/equal-size/grow-to-1KB/equal-size/free lifecycles and
checks free-memory restoration after each. Result: XMS-BOUNDARIES-PASS and exit 0.
The selected deterministic lock address is 0011:0000; it is never dereferenced.
Guest equal-size success tests HIMEM's public behavior; direct host equal-size
execution is established by the provider fixture, not claimed from that guest run.

The Existing case reuses xms_capability.asm with DEFAULT_INT15; allocation,
real-mode/XMS moves, growth with retained data, release and A20 marker pass with
exit 0. First attempt failed before launch due to an assembly-argument array
error in the new runner; the corrected Existing-r2 passed. No guest/runtime
assertion was weakened to obtain a pass.

## Product delivery status

Evidence root: build/t478-s7. x86 worker/VDMREDIR/hooks/WOW32 and native x64
worker/frontend/monitor/launcher/service/hook builds completed successfully.
Final coherent isolated package: 25974683-9d30-4f02-a62c-32e8ca3df424, application 0.0.478,
protocol 67. Both final guest cases pass. All 16 selected Full gates pass in
440770 ms; DOS4GW/NESticle remain the owner-excluded non-pass,
not an added success claim. The final supervisor returned zero after cleanup.

The exact tested ten files and manifest are published to this worktree's
assets/release and SHA-256 verified. Prior files are backed up under
build/t478-s7/release-before. No O:/winnt write, push, original guest mutation
or main-worktree change. Source identities, provider and guest results, Full
timings, publication identity and unsuccessful attempts are retained in the
[compact result record](../../../artifacts/mvdm-audit/shard-0/xms-boundary-s7-results.json).

Reproducible checked-in entrypoints:

- verify-xms-block-boundaries.ps1 -OutputRoot build/t478-s7/provider-final2;
  -ProviderSource selects the extracted baseline for the negative comparison.
- verify-xms-block-guest.ps1 -PackageRoot build/t478-s7/package-final
  -Observer build/t478-s7/observer.exe -OutputRoot build/t478-s7/final-guest-Boundaries
  -Case Boundaries; the Existing case uses its corresponding fresh output root.
- Invoke-ProductVerification.ps1 -Suite Full with that same package, the existing
  observer/terminal/windows tools, test-cache, independent G7.COM, retained S2 WOW
  baseline and selected native image paths. Exact local orchestration is retained
  under build/t478-s7/full-final.ps1; final outputs are verify-final-Full.
- Product builds use worker run-ninja-parallel.cmd targets ntvdm.exe,
  VDMREDIR.dll and nthook32.dll, CMake x86 wow32, and native x64 worker/frontend/
  monitor/launcher/service/hook64. Version-bearing x86 RPC fixture targets are
  rebuilt before final acceptance.

S7 closes finding 10 with this minimal original-host correction and verified
local P delivery. S8 remains proposed, not admitted. Broader T478 inventory
qualification remains open. Documentation governance and diff checks gate commit.

## Retained unsuccessful attempts and final-version preparation

The initial package passed the focused guest cases and the first seven Full
gates, then version-negatives correctly rejected inherited APP_VERSION 0.0.477
against active T478. The old S4 run occurred during intermission, so that run did
not exercise this active-task consistency assertion. The checker is not changed
or bypassed. APP_VERSION advances to 0.0.478 as required by version.h; protocol
67 and the RPC ABI do not change. All product components and version-bearing test clients were rebuilt. The final
coherent package repeated guest and Full acceptance successfully before publication.

A local exception-number correction from provisional 869 to 870 avoided the
existing historical bare DIV-869 entry. Its recompilation changed only PE/debug
timestamps (four bytes), with identical executable/data/relocation/resource
sections; this is documented in comment-id-identity.json. The final version
rebuild includes the corrected comment, so the earlier package is not delivered.

The provider metadata recorder initially read ExitCode after disposing its
Process object; the underlying fixture had already reported 139/0. The runner
now saves ExitCode before disposal, and provider-final2 verifies both the actual
fixture and wrapper succeed. This harness failure is retained separately.
