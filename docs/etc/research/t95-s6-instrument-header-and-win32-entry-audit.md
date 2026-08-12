# T95 S6: original stub header and MSVC `WIN32` entry audit

## Decision

The matrix-001 `instrument.h` stop is a missing **original include-path
input**, not a need to add instrumentation behavior.  The next, separately
admitted matrix may use the pinned original `instrument/stubs/instrument.h`
through an include directory and retain `BX_INSTRUMENTATION=0`.  It must not
copy, generate, or edit an `instrument.h`, compile a custom instrumentation
implementation, set `BX_INSTRUMENTATION=1`, or select an example/custom
instrumentation directory.

Likewise `WIN32` is an MSVC translation-unit preprocessor definition, distinct
from Bochs's `BX_WITH_WIN32` GUI feature.  The exact historical MSVC command
shape is `/DWIN32`, with `BX_WITH_WIN32=0` retained.  It is not a license to
enable the Win32 GUI.

## Original source and build evidence

| Item | Classification | Exact evidence | Consequence for a syntax-only fixture |
| --- | --- | --- | --- |
| `instrument/stubs/instrument.h` | required | `bochs.h:483` unconditionally includes `"instrument.h"`; `instrument/stubs/instrument.h:22` supplies no-op macros when `BX_INSTRUMENTATION` is zero. | Add only `-I<copied-bochs>\\instrument\\stubs`. |
| Stub directory selection | default-required | `configure.in:1345` defaults `INSTRUMENT_DIR='instrument/stubs'`; absent/`no` instrumentation sets `BX_INSTRUMENTATION=0` at `1354-1367`; `AC_OUTPUT` emits `${INSTRUMENT_DIR}/Makefile` at `2841`. | Select the original default stub header, not instrumentation behavior. |
| Makefile propagation | required source build mechanism | `Makefile.in:121` puts `-I@INSTRUMENT_DIR@ -I$(srcdir)/@INSTRUMENT_DIR@` in `BX_INCDIRS`; the configured retained `Makefile:118` resolves both to `instrument/stubs`; compile rule is `Makefile:166`. | The second matrix must reproduce only these two header-search inputs relevant to the copied source. |
| Stub library | default-not-required for `/Zs` | `INSTRUMENT_VAR=''` for no instrumentation (`configure.in:1354-1367`); while `Makefile:153,318-320` names/builds the original stub library, syntax-only evaluation produces no link. | Do not compile/link `instrument.cc` or `libinstrument.a` in the matrix. |
| Instrumentation examples/custom directory | rejected | `configure.in:1358-1362` makes a non-`yes` option value an alternate directory and sets `BX_INSTRUMENTATION=1`; `instrument/instrumentation.txt:12-16` describes copying stubs to customize it. | No custom path, behavior, or source addition. |
| `WIN32` translation-unit macro | required | `bochs.h:37-42` conditionally includes `windows.h`; `bochs.h:67-71` selects `io.h` only if `WIN32`. Retained original Win32 `Makefile:114-118` uses `/DWIN32` and `vs2008/bochs.vcproj:48-49,139-140` declares `WIN32` separately from include paths. | Use `/DWIN32` on each future `/Zs` command. |
| `BX_WITH_WIN32` | rejected | `config.h.in:279-284` independently defaults `BX_WITH_WIN32` to `0`; `WIN32` conditionals also drive typedef/endian/API shapes at `123-127`, `310-325`, `372-531`, and `861-866`. | Keep `BX_WITH_WIN32=0`; do not enable a GUI/configuration front end. |
| `BX_CPP_AttrRegparmN` empty fallback | required declaration shape | `config.h.in:769-778` defines the macro empty outside GCC x86 fast-call conditions. Matrix-001 logged its absence before the header stop. | The next analysis-only config must carry the original empty fallback; it does not select fast-call behavior. |

## Retained matrix observations

This audit read only the frozen
`artifacts/analysis/t95-s6-msvc-x86-config-shape-20260811-001` logs.  Its
initial `*-attempt1.log` files show the absent Community `VsDevCmd.bat` path;
the later BuildTools `/Zs` logs contain the Visual Studio 2022 banner.  The
unmapped pass reached `bochs.h:68` C1083 (`unistd.h`).  The `/DWIN32` pass
advanced beyond that and stopped at `bochs.h:483` C1083 (`instrument.h`).
This is evidence of the project-level macro and original header-search
requirements only, not a successful syntax, object, link, or runtime result.

## Minimal second-matrix admission proposal

One fresh analysis root may be admitted only for a finite `/Zs` repeat with:

1. a copied pinned Bochs input and generated analysis-only `config.h` variants;
2. `/DWIN32`, plus original include directories for root, selected candidate
   subdirectories, and `instrument/stubs`;
3. `BX_INSTRUMENTATION=0`, `BX_WITH_WIN32=0`, and the source-template empty
   `BX_CPP_AttrRegparmN(n)` fallback;
4. the already admitted SMF variants and only source-reached conservative
   declaration fallbacks; and
5. no `/Fo`, `/c` object output, link, executable, source edit, configure,
   VS project, `bochs.exe`, `all`, CPU loop, or run.

It must record the full command/config hashes and stop before a non-original
header, instrumentation implementation, device/GUI feature, or source
intrusion would be required.  The proposal is admission-only: it does not
select a product config or prove link/COMDAT/runtime closure.
