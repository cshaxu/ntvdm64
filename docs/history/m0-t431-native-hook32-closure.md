# M0 T431 native Hook32 closure

## Owner acceptance and scope revision

On2026-10-05 the owner reports nthook32 runs correctly, requests preservation
of that baseline and opens a separate T for NTVWM32/64 and nthook32/64.
T431 closes within its delivered x86 scope. S3 concludes source/design and
replanning only; planned S4–S6 were never admitted or implemented and transfer
to T432. This is an explicit scope transfer, not a claim of cross-width closure.
General run16/NTCON/NTMON x64 migration remains a separate queued candidate.

## Accepted baseline and evidence

Production P2 commit12160c657 is retained, with coherent nine-image
build/M0-T431/S2/r027-runtime published to O:/winnt/system32. S2/r031-publication
holds the deployment hashes and preceding recovery set. No artifact, guest,
configuration, running process or published file changes in this closure.
MSVC14.43/SDK22621, x86 /MT CCPU40, APP0.0.427/RPC38/I/O25 stay unchanged.

[S2 evidence](../etc/evidence/m0-t431-s2-nthook32-implementation.md) retains
shared Run16/Hook search and original classifier, suspended-child installer,
context-only launcher seed, native CUI/GUI propagation, real CMD/MEM parent
return,93 installer assertions, selected lifetime checks, Console17/Window17,
retained independent WOW frontiers and published smoke/identity. Owner
acceptance supersedes earlier personal-verification-pending status, not the
explicit evidence limitations.

## Retained limitations and successor

Default private-desktop prepare-text error87, unproved API/security/debug
combinations and failed extra-runner prerequisites/assertions remain recorded
in S2 evidence, not silently passed. No cross-width propagation or Hook64
runtime proof exists. Source-only classifier/ABI/Detours conclusions transfer
through the [T431 design](../etc/operations/t431-native-launch-hook-design.md)
to the [T432 plan](../etc/operations/t432-dual-width-native-workers-hooks-plan.md).
T432 reuses this work rather than restarting research or changing the baseline.

WINMINE's retained visible UI and SOL/WRITE's known error frontiers remain
independent; universal WOW functionality is not claimed. Unrelated concurrent
proposal/TODO edits are excluded. This transition is documentation-only;
governance/link/diff checks apply, with no new runtime test result claimed.
