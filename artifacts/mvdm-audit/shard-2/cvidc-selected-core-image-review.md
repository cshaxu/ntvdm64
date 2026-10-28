# Shard 2: selected C-VID core image samples

Three pending selected-product source files match both original editions. The companion observer verifies each current compile edge and C-VID archive membership, then binds one map-selected COFF function body to the released worker with relocation words masked. The capture confirms that the cached and released PE executable sections match, while their whole-file SHA-256 values differ; executable-section equality is not whole-file equality or proof of relocation targets.

| Entry | Bound sample and bounded disposition |
| --- | --- |
| `softpc.new/base/cvidc/accessfn.c` | `_PurgeLostIretHookLine`, selected `accessfn.obj`, six bytes with one masked relocation word. Candidate **partial**; vector binding, caller timing, interrupt hook ownership and cleanup remain unproved. Its special access compile profile must not be confused with PROD-elided diagnostic objects. |
| `softpc.new/base/cpu/src/evid/vglob.c` | `_getVideobit_prot_mask`, selected `vglob.obj`, twelve bytes with one masked relocation word. Candidate **partial**; actual video state ownership, updates and reset remain unproved. This exact source path is selected rather than assuming a same-named CCPU file. |
| `softpc.new/base/cvidc/sinit013.c` | `_S_0415_CiSetVideofwd_str_read_addr_00000034`, selected `SINIT013.obj`, 27 bytes with one masked relocation word. Candidate **partial**; a generated setter sample does not establish the full rule dispatch table, selected video mode or actual drawing. |

Captured released `assets/release/ntvdm.exe` SHA-256 is `EF546743393DAD7E45BB7BDA605775524ADB26B94DAE4BDEFE32951EB0510294`; cached worker SHA-256 is `84F72C0B33D0D0BA8E6837A25B296C17D115157FC75A4E9DC394330CC6FAC5E6`. Their executable sections were compared byte-for-byte before binding; non-code differences and relocation target identity were not investigated. A representative matched function does not prove all functions from that object, actual execution, callback/guest-memory safety, mode switch or resource lifetime.

Replay `python artifacts/mvdm-audit/shard-2/cvidc-selected-core-image-observer.py` with `PYTHONDONTWRITEBYTECODE=1` and compare its JSON. No guest, product, installation, shared ledger or build tree was modified.
