# T478 S24 — restore original embedding Registry contracts

Baseline: 3f14f47af. Finding 56 exposes actual project-provider gaps, rather
than a reason to replace the original embedding loop with generic failure
recovery. No original mirror file is changed.

## Confirmed defects and minimum corrections

1. Original UpdateEmbeddingAllKeys calls RegEnumKey(HKEY_CLASSES_ROOT,...).
   WOW preserves that predefined root, but the parent enumerator accepted only
   allocated key objects. It returned ERROR_INVALID_HANDLE at every index and
   the original loop never reached ERROR_NO_MORE_ITEMS. The parent now resolves
   the initialized classes root to its existing section. Opened-key handling,
   long-name ERROR_MORE_DATA and original iteration are retained. The same
   premature allocated-key check rejected original SHELL direct-root SetValue
   and DeleteKey calls; those two existing paths now admit the same classes
   root and continue through the existing resolver/admission/persistence code.
   No additional predefined roots or host Registry access are admitted.
2. Original UpdateClassesRootSubKey and WS32RegSetValue pass strlen, excluding
   NUL, to legacy RegSetValue. Its cbData parameter is ignored by contract.
   The project provider instead stored the supplied byte count, producing
   unterminated strings (and an empty value when cbData is zero). The legacy
   setter now calculates strlen+1. RegSetValueEx, old stored data, mutation
   transaction policy and failure returns are not rewritten.
3. WOW's first allocated DWORD token was 1. Original WS32 Registry thunks
   interpret 1 as the Win16 HKEY_CLASSES_ROOT alias, so a newly opened ordinary
   key was misrouted. Start ordinary tokens at 2; existing non-reuse, translation,
   explicit close and populated shutdown remain. No new handle model.

The [legacy setter contract](https://learn.microsoft.com/en-us/windows/win32/api/winreg/nf-winreg-regsetvaluea)
ignores the supplied length. The original WS32 source itself documents that
Win3.1 rule and passes lstrlen. The
[legacy query contract](https://learn.microsoft.com/en-us/windows/win32/api/winreg/nf-winreg-regqueryvaluea)
counts a stored terminator; it does not promise to repair pre-existing malformed
values. This S corrects new legacy writes, not old overlays by migration.

## Focused evidence

[Provider/owner fixture](../../../test/integration/wow_embedding_root_test.c)
and [runner](../../../test/integration/verify-wow-embedding-root.ps1) compile
the actual parent provider and exact original all-key/parser bodies. The
product-path seam points only to a fresh build-owned file, created with
CREATE_NEW. The all-key callback records keys rather than writing native
win.ini. Reverse conversion uses actual parent create/set/query/close and
persistence, only in that fixture file.

The original root baseline hits the fixture's 16-call budget (10 checks, one
failure); opened-root control terminates. After root enumeration repair, the
corrected parser fixture has 24 checks and three failures for unterminated
class/server and ignored-length writes. Direct-root set/delete controls then
show three failures among 31 checks before their correction. Final focused
result: 31 checks, zero failures, including long-key skip, shutdown/stale/foreign
rejection, preserved parser delimiter mutation, root writes/deletion, handle
retirement and restart persistence. The test budget is not product recovery. A final aggregate replay uses the
same fixture and original bodies with the provider extracted from 3f14f47af:
29 checks, ten failures (two normal callback checks are not reached). Current
provider: 31 checks, zero failures. The runner accepts -ProviderSource for this
replay; baseline bytes are obtained with git show and preserved byte-for-byte.

The existing [binding shutdown fixture](../../../test/mvdm-host/wow_registry_shutdown_fixture.c)
adds a non-1-token assertion. It links the actual selected WOW binding object
and mock parent exports. The pre-fix object fails that assertion; the corrected
object passes populated close, shutdown, stale rejection and non-reuse.
Mock parent exports are explicit; this is not a guest execution claim.

The [independent Win16 witness](../../../test/integration/wow_embedding_guest.asm)
and [isolated runner](../../../test/integration/verify-wow-embedding-guest.ps1)
wait beyond the original two-second freshness window and call genuine KERNEL
GetProfileString for all embedding keys. The previous S23 package reaches
phase 1 and times out inside that call. Root/string-only candidate reaches
SHELL SetValue but reports original Win16 error 7: the first token aliases the
classes root. Final package completes both records, reports phase 2/errors 0, verifies a
six-byte terminated Value string, and exits zero. The witness also checks
legacy string size/NUL, non-aliasing handles, close, direct-root set/query and
recursive SHELL deletion before ordinary exit.

The runner requires absent NTVDM.REG at initial launch, so the embedding
projection enumerates no classes and cannot call the native profile writer.
Subsequent SHELL calls write only the isolated package shadow Registry; no
embedding refresh occurs after adding keys. No shared win.ini, real Registry,
guest source or media is modified. Runtime copies have fresh package identities,
recorded separately from the frozen source package.

## Retained original boundaries and diagnostic limitations

All-key, reverse parser and release IsWinIniHelper bodies match both pinned
original families after comments/#line and the DEBUG-only branch are removed.
Original GETPSZPTR maps direct guest memory and FREEPSZPTR is empty; the parser's
two delimiter writes are original behavior, not an omitted copy-back. They are
retained, not advertised as immutable-input semantics. IsWinIniHelper retains
its MAX_PATH/debug-assert limit; no long-path capability is added. Invalid
selectors, malformed Registry strings and arbitrary private-provider failures
remain outside the repaired normal caller contract. A deliberately persistent
unrelated provider error may still exercise the original loop limitation.

The existing unmodified parent persistence fixture initially exits 33, meaning
its second concurrent writer failed. The old harness did not retain that child's
exact code. A diagnostic copy prints both child exits as zero and passes; a
canonical unchanged-fixture replay also passes. The final-source build passes
as well. The existing harness now prints a failed writer exit code before
returning 32/33, without changing test timing or success criteria. No definitive
cause is claimed.
Those children use opened keys and SetValueEx, outside the modified legacy/root
paths. The first failure remains in the receipt, not erased or counted passed.
S23's separate pre-worker RPC startup risk likewise remains unresolved.

Test-only corrections included linking user32, fixing an escaped server-key
literal and a NASM reserved label, and reading the copied report while the
intentionally stalled guest still held its original file open. These are not
additional product defects.

## Delivery

Final coherent x86/x64 build and all 16 Full gates pass (441552 ms).
The ten worktree release images and manifest exactly match tested package
535a3297-cc41-4af6-8463-2998aee07cfa (0.0.478, protocol 67).
DOS4GW/NESticle remains owner-excluded, skipped rather than passed. No
O:/winnt write or push. S25/finding 57 follows after S24 delivery.

The [receipt](../../../artifacts/mvdm-audit/shard-1/embedding-s24-results.json)
retains staged counterexamples, current checks, selected-object identity,
Full timings and release hashes. Production changes are confined to the two
project-owned provider files; the compared original mirror bodies remain intact.
