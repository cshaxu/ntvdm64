# Original export-source recovery findings

**Historical baseline:** the initial file hashes and missing exports below
describe the pre-recovery candidate. The current working-tree supplemental
definition has been restored byte-for-byte to its registered original; the
unpublished template-based candidate and its verification are recorded in
the later sections of this review. No runtime capability is inferred solely
from byte identity.

Two source files require partial classification. Their
[per-file records](export-source-partial-dispositions.csv),
[actual observations](supplemental-export-definition-review.json) and
[independent template review](shard-1/export-template-profile-review.md)
separate source retention, export-name availability and runtime semantics.

## Selected modified declaration

`softpc.new/obj.vdm/obj/i386/ntvdm.def` is absent from the two primary
comparison sets but has a registered supplemental original. Its original
SHA-256 is `98C00C3C4737108674061779A6C1E06F113C597D76536A49E0723AA617D58A7A`;
the current file is `09B1B928016158B2B5492C6726B0E1CE2EFBF07C4D14D0ED1C5DFDD4D0EFE362`.
Commit `9e8821856` added exactly one declaration,
`Sim32FlushVDMPointer`; all 136 original non-comment declaration lines
remain. Text retention is therefore 100%, while the mirror is still partial
under the owner's difference rule. This is not executable-code or behavior
coverage.

The file is active configuration input. T310's generator at lines 168 and
443–454 reads it into `generated/ntvdm-wow32-provider.def`. Actual package-c
dumpbin output contains the added export at ordinal 44, RVA `0004DA90`, and
all 134 original export names. Source/file selection inferred only from
`ninja -t inputs` missed this configuration read. Historical T405 evidence
had already identified this exception; this pass reconnects it to the current
whole-file ledger. Its path beneath `obj/i386` does not justify exclusion.

## Original template/profile gap

Canonical `softpc.new/obj.vdm/ntvdm.src` is byte-equal to both pinned primary
originals at `D47400179C1388B58FB8EAEC8331C6EEA16EE548BBA10E7AFBE588E9549BD0BC`.
It is currently bypassed by the static declaration described above.
Real x86 CL preprocessing with `i386` and `CPU_40_STYLE` yields 140 distinct
declaration names. This preprocesses metadata only; it defines no CPU30
profile and compiles/executes no monitor or guest code.

The sealed package exposes 138 of those 140 names (98.57%). The two absent
names are `VdmQueryFreeVirtualMemory` and `VdmReallocateVirtualMemory`, both
in the template's CPU40 branch. Four of that branch's six names are supplied
by the current project extension list. This is a concrete incomplete original
profile surface, not a reason to exclude the original template.

Name presence does not prove aliases, DATA, calling convention, provider
identity, ordinals or behavior. Name absence does not prove an internal call
fails: original DPMI code also has direct/in-process routes and approved
allocator adaptations. Source search finds original `nt_mem.c` definitions;
the cached map includes the query provider, while reallocation linkage and
its selected consumers require their own acceptance. No executable-code
restoration percentage is asserted for this declarative template.

## Finite recovery and acceptance

S11 owns recovery of the canonical original export-generation boundary:
use the primary-original template and explicit admitted x86/CPU40 metadata
profile; retain project additions in the existing generated extension layer.
Do not silently widen supplemental source admission or remove the published
`Sim32FlushVDMPointer` API. Restore the modified historical declaration's
original bytes when it no longer carries a project addition.

The group must resolve both missing original names through their real memory
owners, verify signatures/aliases/DATA, preserve the existing published
surface, and exercise valid and invalid allocation/query/reallocation/free
lifecycles before enabling or declaring the new surface complete. Coordinate
the original memory service checks with S12. Verify original-template profile
selection, cached generation/link propagation, no-change incremental builds,
actual PE exports, WOW restart/retirement and existing DOS/XMS/DPMI/VDD gates.
Source presence or forcing a link is not that acceptance.

This source-only audit changes no runtime or guest. The two partial rows keep
the source-policy/registration and runtime obligations visible instead of
labelling the generated carrier unused or the name ratio full functionality.

## S11 current-object owner recheck

