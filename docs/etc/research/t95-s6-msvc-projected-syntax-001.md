# T95 S6 MSVC projected syntax observation 001

## Result

The one admitted projection-baseline `/Zs` observation is **not syntax-closed**.
It stops at the existing `FMT_LL` declaration path in the SIM/parameter
candidate sources.  No retry, SMF variation, extra include/header, source
change, or feature enablement was made.  This result is not object, link,
runtime, guest, or product-configuration evidence.

## Inputs and command

The fresh root is
`artifacts/analysis/t95-s6-msvc-x86-projected-syntax-001-20260811-001`.  It
contains a copied pinned Bochs tree with only its root `config.h` replaced by
the accepted projection.  The source config SHA-256 remains
`A1FADC25502FB0A4644917B6F3A62987C125CC7335457F9C59E7CE7E62A45AC8`; both the
accepted projection and copied root config SHA-256 are
`A77D063C5960C9825F218253085BA7EEE288E2653E2CC07D568259CDCF0C9A11`.
The copied original `instrument/stubs/instrument.h` was present.

`logs/syntax-command.txt` retains the full absolute command.  It entered the
confirmed BuildTools x86 environment and invoked only:

```text
cl.exe /nologo /Zs /DWIN32 /DBX_NTVDM_ENABLE_EXCEPTION_INTERCEPT=0
```

with the copied root, `cpu`, `memory`, `iodev`, `gui`, and original
`instrument/stubs` include directories; the fixture and declared minimal
composition/SIM/log/PC/memory/port-space/CPU candidates, including
`cpu/exception.cc`, were its sole translation units.  The command contains no
`/c` or `/Fo`; there are zero `.obj` files below the analysis root.  It did not
link, run, configure, use a VS project, invoke `bochs.exe`, or invoke `all`.

## Observation

The log records Visual Studio 2022 Developer Command Prompt v17.13.5.  Its
first candidate boundary is:

```text
gui/siminterface.cc(1302): error C3688: invalid literal suffix 'FMT_LL'
gui/paramtree.cc(296,315): error C3688: invalid literal suffix 'FMT_LL'
```

The accompanying `fprintf` C2664 is a consequence of the same unexpanded
format-token path.  Later declared units were parsed by the single compiler
batch, including `exception.cc`, but that does not cure or supersede the first
boundary.  Resolving it would require a new, separately reviewed configuration
or original-header-mechanism decision; this observation did not supply one.

## Next gate

The projected shape has no syntax closure.  The sole next action is coordinator
review of the preserved original MSVC format-token mechanism and this observed
failure.  No automatic config change, retry, SMF variant, source/header input,
or further `/Zs` invocation is admitted.
