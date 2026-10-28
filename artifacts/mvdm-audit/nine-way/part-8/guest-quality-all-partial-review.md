# Frozen guest partial images and classified DOS source review

Later ledger reconciliation reopened `dos/v86/cmd/nlsfunc/nlsparm.asm`,
`dos/v86/cmd/setver/setver.c`, and `dos/v86/cmd/share/share.asm` from excluded
to pending. The table below preserves the earlier snapshot, not their current
disposition. Candidate Markdown search hits are likewise a historical index;
later reports may add hits without changing the frozen identity observations.

The scope is the saved combined-ledger snapshot SHA256 `5013836C1E9D0B60E6B4BD4C98BB3E219EA3D77628F4EE4AEFFAF61EBDC6F533`: all partial guest images ending .com/.exe/.sys/.dll/.drv/.hlp under DOS v86, DPMI or WOW16, and all classified partial or excluded DOS `.c/.h/.asm/.inc` source rows. This does not cover guest images currently pending, other source extensions, or every repository capability. `guest-quality-all-partial-observer.py` reproduces file hashes and evidence-search indices in `guest-quality-all-partial-observations.json`; it does not run tests. Sources are compared to both original editions; missing reference paths do not prove a capability absent. A file differing from both originals is a mirror diff and must remain partial. The canonical retained-code percentage, when available for matching current bytes, is text similarity and does not prove behavior.

## Current partial guest images

| Path | Current SHA-256 | Existing witness and limit |
|---|---|---|
| `dos/v86/cmd/mem/mem.exe` | `C4582B1E0738F3AAF89E17DE0F940CAA7E4B3D77A21FCCD6FF91B7E075BDF6EB` | bound: MEM totals and exit 0; `docs/etc/evidence/m0-t406-s1-physical-mapping-design.md` |
| `dos/v86/cmd/qbasic/edit.com` | `E0A0B24FEE4037CB050670661C30AD7ECC0EA9483938152FCA3D807C443E8A46` | bound: welcome and Esc; job cleanup, no normal exit; `docs/etc/evidence/m0-t406-s1-physical-mapping-design.md` |
| `dpmi/486/dosx.exe` | `C5AF29A29ABF167B243DAABF877459E8278B8C9A339BF8E1E2576EAD5F6CEEFF` | historical identity and entry; timing binding bounded: same record identifies SHA and r048 DEM open/read/entry transfer; confirm per-run immutable identity before stronger claim; `docs/etc/evidence/m0-t404-s2-dosx-bop-fd-contract-audit-001.md` |
| `wow16/user/k/daytona/USER.EXE` | `6746088F2A87CD366E4FF5DA424B39B5BD91586D5F7570AFBF3C05BCA94E935C` | bound historical: T447 S2 CBT nested/veto/destroy callback; not all USER; `artifacts/mvdm-audit/nine-way/part-1/guest-identity-runtime-witness-review.md` |
| `wow16/winsock/WINSOCK.DLL` | `629E86C59C83F66D3623388B9ED6AE9D5A664979E588A9EB45B8A54441126FD9` | bound historical optional image: T467 S2 synchronous loopback and blocking hook, optional staged image; `artifacts/mvdm-audit/nine-way/part-1/guest-identity-runtime-witness-review.md` |

DOSX historical record supplies the same C5AF… input hash and an r048 DEM open, full load and entry transfer; it does not explicitly establish in this review that the hash was frozen continuously across that exact r048 run. Treat it as bounded historical witness and do not infer DPMI lifecycle or current-release coverage. T467 WINSOCK witness uses a disposable optional staged image, not necessarily current installation. No per-file feature completion percentage follows.

## DOS source rows

