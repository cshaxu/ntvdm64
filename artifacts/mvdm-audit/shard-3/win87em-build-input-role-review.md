# S10 shard 3: historical WIN87EM build inputs and guest package boundary

Read-only review of four pending paths: `wow16/win87em/emoemwin.t2`, `key`, `switch.src` and `bin86/i386/win87em.dll`. No guest build, execution, installation or modification. No FPU compatibility percentage is inferred.

| Path | Evidence and proposed role |
|---|---|
| `emoemwin.t2` | SHA-256 `E66B5B533AF8191A57203B2D88C407359300059E1A0CD0EBE670420AC76BDD3A`, exactly equal to retained `emoemwin.asm`. `win87em/makefile:670-671` assembles the latter to `emoemwin.obj`, then links it with `win87em.obj` using `win87em3.def` at 646-656. The identical `.t2` alternate is an input/source copy, but this does not establish that retained source generated the packaged guest image. |
| `key` | SHA-256 `A7BA0FC279CD026280A7DF77893E03B3B09E7CE59FEDA78273800D5FC1237437`; human-readable key explaining emulator library name prefixes/suffixes. Candidate historical build documentation file, not floating-point code or evidence of runtime behavior. |
| `switch.src` | SHA-256 `F019093C79C32A5515070A05B7C59841983C8BF89C04B18491D857D6A2A01BEB`; makefile:720 feeds it to `IFSTRIP` while generating `emoem.src` from `emoem.asm`. That recipe concerns OEM source generation; the WOW DLL recipe directly assembles `emoemwin.asm`. Historical build-control role, not a reason to exclude FPU semantics. |
| `bin86/i386/win87em.dll` | SHA-256 `30D6B965D45EDDF306268C9526B01494DF4841B8B530BAB2654C379E1CF49DB2`, matches approved guest manifest entry and its source path. The currently checked `O:/winnt/system32/win87em.dll` path is absent. Partial archive/image identity only; actual loader resolution, alternative path, FPU exception handling and runtime are unverified. |

`kernel31/ld.asm:1797-1822` describes a Win2 compatibility load of `WIN87EM.DLL` when conditions apply; `dpmi/dxintr.asm` contains emulator-related exception frame comments. Those original source references do not prove current installed guest loaded the file or that selected x87/emulator behavior is complete. Preserve package/loader and numerical behavior questions as separate obligations; the file-role classification of documents and source variants does not close them.

## 2026-10-29 archive and installed-path recheck

The current `assets/winnt.zip` (SHA-256
`7A0534DEC9940B2D8366C8EF71DD536ED94BE7817D7C4E1E19BED05F6E60B8F7`)
does contain `WINNT/SYSTEM32/WIN87EM.DLL`; hashing the uncompressed entry
gives `30D6B965D45EDDF306268C9526B01494DF4841B8B530BAB2654C379E1CF49DB2`,
matching the manifest and retained original source binary. The independently
installed `O:/w31/SYSTEM/WIN87EM.DLL` also has that hash. In contrast,
`O:/winnt/system32/win87em.dll` and the current reconciled ten-image staging
directory do not contain the file. This is an archive/installed-path
observation, not proof of an attempted guest load or an installation defect:
the candidate stage is a product-image stage and may resolve guest inputs
elsewhere. Trace actual WOW search paths and installation ownership before
changing any guest file or asserting a missing capability.

The original loader condition is narrower than all floating-point execution:
the inspected branch checks an application's expected Windows version below
3.0 and the presence of a WIN87EM dependency before calling `ILoadLibrary`.
No Win2 test application, resulting loaded module or arithmetic behavior was
observed in this recheck. Guest images were only read, never rebuilt or changed.

### Loader source trace

