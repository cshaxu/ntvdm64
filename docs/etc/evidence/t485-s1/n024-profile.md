# S15 N024 profile migration

P78 runtime94 verified for delivery. Starting HEAD f3907f7d632ff55be3e4e2c506af18a8d98bc4cc,
accepted runtime93. Baseline profile.c is OpenNT1point/1332nonerror lines.
XP commit a2f6d7c93aa4f11efc51147566d5961c59a8ef89, original path
Source/XPSP1/NT/windows/core/ntuser/kernel/profile.c, blob
b9f10cb901fd5eed0ecb35cffd46c439c86df844. OpenNT blob
a37cb4d5d14f2c441c1ecd4dc5ee115358cc4473. Raw objects in
build/t485-s15/n024-profile.

## Whole-file disposition

Restore original table/type/root declarations and harmless user-string storage.
Restore complete FastGetProfileDwordW, FastGetProfileKeysW,
FastGetProfileStringW, FastGetProfileIntW and FastWriteProfileStringW bodies,
including XP output-parameter and policy-only return semantics. Preserve original
DBG conditions as #error with unconditional post-branch diagnostic recovery.
The previously selected task string wrapper keeps its checked buffer contract;
the registered integer boundary adapts the original XP output parameter to the
existing product callback shape. No second profile implementation is introduced.

The native provider remains read-only, current-process-user scoped, with only
the preexisting private compatibility14/desktop5 identifiers. These are the
existing callback ABI, not an assertion that XP aFastRegMap uses those indices.
The restored table is not used by this finite provider. No user mapping,
impersonation, policy search or writes are provided; requests for those inputs
are rejected. Policy iteration is consumed at this provider, avoiding repeated
retries with an unchanged policy mask. No global policy state is added.

Unconditional omissions are not disguised as invented #error branches:

| Source group | Concrete unsupported ownership |
| --- | --- |
| InitPreviousUserString / CreateProfileUserName / FreeProfileUserName | kernel LUID/logon identity, USER critical region and per-thread pool locks; current process HKCU provider has no impersonated user cache |
| OpenCacheKeyEx | kernel object handles, process access checks, session policy chain and CSRSS/init privileges; existing native read-only provider owns opened HKEY lifetime |
| CheckDesktopPolicy / CheckDesktopPolicyChange | system policy owner and process-global policy refresh, not owned by this private WOW preference reader |
| resource-ID readers/writers, FastGetProfileValue, FastGetProfileIntsW, FastUpdateWinIni | kernel USER resource strings, server metric conversion and write/update service; no corresponding server module/resource table in this composition |

FastGetProfileKeysW is retained despite no current production caller: its
enumeration/reallocation dependencies can use existing native registry and heap
APIs without inventing USER server behavior. The original string writer also
remains, but the provider rejects KEY_WRITE; this is not advertised as a newly
supported write capability. Original guest is unchanged.

## Verification plan

Cached full build, dual publication/hash verification, profile success/absence,
truncation, default, invalid-buffer and handle-retirement tests in the existing
control cohort; actual task initialization/order and real Win16 controls.
This is the third file after P75 Full24/24. Record final census and line audit
before delivery; no S/T closure.

## Final delivery

[Receipt](n024-profile-result.json) and [census](n024-profile-census.json).
Cached full build, dual13-image publication/26hashes, controls40/40,
task-order, init-notification and real Win16 controls pass. Profile cases retain
real native success/absence/truncation/default/handle-count assertions and add
policy-only rejection with unchanged integer, DWORD and string outputs.
The initial build passed after an evidence-edit command failed; the remaining
diagnostic line was then corrected and the final cached build passed before
publication. Final original-line audit:478unchanged nonblank lines, no mismatch.
Two original DBG branches are excluded with #error; no original guest changes.

profile.c: OpenNT1point/1332lines -> XP11points/781lines;
delta+10points/-551lines. Original LF recovery unfolds
the whole-file CRLF difference. Normalized and byte-sensitive final nonerror
metrics agree. Omitted kernel ownership is enumerated above, not hidden by
invented conditions. Extra original unused functions and tables remain.

Whole census: 1332files/792byte-identical/
803nonerror-identical/529different/
7990points/62434lines.
Commit delta0files/0exact/0same/0different/+10points/-551lines.
1318XP/14OpenNT. N0245/9; S15 11/14; overall59/159. Three files since P75 Full.
Next queue.c; no S/T closure.
