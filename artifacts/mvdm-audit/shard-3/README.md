# T473 S10 — shard 3 provisional source review

Frozen ledger SHA-256 `F73FFD45DFF44FCA4ECEFC49EF769BEAB5C085542CD04FFE4D6A7B5F942E3196`; partition is SHA-256 of the UTF-8
relative path, first eight bytes as an unsigned big-endian integer modulo four
equals 3. There are 1168 unique rows in `file-review.csv`.

## Status and evidence limits

- Partial: 47; raw mirror difference is enough to require this
  reporting class, not proof that the changed behavior is missing.
- Excluded: 12; retired CPU30 V86 executor paths only, on the
  existing CPU40 source-policy and product-selection evidence. No claim of
  equivalent CPU30 behavior.
- Pending: 1109; neither identical bytes nor build selection
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
1,109 pending rows have only inventory evidence and require substantive
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
