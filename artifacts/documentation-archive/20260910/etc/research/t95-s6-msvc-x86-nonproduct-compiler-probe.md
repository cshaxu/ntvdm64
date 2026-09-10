# M0 T95 S6: MSVC/x86 Non-Product Compiler Probe

## Scope

The sole probe root is `artifacts/analysis/t95-s6-msvc-x86-platform-probe-20260811-001`.
It contains only `platform_probe.cpp` and captured `/EP`/`/Zs` output.  It
includes Windows/UCRT headers only; no Bochs, adapter, OpenNT, CLI or guest
file is included.  No object, link, executable, or run occurred.

## Commands And Inputs

After `VsDevCmd.bat -arch=x86 -host_arch=x64`, the exact probe commands were:

```text
cl.exe /nologo /EP platform_probe.cpp > platform_probe.i
cl.exe /nologo /Zs platform_probe.cpp
```

Both return zero.  Source SHA-256 is
`D4C584F9F578CB80D16A4EC85BABB63B28889B4C3409D8D83CD5D3E9ACA89614`; the
preprocessor output SHA-256 is
`B1D2FC2236C63B7ECDE3914BBAF69992C069405F2A57B19F81EAA461C88F3E27`.

## Direct Results

`/EP` expands the probe enum to `_MSC_VER=1943`, `_WIN32=1`, and `WIN32=1`.
`/Zs` accepts static assertions for `char=1`, `short=2`, `int=4`, `long=4`,
`long long=8`, and `void*=4`.  It also accepts declarations of `_snprintf`,
`_vsnprintf`, `_strtoui64`, `_strdup`, `_stricmp`, `_strrev`, `abort`, and
`getenv` from the recorded Windows SDK/UCRT headers.

This directly establishes the compiler-to-`WIN32` mapping needed by
`bochs.h`'s platform conditional.  It does **not** select `BX_WITH_WIN32`:
that is a Bochs GUI backend configuration and remains source-forced `0`.

## BX_HAVE Interpretation

The accepted declaration checks are only declaration facts.  They do not
prove the matching `BX_HAVE_SNPRINTF`, `BX_HAVE_VSNPRINTF`,
`BX_HAVE_STRTOULL`, `BX_HAVE_STRDUP`, `BX_HAVE_STRICMP`, `BX_HAVE_STRREV`,
`BX_HAVE_ABORT`, or `BX_HAVE_GETENV` semantic selections, because Bochs may
require a particular standard-name mapping, behavior, fallback, or retained
call path.  `BX_HAVE_UNISTD_H`, time/sleep, filesystem, socket/select,
locale/readline, mmap/XPM, seek/tempfile and builtin-bswap families were not
tested and remain unresolved.  No similarly named CRT declaration or export
may fill any of those ledger rows.

## Strict Next Gate

Before a configuration can be proposed, create a documentation-backed matrix
for each remaining platform-probed row specifying its exact standalone header
declaration test and any required semantic/fallback evidence.  The
retained-symbol-dependent rows (`BX_USE_*_SMF`, A20, exit/frontend, fast-call,
and size/default families) remain closed until separate COMDAT evidence.

## Non-Claims

This proves no Bochs configuration, `BX_HAVE_*` value, CRT model, object,
link/import, COMDAT closure, device, fixture, or runtime behavior.
