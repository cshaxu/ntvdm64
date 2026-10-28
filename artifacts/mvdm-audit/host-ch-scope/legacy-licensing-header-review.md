# Historical licensing headers: bounded physical roles

Coordinator update: all three files now have scoped physical-file dispositions in the captured NT product inventory. The frozen identity ledger records current bytes matching both original editions for each; the observer's direct original comparison was narrower. The separate historical licensing profile and external-provider applicability remain open.

The pending host-candidate inventory includes `softpc.new/base/inc/flexlmif.h`, `lm_attr.h` and `lm_clien.h`. These describe historical SoftPC licensing dialog or licensing-client interfaces, not DOS guest services. This review neither imports a licensing provider nor decides its permissions or future applicability; it only examines the source already in this tree and the captured worker/WOW32 x86 release profile.

`base/bios/reset.c:86-88,954-974` and `base/support/terminat.c:39,135-136` show that the original host had conditional `LICENSING` initialization/termination paths. The BIOS path includes `host_lic.h`, not the three exact headers audited here; absence of the latter therefore does not prove every licensing mechanism absent. `flexlmif.h:21-55` defines its dialog interface only under `LICENSING`. The other two headers carry historical licensing-client declarations/attributes; this evidence does not establish the client provider, license or use in another build.

Run `python artifacts/mvdm-audit/host-ch-scope/legacy-licensing-header-observer.py` to collect hashes, exact literal C/H include searches within `src/mvdm/softpc.new`, and current Ninja dependency snapshots. On this run, no direct literal include of any of the three was found; among 639 VALID worker and 229 VALID WOW32 x86-release dependency blocks, none listed an exact header path. Captured hashes and counts are in `legacy-licensing-header-observation.json`. The inventory records `flexlmif.h` identical to its recorded OpenNT reference; the other two were not independently compared with historical copies here.

| Physical file | Candidate decision for captured profile | Unresolved boundary |
| --- | --- | --- |
| `flexlmif.h` | Scoped physical exclusion, only if the ledger explicitly permits per-profile exclusion and retains historical licensing behavior independently. | No assertion about non-release profiles, dynamic or external consumers, or licensing dialog behavior. |
| `lm_attr.h` | Same scoped physical exclusion candidate. | Original client dependency/provider and rights were not established; no claim of recovery. |
| `lm_clien.h` | Same scoped physical exclusion candidate. | Original client dependency/provider and rights were not established; no claim of recovery. |

If a ledger exclusion means all build profiles or the whole historical feature, all three remain pending. This is negative dependency evidence with explicit scope, not behavioral execution or a current provider-equivalence claim. No product, guest or shared ledger was changed.
