# T473 S10 — shard 3 provisional source review

Frozen ledger SHA-256 `F73FFD45DFF44FCA4ECEFC49EF769BEAB5C085542CD04FFE4D6A7B5F942E3196`; partition is SHA-256 of the UTF-8
relative path, first eight bytes as an unsigned big-endian integer modulo four
equals 3. There are 1168 unique rows in `file-review.csv`.

## Status and evidence limits

- Partial: 47; raw mirror difference is enough to require this
  reporting class, not proof that the changed behavior is missing.
- Excluded: 13; 12 retired CPU30 V86 executor paths under existing
  CPU40 policy and one original-empty ToolHelp source. No claim of
  equivalent CPU30 or full guest capability.
- Pending: 1108; neither identical bytes nor build selection
  proves a fully connected original implementation. No fully restored result
  is asserted without the needed per-owner runtime evidence.
- Diff observations: {'content-diff-or-other-line-change': 45, 'same-splitlines': 2}. `same-splitlines` is an exact
  line-content comparison ignoring line endings, not a behavioral result. The
  original-line similarity fields are heuristics, not restored-code percent.
- 656 rows carry the guest/media/build-input role. This role merely
  excludes them from host linkage; it does not establish guest capability.

The original three-way reporting target is not satisfied by pending rows.
Investigate their selected edition, actual linker extraction, callers,
execution, failure/lifecycle behavior and role-specific exclusion rationale
before claiming completion. The separately frozen external WOW audit rows are
outside this MVDM path partition and require the main-session reconciliation.

## Diff-marker follow-up, not a defect claim

The following non-line-ending-only partial files have no literal
`MVDM-HOST-DIV-*` marker in their bytes. Some may be formatting, declarations
or already registered exceptions with a different marker; they require
individual diff/provenance review, not automatic remediation:

- `dos/command/cmdkeyb.c`
- `dpmi32/data.c`
- `dpmi32/dpmi32.c`
- `dpmi32/dpmiselr.c`
- `inc/mvdm.h`
- `softpc.new/base/ccpu386/cpu4gen.h`
- `softpc.new/base/ccpu386/fpu.c`
- `softpc.new/base/ccpu386/iret.c`
- `softpc.new/host/src/nt_lpt.c`

## Targeted review of the 47 difference rows

Every partial row now records a specific source-owner chain, registered or
inline divergence IDs where found, a relevant retained evidence source and
its actual limitations. For eight selected WOW32 files the linked map contains
representative original object symbols; the corresponding current WOW32 DLL
hash matches package r10. For selected NTVDM files the NTVDM hash also matches
r10, and corresponding object symbols were reviewed where named. Map symbols
do not establish runtime dispatch or full semantic equivalence. For
`dos/dem/demdata.c`, the map has the four DEM data symbols as `<common>` but
does not identify `demdata.obj` as their source; extraction remains unproved.
For `softpc.new/obj.vdm/ntvdm.c`, the generated graph explicitly maps that
source to `ntvdm_entry.obj`, whose `_main` appears in the product map.

All 47 stay **partial** even for the two line-ending-only cases. Their
semantic restored-code percentages are intentionally blank: matching-line
percentages cannot prove which original behavior is connected. The remaining
1,108 pending rows have only inventory evidence and require substantive
owner-group investigation; this stage is not a finished three-way audit.

The marker list above is a heuristic. In particular, `dpmiselr.c` has an
abbreviated `DIV-310` marker, `inc/mvdm.h` uses `MVDM-SUPPORT-DIV-006`, and
other names are covered in S1's semantic-delta ledger. Verify local markers
and the register together before treating a file as unregistered.

## Unselected host-source review

All 23 paths originally labelled `unselected-host-source-requires-s2-
disposition` now have source-specific role, source references and bounded
next proof in the CSV. They remain pending; unselected status is not a
three-way disposition. Selected findings:

- `wow32/i386/fastwow.asm` is explicitly in the original
  `wow32/i386/sources`, not one of the generated C-table entries disabled by
  `!IF 0`. It provides original fast WOW callback/BOP code which references
  monitor state. The chosen non-fast path and its behavioral/performance
  boundary need specific proof.
- `ccpu386/ntstubs.c` contains the original direct
  `sas_overwrite_memory -> c_sas_overwrite_memory` wrapper. The current
  product map assigns the public symbol to C-VID `accessfn.obj`: its vector
  call is bound to the same CCPU leaf by `mvdm_cvidc_bind_vectors` and invoked
  from original `ev_glue.c`. This corrects the historical S1 provider
  description, but does not establish all the other stub capabilities.
- `nt_vflop.c` contains the actual `VFLOPPY`-conditional virtual floppy
  implementation; its scope, alternative and user-visible behavior require
  review. `forcedos.c` is a separate launch utility, so compare its promised
  behavior with the planned external launcher rather than discarding it merely
  because it is not an NTVDM library.
