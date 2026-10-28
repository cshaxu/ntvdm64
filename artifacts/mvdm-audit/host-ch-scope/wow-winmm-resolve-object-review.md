# Controlled selected-object WINMM resolver observations

This experiment calls the actual cached x86 `wmmedia.obj` implementation of
`WOW32ResolveHandle` and `WOW32DriverCallback`, with **test-only** loader and
provider seams. The source `wmmedia.c` is byte-identical to both compared
OpenNT editions. The [observer](wow-winmm-resolve-object-observe.py) and
[receipt](wow-winmm-resolve-object-evidence.json) record source, selected and
seam object, fixture executable, build recipe, graph and cached map hashes.
The receipt identifies this cached object; it does not prove the currently
installed DLL has the same body or that a real guest reaches these paths.

The build copies the selected COFF object and rebinds only its
`LoadLibraryA` and `GetProcAddress` imports to test-owned function pointers.
All disassembled instructions match the source object after ignoring extra
zero-padding markers emitted by `objcopy`, and all relocations match after
normalizing those two import renames. The linker succeeds without
`/force:unresolved`; it emits LNK4078 for differing `.sxdata` attributes.
Unrelated GDI, pointer and directory seams fail fast if reached. The test
does not claim the re-bound object is byte-identical or suitable for release.

Six separate process invocations returned zero with these recorded observations:

| Controlled case | Result |
| --- | --- |
| Missing loader module | Resolve and callback each return `6`; neither provider is called. |
| Missing named export | Resolve and callback each return `6`; neither provider is called. |
| Provider success, 16→32 | Fake writes `0x13579bdf`; wrapper returns `1`, final output is `0x11112222`. |
| Provider success, 32→16 | Fake writes `0x2468`; wrapper returns `1`, final output is `0x2222`. |
| Provider failure, 16→32 | Fake returns FALSE; wrapper returns `1`, final output is `0x11112222`. |
| Provider failure, 32→16 | Fake returns FALSE; wrapper returns `1`, final output is `0x2222`. |

Each successful or failed fake-provider case records exactly one provider call.
The fake writes the stated sentinel even when returning FALSE, deliberately
exercising wrapper behavior; this is not a claim about real WINMM's failure
output contract. Source inspection shows the original BOOL functions pass
`MMSYSERR_NODRIVER` (6) to a macro that returns it directly. The original
multimedia switch arms call the provider but then write local values that
those arms did not initialize; the captured object exhibits the values above.
The exact manifestation of uninitialized values is compiler/profile-specific.

For S16, first establish the documented/observed private provider return and
output contract and a real immutable-guest caller, then test both handle
directions, all admitted multimedia kinds, null/invalid outputs, missing DLL
and export, provider failure, error values and cleanup. Any correction to the
original inherited behavior needs the established source exception process;
do not claim a production repro or unilaterally change semantics from this
controlled fixture. Preserve real native ownership and avoid a second handle
registry.

The recorded independent replay used a previously nonexistent output directory.
From the repository root, reproduce with
`cmd /c artifacts\mvdm-audit\host-ch-scope\wow-winmm-resolve-object-build.cmd build\t473-s10\winmm-resolve-object-replay`,
then `python artifacts/mvdm-audit/host-ch-scope/wow-winmm-resolve-object-observe.py build/t473-s10/winmm-resolve-object-replay`.
The recipe creates the specified directory and uses `objcopy` to produce the
two-import-rebound copy from the cached selected object before compiling and
linking; it requires MSVC x86 and `objcopy` on PATH. The observer checks all
six results, code and relocation equivalence and input hashes. No guest or
product source, installed image, shared ledger or global WINMM state is modified.