| Path | Status | Diff vs both original references | Current canonical text estimate | Candidate report index |
|---|---|---|---|
| `dos/command/cmd.c` | partial | yes | 92.31% | `artifacts/mvdm-audit/shard-0/command-home-drive-audit.md` |
| `dos/command/cmd.h` | partial | yes | 97.81% | No basename hit in audited Markdown; follow call chain. |
| `dos/command/cmdconf.c` | partial | yes | 99.75% | `artifacts/mvdm-audit/shard-0/bin86-country-config-review.md`; `artifacts/mvdm-audit/shard-0/command-config-root-audit.md`; `artifacts/mvdm-audit/shard-1/package-uuid-dos-bootstrap-audit.md` |
| `dos/command/cmddata.c` | partial | yes | 91.43% | `artifacts/mvdm-audit/shard-0/command-home-drive-audit.md`; `artifacts/mvdm-audit/shard-2/command-state-pif-redirection-review.md` |
| `dos/command/cmddisp.c` | partial | no | not established / not applicable | `artifacts/mvdm-audit/nine-way/part-1/mvdm-core-review.md`; `artifacts/mvdm-audit/parallel-native-role-review.md`; `artifacts/mvdm-audit/shard-1/command-exit-owner-audit.md` |
| `dos/command/cmdenv.c` | partial | yes | 99.81% | `artifacts/mvdm-audit/nine-way/part-1/external-rtl-review.md` |
| `dos/command/cmdexec.c` | partial | yes | 91.04% | `artifacts/mvdm-audit/findings.md`; `artifacts/mvdm-audit/shard-1/codepage-concurrency-audit.md`; `artifacts/mvdm-audit/shard-1/codepage-handoff-design.md` |
| `dos/command/cmdkeyb.c` | partial | yes | 99.30% | `artifacts/mvdm-audit/shard-3/cmd-keyboard-layout-role-review.md`; `artifacts/mvdm-audit/shard-3/external-device-provider-review.md`; `artifacts/mvdm-audit/shard-3/README.md` |
| `dos/command/cmdmisc.c` | partial | yes | 94.99% | `artifacts/mvdm-audit/shard-0/command-home-drive-audit.md`; `artifacts/mvdm-audit/shard-2/dem-divergence-registration-reconciliation.md`; `artifacts/mvdm-audit/shard-3/cmdsvc-abi-role-review.md` |
| `dos/command/cmdpif.c` | partial | yes | 99.52% | `artifacts/mvdm-audit/addon-layout-review.md`; `artifacts/mvdm-audit/shard-2/command-state-pif-redirection-review.md`; `artifacts/mvdm-audit/shard-2/oemuni-process-path-review.md` |
| `dos/command/cmdredir.c` | partial | yes | 99.58% | `artifacts/mvdm-audit/shard-1/package-uuid-dos-bootstrap-audit.md`; `artifacts/mvdm-audit/shard-2/command-state-pif-redirection-review.md` |
| `dos/dem/dem.c` | partial | yes | 97.62% | `artifacts/mvdm-audit/shard-2/dem-initialization-dispatch-file-review.md` |
| `dos/dem/dem.h` | partial | yes | 98.41% | `artifacts/mvdm-audit/shard-1/dem-directory-label-audit.md`; `artifacts/mvdm-audit/shard-2/dos-command-dem-abi-review.md`; `artifacts/mvdm-audit/shard-2/guest-declaration-resource-owner-review.md` |
| `dos/dem/demdasd.c` | partial | no | not established / not applicable | `artifacts/mvdm-audit/parallel-native-role-review.md`; `artifacts/mvdm-audit/shard-2/parallel-review-current.md`; `artifacts/mvdm-audit/shard-2/parallel-review-next.md` |
| `dos/dem/demdasd.h` | partial | yes | 99.58% | No basename hit in audited Markdown; follow call chain. |
| `dos/dem/demdata.c` | partial | yes | 20.00% | `artifacts/mvdm-audit/shard-3/README.md` |
| `dos/dem/demdir.c` | partial | no | not established / not applicable | `artifacts/mvdm-audit/shard-1/dem-directory-label-audit.md`; `artifacts/mvdm-audit/shard-1/dem-directory-review.md` |
| `dos/dem/demdisp.c` | partial | yes | 99.04% | `artifacts/mvdm-audit/shard-1/dem-directory-label-audit.md`; `artifacts/mvdm-audit/shard-1/dem-directory-review.md`; `artifacts/mvdm-audit/shard-2/dem-initialization-dispatch-file-review.md` |
| `dos/dem/demerror.c` | partial | no | not established / not applicable | `artifacts/mvdm-audit/parallel-text-dem-role-review.md`; `artifacts/mvdm-audit/shard-3/dem-error-role-review.md`; `artifacts/mvdm-audit/shard-3/dem-hard-error-audit.md` |
| `dos/dem/demfcb.c` | partial | yes | 98.38% | `artifacts/mvdm-audit/shard-0/dem-search-fcb-lifetime-audit.md`; `artifacts/mvdm-audit/shard-1/dem-directory-label-audit.md`; `artifacts/mvdm-audit/shard-1/dem-directory-review.md` |
| `dos/dem/demfile.c` | partial | yes | 97.53% | `artifacts/mvdm-audit/shard-1/dem-directory-label-audit.md`; `artifacts/mvdm-audit/shard-1/dem-directory-review.md`; `artifacts/mvdm-audit/shard-1/dem-media-sft-lifecycle-audit.md` |
| `dos/dem/demgset.c` | partial | yes | 97.96% | `artifacts/mvdm-audit/shard-1/dem-media-sft-lifecycle-audit.md` |
| `dos/dem/demhndl.c` | partial | yes | 93.29% | `artifacts/mvdm-audit/shard-2/dem-divergence-registration-reconciliation.md`; `artifacts/mvdm-audit/shard-3/redir-service-role-review.md` |
| `dos/dem/demioctl.c` | partial | no | not established / not applicable | `artifacts/mvdm-audit/parallel-native-role-review.md`; `artifacts/mvdm-audit/shard-2/parallel-review-current.md`; `artifacts/mvdm-audit/shard-2/parallel-review-next.md` |
| `dos/dem/demlabel.c` | partial | no | not established / not applicable | `artifacts/mvdm-audit/parallel-controls-directory-role-review.md`; `artifacts/mvdm-audit/shard-1/dem-directory-label-audit.md`; `artifacts/mvdm-audit/shard-1/dem-directory-review.md` |
| `dos/dem/demlock.c` | partial | no | not established / not applicable | `artifacts/mvdm-audit/parallel-native-role-review.md`; `artifacts/mvdm-audit/shard-2/parallel-review-current.md`; `artifacts/mvdm-audit/shard-2/parallel-review-next.md` |
| `dos/dem/demmisc.c` | partial | yes | 99.50% | `artifacts/mvdm-audit/shard-1/dem-media-sft-lifecycle-audit.md`; `artifacts/mvdm-audit/shard-1/package-uuid-dos-bootstrap-audit.md` |
| `dos/dem/demsrch.c` | partial | yes | 99.41% | `artifacts/mvdm-audit/external-kernel-vdm-boundary.md`; `artifacts/mvdm-audit/shard-0/dem-search-fcb-lifetime-audit.md`; `artifacts/mvdm-audit/shard-1/dem-directory-label-audit.md` |
| `dos/dem/dosdef.h` | partial | no | not established / not applicable | `artifacts/mvdm-audit/guest-declaration-resource-role-review.md`; `artifacts/mvdm-audit/shard-2/dos-command-dem-abi-review.md`; `artifacts/mvdm-audit/shard-2/guest-declaration-resource-owner-review.md` |
| `dos/v86/cmd/command/init.asm` | partial | yes | 100.00% | `artifacts/mvdm-audit/nine-way/part-1/command-mirror-runtime-review.md`; `artifacts/mvdm-audit/nine-way/part-3/simulator-role-review.md`; `artifacts/mvdm-audit/nine-way/part-5/bios-resource-family-review.md` |
| `dos/v86/cmd/command/rdata.asm` | partial | yes | 100.00% | `artifacts/mvdm-audit/nine-way/part-1/command-mirror-runtime-review.md`; `artifacts/mvdm-audit/nine-way/part-8/existing-classifications-sample101-300-review.md`; `artifacts/mvdm-audit/nine-way/part-8/sample101-300-command-diff-correction.md` |
| `dos/v86/cmd/command/stub.asm` | partial | yes | 100.00% | `artifacts/mvdm-audit/kernel31-source-role-review.md`; `artifacts/mvdm-audit/nine-way/part-1/command-mirror-runtime-review.md`; `artifacts/mvdm-audit/nine-way/part-8/existing-classifications-sample101-300-review.md` |
| `dos/v86/cmd/nlsfunc/nlsparm.asm` | excluded | no | not established / not applicable | No basename hit in audited Markdown; follow call chain. |
| `dos/v86/cmd/setver/setver.c` | excluded | no | not established / not applicable | No basename hit in audited Markdown; follow call chain. |
| `dos/v86/cmd/share/share.asm` | excluded | no | not established / not applicable | `artifacts/mvdm-audit/shard-1/dos-utility-family-review.md` |
| `dos/v86/tools/src/h2inc/h2inc.c` | excluded | no | not established / not applicable | `artifacts/mvdm-audit/nine-way/part-2/dos-shared-inputs-review.md`; `artifacts/mvdm-audit/shard-2/dos-v86-build-tools-review.md`; `artifacts/mvdm-audit/shard-2/parallel-review-current.md` |
| `dos/v86/tools/src/stripdd/stripdd.c` | excluded | no | not established / not applicable | `artifacts/mvdm-audit/shard-2/dos-v86-build-tools-review.md`; `artifacts/mvdm-audit/shard-2/parallel-review-current.md` |
| `dos/v86/tools/src/stripz/stripz.c` | excluded | no | not established / not applicable | `artifacts/mvdm-audit/shard-2/dos-v86-build-tools-review.md`; `artifacts/mvdm-audit/shard-2/parallel-review-current.md` |

All 24 observed dual-original diffs are partial in this snapshot; 8 unchanged original sources remain partial for separate functionality concerns. Six excluded source rows are unchanged; their status is not proven correct by hash identity. Report hits are a search index only: name hits can be incidental and must be read before using them as behavior evidence. Original-source inclusion, a link map, or an isolated test stub does not establish full observable behavior, failure handling or lifecycle. Existing focused reports such as `shard-2/command-state-pif-redirection-review.md` separate PIF fixture stubs, the thread adapter, and missing session/cleanup witnesses.

No new build, runtime, guest change, product change or shared-ledger change occurred. The frozen ledger is an audit snapshot; compare hashes before applying conclusions to a newer ledger or release.
