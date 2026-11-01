# S12 — unselected alternatives of PROD branches

DIV-503 marks the unused `#else` / `#elif` alternatives of simple positive
PROD conditions in ten product-selected MVDM host C files. All observed
commands for these files define PROD. Eleven bodies matched original OpenNT;
the twelfth, the old non-PROD exception-environment macro in `c_xcptn.c`,
contained an earlier host-getenv declaration adaptation. Its selected
DIV-271 production path remains untouched. All original conditional shells
and active alternatives remain. The [per-branch ledger](s12-prod-else-subsets.jsonl)
records original body hashes, baseline and this prior adaptation.

Independent reconstruction verified ten files and 12 replacements with zero
unrelated byte changes. Cached NTVDM rebuild passed; after applying the
published identity, all ten product images were identical to the published
set or differed only in PE link timestamps. No runtime regression or republish
was required for unchanged code. The guest was not modified. Compound
conditions, other configurations and headers are still under audit.
