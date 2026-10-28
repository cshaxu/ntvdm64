# Local WINMM null-handle and original-wrapper comparison

This isolated x86 observation uses test-owned output buffers and starts a
separate process per case. It does not start a guest, open a device, use UI,
or change system settings. The test assumes the six-parameter `BOOL WINAPI`
signature of the original private `WOW32ResolveMultiMediaHandle`; a successful
return and balanced stack for these inputs do not prove the modern private ABI
for all values or host versions.

Both test executables load the local x86 WINMM by system-directory path.
`GetModuleFileName` reports the redirected `system32` spelling; opening that
path within the x86 process and resolving its file handle reports
`C:\Windows\SysWOW64\winmm.dll`. The captured DLL is version
`10.0.26100.9549`, SHA-256
`FF2ABD18BF1404A2D41831CEB6BFD25A812FABA7C29E66C0F2E0CFF5389FC344`,
unchanged by the before/after hash checks.

| Wave-output type and direction | Direct local provider | Selected-object wrapper |
| --- | --- | --- |
| 16→32, input `0` | FALSE; both output sentinels unchanged | FALSE; 32-bit output becomes `0` |
| 16→32, input `1` | FALSE; both output sentinels unchanged | FALSE; 32-bit output becomes `0` |
| 32→16, input `0` | FALSE; both output sentinels unchanged | FALSE; 16-bit output becomes `0` |
| 32→16, input `1` | FALSE; both output sentinels unchanged | TRUE; 16-bit output becomes `1` |

All eight processes exited zero, logged no exception and retained a balanced
x86 stack. Each logged `GetLastError=126`; this is only a post-call
observation, not proof of the provider's internal failure cause or a
documented error contract. Outputs are initialized to `0xaaaa` and
`0xcccccccc`; the integers `0` and `1` are test-owned null/small-invalid
inputs, not handles obtained from a real device.

The first executable resolves and calls the local WINMM export directly.
The second links the **unmodified cached** `wmmedia.obj` without
`/force:unresolved`, and its map attributes `WOW32ResolveHandle` to that
object. It preloads the same WINMM module and assigns its handle to the
object's `hWinmm` variable; GDI and unrelated memory/directory entrypoints
are fail-fast seams. Thus the wrapper comparison exercises the real cached
original function and local native export, but not the ordinary loader path,
full WOW execution boundary, actual guest caller, real device ownership or
installed WOW32 binary. Other Windows versions and valid handle conversions
remain untested. The differing wrapper output and TRUE return are an observed
result for this controlled configuration; they do not establish the full
intended failure contract by themselves.

The [observer](wow-winmm-real-null-observe.py) and
[receipt](wow-winmm-real-null-evidence.json) retain all outputs, process exit
codes, selected object, fixture, recipe, map and DLL hashes; an independent
replay matched. The recorded independent build used
`build/t473-s10/winmm-real-null-replay`, which did not exist before the build;
the earlier fixture directory was not overwritten. From the repository root,
rebuild with
`cmd /c artifacts\mvdm-audit\host-ch-scope\wow-winmm-real-null-build.cmd build\t473-s10\winmm-real-null-replay`,
then run `python artifacts/mvdm-audit/host-ch-scope/wow-winmm-real-null-observe.py build/t473-s10/winmm-real-null-replay`.
Both scripts accept an optional output-directory argument. The recipe needs
MSVC x86; the observer also uses `objdump` to check bitness. No product,
guest or shared ledger was modified for this observation.
