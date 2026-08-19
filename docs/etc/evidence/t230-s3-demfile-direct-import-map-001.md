# T230 S3 — Direct `demfile.c` / `demlabel.c` Import Map

## Imported source

- Source: `src/opennt/base/mvdm/dos/dem/demfile.c` and `demlabel.c`
- Baseline commit: `d450bfc1b7711234072eebfdfa23735ea509e7c5`
- Destinations: `src/bx-vdm/bop/opennt/dem/demfile.c` and `demlabel.c`
- The comparison-only predecessor was deleted after the direct imported
  package passed its focused fixture and reference audit.

## Owned BOP service bodies

| Original body | DEM service |
| --- | --- |
| `demChMod` | `50:01` |
| `demCreate` / `demCreateCommon` | `50:03` |
| `demDelete` | `50:05` |
| `demOpen` | `50:12` |
| `demRename` | `50:17` |
| `demCreateNew` / `demCreateCommon` | `50:22` |
| `demCheckPath` | `50:44` |

`demlabel.c` supplies the original volume-label helpers consumed by the
`ATTR_VOLUME_ID` branch of `demCreateCommon`; it is not a separate BOP table
owner.

## Closure classification

The original files require historical `dem.h`/`softpc.h` register and guest
pointer helpers, OEM pathname Win32 helpers, native-handle publication, and
optional VdmRedir named-pipe helpers. `demfile.c` additionally retains its
original VDD SFT/JFT helper block. That VDD block is part of the imported
translation unit and must remain source-shaped; it cannot become a reason to
replace the BOP service bodies with partial-v2 code.

The first direct object observation is deliberately recorded before any shim:

```text
fatal error C1083: Cannot open include file: 'dem.h'
```

This is the expected historical-composition boundary, not an S3 failure or a
reason to restore the old file. The next allowed change is a named neutral
include/ABI shim that preserves original order and records every local source
divergence adjacent to the imported code.

## Implemented composition seam

`demfile_shim.h` reuses the scoped register/result and checked guest-memory
call record of `demhndl_shim`, rather than recreating a second CCPU surface.
`demfile_shim.c` supplies only the unavailable historical OEM helper layer:
OEM-to-ANSI conversion followed by the same Win32 file, attribute, rename,
volume and font APIs selected by the original owner.  It does not contain a
DEM algorithm, selector decoder, mutation profile, or filesystem model.

The imported `demfile.c` differs only at named composition boundaries:

1. its historical include closure becomes `demfile_shim.h`;
2. two historical raw-`HANDLE` AX:BP publications become checked session-local
   opaque-token publications, with the original `demClientError` failure path;
3. C11 forward declarations precede its retained K&R definitions.

The checked pathname gather is bounded at `MAX_PATH`, with one or two
NUL-terminated OEM strings copied from guest RAM according to the original
DS:SI, DS:DX and ES:DI references.  This replaces an unavailable raw SAS
pointer, not OpenNT pathname parsing.  VdmRedir named-pipe and VDD JFT/SFT
composition remain explicit historical-host dependencies: the original code
is retained, and the named-pipe branch returns its normal error when the
separate Redirector owner is unavailable.  No S3 BOP path invokes the VDD
fallback.

## Focused Direct witness

Fresh formal Ninja graph:

```text
build/M0-T230-S3/direct-demfile-r2
```

The MSVC x64 `/MT` graph source-built `bx-vdm.lib`, linked and ran
`t230-s3-demfile-direct-import-fixture.exe` with exit code zero on 2026-08-19.
It calls the imported bodies for `50:01`, `03`, `05`, `12`, `17`, `22` and
`44` against disposable temporary files, including token publication and
closure.  It also calls the `ATTR_VOLUME_ID` path through imported
`demlabel.c` with an invalid `?:LABEL` root and verifies the original carry
failure without changing any real host volume label.

This is local Direct source parity only.  It is not selector routing, a
Readonly/Overlay/Virtual claim, VdmRedir composition, VDD process-table
closure, or native guest execution evidence.
