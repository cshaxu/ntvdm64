# S10 shard 3: historical WIN87EM build inputs and guest package boundary

Read-only review of four pending paths: `wow16/win87em/emoemwin.t2`, `key`, `switch.src` and `bin86/i386/win87em.dll`. No guest build, execution, installation or modification. No FPU compatibility percentage is inferred.

| Path | Evidence and proposed role |
|---|---|
| `emoemwin.t2` | SHA-256 `E66B5B533AF8191A57203B2D88C407359300059E1A0CD0EBE670420AC76BDD3A`, exactly equal to retained `emoemwin.asm`. `win87em/makefile:670-671` assembles the latter to `emoemwin.obj`, then links it with `win87em.obj` using `win87em3.def` at 646-656. The identical `.t2` alternate is an input/source copy, but this does not establish that retained source generated the packaged guest image. |
| `key` | SHA-256 `A7BA0FC279CD026280A7DF77893E03B3B09E7CE59FEDA78273800D5FC1237437`; human-readable key explaining emulator library name prefixes/suffixes. Candidate historical build documentation file, not floating-point code or evidence of runtime behavior. |
| `switch.src` | SHA-256 `F019093C79C32A5515070A05B7C59841983C8BF89C04B18491D857D6A2A01BEB`; makefile:720 feeds it to `IFSTRIP` while generating `emoem.src` from `emoem.asm`. That recipe concerns OEM source generation; the WOW DLL recipe directly assembles `emoemwin.asm`. Historical build-control role, not a reason to exclude FPU semantics. |
| `bin86/i386/win87em.dll` | SHA-256 `30D6B965D45EDDF306268C9526B01494DF4841B8B530BAB2654C379E1CF49DB2`, matches approved guest manifest entry and its source path. The currently checked `O:/winnt/system32/win87em.dll` path is absent. Partial archive/image identity only; actual loader resolution, alternative path, FPU exception handling and runtime are unverified. |

`kernel31/ld.asm:1797-1822` describes a Win2 compatibility load of `WIN87EM.DLL` when conditions apply; `dpmi/dxintr.asm` contains emulator-related exception frame comments. Those original source references do not prove current installed guest loaded the file or that selected x87/emulator behavior is complete. Preserve package/loader and numerical behavior questions as separate obligations; the file-role classification of documents and source variants does not close them.

Replay from repository root:

```powershell
Get-FileHash src/mvdm/wow16/win87em/emoemwin.t2,src/mvdm/wow16/win87em/emoemwin.asm,src/mvdm/wow16/win87em/key,src/mvdm/wow16/win87em/switch.src,src/mvdm/bin86/i386/win87em.dll -Algorithm SHA256
Test-Path O:/winnt/system32/win87em.dll
rg -n -i 'emoemwin\.asm|switch\.src|win87em\.dll' src/mvdm/wow16/win87em/makefile src/mvdm/wow16/kernel31/ld.asm assets/winnt-guest-manifest.json
```
