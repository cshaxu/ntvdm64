# Compile-condition inventory (T473 S11)

Snapshot: commit `64840052bebf7386c0c91608ab1d8b4799177e29`. This is an inventory only: no condition was accepted, excluded, or changed by it.

- [tags.csv](tags.csv): identifiers appearing in C/H `#if`, `#ifdef`, `#ifndef` and `#elif`, ordered by occurrence. The build-profile column means at least one command in that build tree explicitly mentions a `/D` or `-D`; it does not mean every translation unit or occurrence uses that definition.
- [sites.csv](sites.csv): every conditional directive, full line-spliced expression, path, starting line, identifiers and source SHA-256. Conditional `#else` alternatives are represented by their enclosing conditional, not additional rows.
- [build-defines.csv](build-defines.csv): explicit compiler `/D` or `-D` flags found in Ninja command dependency closures for the ten named product targets. Counts are command occurrences, not live objects, linked functions, or behavioral coverage. Generated commands may contain fixture or generator dependencies.
- [summary.json](summary.json): scope and limitations.

The lexical C/H scan includes all tracked `src` C/H, including unselected sources and guest-target files; it does not authorize modifying guest or make those files product inputs. It includes include guards, local and compiler macros, numeric-comparison identifiers and inactive branches. Identifiers are not individually resolved macro definitions. Multi-line directives are joined; C comments and string literals are excluded from token extraction. Assembler, resources, generated source and other non-C/H formats are not enumerated here; they remain separate dependencies where relevant. Build flags hidden in headers, compiler predefined macros, `/U`, forced includes, macro redefinitions and transitive execution paths need actual preprocessor/source analysis. A missing explicit build flag is not proof that a macro is undefined.

Examples requiring source- and translation-unit-level review: `CPU_40_STYLE`/`CCPU` select the software executor; `CPU_30_STYLE` has no C/H conditional site in this snapshot; `MONITOR` appears in both excluded historical code and selected printer compilation; `X86GFX` also covers selected software video; `i386` and `_X86_` are distinct identifiers; `V7VGA` selects a video profile. A macro name by itself is never a removal decision.

## Pre-remediation target command-line selection

The following CSVs record the pre-remediation command graph; their names are
historical and do not describe the candidate's rebuilt configuration. The
candidate's current authored-source boundary is checked against actual Ninja
commands by `tests/build/test-authored-softpc-profile.py` below.

- [current-target-conditions.csv](current-target-conditions.csv): one row per target and observed identifier. `last_D` and `last_U` describe the last explicit `/D` or `/U` option in each source compile command; `neither` means no command-line instruction. `both_D_U` counts commands containing both. Values are before preprocessing and can be overridden by forced includes and source/header directives. The command set is the Ninja dependency closure for each target, including generated or shared dependencies; it is not a count of linked functions.
- [current-compile-units.csv](current-compile-units.csv): individual command source paths, explicit definitions/undefinitions and their original option order. A row is a compiler command, not necessarily a unique source file or a linked provider.
- [compiler-predefined-x86.txt](compiler-predefined-x86.txt) and [compiler-predefined-x64.txt](compiler-predefined-x64.txt) record actual bare-compiler preprocessing using [compiler-predefined-probe.c](compiler-predefined-probe.c). They do not include any project headers or command-line macros.
- MSVC test [D then U](compiler-ansi-du.txt) / [U then D](compiler-ansi-ud.txt) uses [compiler-ansi-probe.c](compiler-ansi-probe.c): the later option determines `ANSI` at the start of preprocessing for the observed commands. All 56 WOW32 commands containing both use `/DANSI /UANSI` and also `/DUNICODE`; the source may still modify those macros later.

At this snapshot, the NTVDM target closure contains 481 compiler-command rows: `/DCCPU` on 450, `/DCPU_40_STYLE` on 451, `/DMONITOR` on 5, and `/DX86GFX` on one. The five explicit MONITOR sources are `nt_lpt.c`, `bios.c`, `keybd_io.c`, `video.c`, and `printer.c`; video.c is the sole explicit X86GFX source. The NTVDM `accessfn.c` command has CPU_40_STYLE but no `/DCCPU` or `/DPROD`. Never project a target-wide setting onto every row.

## Authored-component isolation candidate

The owner requires original-source selection to be established at its source
boundary; authored consumers should use the selected interfaces without
inheriting historical execution-profile selectors. The candidate remains
unpublished. [Non-MVDM sites](non-mvdm-conditional-sites.csv) include
the project host-compat subtree and provenance hints, not a final classification.
Other OpenNT ABI/host mirrors and addons require separate provenance review.
[WOW selector uses](wow-selector-build-uses.csv)
are observed Ninja command inputs, not proof of function reachability.

The selected WOW original bodies retain their original options. A separate
adapter profile removes CPU_40_STYLE, NEW_CPU, CCPU, C_VID, SPC386, SIM32,
V7VGA and PROD only from project-owned support sources. Replaying the actual
MSVC preprocessing commands with and without those options yielded identical
output for all [71 selected adapter commands](wow-adapter-profile-comparison.csv).
This is configuration evidence, not runtime or link-semantic proof.

