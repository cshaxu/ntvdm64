# Part 1 external: native headers, GDI utilities and other OS providers

The remaining 298 assigned keys in the initial non-NLS/non-RTL/non-BASE partition are recorded per-key in `external-native-other-evidence.json`: original edition/path/hash, coarse family, known prior priority association, every same-basename project candidate (except suppressed generic names), candidate hash and exact current compile edge, plus normalized four-graph VALID dependencies for byte-identical project candidates. Replay `external-remaining-observer.py` then `PYTHONDONTWRITEBYTECODE=1 python -B artifacts/mvdm-audit/nine-way/part-1/external-native-other-observer.py`. The 298 comprise 143 header/ABI candidates, 93 GDI utility/other candidates and 62 other native/kernel/provider candidates. This is a source/owner investigation, not a claim every row's behavior is audited. All retain `pending-specific-owner-and-semantics`.

There are 56 keys with a byte-identical project same-basename copy; 15 of those copies have actual VALID object dependencies in the scanned build graphs, listed below. Full equality is a file identity fact, not proof that every declaration or original OS service is active. Conversely, 21 generic names were intentionally not matched by basename due to collision; they remain unresolved, not absent. Three records had a same-basename compile candidate that is **not** byte-identical (two unrelated old `vdm.c` paths versus the BASE client copy; `ntgdi/mf3216/text.c` versus NTCON/NTUSER files). These are collisions, not source lineage or proof of restored behavior.

| External key | Original path | Exact project copy | VALID object references |
| --- | --- | --- | ---: |
| `SRC:da55071f9c35fb567510` | `base/win32/inc/base.h` | `src\opennt-host\base\win32\inc\base.h` | 33 |
| `SRC:c4cf8cc17d3e1e15dc80` | `base/win32/inc/basemsg.h` | `src\opennt-host\base\win32\inc\basemsg.h` | 37 |
| `SRC:f9ae488370f3830b7bf2` | `base/win32/inc/basevdm.h` | `src\opennt-host\base\win32\inc\basevdm.h` | 44 |
| `SRC:01f0ce71f68080b0b8aa` | `nt/private/inc/crypt.h` | `src\opennt-abi\source\public\internal\ds\inc\crypt.h` | 5 |
| `SRC:906cad596e405fda73ea` | `nt/private/inc/debugfmt.h` | `src\opennt-abi\source\public\internal\base\inc\debugfmt.h` | 19 |
| `SRC:070b6ef0a87ad349368c` | `nt/private/inc/dlcio.h` | `src\opennt-abi\source\private\inc\dlcio.h` | 5 |
| `SRC:2f0188be0b1517bc0aa7` | `nt/private/inc/nettypes.h` | `src\opennt-abi\source\public\internal\base\inc\nettypes.h` | 6 |
| `SRC:3e81f57c6368c0735594` | `nt/private/inc/ntddvdm.h` | `src\opennt-abi\source\public\internal\base\inc\ntddvdm.h` | 1 |
| `SRC:f4934116a2f4e6294b5b` | `nt/private/inc/packoff.h` | `src\opennt-abi\source\public\internal\base\inc\packoff.h` | 34 |
| `SRC:56e4b41d40b65c0aa4cd` | `nt/private/inc/packon.h` | `src\opennt-abi\source\public\internal\base\inc\packon.h` | 34 |
| `SRC:49c2fd22777f73934c2d` | `nt/private/inc/prefix.h` | `src\opennt-abi\source\public\internal\ds\inc\prefix.h` | 8 |
| `SRC:55f37ec143b0ab3a83ba` | `nt/private/inc/smbgtpt.h` | `src\opennt-abi\source\public\internal\base\inc\smbgtpt.h` | 14 |
| `SRC:6a25b98594d5989f0e82` | `nt/private/inc/smbmacro.h` | `src\opennt-abi\source\public\internal\base\inc\smbmacro.h` | 6 |
| `SRC:7ca9456090f800c99ad0` | `nt/private/inc/smbtypes.h` | `src\opennt-abi\source\public\internal\base\inc\smbtypes.h` | 14 |
| `SRC:f491f4fc9a18546e39c3` | `nt/private/inc/tstr.h` | `src\opennt-abi\source\public\internal\base\inc\tstr.h` | 16 |

Project-owned BASE/ABI declarations can carry shared structures and constants; modern Windows remains authority for actual kernel, desktop, I/O and opaque resources. The three exact BASE headers support the selected BASE client/server closure; `dlcio.h` and related interfaces intersect the independently audited VDMREDIR provider boundary, and network/packing declarations cannot be assumed unused simply because the old external path itself is not compiled. Historical GDI diagnostic utilities, loader/system interfaces and device-provider records need a named operation and real owner before any recovery decision. `external-priority-review.md` separately identifies 20 loader, sound, I/O and CRT keys from this broader census; priority is an audit lead, not a runtime pass.

No full cross-version ABI/layout comparison, registered-exception review, native system API, device, guest or product execution was performed. Resolve each pending key's source identity, consumer, representation, lifetime and errors before updating the ledger; do not interpret broad file-type grouping as blanket exclusion. No shared ledger, product, guest, host configuration or build tree modified.
