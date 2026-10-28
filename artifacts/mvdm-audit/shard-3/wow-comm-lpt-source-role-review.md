# S10 shard 3: original COMM composite and selected LPT provider

Read-only continuation of `parallel-review-current.md` on pending `wow16/drivers/comm/ibmcom1.asm`. No guest build, modification, device or print test. The file's SHA-256 is `1063ABFEB8DD98DB6531280D366797F62B9CBC2112A1801CDEBC7F057513BBD5`, identical to both source inventories.

The historical selected `comm/makefile:99-119` builds and links `ccom`, `ibmsetup`, `ibmcom`, `ibmint`, `ibmlpt`, `commmsg`; it does not name `ibmcom1`. `ibmcom.asm:153-154` imports `DoLPT` and `StringToLPT`; its callers at 573 and 719 route LPT work to them. The selected recipe builds `ibmlpt.obj`, whose `ibmlpt.asm:82-266` defines `DoLPT` and lines 318-400 implement `StringToLPT`. Both the old composite `ibmcom1.asm` and selected `ibmlpt.asm` expose `DoLPT`; the latter issues hardware port operations and consults `GetSystemMSecCount`. The old source contains additional routines and FCLI/FSTI, so matching one symbol is not a semantic equivalence proof. A source-tree search finds no other explicit `ibmcom1` reference beyond its own module comment, but alternate builds and generated inputs remain possible.

Recommendation: classify `ibmcom1.asm` as a **retained historical alternate guest source, unselected by this inspected COMM recipe**, not as a missing guest LPT/COM feature. The approved manifest has a `COMM.DRV` NE image, SHA-256 `ee589c26791b66742f7b5e183a44c56a13053dc584b0dca3fcc20cd87bdca69e`; the recipe alone does not prove its byte provenance or actual selected guest path. Actual port provider, physical printer/serial behavior, timing, IRQ and teardown remain unverified here and cannot be assessed through this source-level comparison. No behavioral restoration percentage is claimed.

Reproduce:

```powershell
Get-FileHash src/mvdm/wow16/drivers/comm/ibmcom1.asm -Algorithm SHA256
rg -n -i 'ibmcom1|ibmlpt|DoLPT|StringToLPT|comm\.drv' src/mvdm/wow16/drivers/comm
rg -n -i 'ibmcom1' src/mvdm/wow16 -g '!*.map' -g '!*.sym' -g '!*.obj'
rg -n -i 'COMM\.DRV' assets/winnt-guest-manifest.json
```