The current cached worker link map binds `VdmQueryFreeVirtualMemory` to the
selected original `nt_mem.obj`; it does not list `VdmReallocateVirtualMemory`.
The separately linked original-external-memory fixture map binds **both** to
that original object, and its source exercises grow and shrink. This shows the
reallocation body is available in the selected fixture composition, but does
not by itself prove why it is absent from the worker map, its PE export, or
its use through an external caller. The current generated provider definition
still omits both names while explicitly adding the other four CPU40 memory
exports. Input hashes are in `s11-foundation-evidence/export-current-owner/`.

The original template also carries aliases and DATA entries, and the generated
provider adds a substantial private ABI. Recovery therefore needs an actual
preprocessing and complete declaration/alias/DATA comparison, selected-provider
link, published PE export comparison and functional lifecycle checks; simply
appending two names would not recover or verify the original template.
No generator, mirrored file, guest or product image was changed by this check.

The saved real-CL `i386`/`CPU_40_STYLE` template output was compared with the
current generated worker definition by export name, alias target and DATA
attribute. All 138 shared template entries matched their alias and DATA
attributes; exactly the two previously identified memory exports were absent.
This is a declaration-level comparison, not a PE ordinal/ABI or behavior
comparison. Four explicitly appended project-extension lines duplicate names
already present in the CPU40 template (`VdmAllocateVirtualMemory`,
`VdmFreeVirtualMemory`, `VdmAddVirtualMemory`, `VdmRemoveVirtualMemory`), so a
template-based generator must avoid duplicating those declarations.
`Sim32FlushVDMPointer` is present in the currently selected supplemental list
but absent from the selected i386 template and must remain in the generated
extension layer. `CMakeLists.txt` currently tracks the generator scripts for
reconfiguration but not `ntvdm.src`; selecting the template at configure time
must also track that input. Template and definition input hashes are retained in
`template-comparison-hashes.json`. Existing PE ordinals, ABI consumers, cached
regeneration and actual executable lifecycle remain acceptance obligations.

## Unpublished canonical-template candidate

The current worktree generator preprocesses original `ntvdm.src` with MSVC
`i386`/`CPU_40_STYLE`, appends private exports without duplicating the four
original memory names, and retains `Sim32FlushVDMPointer` in the extension.
CMake tracks the template as a configure dependency. The now-unselected
supplemental `obj/i386/ntvdm.def` was restored byte-for-byte to its registered
original hash `98C00C3C4737108674061779A6C1E06F113C597D76536A49E0723AA617D58A7A`.
No guest source or binary changed.

The cached x86 worker and dependent WOW32 provider linked successfully; a
subsequent no-change build reported no work in both Ninja subtrees. Original
external-memory fixture returned zero and reported allocation grow/shrink,
EMS/SAS and DIB sharing controls; it links controlled seams with `/force:multiple`
and is not an external PE export call. Dumpbin comparison with the prior
idle-shared candidate shows 269 old names, 271 new, no missing names and only
`VdmQueryFreeVirtualMemory` and `VdmReallocateVirtualMemory` added. **197
existing ordinal numbers changed** due to linker assignment; this is an ABI
risk, not an accepted compatibility result. Consumer import mode and ordinal
contract must be established and handled before any release. Evidence and
hashes are in `s11-foundation-evidence/export-template-candidate/`. No coherent
candidate package, original external PE entry call, product Full gate, or
release has been produced for this candidate.

The prior coherent candidate's PE import tables were read directly: WOW32.DLL
imports 150 NTVDM entries by name and no NTVDM ordinal; VDMREDIR.DLL imports
44 by name and no NTVDM ordinal. Dumpbin receipts and hashes are in
`export-template-candidate/`. These two binaries therefore do not rely on the
changed ordinals for their static NTVDM imports. This is not proof about
third-party VDDs, dynamic `GetProcAddress` calls or other versions; dumpbin's
hex value beside a named import is a hint, not an ordinal import. The new
candidate still needs compatibility and product validation before publication.

An explicit-tail-ordinal experiment assigned the two new names 270 and 271,
leaving existing names implicit. Actual MSVC LINK did not fill the earlier
implicit range: it assigned existing names starting at 272, changing all 269
previous ordinals. `tail-ordinal-experiment.txt` retains the failed PE export
witness. That experiment was removed from the generator and both worker and
WOW32 were rebuilt successfully. The current unnumbered candidate still has
the earlier observed 197 ordinal changes; no ordinal compatibility is claimed.
A complete explicit ordinal mapping or a source-proven absence of ordinal
consumers would require separate justification. No experiment was published.

## Explicit released ordinal candidate

