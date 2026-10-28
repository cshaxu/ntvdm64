# S10 shard 3: MSCDEX guest and VDD build-input group

This complements shard-0 `mscdex-package-review.md` for three current pending shard-3 paths. Read-only source, manifest and file-hash comparison; no guest/VDD execution, rebuild, installation, optical-media access or shared-ledger change.

| Path | Verified role and proposed disposition |
|---|---|
| `vdd/samples/mscdex/tsr/mscdexnt.exe` | Current-only inventory binary, SHA-256 `B3151A2DFE4A67A6C2E6113071D1F1C713BB78A04ADEA986FE01540EB96762D0`. It matches the approved guest manifest and `O:/winnt/system32/mscdexnt.exe` byte for byte; installed `autoexec.nt:8` names it. **Partial guest binary identity**, not a complete CD-ROM implementation or proof the source built this file. |
| `vdd/samples/mscdex/tsr/mscdexnt.inc` | SHA-256 `329B698242F683B53C1B283295F3D3B5A2B43B12550B45451FD6537246DE259E`, both original inventories identical. Defines DOS TSR/INT 2F constants and device header macro; `mscdexnt.asm:16` includes it and `tsr/makefile` explicitly compiles/links the TSR. **Historical guest build input**; source-to-installed EXE identity beyond binary hash is unproved. |
| `vdd/samples/mscdex/vdd/vcdex.h` | SHA-256 `EF58FF9747B8B14A904A3B2894FB496FEA500771AAEC462DD8CEFED63BC00B5A`, both original inventories identical. `vcdex.c:52` includes it; declares drive state, service/IOCTL handlers and physical-drive boundary. `vdd/sources` builds separate VCDEX DLL, not a host-worker source. **Historical VDD build input**, while the guest/VDD capability itself remains open. |

Original TSR hooks INT 2F; the first MSCDEX call attempts `RegisterModule` for `VCDEX.DLL` and `VDDRegisterInit`/`VDDDispatch`, and has explicit registration-failure paths. The sample readme requires a VCDEX DLL on the path; current standard release and O:/winnt/system32 list `mscdexnt.exe` but no VCDEX DLL by name. This is a specific package boundary, not proof that all paths cannot supply another VCDEX, that the TSR successfully started, or that any particular optical-drive request fails. The shard-0 report records the retained historical DLL's 12 import-name matches to installed worker exports; ABI, provider search, resource teardown, medium I/O and guest runtime remain S17 obligations. Do not infer file exclusion means CD-ROM capability exclusion.

Replay:

```powershell
Get-FileHash src/mvdm/vdd/samples/mscdex/tsr/mscdexnt.exe,src/mvdm/vdd/samples/mscdex/tsr/mscdexnt.inc,src/mvdm/vdd/samples/mscdex/vdd/vcdex.h,O:/winnt/system32/mscdexnt.exe -Algorithm SHA256
Get-Content src/mvdm/vdd/samples/mscdex/tsr/makefile
Get-Content src/mvdm/vdd/samples/mscdex/vdd/sources
rg -n -i 'RegisterModule|vcdex\.dll|DispatchCall|include mscdexnt' src/mvdm/vdd/samples/mscdex/tsr/mscdexnt.asm
rg -n -i 'mscdexnt|vcdex' O:/winnt/system32/autoexec.nt assets/winnt-guest-manifest.json
Get-ChildItem O:/winnt/system32 -Filter '*cdex*' | Select-Object Name,Length
```
