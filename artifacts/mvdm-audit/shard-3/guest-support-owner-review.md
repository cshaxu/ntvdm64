# Win16 support guest source and resources — shard 3

The companion `guest-support-owner-review.csv` covers all 61 pending shard-3 paths in DDEML, WIN87EM, REGEDIT, CTL3DV2 and SHERLOCK, with both original-edition hashes (where present), frozen and checked current SHA-256, concrete build edges for candidates, and explicit unresolved reason for all others. All current hashes match frozen. This is a bounded source review, not a ledger update; none of the 23 candidates is a mirror diff.

Twenty-three candidates have an identified historical **guest build file** role:

- DDEML: four C sources enter makefile OBJ1/OBJ2, CL16 and DDEML.DLL LINK16; `ddeml.dlg` is a copied, conditional resource input to RC16, not proof of the distributed image's bytes.
- REGEDIT: four source files and two directly included headers flow through explicit objects, original LINK16 and REGEDT16.EXE, with debug/retail destination conditions.
- SHERLOCK: default-USA resource source and icon enter RC16/DRWATSON.EXE; `drwatson.h` is directly included by drwatson.c whose object belongs to the linked target. Country/locale selection remains conditional.
- WIN87EM: six included assembly modules feed `emulator.asm` → `win87em.obj` → WIN87EM.DLL. `87emstar.asm` and `emoemqp.asm` enter separate guest libraries; `win87em2.def` is for the distinct historical Win2 `.ex2` target. They are not claims that this variant runs on current NTVDM.

The other 38 rows remain pending. Importantly `ddeml/tests/ddestrs/sources` explicitly says `C_DEFINES=-DWIN32` and `UMAPPL=ddestrs`, so its client/wrapper test sources cannot be blindly classified as 16-bit guest just from their directory. Other test resources, `*.sym`, resource binaries, CTL3DV2 files, generated inputs, and existing guest executables need their own exact consumer and role proof. No runtime image identity, feature behavior or host-source admission is inferred from a guest build edge. Product, existing guest, other shard files and ledgers were not changed; no test or rebuild was run.
