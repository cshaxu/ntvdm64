# T473 S12: original macro propagation into authored components

The owner accepted the delivered package and reopened S12 to audit macro
propagation. This is a read-only investigation against source commit
`6e48bef8c`, not an implementation or product release.

## Scope and method

The selected product graphs for x86 NTVDM/WOW32 and x64 NTVWM, NTCON, NTMON,
Run16, NTSRV and Hook64 contain 428 authored/generated compiler commands in
the existing SoftPC-profile gate. Twenty are vendored Detours commands, which
are outside this authored-code count. The remaining 408 commands (191 x86,
217 x64) were replayed with their actual options using MSVC `/E /d1PP`.
Definitions and undefinitions were tracked with emitted `#line` ownership;
a name is counted if a definition from `src/mvdm-host` or `src/non-mvdm` was
still active while a project-authored source or header was being processed.
Generated RPC sources were checked with their build-directory paths as well.
Two x86 transport probes initially failed because the audit runner parsed a
quoted `/I` option incorrectly; both were rerun successfully with that
runner fixed. No compile output or source was changed.

This is a union of names visible at any such boundary, not 4,295 macros all
visible together, all used, or all build selectors. It includes ordinary API
constants, function macros, header guards and definitions introduced by
registered project divergences. `#line` provenance and preprocessing prove
observation within these selected commands, not every configuration, include
ordering at every line, independent add-on, test fixture, Detours body, or
guest build. The deduplicated [name, provider and unit inventory](macro-names.csv)
is the complete result for this method. The [runner](reproduce-live.py)
requires the corresponding MSVC developer environment and writes disposable
output below `build/t473-s12-flat`; it does not build guest.
An independent rerun of the retained runner completed all 191 x86 and 217 x64
commands with no failures or unvisited owned source, and reproduced all
4,295 CSV names, origins and per-architecture/graph/source counts exactly.

## Counts and established selections

| Observation | Count |
| --- | ---: |
| Distinct original-header macro names visible at an authored boundary | 4,295 |
| MVDM-only / non-MVDM-only / defined by both origins | 3,573 / 715 / 7 |
| Authored/generated commands with at least one such name | 229 of 408 |
| Commands with an MVDM-origin name | 26, all x86 |
| Clearly identified leading include guards | at least 25 of the 4,295 |

The existing `test-authored-softpc-profile.py` still passes, but checks only
the ten named `/D` flags and conditionals in selected source files; it does
not detect macros introduced later by original headers or conditions in all
included headers. Examples confirmed by the compiler trace:

- `host_def.h` defines `NTVDM`; it becomes visible in six authored NTVDM
  source commands even though the build no longer passes `/DNTVDM` to them.
  Visibility alone does not establish an authored `#if NTVDM` or a runtime
  defect. Its downstream influence needs an interface-by-interface decision.
- `wow32/precomp.h` defines the project-added, registered mirror selector
  `WOW_USER_PRIVATE_REDIRECT`. It is visible in 11 authored WOW commands
  and selects redirects at three conditional sites in two authored headers.
  This is a project divergence inside the mirror, not an NT4-original macro.
- The original-subset `ntrtl.h` defines the project-added include guard
  `OPENNT_HOST_NTRTL_SUBSET_H`; an authored WOW source tests that guard to
  choose a fallback declaration. Its meaning is a header-presence guard, not
  a CPU feature flag.
- The owned generated `sas4gen.h` contains 58 `CCPU` and 58 nested `PROD`
  condition sites. Actual C-VID binding preprocessing sees both undefined;
  `NTVDM` is visible from `host_def.h`. This is an inherited conditional
  structure in a source-derived header, not evidence that the compiler passed
  either CPU flag to the authored C file. The previously recorded binder
  disassembly and vector fixture remain separate behavioral evidence.
- `src/addon/pif/pif.c` directly defines `NTVDM 1` before including the
  original `pif.h`; this independent add-on is outside the 408 product
  commands above. An MSVC syntax-only comparison with its actual x64 include
  path and warning flags passes as written, but removing the definition in a
  temporary source copy fails on the unselected property-sheet declaration
  (`LPFNADDPROPSHEETPAGE`). This proves the current compile dependency, not
  that keeping the selector is the only clean implementation.

A lexical same-translation-unit join produced four candidate names. Ordered
compiler traces show why raw intersections must not be treated as four
leaks: `WCB16_CDECL` and `WM_DRAGLOOP` are first decided by project fallback
headers before the relevant original definitions; `WOW_USER_PRIVATE_REDIRECT`
and `OPENNT_HOST_NTRTL_SUBSET_H` are actually present at the documented
authored decisions. Deduplicated ordered [trace states](x86-trace.json) and
[SAS states](x86-sas-trace.json) preserve those observations. This candidate
scan does not prove there are no indirect, nested or generated cases beyond
those observed.

A separate lexical sweep covered 534 tracked C/C++ source and header files
under project `src` roots, excluding both original mirrors, the guest tree and
vendored Detours. For the explicitly checked SoftPC profile names and
`WOW_USER_PRIVATE_REDIRECT`, it found the 116 generated `sas4gen.h` conditional
sites, three WOW redirect condition sites in two headers, and the PIF source
definition of `NTVDM`; the other named profile labels had no direct directive
in that sweep. This does not scan assembly, build scripts, inactive generated
files under `build`, macro aliases, ordinary expression uses or other labels.

## Command-line selections

`_CSRSRV_`, an NT4 header selection macro, and the project-defined
`OPENNT_BASE_VDM_SERVER` each occur in 43 authored compiler commands across
17 distinct NTSRV source paths. In the 17 x64 service commands, removing
`_CSRSRV_` changes 15 preprocessor outputs and leaves two unchanged; the
original `ntcsrsrv.h` uses it to determine `NTCSRAPI` import decoration.
Removing the project selector leaves five outputs unchanged and makes 12
preprocessing runs fail because the selected original branch asks for the
unavailable `ntdddfs.h`. See [per-source outcomes](service-preprocess.json).
Neither finding alone proves the right replacement or a runtime failure.

`OPENNT_ADAPTER_NT_ALERT_THREAD` is a project-defined adapter selection in
two x86 authored commands; it controls an adapted original header, rather
than being an original macro inherited by the application. Common compiler,
CRT and SDK options such as `WINNT`, `_WIN32_WINNT` and
`_NO_CRT_STDIO_INLINE` are separately configured and are not classified as
SoftPC macro leakage merely because original headers also see them.

## Disposition

The audit disproves the broad claim that no original-tree macros reach
authored code. It does not justify deleting thousands of API definitions or
switching CPU branches. The concrete follow-up is to audit and minimize the
six-unit `NTVDM` header exposure, the PIF `NTVDM` definition, the 11-unit WOW redirect boundary, the
`_CSRSRV_`/service declaration selection, and the source-derived SAS header's
116 conditional sites as coherent interface changes; preserve selected ABI
and prove preprocessed and compiled behavior before any removal. Other visible
names are classified by the inventory, not presumed defective.