`wow16/kernel31/ldboot.asm` derives `lpSystemDir` from the loaded kernel's
own file path and `lpSystem16Dir` from the Windows directory plus `\system`.
The WOW default order in `ldopen.asm` searches current directory, Windows
directory, 16-bit system directory, kernel system directory, Wx86 system
directory and application directory, followed by the DOS/Novell path. The
initial guest environment is projected by
`mvdm_softpc_project_wow_initial_environment`; Run16 separately sets the
guest `WIN16DIR`. `wow32.c` also publishes the selected worker's root and
`root\system32` via `NtvdmGet*Directory`, but these WOW32 globals alone do
not prove the guest kernel's particular `ILoadLibrary` search result.

The candidate staging script copies its baseline tree and replaces specified
host images; it does not unpack the guest archive. Thus the observed missing
file in that stage is not evidence that archive packaging omitted it. The
stage contains `system32/krnl386.exe`, so the original source predicts that
directory as a system-directory candidate when that exact guest is loaded;
current directory, `WIN16DIR\system` and other search entries can still
change the result. The installed directory's absence likewise is not an
observed `ILoadLibrary` failure. A hash-identified existing Win2-dependent
guest and observed successful/failed load are still needed to establish the
actual runtime path, without changing or rebuilding guest images.

A read-only NE-header scan of the complete-media candidate and `O:/w31`
found 39 and 56 NE `.exe`/`.dll`/`.drv` files respectively, with no parsing
errors. In these trees no inspected image simultaneously declares an expected
Windows version below 3.0 and statically imports WIN87EM. The candidate's
`MOUSE101.DRV` declares 1.00 without that import; `O:/w31/CALC.EXE` declares
3.10 with it. [Paths, hashes and observations](../s11-foundation-evidence/reconciled-boundary/complete-media/win87em-ne-scan.json)
are retained. This bounded header census is not a runtime test, does not
cover dynamic loads or other media, and does not establish WIN87EM is unused.

Source recheck confirms `search_mod_dep_list` sets carry when the import name
matches, and the Win2 branch calls `ILoadLibrary` only on that match. The
installed 3.10 `O:/w31/CALC.EXE` has static imports KERNEL, GDI, USER,
WIN87EM and SHELL, so it could witness the ordinary static-import path, not
the special pre-3.0 branch. The candidate root's same-named `CALC.EXE` is a
different PE file (SHA-256
`621BA3934AFC45C35A4EE16386F4DA30119F39FA243E8DC8FEF3491A76F829D8`),
not that Win16 calculator. No executable was launched in this comparison; a
static import alone does not prove an observed successful runtime load.

Replay from repository root:

```powershell
Get-FileHash src/mvdm/wow16/win87em/emoemwin.t2,src/mvdm/wow16/win87em/emoemwin.asm,src/mvdm/wow16/win87em/key,src/mvdm/wow16/win87em/switch.src,src/mvdm/bin86/i386/win87em.dll -Algorithm SHA256
Test-Path O:/winnt/system32/win87em.dll
rg -n -i 'emoemwin\.asm|switch\.src|win87em\.dll' src/mvdm/wow16/win87em/makefile src/mvdm/wow16/kernel31/ld.asm assets/winnt-guest-manifest.json
```

The candidate subsequently ran the unmodified 3.10 Calculator with a static WIN87EM import. The actual Calculator window and About dialog were observed after correcting an initial false match to WOWExec, and the launcher completed; see [witness](../s11-foundation-evidence/atomic-task-retire/execution-check/calc-win87/review.md). This narrows ordinary startup uncertainty but does not identify the loaded DLL at runtime, demonstrate FPU arithmetic, or test the special pre-3.0 branch.

The current ten-build-input candidate independently passed the same bounded
Calculator window, About, parent-recovery and launcher-exit witness. Its
manifest and Calculator/WIN87EM hashes are retained in
[coherent-current/calc](../s11-foundation-evidence/coherent-current/calc/).
This remains indirect static-import startup evidence, not a loaded-module
identity, FPU arithmetic result or pre-3.0 branch test. The earlier arithmetic
probe non-passes remain recorded in the preceding witness review.