The installed and `assets/release` NTVDM images have equal hashes. Their
release PE export table and the preceding idle-shared candidate both contain
269 names with identical name-to-ordinal mappings. A project-owned ordinal
CSV now records that released mapping, plus the two recovered original CPU40
names at 270 and 271. The generator applies it to the original preprocessed
exports, private declarations and existing dllexport-only providers; duplicate
names/numbers are rejected. A duplicate private declaration was removed.
CMake tracks both the original template and ordinal CSV as inputs.

The rebuilt worker and dependent WOW32 link succeeded. An actual dumpbin
comparison of the candidate with the release reports all 269 previous names
and ordinals unchanged, no removed names, and exactly the two intended new
names at 270 and 271. A subsequent build reports no work in both worker and
WOW32 Ninja trees. [PE dumps and input hashes](s11-foundation-evidence/export-template-candidate/)
are retained. This establishes name/ordinal preservation for the measured
release, not equivalent alias bodies, data targets or runtime behavior. The
candidate remains unpublished and requires an automated ABI regression,
functional export calls, coherent package and required product gates.

`tests/observation/verify-ntvdm-export-ordinals.py` reads the actual PE export
address/name/ordinal tables and checks each contract entry against the image;
it also rejects zero addresses and invalid or duplicate contract names and
ordinals. The candidate passed 271/271 entries. As negative controls, the
previous release correctly failed on the two newly required names, a contract
with two swapped ordinals failed on both entries, and a duplicate ordinal was
rejected. Input hashes are in `ordinal-verifier-hashes.json`. This standalone
check does not yet belong to the product gate and does not validate executable
function signatures, target identity or runtime allocation behavior.

The original external-memory fixture now exports the selected original
`nt_mem` query and reallocate functions under names and ordinals 270/271,
resolves them using `GetProcAddress` by both name and ordinal, checks that the
addresses agree, and invokes query and grow/shrink with content preservation.
It passed together with the existing memory controls. The first link failed
because the fixture pragmas lacked the x86 C symbol aliases; that attempt was
corrected and is not counted as a pass. The fixture uses its own executable,
controlled host seams and `/force:multiple`: it does not call functions inside
the actual product NTVDM process. [Output and hashes](s11-foundation-evidence/export-template-candidate/)
are retained; actual product invocation and coherent guest regression remain.

A fresh coherent ten-image test package containing the rebuilt NTVDM and
WOW32 passed the immutable prebuilt same-worker Win16 idle witness: the
registrar's callback ran on its peer task and both tasks returned normally.
The same package passed the complete selected `Control` suite, including GUI,
RPC version rejection, real EDIT, DOS/native handoff, repeated relaunches,
frontend loss and nested I/O. The first Control invocation was incomplete
because the coordinator omitted the mandatory terminal observer; the corrected
second invocation passed. The package manifest and shared result are retained
here; Control logs remain at `build/t473-s11/export-control-results-r2`.
These do not prove the two new exports are called by the live guest or resolve
SOL/Winmine performance and the unidentified 1400. Full is not claimed.

The actual-PE ordinal verifier is now invoked as a named gate by product
verification before the selected suite. It uses the isolated runtime image and
accepts an explicit Python executable; Python is now a verification dependency.
The first run failed because command discovery returned two Python paths; the
corrected run selected one and the complete Control suite passed with all 271
ordinal checks and the same ten-image package. The first non-pass is not
counted. Gate output, timings and source hashes are retained in the candidate
evidence directory. Full and real guest usage of the two new exports remain
unproved.

The selected `i386`/CPU40 template has nine alias or DATA declarations. The
new `tests/observation/verify-ntvdm-export-owners.py` compares the real dumpbin
PE RVAs against the exact candidate link map and checks the map's function or
DATA kind. All nine passed, including `MGetVdmPointer` to the original
`ExpSim32GetVDMPointer`, `call_ica_hw_interrupt` to `ica_hw_interrupt`, and
`ExpVdmTib` to the registered `VdmTib` owner. A deliberately changed
`MGetVdmPointer` RVA was rejected. Source, PE, dump and map hashes are recorded
in `owner-check-hashes.json`. This verifies the selected linked-image mapping;
it is not a call to the exports or validation of unselected profiles.

