# S10 shard 3: WFWNET guest network and optional IPX VDD input group

Three pending original `wow16/wfwnet` paths are reviewed together. `wfwnet-guest-input-observer.py` / JSON records hashes, both original inventory identities and current installed-file observation. No guest/VDD execution, guest build, networking or product mutation.

| File | Original call/build role and proposed disposition |
|---|---|
| `WFWNET.DRV` | Retained original hash `4C321D43511F845EC1A95518507133D1CF4CF3993BAE422037DBA95CE6258CCA` matches installed system32 and approved guest manifest. **Partial deployed guest image identity**, not proof of runtime APIs or source-to-binary provenance. |
| `ints.asm` | The historic makefile explicitly assembles/links `ints.obj`. `wfwnet.c::LibMain` invokes `GrabInterrupts` only when `WF_ENHANCED` and profile `NWCS/NwcsInstalled=1`; it requests `VWIPXSPX.DLL` exports `VwInitialize`/`VwDispatcher` via RegisterModule, then hooks INT 2F/73 only if registration succeeds. Historical guest source input; optional IPX registration/IRQ lifecycle remains unverified. |
| `wfwnet.def` | Explicitly linked into the historical WFWNET.DRV recipe; declares WNET job, connection and LFN exports. Guest build/export interface, not proof each ordinal works. |

`O:/winnt/system32/VWIPXSPX.DLL` was absent during this check, but that bounded lookup does not rule out other library search paths or profile-selected providers. The guest may never take the optional branch for other profiles; do not claim all WFW networking or all IPX calls fail from the missing file. The retained guest source is not proven to have generated the installed driver. Real initialization, external provider loading/ABI, callback interrupts, cleanup, network operations and failure behavior remain untested and should remain under the corresponding network/device recovery owner.

Replay:

```powershell
$env:PYTHONDONTWRITEBYTECODE='1'; python artifacts/mvdm-audit/shard-3/wfwnet-guest-input-observer.py
Get-Content src/mvdm/wow16/wfwnet/makefile
rg -n 'GrabInterrupts|NwcsInstalled|WF_ENHANCED' src/mvdm/wow16/wfwnet/wfwnet.c
rg -n 'VWIPXSPX|VwInitialize|VwDispatcher|RegisterModule|DispatchCall' src/mvdm/wow16/wfwnet/ints.asm
rg -n -i 'WFWNET\.DRV' assets/winnt-guest-manifest.json
```
