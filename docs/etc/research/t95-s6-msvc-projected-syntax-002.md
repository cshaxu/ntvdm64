# T95 S6 MSVC projected syntax observation 002

## Result

The one admitted v2 `/Zs` observation is **not syntax-closed**. It exposes a
new source-token boundary, not another missing configuration feature: original
`osdep.h:59` redefines the v2 projected `FMT_LL` token to its MSVC form
`"%I64"`, but modern MSVC still parses original adjacent text such as
`"0x"FMT_LL"x"` and `FMT_LL"u\n"` as an invalid literal suffix before that
macro can serve the intended concatenation role.

Resolving this requires a separately authorized source-level token-boundary
decision. No retry, header/config change, feature enablement, or source edit
was made in this observation.

## Inputs and command

The sole fresh root is
`artifacts/analysis/t95-s6-msvc-x86-projected-syntax-002-20260811-001`. It
copied pinned Bochs input and replaced only its root config with accepted v2
SHA-256 `C47030F79880C894A346F56E3AB3242924308983E00B5845EDCE849450C6C19D`.
The pinned source config remains
`A1FADC25502FB0A4644917B6F3A62987C125CC7335457F9C59E7CE7E62A45AC8`; original
`instrument/stubs/instrument.h` was retained.

`logs/syntax-command.txt` contains the full command. It uses confirmed
BuildTools x86 `cl.exe /nologo /Zs /DWIN32
/DBX_NTVDM_ENABLE_EXCEPTION_INTERCEPT=0`, the original copied include paths,
the non-product fixture, and all declared minimal candidate units including
`cpu/exception.cc`. No SMF variant, `/c`, `/Fo`, link, run, configure, VS
project, `bochs.exe`, or `all` action occurred. There are zero `.obj` files.

## Observations

The log records Visual Studio 2022 Developer Command Prompt v17.13.5. For
each translation unit it first reports C4005: projected `config.h:35` defines
`FMT_LL`, then original `osdep.h:59` redefines it. The first candidate failure
remains C3688 at `gui/siminterface.cc:1302`, followed by the same C3688 at
`gui/paramtree.cc:296,315`; the `fprintf` C2664 is consequential.

The source has already reached the original platform definition, so adding
another config/header definition cannot be justified by this evidence. The
remaining boundary is the original source's adjacent token spelling under the
observed modern MSVC lexer. This observation does not authorize changing that
spelling or assert that any replacement would preserve runtime formatting.

## Next gate and non-claims

The projected v2 shape has no syntax closure. The sole next gate is
coordinator review of an exact source-token-boundary exception or another
evidenced original MSVC compatibility mechanism. No automatic source patch,
configuration change, new syntax invocation, object, link, guest, or runtime
claim is admitted.
