# T95 S6 MSVC/x86 configuration-shape matrix 002

## Result

Matrix-002 confirms the original MSVC entry, `/DWIN32`, and the original
`instrument/stubs` header path for the analysis fixture.  It is nevertheless
**stopped without a syntax-compatible candidate**: the first candidate pass
reaches both omitted source-template declaration macros and the pre-existing
Bochs exception seam's out-of-tree adapter ABI header.  The latter would
expand a Bochs-local syntax matrix into adapter input, so no config adjustment,
second SMF pass, or retry was made.

This record proves no object, link, COMDAT, executable, runtime, guest, or
product-configuration result.

## Frozen root, inputs, and variants

The sole matrix root is
`artifacts/analysis/t95-s6-msvc-x86-config-shape-002-20260811-001`.  It copied
the pinned `src/bochs` input only.  No `instrument.h` was generated or copied:
the command searches the original copy's `instrument/stubs` directory.

| Variant | SMF values (CPU/MEM/DEV) | Status | `config.h` SHA-256 |
| --- | --- | --- | --- |
| `a-smf111` | `1/1/1` | fixture accepted; candidate pass stopped | `2B0338599D06F4B44D53D2FC1E730715B49EFB54DD0156334D9B8ED03F8F672C` |
| `b-smf000` | `0/0/0` | generated but not invoked after first boundary | `100C2A8A432DD86F0D7E64A52BEDC75E46FD7A0431D8AA6EDDEFD2402F820606` |

The common explicit analysis base hash is
`228147A029ABEB07DC4666154B2B9EDF92BA64661494D872390D92BD99BDC78B`.
Both variants retain `BX_INSTRUMENTATION=0`, `BX_WITH_WIN32=0`, denied feature
families at zero, and the source-template empty `BX_CPP_AttrRegparmN(n)`
fallback.  All remaining non-source-derived values are declaration-shape
placeholders only, not configuration decisions.

## Exact execution

The confirmed entry was:

```text
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat" -arch=x86 -host_arch=x64
```

The retained command files under `logs/` record the complete absolute command
lines.  Both passed `/DWIN32` and `/I` for the copied Bochs root, `cpu`,
`memory`, `iodev`, `gui`, and original `instrument/stubs`; all used
`cl.exe /nologo /Zs`.  There was no `/Fo`, `/c` object request, link, run,
configure, VS project, `bochs.exe`, or `all`.

`a-smf111`'s `fixture_syntax.cc` accepted with the Visual Studio 2022
Developer Command Prompt v17.13.5 banner.  It declares only a null
`bx_ntvdm_minimal_machine_c*`; it does not instantiate a machine or enter a
CPU loop.

The immediately following declared-candidate pass included only the minimal
composition/helpers, SIM/parameter support, log/PC, memory/empty port space,
and CPU support candidates.  Its first declaration failures are:

1. `gui/siminterface.cc:1288` and `gui/paramtree.cc:243` lack `BX_MIN_BIT64U`,
   `BX_MAX_BIT32U`, and related extrema; `config.h.in:482-497` supplies their
   original definitions.  `FMT_LL` was also represented as a string literal
   rather than the original token form, producing the adjacent invalid literal
   diagnostics.
2. `cpu/exception.cc` includes its existing
   `cpu/bx_ntvdm_exception_intercept.h`, which at line 9 requires
   `bx_ntvdm_cpu_state_abi.h`.  The source is under
   `src/bx-ntvdm-adapter/`, outside the copied Bochs input.  The terminal
   diagnostic is C1083 for that header.

The first category is a source-template configuration completeness finding;
the second is the stop boundary.  Adding that adapter include directory,
copying the ABI header, or changing exception source is not authorized by this
matrix packet.  `b-smf000` therefore remains uninvoked, and no SMF comparison
or selection is available.

## Next gate and non-claims

The only next action is a coordinator decision separating a future
Bochs-local declaration pass from the already implemented exception/adapter
seam: either exclude `cpu/exception.cc` from the declared minimal syntax set
with a source-to-object justification, or separately authorize an exact
same-island adapter-header input boundary.  It may then admit another fresh
matrix that adds only the original `config.h.in:482-497` extrema and original
`FMT_LL` shape.  No route is admitted to enable instrumentation, GUI, devices,
plugins, `BX_WITH_WIN32`, or any OpenNT/adapter runtime behavior.