The corresponding NTVDM comparison found 50 of 59 selected adapter command
outputs byte-identical; nine differed. A per-flag follow-up found one of the
nine differs only in preprocessor line directives, while the other eight have
selected header declarations, layouts or call expansion differences. Do not
conclude that an absent option is safe just because compilation succeeds:
selected SAS and VGA headers use the flags to expose different types and
behavior. The affected units need narrow original-owner interface review.
The [per-flag comparison](adapter-individual-flag-comparison.csv) records
normalized preprocessor results against the pre-remediation adapter sources;
it is a diagnostic snapshot, not proof about later changed source or ABI.
The C-VID vector binder now uses provider declarations and the SAS enum
extracted by its existing generator from original headers; it supplies no CPU
profile or local CPU_PRIVATE selector. The prior and integrated macro-free
binder disassemblies matched in 633 normalized lines, and the
actual original C-VID fixture passed its 81-slot and read/write/mark binding
checks with the integrated generator. The text-video adapter consumes
source-derived V7-selected declarations generated from the original headers;
its experimental and original disassemblies matched in 228 normalized lines
after preserving the source path. In the actual integrated source, the only
three instruction differences are updated `__LINE__` diagnostic arguments
after removing include lines; executable behavior and source path otherwise
match. The actual NTVDM/WOW32 graphs now pass
`tests/build/test-authored-softpc-profile.py`, covering 428 authored/generated
compile commands in the selected x86 and x64 product graphs. Runtime integration
passed the [16-gate Full suite](profile-p-full-timings.json) on the exact
[ten-image candidate](profile-p-package-manifest.json). Final focused evidence:
134 CCPU boundary cases, the original C-VID vector fixture, the V7 pointer
binding fixture, 46 keyboard/PIC assertions and 24 PIC reset/EOI assertions.
The [Win3.1 matrix](profile-p-win31-matrix.json) passed standard/enhanced by
DOSONLY on/off on a temporary installation copy; [integrity](profile-p-win31-integrity.json)
records 68 unchanged original and copied guest images. These startup/exit
probes do not establish physical mouse or exhaustive application behavior.

The explicit MONITOR consumers and C-VID accessfn exception described above
remain original-source selection, not an authored runtime mode. CPU30 is
excluded; the active CCPU software V86 remains in scope. No guest is rebuilt.

`OPENNT_ADAPTER_NT_ALERT_THREAD` selects the adapter-backed, non-imported
`NtAlertThread` declaration in `opennt-host/public/sdk/inc/ntpsapi.h`.
The candidate retains it for the authored event-thread caller and its
implementation, rather than handing it to every adapter.
`MVDM_SOFTPC_NO_HOST_BOOT_FILE_MUTATION` selects the original host bootstrap's
no-host-mutation branch in `nt_msscs.c`; it is not an adapter profile.
In the original SoftPC headers `ANSI` selects standard-C prototypes and
`const`, not just character encoding. The original `stubs.c` undefines it;
compiling a candidate with that undefinition removed produced identical
instructions and relocations for the selected object, with source-path debug
metadata differing. Three headers now retain the standard-C branch and omit
the old branch, and the selected stub keeps `ANSI`; see MVDM-HOST-DIV-479.
The ten affected NTVDM adapters matched previous preprocessed content after
removing line markers and blank lines; 51 others were byte-identical already.
The 71 actual authored WOW adapter compile commands produce byte-identical
preprocessed output without `ANSI`: see [comparison](adapter-remaining-flags.csv).
The authored WOW build no longer defines it; the original WOW source profile
and original USER Unicode selection remain unchanged. Authored NTVDM units
likewise no longer define it. The published candidate passed the Full suite
and Win3.1 mode matrix linked above.
For `NTVDM`, all 71 WOW and 60 of 61 NTVDM adapter commands were identical
without the command-line flag. The remaining keyboard reset unit included
configuration-sensitive PIC headers for two functions; it now declares those
original functions' exact signatures instead. The comparison
records the pre-change results; original-source compile flags remain intact.
MSVC disassembly of the keyboard reset object before and after the dependency
change differs only in the dump's input filename.
After adding local divergence comments without changing line counts, a rebuild
of NTVDM was compared after applying the candidate's package identity. All
bytes match the previously built candidate except the PE COFF timestamp and
the matching timestamp in its debug-directory entry. The WOW32 build input
hash is identical. The sealed earlier candidate remains the tested package.

The V7 pointer binding uses the selected original declarations from
`egaports.h` and `gfx_upd.h`, including the original 0x20000-byte plane size,
without importing their alternate configuration branches. Its focused test
checks callback registration, first invalid offset and null-plane behavior.
The worker shutdown fixture passed all 37 assertions in the same unrestricted
process environment used for product tests. In the restricted environment its
invalid-handle case instead raised `STATUS_INVALID_HANDLE`; this was also
observed when linked directly to the unchanged shutdown object. The restricted
result is an environment-specific non-pass, not evidence of a runtime fix.
The unused WOW build-only `MVDM_WOW32_PROVIDER` and `STD_CALL` defines were
removed after a repository-source and generated-WOW-source search found no
consumer. The selected `/Gz` calling convention remains unchanged.
