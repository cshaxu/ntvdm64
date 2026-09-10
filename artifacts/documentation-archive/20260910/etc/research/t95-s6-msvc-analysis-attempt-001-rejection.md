# M0 T95 S6: MSVC Analysis Attempt 001 Rejection

## Question

Did the first admitted artifact-only MSVC/x86 closure investigation establish
an effective configuration or any COMDAT/link closure result?

## Inputs

- analysis root: `artifacts/build/t95-s6-msvc-x86-comdat-closure-20260810-001`;
- copied Bochs input below that root only;
- generated analysis-only `bochs/config.h`, SHA-256
  `A0EBFC64BA2782DCB2911F1D48FA6DA90BFCA37BCFCB2095565156E319909904`; and
- `fixture_main.cc`, an analysis-only root that constructs, initializes, and
  cleans up `bx_ntvdm_minimal_machine_c` without a CPU loop or executable run.

## Procedure And Observation

The attempt used VS2022 `VsDevCmd.bat -arch=x86 -host_arch=x64`, then invoked:

```text
cl.exe /nologo /c /Gy /I<analysis-root>\bochs <analysis-root>\fixture_main.cc /Fo<analysis-root>\fixture_main.obj
```

No `configure`, VS2008 project, `bochs.exe`, `all`, linker, or executable run
was invoked.  Both retained compile logs stop at the same first diagnostic:

```text
bochs.h(68): fatal error C1083: Cannot open include file: 'unistd.h': No such file or directory
```

The first generated configuration hash was
`20AC8AA58923565C2D4CEBF73B5A65EE24170F3D88E11E5B650E5551644CAA9F`.
An artifact-only attempted addition of `BX_HAVE_UNISTD_H=0` changed the hash
to the value above but did not change the diagnostic.  The root is preserved
unchanged from this point onward.

## Rejection

This is **not** configuration, compiler, object, COMDAT, linker, import, or
runtime evidence.  The diagnostic is non-decisive because the generated file
assigned values that the S6 contract explicitly leaves unresolved:

- `BX_USE_CPU_SMF`, `BX_USE_MEM_SMF`, `BX_USE_DEV_SMF`;
- `BX_SUPPORT_A20`, `BX_USE_IDLE_HACK`, `BX_FAST_FUNC_CALL`, and assertion
  policy;
- `BX_EXIT`, `BX_DEFAULT_CONFIG_INTERFACE`, and
  `BX_DEFAULT_DISPLAY_LIBRARY`; and
- device-SMF, ATA, platform/CRT, and probe-result macro families.

Those assignments preceded effective-preprocessor and retained-symbol
derivation.  They can change include selection, linkage shape and live failure
paths, so an error observed under them cannot classify the legal fixture route.

`_WIN32`/`_MSC_VER` are compiler facts.  They are distinct from Bochs's
configuration selection `BX_WITH_WIN32`; the latter controls a Bochs GUI
backend and remains denied.  Likewise, a platform include decision must come
from a recorded effective MSVC preprocessor derivation, not from guessing a
`BX_HAVE_*` value in an analysis configuration.

## Follow-Up And Non-Claims

The next action is a documentation/read-only source and preprocessor
derivation that enumerates every configuration macro needed by the candidate
translation units, classifying each as source-forced, platform-probed, or
unresolved.  It must occur before any new artifact configuration or build.

This attempt makes no claim about MSVC availability, correct Win32 headers,
object compilation, `/Gy` sections, `/OPT:REF`, maps, undefined symbols, CRT
selection, imports, machine initialization, or guest execution.  It does not
modify `refs/bochs` and it does not authorize another build in this root.
