# S10 shard 3: legacy Novell and LAN Workplace interfaces

Scope: `softpc.new/base/inc/novell.h` and `tcpip.h` (both pending in `file-review.csv`). Read-only source and selected build/dependency evidence; no build, guest mutation, or runtime tests.

| file | role and proposed disposition | evidence and limits |
|---|---|---|
| `novell.h` | historical Novell ODI control-block/host-service interface; **excluded as a header from the currently observed worker configuration**, not an assertion about every network feature | SHA-256 `4FB1D6008CBAEA0C3B36985743EC00BBF0441FA3AEEA1FBEFB17C6A598A0BCBF`, identical to both comparison inventories. The only source includes in reset.c:75-77 and terminat.c:32-34 are under `NOVELL`; corresponding shutdown `net_term()` is likewise conditional at reset.c:774-776 and terminat.c:131-133. No exact selected dependency hit in `header-inclusions.csv`, and no `NOVELL` in the observed `build/.ninja/msvc-x86-release/build.ninja`. Other profiles, generated/assembly consumers and runtime cleanup were not tested. |
| `tcpip.h` | historical LAN Workplace TCP service selector, error and lifecycle declarations; **excluded from the observed worker configuration** | SHA-256 `240F9FBCC78213F7E3DC458013A096A4C2F0768E05D8F34808E5610A7E38BFB1`, identical to both comparison inventories. Search found no source include; no exact selected dependency hit in `header-inclusions.csv`. `bios.c:495-503` conditionally dispatches A4/A5 to `TCPResInit`/`TCPResEntry` only under `NOVELL_TCPIP`, otherwise `illegal_bop`; observed build.ninja has no `NOVELL_TCPIP`. No proof of guest use or tested runtime BOP behavior. |

The observed alternative network code is distinct: `vdmredir/vrnetb.c:255,282` uses host `Netbios`, and `wow32/wsraw.c:1559` provides the Win16 Winsock socket thunk. Their existence neither restores nor proves absence of Novell ODI or LAN Workplace services. No guest-side provider, external network dependency, alternate profile, or semantics were exercised; no restored behavioral percentage can be assigned from these declarations.

Reproduction (PowerShell, from repo root; reads only):

```powershell
Get-FileHash src/mvdm/softpc.new/base/inc/novell.h,src/mvdm/softpc.new/base/inc/tcpip.h -Algorithm SHA256
rg -n -i 'novell\.h|tcpip\.h|NOVELL_TCPIP|net_term' src/mvdm/softpc.new/base
rg -n 'NOVELL|NOVELL_TCPIP' build/.ninja/msvc-x86-release/build.ninja
rg -n 'novell\.h|tcpip\.h' artifacts/mvdm-audit/shard-3/header-inclusions.csv artifacts/mvdm-audit/shard-3/file-review.csv
rg -n 'Netbios\(|WWS32socket' src/mvdm/vdmredir/vrnetb.c src/mvdm/wow32/wsraw.c
```

Negative build/dependency search applies to this snapshot and is not a proof about other configurations; retain those limits in any ledger decision.
