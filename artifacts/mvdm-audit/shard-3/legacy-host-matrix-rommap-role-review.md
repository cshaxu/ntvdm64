# S10 shard 3: host keyboard matrix and ROM map legacy interfaces

Read-only source audit; no build, runtime exercise, guest or shared-ledger modification. Both files are pending in the observed shard 3 ledger and have matching current, OpenNT and OpenNT 4.5 inventory hashes.

| File | Role, recommendation, and evidence |
|---|---|
| `softpc.new/base/inc/kybdmtx.h` | The historical matrix interface declares `kybdmtx`, `kybdcpu101`, `kyhot*` and switch/lock actions. Current `src/mvdm` search finds those names only in that header and the valid selected-header index has no hit. SHA-256 `04A7C5CFCBC08FCAEF296FEB55EDCF779D616BA7FD0AB8FC8110ED2442A03C37`. Recommend **excluded as this interface in the observed configuration**; do not infer keyboard is absent. `host/src/nt_reset.c:197` selects `nt_keybd_funcs`; `base/keymouse/keyba.c:3300,3356` registers controller port input and IRQ EOI. The WOW keyboard thunk path is separately documented in `keyboard-abi-role-review.md`. These paths do not establish identical matrix semantics or runtime keyboard behavior. |
| `softpc.new/base/inc/rommap.h` | Declares legacy host fullscreen ROM map, IVT snapshot and mapping ranges. Search for `ROMMapInfo` and its pointer finds only the declaration; valid selected-header index has no hit. SHA-256 `B3E37F197C8BF67333BC72EA5035DCC529D714330ECE237ABDF0BC06A1152105`. Recommend **excluded as this interface in the observed configuration**, not a claim that BIOS ROM mapping is absent. `base/system/rom.c:280,350` contains ROM initialization, while `host/inc/host_def.h:89` defines the host ROM-init hook empty. This alone does not prove how all ROM images reach guest memory. |

Replay from repo root:

```powershell
Get-FileHash src/mvdm/softpc.new/base/inc/kybdmtx.h,src/mvdm/softpc.new/base/inc/rommap.h -Algorithm SHA256
rg -n '\b(kybdmtx|kybdcpu101|kyhot|kyhot2|kyhot3|KYSWITCHUP|KYSWITCHDN|ROMMapInfo|pROMMapInfo)\b' src/mvdm
rg -n 'kybdmtx\.h|rommap\.h' artifacts/mvdm-audit/shard-3/header-inclusions.csv artifacts/mvdm-audit/shard-3/file-review.csv
rg -n 'nt_keybd_funcs|KbdEOIHook|io_define_inb\(AT_KEYB_ADAPTOR|host_rom_init|rom_init IFN' src/mvdm/softpc.new
```

Limits: a snapshot of selected C/C++ dependencies and source search cannot rule out generated, assembly, different-profile, or external consumers. No assertion about runtime equivalence, guest binary identity, or full functionality follows; no numerical behavior-coverage estimate is justified.
