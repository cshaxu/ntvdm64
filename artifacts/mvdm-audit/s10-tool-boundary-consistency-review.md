# S10 tool/scaffold classification consistency review

Read-only reconciliation against `host-ch-scope/inventory.csv` and the four per-file ledgers; no ledger or product files were changed. The 72 `tool-or-scaffold-candidate` C/H rows have no pending classification: **57 partial**, **15 excluded** (including the Chinese `整体排除` label). These are physical/source-role dispositions, not a measurement of functionality restored.

| Family | Partial | Profile excluded | Meaning |
| --- | ---: | ---: | --- |
| BDE | 10 | 0 | Original separate debugger-extension product; debugger commands remain outstanding. |
| IEUVDDEx | 14 | 1 | Original separate debugger extension; excluded `mips/helpp.c` is an empty MIPS-specific helper, while the x86 extension is retained as partial. |
| VDMEXTS | 21 | 0 | Original DOS/WOW debugger extension; functionality remains outstanding. |
| VDMUTILS | 12 | 0 | Original standalone FORCEDOS/GRAFTABL/PIFEDIT/WIN utilities; tool and corresponding product behavior remains outstanding. |
| V86 scaffold | 0 | 14 | Retired CPU30 kernel-VDM monitor/scaffolding. Selected CPU40 profile and explicit source policy exclude that executor, not keyboard, display, BIOS or other device functionality as such. |

The excluded MIPS entry cites `shard-1/debugger-extension-trace.md` and the original MIPS/i386 source manifests; the current selected product is x86. The 14 scaffold entries cite `docs/etc/operations/policy/source-policy.md` and `docs/etc/evidence/m0-t473-s1-whole-closure-baseline.md`; original records require re-admission if the executor policy changes. All referenced files exist. No debugger or standalone tool family has been dropped merely because it is absent from worker linkage.

Per-file follow-up lives in [tool boundaries](s10-followup-tool-boundaries.md) and [last three pending roles](s10-tool-last-pending.csv); both paths exist, and the latter has all three data rows. The last PIF result is based on the selected x86 worker's VALID Ninja depfile resolving `nt_pif.c` to `src/opennt-abi/source/public/internal/windows/inc/pif.h`, not the separate FORCEDOS header. That is not evidence about every alternative configuration. The reports identify each original target using the respective `sources`/DEF and identify untested behavior. The artifact references in the later three source-ledger rows and the debugger-extension report are present.

The implementation-stage plan [S12/S17](../../docs/etc/operations/m0-t473-whole-recovery-stage-plan.md) covers DOS startup/services and remaining owner packages respectively, but does not explicitly name individual debugger extensions or standalone utilities. The per-file remaining-proof fields or shard-2 `review_limits` preserve these obligations. To make handoff unambiguous, explicitly link this review and its named utility/debugger packages from S12/S17 when updating the stage plan; do not infer a feature is implemented from source identity or target classification.

No standalone debugger, utility, alternate CPU-profile, PIF interoperability, GUI or guest runtime test was performed in this review. The record confirms role accounting only, not capability restoration.