- Three BIOS assembler sources are firmware-construction inputs. Their role
  does not prove that the distributed immutable BIOS supplies their behavior;
  no guest rebuild or patch is authorized.

## Selected-source first-pass reconciliation

All 73 pending rows labelled selected product source now record a source
owner, relevant product-map observation, coherent r10 image identity where
available, and the remaining behavioral proof. The 34 CCPU rows include 29
objects whose exact original-ccpu386 archive/object names occur in the worker
map; the other 39 rows include 28 matching object names, eight without a
matching selected-binary map symbol and three VDMREDIR rows for which this
review did not locate a VDMREDIR map. These are symbol observations, not
proof of full archive extraction, original dispatch or guest behavior.

In particular, under the selected `SPC386` graph, original `c_main.c` sends
BSWAP and WBINVD to Int6 unless `SPC486` is selected; original opcode 0x90
handles NOP directly with `break`. The missing BSWAP, WBINVD and NOP object
map symbols therefore require a profile/behavior decision, not a claim that
they simply were not invoked by the tested workload. They remain pending.

[Fast WOW comparative audit](fastwow-comparative-audit.md) documents the
original i386 assembler path, actual CPU40 source path and object evidence,
real limits, and three implementation choices. It is an analysis, not
completion or approval to restore the NT4 monitor.

## Review of two original source roles

`wow32/wmmedia2.c` retains original whole-file `#if 0` around historical
structured multimedia bodies; original `wmmtbl2.h` registers only the
separate `WMM32CallProc32` route. T466 S1/S3 document the guest mmsystem and
WINMM ownership. This is not sufficient to establish equivalent guest/host behavior for
each disabled operation. The row remains **pending** until the active
MMSYSTEM/WINMM route and relevant normal, failure, callback and teardown
semantics are traced and verified.

`wow32/wthman.c` is the byte-identical original 20-line file containing only
header/include/pragma/MODNAME and no callable implementation. Original
`wthtbl2.h` maps ClassFirst/ClassNext to UNIMPLEMENTEDAPI. T470 S1/S2 trace
the guest ToolHelp owner and document the supplied guest-pair query limitation.
Exclusion means no host body is available to link; it does not imply the
ToolHelp guest APIs are functional.

A closer check of T457 evidence found S4 labels the dialog witness
“input/caret”, while S2 explicitly describes keyboard processing and records
the caret desktop-state fixture as a non-pass. `wucaret.c` thus remains
pending; the broad S4 label cannot replace a real caret-specific witness.

## Guest input audit

[Guest input and image review](guest-input-review.md) adds per-file guest
roles, precise SHA-256 matches to the archived/installed guest images and
strictly bounded family observations for all 656 guest-labelled rows. Twelve
mirror rows match eleven different installed and archived guest files; this
does not establish source-to-image lineage or full guest behavior. The ZIP
contains the two separately staged MMSYSTEM/TOOLHELP images that were absent
at the currently inspected System32 path. All formerly pending guest rows
remain pending pending actual source/capability evidence.

## Header and build-carrier first pass

For 199 pending `.h`/`.inc` inputs, `header-inclusions.csv` records the
exact observed inclusion relationships in VALID cached Ninja deps, restricted
to objects whose source has a product-selection row in the frozen ledger.
There are 96 headers with 2,633 selected-source inclusion edges and 103 with
no such observed edge. The edges show compilation dependencies, not which
conditional declarations were active, final link extraction or runtime
behavior. Assembly, generated carriers and non-product consumers need their
own proof. The remaining 59 pending build/resource/binary carrier rows now
record their role and required provenance or interface evidence.

One important instance is the original `cvidc/sas4gen.h`: no selected-source
VALID dependency names that mirror path. Eighty-four selected worker objects
include the retained NTVDMx64 x86/prod generated carrier instead. The latter
has additional `sas_touch`, `VirtualiseInstruction`, and `IsPageInstanceData`
slots/declarations; the selected CCPU fallback traps for the first two. T411
and the original-source comparison document the patch provenance. Actual
reachability and behavior cannot be inferred from header identity alone.

The historical `wow32/obj/i386/wow32.def` lists 27 exports, all present in
the maintained source DEF; the current generated x86 DEF adds the existing
task-termination export and two decorated COMM aliases. Selected export
spot checks do not prove caller ABI/semantics. The original SoftPC resource
script is input to the build-owned generated RC and linked into NTVDM; its
individual dialog/resource behavior still needs separately bounded witnesses.

[SAS generated vector audit](sas4gen-vector-audit.md) traces both breakpoint
tails from original initializer through the selected product object, separates
the absence of direct textual calls from proof of unreachability, and records
the minimal recovery decision without changing product code.
