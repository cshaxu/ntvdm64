# T95 S6 MSVC/x86 configuration-shape matrix 001 (externally blocked)

## Result

This is a frozen analysis attempt, not a configuration selection.  The matrix
root is `artifacts/analysis/t95-s6-msvc-x86-config-shape-20260811-001` and
must remain unchanged.  No variant is syntax-compatible; there is no selected
shape and no link, COMDAT, runtime, or product-recipe claim.

The coordinator stopped the activity after the recorded entry-path failures.
The first command did not start a compiler because the historical Community
`VsDevCmd.bat` path did not exist.  A later command using the installed
BuildTools path did start `cl.exe /Zs`; this distinction is retained rather
than treating the latter diagnostics as an entry-path proof.

## Frozen input and variants

Each variant contains a copied pinned `src/bochs` input.  Its generated
`config.h` includes the root-local `matrix-config-base.h`; none is under
`src/bochs`.

| Variant | Values varied | SHA-256 of `config.h` |
| --- | --- | --- |
| `a-smf111-win32` | CPU/MEM/DEV SMF `1/1/1`; attempted project `/DWIN32` | `31195A0B0FC737501A583130E1638077F5D38036B0E949A550A0CC0A4768D3CA` |
| `b-smf000-win32` | CPU/MEM/DEV SMF `0/0/0`; attempted project `/DWIN32` | `583BA7DA8F27FECFFD4F8101ABA67896253B0F5025031CACB1560D9AE33D560F` |
| `c-smf111-no-win32` | CPU/MEM/DEV SMF `1/1/1`; no project mapping | `6CF69E6F177195CE9462C7ED69A8EFEAC50D62E755FF4497D2BECB6905172BF8` |

The common base hash is
`7A92DF918B6DF2526873608B9A3E0C83DA1E127DA23D93116186941B107411E0`.
All denied device, GUI, plugin, x64, SMP, APIC, FPU, MSR, VMX/SVM/AVX and
WX feature values are `0`; `BX_WITH_NOGUI=1` and `BX_WITH_WIN32=0`.
`BX_SUPPORT_A20`, `BX_USE_IDLE_HACK`, `BX_EXIT`, and `BX_HAVE_*` fallbacks
are declaration-shape placeholders only, not a resolution of their ledger
rows.  A source-required empty `BX_CPP_AttrRegparmN(n)` fallback was added
only in the analysis root before the stop.

## Commands and observations

The fixture is `fixture_syntax.cc`; it declares only a null
`bx_ntvdm_minimal_machine_c*` and contains no constructor call, loop, or
runtime entry.  Candidate units were the two minimal helpers, SIM/parameter
support, log/PC, memory, empty port-space, and the declared CPU support set.
All invocations used `/Zs`, no `/Fo`, link, executable, configure, VS project,
`bochs.exe`, `all`, CPU loop, or run.

1. The initial commands used
   `C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat`.
   `Test-Path` was false and each `cmd.exe /d /c` command returned
   `The system cannot find the path specified.`  Logs are retained as
   `*-attempt1.log`; no compiler banner appears there.
2. The installed entry point is
   `C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat`.
   With `call ... -arch=x86 -host_arch=x64 && cl.exe /nologo /Zs`, compiler
   banner `Visual Studio 2022 Developer Command Prompt v17.13.5` appeared.
   The first pass reached `bochs.h:68` and reported C1083 for `unistd.h` in
   all variants, demonstrating that no `WIN32` project macro was inherited by
   that command.  These logs are the un-suffixed `*-zs.log` files.
3. A permitted local matrix retry supplied `/DWIN32` to variants A and B and
   added candidate include directories.  It reached the next declaration
   boundary: missing `BX_CPP_AttrRegparmN`, then C1083 for the ungenerated
   `instrument.h`.  The retained `*-attempt3.log` files record both facts.
   Variant C remained at the expected no-mapping `unistd.h` boundary.

No generated object, linked library, or new executable was produced; copied
historical files are not outputs of this attempt.

## Classification and next gate

Classification: **externally blocked before a valid, finite syntax matrix can
be evaluated**.  The frozen diagnostics establish only that a project-local
`WIN32` mapping is necessary for this Bochs header path and that the pinned
source assumes a generated instrumentation-header selection.  They do not
establish which instrumentation fallback is legal, any SMF result, or an
MSVC/Bochs build closure.

The sole next action is coordinator-owned verification of the approved MSVC
toolchain entry contract (exact entry path, project macro injection behavior,
and the source-backed instrumentation-header selection).  It must admit a new
packet before any new analysis root or syntax invocation.  Do not edit this
root, promote its placeholder values, or infer a product configuration.
