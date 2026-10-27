# Native pending follow-up batch 2

Frozen selection: ecd4a604a:artifacts/mvdm-audit/combined-initial-status.csv pending intersection with shard-2 native-pending-owner-review.csv, sorted case-insensitively, slice [30:90]. Exactly 60 paths, without first 30 duplication. Original source and current SHA independently computed. No guest, product, ledger or runtime modification; no test execution.
Proposals: {'pending': 45, 'partial': 15}. Partial requires historical selected object and concrete original decoder/caller/table evidence; no file is full solely due to binary map or identical source. All remaining unresolved conditions are recorded per row.

dasm.c and ega_dump.c are actually compiled in selected cc /DPROD but their guarded main implementation is inactive, not uncompiled. In contrast accessfn.c is compiled with special cc_cvidc_access flags omitting PROD, so its guarded accessor body is active and map symbols are plausible; no claim all routes execute. `cpu/src/evid/vglob.c` is the selected source of cvidc/vglob.obj and is not conflated with the same-named ccpu file.

Evidence snapshot hash:
- `artifacts/mvdm-audit/shard-2/native-pending-owner-review.csv` `d3d97a099c86fc184e6ac305507d90f643a51ce0ca4e0d9182acb083ba4c0a3b`
- `artifacts/mvdm-audit/shard-2/ccpu-selected-link-inventory.json` `bbcdf77520f0635ac07957ed7cec311054f236eb978070704e8b682cd4280a4a`
- `artifacts/mvdm-audit/shard-2/non-ccpu-selected-link-inventory.json` `e7798805875e897bf44bc48068dd9f4070a4511df2353e51fc8ab13c9ccffea2`
