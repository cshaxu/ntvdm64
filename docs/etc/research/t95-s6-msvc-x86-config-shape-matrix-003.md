# T95 S6 MSVC/x86 configuration-shape matrix 003

## Result

Matrix-003 is frozen after its first declared-candidate syntax pass.  The
`a-smf111` shape is **not syntax-compatible** within the admitted inputs:
MSVC reached missing `FMT_LL`, `GET32L`, and `GET32H` declarations.  The
generated `b-smf000` shape was deliberately not invoked after that first
boundary, so it has no compatibility classification and no SMF comparison is
available.

No macro was added to cure this boundary, and no retry was made.  Continuing a
hand-authored macro chase would recreate Bochs configuration machinery rather
than validate a minimal import.  This record proves neither an object, link,
COMDAT, executable, runtime, guest, nor product-configuration result.

## Frozen root and admitted input

The sole new root is
`artifacts/analysis/t95-s6-msvc-x86-config-shape-003-20260811-001`.  It copied
the pinned `src/bochs` input; earlier matrix roots were not changed.  The
root-local common analysis header has SHA-256
`6E6F36368529C7EA7B29135A95D85B98DA8280B34C2B7801DDE932A5469AEA73` and adds
only the original `config.h.in:482-497` extrema declarations to the
matrix-002 declaration shape.  It keeps the source-visible `osdep.h` provider
for the MSVC `FMT_LL` token form (`"%I64"`) rather than inventing a substitute.

| Variant | CPU/MEM/DEV SMF | `config.h` SHA-256 | Invocation/result |
| --- | --- | --- | --- |
| `a-smf111` | `1/1/1` | `2B0338599D06F4B44D53D2FC1E730715B49EFB54DD0156334D9B8ED03F8F672C` | invoked once; stopped at declaration boundary |
| `b-smf000` | `0/0/0` | `100C2A8A432DD86F0D7E64A52BEDC75E46FD7A0431D8AA6EDDEFD2402F820606` | generated, uninvoked |

Both retain `/DWIN32`, the copied original `instrument/stubs` include path,
`BX_INSTRUMENTATION=0`, `BX_WITH_WIN32=0`, the empty original-template
`BX_CPP_AttrRegparmN(n)` fallback, and the locally default-off
`BX_NTVDM_ENABLE_EXCEPTION_INTERCEPT=0` path.  No adapter include/input was
added.

## Exact observation

The complete command is retained as
`logs/a-smf111-candidates-command.txt`; it enters:

```text
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat" -arch=x86 -host_arch=x64 && cl.exe /nologo /Zs /DWIN32 ...
```

It passes only the copied Bochs root, `cpu`, `memory`, `iodev`, `gui`, and
original `instrument/stubs` include directories, and only the predeclared
minimal-machine, SIM/parameter, log/PC, memory, empty-port-space, and CPU
candidate translation units.  The log records the Visual Studio 2022
Developer Command Prompt v17.13.5 banner and contains no `/Fo`, `/c`, link,
run, configure, VS project, `bochs.exe`, or `all` invocation.

The first terminal diagnostics in
`logs/a-smf111-candidates-zs.log` are:

1. `gui/siminterface.cc:1302`, and `gui/paramtree.cc:296,315`: C3688,
   invalid literal suffix `FMT_LL` (with the consequent `fprintf` C2664).
   Original `bochs.h:103` includes `osdep.h`; `osdep.h:59` defines
   `FMT_LL` as `"%I64"` for `WIN32`/MSVC.  The effective analysis declaration
   path did not yield that macro at these uses.  Matrix-003 does not decide why
   or introduce another definition.
2. `cpu/exception.cc:530-531`: C3861, `GET32L`/`GET32H` not found.  The
   original source-tree `config.h:438-439` supplies them, but they are not in
   `config.h.in` and were not admitted as matrix additions.

These observations are configuration-source boundaries, not authorization to
copy or project those historical definitions into another generated header.
No non-original header, denied feature, or adapter input was reached after
the BX-ABI-027 default-off guard; the stop is solely the admitted declaration
limit.

## Next gate and non-claims

The sole next gate is a **coordinator decision** between an auditable
projection of original MSVC configuration/platform mechanics with an explicit
denial overlay, and an original generated-configuration source.  It must
define provenance and a bounded denial policy before any further config,
matrix, compiler, or build action.  No new hand-authored configuration-macro
derivation, matrix root, `/Zs` invocation, source edit, object/link/run, or
feature enablement is admitted by this result.
