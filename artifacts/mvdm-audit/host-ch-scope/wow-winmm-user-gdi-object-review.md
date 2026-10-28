# WOW32 USER/GDI resolution baseline for S16

This is a controlled pre-fix observation of the selected cached x86 `wmmedia.obj`, not a guest integration test. The source, selected object, test source, recipe, observer, seam object and executables are identified by SHA-256 in `wow-winmm-user-gdi-object-evidence.json`. Each case runs in a fresh x86 process. Reproduce from the repository root:

```cmd
artifacts\mvdm-audit\host-ch-scope\wow-winmm-user-gdi-object-build.cmd build\t473-s10\winmm-user-gdi-replay
python artifacts\mvdm-audit\host-ch-scope\wow-winmm-user-gdi-object-observe.py build\t473-s10\winmm-user-gdi-replay
```

The build copies the selected object and rebinds only its `LoadLibraryA` and `GetProcAddress` import symbols to controlled test pointers. GDI conversion is a controlled test seam; unused dependencies fail fast. Linking does not use `/force`; the linker emits LNK4078 for `.sxdata`. Eight cases completed with exit 0 and expected output. The ordinary USER identity conversion succeeds in both directions; the fake GDI converter is called exactly once in the appropriate direction, and its zero result is passed through as failure and a zero output. A missing multimedia export returns 6, leaves output untouched, and prevents GDI conversion even for USER/GDI inputs: provider resolution currently happens first.

S16 should preserve the existing USER/GDI conversion and resolution order while explicitly addressing the incorrect BOOL result for a missing provider as a separately documented original-host defect. The controlled fake GDI handles are not actual USER/GDI resources; this does not establish real object lifetime, real DLL loading, guest reachability, or behavior after the fix. The selected object is a cached build snapshot; its hash alone does not prove it is built from the currently identified source. An independent clean rebuild must bind source and object before treating later results as product evidence.
