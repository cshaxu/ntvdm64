# S12 conditional-exclusion audit — preliminary measured snapshot

Baseline: 6780e9bfb. Scope: current checked-out `src/mvdm`,
`src/non-mvdm`, `src/compat` C and H files. Read-only source survey;
no claim of whole-program reachability, supported-configuration closure,
or permission to delete guest material.

## Method and limits

Enumerated C/H and literal `#if 0` blocks by physical-line scanning.
Counts below are nonblank, non-preprocessor-directive physical text lines;
comments and non-code text remain included. Nested literal-zero blocks are
counted once; `#else` / `#elif` bodies are not presumed inactive. This is a
lower-bound *lexical sample*, not a count of all compiler-excluded code; it
also includes files outside any selected graph. It does not perform complete
C comment/line-splice parsing and therefore is approximate.

Selected C sources are identified by `ninja -t commands` for the 10 product
targets in the current x86/x64 release graphs and normalized by source path.
They are graph-command inputs, not proof of linked or executed code; this
method does not count a `.c` file textually included by another source as a
standalone compiler input. Example: non-MVDM `handles.c` is included by
`src/wow32-dll/source/wow_user_object_bindings.c`; it is active input even
though it has no separate compile command. No header or file may be deleted
based on this count.

| Tree | C/H files | C/H physical lines | literal `#if 0` nonblank nondirective lines, all C/H | C files with product graph command | lines in those C files | literal `#if 0` nonblank nondirective lines in those C files |
| --- | ---: | ---: | ---: | ---: | ---: | ---: |
| mvdm | 1,794 | 855,142 | 9,175 | 443 | 450,471 | 6,169 |
| non-mvdm | 315 | 136,442 | 142 | 94 | 58,271 | 56 |
| compat | 23 | 1,645 | 0 | 2 | 370 | 0 |

Other MVDM C source files not seen as standalone ten-target commands number
364 / 238,300 physical lines. This includes Win16 guest source, separately
built tools, and potential textual includes: it is not dead-code inventory.
In non-MVDM, `handles.c` and `srvinit.c` have no standalone command; the
former is demonstrably textually included and the latter has audit/test
references. `src/compat` is authored compatibility code, not an original
mirror.

## Examples

Five selected WOW32 files contain original enclosing literal-zero blocks:
`wmmedia2.c` (2,735), `wmmstru1.c` (1,318), `wmmstruc.c` (593),
`wmmstru2.c` (410), `wmmedia1.c` (407), total 5,463 lines under this
textual metric. The first two have matching `#if 0` locations in local
OpenNT source. These contain historical wave/MIDI and MCI conversion code;
the distinct selected `wmmedia.c::WMM32CallProc32` bridge is not evidence
that all equivalent multimedia behavior is implemented. Prior bounded
reviews: [MCI converter](cross-369/mci-converter-disposition.md),
[multimedia owner](host-wow/local-multimedia-declaration-followup.md).

`src/mvdm/oemuni/process.c` contains about 170 such lines around a disabled
`CreateProcessOem`. Selected non-MVDM source examples include `editsl.c` (21),
`clres.c` (14), `mngrayc.c` (12), `editml.c` (7). These are candidates for
individual analysis, not blanket removal recommendations.

Existing S11 inventory [conditions](condition-inventory/README.md) records
conditional-site and compiler-flag census but explicitly does not determine
preprocessor reachability. Common labels such as `CCPU`, `CPU_40_STYLE`,
`PROD`, `MONITOR`, `V7VGA`, `DBCS`, `DBG`, `UNICODE` and `SUPPORT_LPK`
cannot be globally characterized from a name or command-line flag alone.

## Required remaining work

For each actual product translation unit and relevant supported configuration,
replay compiler preprocessing with its actual include/define/undefine order;
attribute emitted original lines and macro-only expansions by source file;
combine all contexts, including textually included C and headers, and classify
never-selected vs context-dependent alternatives. Separately classify whole
unselected files against guest, add-on, tooling and recovery ownership. Trace
semantics, consumers, upstream provenance and possible duplicate/recovery
obligations for each proposed removal. A line-count scan cannot prove any of
these. This snapshot does not yet provide the requested complete exclusion
inventory or total.

The existing [source policy](../../docs/etc/operations/policy/source-policy.md)
already admits a registered true subset, while retaining provenance and
package boundaries. Guest immutability remains independent of that rule.

## Current build-object observation

The current ten product target command closures were enumerated again and
actual COFF object section tables inspected for each C input. Among 443 unique
MVDM C inputs, 38 have no machine-code section in any observed object; among
94 non-MVDM C inputs, 3 have no code section; both selected compat C inputs
have code. All 539 objects/unique-source groups were readable (zero object
format/missing errors). The other 405 / 91 MVDM / non-MVDM source inputs have
some object code. [Full path list](s12-conditional-coff-snapshot.json).

No-code-section is not equivalent to empty or removable: data-only objects
(such as DEM and command data files), linker directives, static initialization,
header effects and textual includes can be significant. A prior object audit
specifically confirms `prt_scrn.c`, `cpucstbs.c` and `x86_emm.c` as having no
code sections for the then-observed selected profile; these correspond to an
old Print Screen placeholder, A3CPU guard, and LIM/MONITOR alternative.
[Evidence](shard-0/excluded-empty-host-objects-review.md). The MCI converter
`wmmstru2.c` has an original `#if 0` body, although its object may contain
metadata. [Evidence](cross-369/mci-converter-disposition.md).

COFF binary section size cannot be converted reliably into a percentage of
source lines compiled; therefore this observation does not fill the requested
mostly-excluded / partly-excluded / fully-included source-line buckets.
