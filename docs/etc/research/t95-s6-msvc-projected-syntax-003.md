# T95 S6 MSVC projected syntax observation 003

## Result

The one admitted v2 projected syntax batch **closes for every declared
translation unit**: BuildTools x86 `cl.exe /nologo /Zs` returned exit code 0
for the fixture and all minimal composition, SIM, memory, port-space, CPU,
SIM/parameter, and `cpu/exception.cc` candidates. This is syntax-only
evidence. It establishes no object, COMDAT, link, executable, reset, firmware,
guest, adapter-runtime, or product-configuration claim.

## Inputs and command

The root is
`artifacts/analysis/t95-s6-msvc-x86-projected-syntax-003-20260811-001`.
Before replacement its copied root config matched pinned `refs/bochs/config.h`
at SHA-256 `A1FADC25502FB0A4644917B6F3A62987C125CC7335457F9C59E7CE7E62A45AC8`.
Only that copied config was then replaced with accepted v2 SHA-256
`C47030F79880C894A346F56E3AB3242924308983E00B5845EDCE849450C6C19D`.
The original copied `instrument/stubs/instrument.h` exists, and a pre-command
complete C/C++ scan found zero unspaced `FMT_LL` forms, preserving the accepted
14 whitespace repairs.

`logs/syntax-command.txt` retains the complete absolute command. It enters the
confirmed VS2022 BuildTools x86 environment and invokes exactly one batch with
`/DWIN32`, `/DBX_NTVDM_ENABLE_EXCEPTION_INTERCEPT=0`, original copied root/
candidate/instrument-stub include directories, the prior non-product fixture,
and declared minimal candidates including `gui/siminterface.cc`,
`gui/paramtree.cc`, and `cpu/exception.cc`. No flags/SMF/config variant were
changed. The command has no `/c` or `/Fo`, and the root contains zero `.obj`
files. No link/run/configure/VS project/`bochs.exe`/`all` action occurred.

## Warnings and limits

The log has zero compiler errors and 16 C4005 warnings: projected
`config.h:35` provides `FMT_LL`, then original `osdep.h:59` provides its MSVC
`FMT_LL` definition in each translation unit. This warning is retained as an
observation; no macro/config source change is authorized or implied here.

## Next gate

The next gate is coordinator review of syntax closure and the retained C4005
warning before any separately admitted object/COMDAT/link closure work. No
automatic compilation retry, warning suppression, configuration edit, or
runtime claim follows.