A test-only VDD, built by the selected x86 Ninja graph, was loaded by the
existing package-local `NTVDM.REG` mechanism in a fresh isolated ten-image
stage. Without modifying or compiling guest media, the original `MEM.EXE`
started and exited zero. On its task-create callback the VDD resolved the
actual running NTVDM process's new query/reallocate exports by both name and
ordinals 270/271, checked both identities, then successfully called query,
allocate, reallocate and free. [Trace, observer, package manifest and input
hashes](s11-foundation-evidence/export-template-candidate/live-worker/) are
retained. This is one live worker normal-path witness, not an allocation
failure, concurrent-guest, all-provider or complete product regression.
Only the isolated stage's overlay was changed; it was removed with the stage.

The live worker VDD additionally requests a size exceeding the observed largest
free block plus the current allocation. The allocation failure left the output
sentinel, free-byte count and largest-free-block value unchanged; the same
original allocation then successfully grew and was freed. The same VDD run
against the previous package correctly produced `FAIL resolve/query/allocate`
and the verification script returned nonzero; its known PE lacks both new
names. Original MEM exited zero in both runs. The [positive and negative
receipts](s11-foundation-evidence/export-template-candidate/live-worker-failure/)
include package manifests and inputs. This is one real-worker exhaustion
failure, not injected OS commit failure or concurrent allocation coverage.
The final fixture also handles the unexpected-success branch by freeing the
returned allocation rather than a potentially retired old address. Its
rebuilt positive worker run again passed both markers; `final-*` files bind
the final source and binary. The earlier negative used the prior fixture
binary and remains a separate retained observation.

A fresh ten-image candidate rebuilt after separating DIV-469/470/471 was
staged and reran the real-worker VDD/MEM export witness. Normal query,
allocation, reallocation and free plus the bounded exhaustion/rollback check
passed; the unchanged guest MEM exited zero. [Manifest and receipts](s11-foundation-evidence/export-template-candidate/boundary-only-live/)
identify this boundary-only package. It has not passed a full product gate and
is not published.

The boundary-only ten-image package subsequently passed the complete Control
suite, including the PE ordinal gate, RPC, GUI, version-negative, terminal,
relaunch, frontend-loss and nested I/O checks. [Timings, runtime manifest and
ordinal receipt](s11-foundation-evidence/export-template-candidate/boundary-control/)
are retained; detailed logs remain in `build/t473-s11/boundary-control-results`.
This run excludes the held CPU changes and is not the product Full suite.

After separating the held CPU internal changes, the boundary-only candidate
also passed Console-17 and Window-17 command matrices. The [package manifest,
summary and per-receipt hashes](s11-foundation-evidence/export-template-candidate/boundary-product/)
bind this result to that candidate. This supplements the Control suite and
live VDD memory-export witness, but it is not the Full product gate and does
not establish all WOW or x87 behavior.

The refreshed ten-input candidate passed the real-worker VDD/MEM witness again: named/ordinal exports, query, allocation, reallocation, release, and preservation after a deliberately exhausted allocation; unchanged MEM completed. [Receipt and stage manifest](s11-foundation-evidence/export-template-candidate/current-ten-live/) identify this run. This is one actual worker/VDD path, not all export consumers, fault cases or Full acceptance.

The later candidate containing ADAPTER-WOW-106/107 and the Alt+Enter changes
passed the same VDD/MEM normal and exhausted-reallocation witness using a
fresh isolated package identity. [Receipts and actual staged manifest](s11-foundation-evidence/export-template-candidate/current-record-live/)
bind this run; the unchanged guest MEM and observer exited zero. The fresh
identity reseals the stage, so its manifest must not be substituted for the
original input package's hash. This does not broaden the witness to all
consumers or failure modes.

The current working-tree supplemental `i386/ntvdm.def` was rechecked against
its registered `opennt-src-2` source: both hash to
`98C00C3C4737108674061779A6C1E06F113C597D76536A49E0723AA617D58A7A`.
The sole substantive change relative to the committed local file is removal
of `Sim32FlushVDMPointer`; the generated project extension still includes it,
and the released ordinal ledger retains ordinal 44. The latest coherent
candidate's Control export gate reported 271 named exports matching 271
ordinal contract entries. The historical line-ending diff is not a new
mirror change, and this evidence does not prove every export's behavior.

The newer candidate reran the real-worker VDD/MEM normal and exhausted
reallocation witness with all ten input hashes checked, and its `ntvdm.exe`
passed the 271-entry export ordinal check. See
[retirement-error-integrated](s11-foundation-evidence/retirement-error-integrated/review.md).
