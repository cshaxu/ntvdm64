# VDM redirector diagnostic provider

| Owned source | Disposition proposal for selected x86 release | Evidence |
| --- | --- | --- |
| `vdmredir/vrdebug.c` | excluded code for this product configuration; retain source as original debug provider | File body `#if DBG` at line 181 through line 2372; the generated x86 release command specifies PROD without an explicit DBG definition. Its actual compiled object has no ordinary external functions or data symbols. |
| `vdmredir/vrdlcdbg.c` | excluded code for this product configuration; retain source as original DLC diagnostics | Body `#if DBG` at line 87 to final `#endif`; the selected release object likewise has no ordinary external functions or data symbols. |

The build graph names both objects as inputs to `VDMREDIR.dll` (`build.ninja:406,408,1363`), so treating their graph presence as evidence that the debug routines execute would be wrong. Conversely, this is not a reason to exclude `vdmredir` service dispatch, DLC, named pipes or NetBIOS: they have separate actual providers. The conclusions are limited to the observed x86 release object and do not establish debug-build behavior or assert binary equivalence of other configurations. Original files and object SHA-256 values, generated command flag observations, and reproducible object symbol entries are in `redir-debug-provider-observations.json`, produced by `redir-debug-provider-observer.py`. No runtime tests or guest modifications were performed.
