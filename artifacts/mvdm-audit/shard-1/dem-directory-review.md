# S10 shard 1: DOS directory and volume-label calls

The [observer](dem-directory-observer.py) and [evidence](dem-directory-evidence.json)
bind three previously pending shard-1 sources to both original editions and
both frozen catalogs. Their current source bytes match both originals. All
three objects are selected with valid dependencies. The release and installed
NTVDM hashes match; its executable sections match the cached map-producing
worker, even if the complete cached executable has a different identity.

The actual `apfnSVC` image slots 3, 4, 6, 7, 19, 24 and 44 resolve to original
create, directory, FCB and current-directory owners. Nine selected mapped
bodies match their object code outside relocation words; three callers are in
files with existing mirror differences. Ten relevant
relocations were resolved individually: create/remove and current-directory
operations reach their OEM host boundaries; both normal create and FCB create
reach `demCreateLabel`; FCB delete reaches `demDeleteLabel`; label handling
reaches the Windows OEM conversion and volume-label imports. This is linked
dispatch evidence, not proof that every guest path or return code was executed.

The prior [guarded label review](dem-directory-label-audit.md) reported an
original direct-object NUL overread. A fresh isolated x86 test using the
identical selected object reproduced the exact read into a guard page before
the volume API; its [result and input hashes](dem-directory-guard-evidence.json)
are retained. No actual volume label was changed. The normal create path now
copies the pathname into a fixed host buffer, while FCB uses a guest-memory
alias. Neither fact alone proves whether a normal guest can reach an unsafe
boundary or suffer an observable effect. This is an inherited source behavior,
not evidence that a previously working user flow regressed.

| File | Recommendation | Remaining evidence |
| --- | --- | --- |
| `demdir.c` | Partial candidate | Guest pointer mapping/lifetime, missing and invalid drives, OEM paths, environment update failures, EA branch, rollback and reentry. |
| `demlabel.c` | Partial candidate | Actual FCB and normal-create input boundaries and cleanup; volume success/failure behavior on an authorized isolated volume, without modifying a user's media. The guard fixture alone is not guest reachability proof. |
| `demmsg.c` | Pending | The release object's debug-only `demPrintMsg` is absent. Verify the intended separately selected debug profile before deciding this file's role; the release graph listing alone is not a debug feature. |

All three retain 100% original source text; a complete restored-behavior
percentage is not measurable from link evidence or the bounded guard test.
`demfile.c`, `demfcb.c` and `demdisp.c` have separate mirror differences and
are supporting call-chain inputs, not dispositions in this shard group. No
guest or product source was changed; the fixture did not start a DOS worker.
