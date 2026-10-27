# Write original guest input dependency review

Scope: pending Write rows in shard 2. Source-owner classification only, not guest capability or current binary lineage. No guest rebuilt or changed.

Rows 66; historical guest-build-source-file-only 32; pending 34; mismatched hashes 0.

Original makefile lines 89–113 define OBJ1–OBJ6; lines 120–124 select MASM/CL16; 145–152 depend on those groups and call link16 @write.lnk followed by rc16. Actual write.lnk lines 1–20 enumerate objects (not merely a guessed link dependency); C/assembly rows have explicit recipe mapping or same-name suffix rule and linked object evidence in the CSV. DBCS/OS2/debug variations remain conditional; source identity does not prove these variants shipped.

Resource entries map to write.rc statements and makefile resource prerequisites. Original makefile copies localized RES_DIR write.rc then compiles it; current local write.rc is a generated view, so its own source identity remains pending. For headers, direct includes by a selected linked source establish an original source dependency, not unconditionally enabled execution.

Unproven variants, original object/executable/resource binaries, scripts and generated outputs remain pending. This review does not assert file is compiled into an installed immutable guest image.

SHA256 of evidence:
- `src/mvdm/wow16/write/makefile` `d5eb390f308326d42ecb1b81ec13136c8ae01c1adf2623d64a45d489be60ad83`
- `src/mvdm/wow16/write/write.lnk` `bcd0a358acd9aa0271b1eb17bb3929792c9fc33f2e2dd643ad992a40bd74d151`
- `src/mvdm/wow16/write/write.rc` `844a7f4ac20bf3be1e1985ba557ed986c427d8a4316b8a3509eecaa1c4957d66`
- `artifacts/mvdm-audit/shard-2/file-audit.csv` `52908a9f74491970439c712f6464535696f98dbf878bdf140e54650a0573a8b1`

Local `write.rc` and original `messages/usa/write.rc` both have SHA-256 `844a7f4ac20bf3be1e1985ba557ed986c427d8a4316b8a3509eecaa1c4957d66`; this establishes byte identity for the USA resource snapshot, not other LANG/DBCS variants. Makefile also regenerates write.lnk using write.sed (lines 375–376); the audited checked-in write.lnk is the observed link input.
