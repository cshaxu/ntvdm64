# S14 migration readiness and counts

Initial P60 baseline `85700b2a5`; P61/P62/P63/P65/P68/P70/P72/P73 updates below supersede its
classification, implementation and package statements. The [machine-readable partition](s14-migration-readiness.json)
assigns all 392 currently open stable IDs to the existing ten chains.

## Exact current population

| Chain | Symbols | Headers | Include roots | Image imports | Total |
| --- | ---: | ---: | ---: | ---: | ---: |
| 1. guest-access-leases | 0 | 0 | 0 | 0 | 0 |
| 2. wow-machine-context | 1 | 0 | 0 | 0 | 1 |
| 3. user-projection-generations | 10 | 3 | 0 | 0 | 13 |
| 4. console-mechanics | 50 | 7 | 0 | 0 | 57 |
| 5. command-capture | 2 | 3 | 0 | 0 | 5 |
| 6. launch-csr-completion | 19 | 4 | 0 | 0 | 23 |
| 7. thread-lifecycle | 85 | 7 | 0 | 0 | 92 |
| 8. redirector-printer-devices | 6 | 2 | 0 | 0 | 8 |
| 9. native-user | 137 | 25 | 0 | 0 | 162 |
| 10. original-helpers-build-abi | 13 | 6 | 11 | 1 | 31 |
| Total | 323 | 57 | 11 | 1 | 392 |

The historical 79-provider body-read census remains evidence; P68 removes one
provider from the open implementation set. Recounting the remaining chain
provider-file union gives 74 distinct paths (the historical 79 is not current). Header responsibility classification is 59/59; three generated contracts
still await implementation of their owner/ABI disposition. All 11 remaining search roots have current-use evidence. VCDEX imports
have C implementation evidence; that is not final binary-boundary disposition.

Whole migration chains ready for implementation: **0/10** under the exact
destination/removal/mirror-cost/validation criterion. Whole chains still needing
design: **10/10**. This does not discard previously delivered subchains. It makes
the missing candidate-level decisions explicit rather than treating source reading
or broad B/C/F classification as a completed migration design.

The historical P65/P68 ledger snapshot was 157 eliminated, 411 open, 43 supporting, six
eliminated-transitive and one excluded diagnostic, total 618. P65 eliminates
C0456 with +1 non-error mirror point/+2 lines; P68 moves the shared runtime body/header into B with zero mirror delta; current package is
`360c2b7f-3879-4f20-9a36-55e8395fd196`. Direct-ID closure is not whole-chain
or indirect callback/lifecycle closure.

## What these counts do not hide

The four-root inventory contains 1494 tracked files; it is not a claim that
1494 complete bodies have been reviewed. P54 separately reconciles 190 C caller
files. The direct-provider set is 79 files, not the whole inventory.

The open direct IDs do not include every indirect implementation dependency. In
particular, previously eliminated includes do not excuse the B guest-access/lease
callbacks to C or outer lifecycle hooks. Those are attached to the same chains
using P55/P59 and the callback-provider review, not counted as new approved
exceptions. A provider file can participate in several chains; file totals use
the distinct union, while each open ID has exactly one primary accounting owner.

## Concrete next decisions

The JSON carries the exact next design gate for each chain. Current execution prioritizes the largest complete dependency batches. The
following chain1 candidate description is historical and superseded by P62/P65. For chain 1, produce the actual C coordinator/B-resource
candidate and measure mirror cost before production edits. Keep the existing
SAS provider; do not replace it with raw host memory copying.

The current acquire sequence in `src/compat/guest_memory_lease.c` selects an
inactive slot, allocates its bounce buffer, calls read (including WRITE-only
leases), and only then publishes epoch/address/access/active and output pointers.
A refactor that marks the slot active before the C access changes that order.
The proposed reserve operation must therefore preserve the unpublished state
until the C coordinator finishes hydration; read failure clears the slot and
publishes no outputs. This is preservation of existing sequencing, not a claim
that arbitrary nested acquisition is already safe.

The source route in `ccpusas4.c` performs range preflight followed by original
`c_sas_loads` / `c_sas_stores`. SAS includes video/device access and BIOS
virtualisation paths. Consequently, the design must not assume that SAS is a
pure memory copy or use that assumption to justify earlier lease publication.
No new reentry fault is claimed from source inspection alone. The candidate must
compare the old and new publication/failure traces and exercise the real C path.

## Verification contract

Check exact open-ID set equality, uniqueness, per-kind totals, input hashes,
provider source hashes and evidence paths. Documentation governance and link
checks accompany delivery. These checks prove ledger consistency only. They do
not replace candidate compilation, product tests or final dependency elimination.

## P61 header classification update

All59 headers now have responsibility classification. The three generated
headers belong to C private machine ABI; their original-carrier proposals and
whole-file differences are in [the disposition](s14-generated-header-disposition.json).
Migration of those three still requires layout/selection verification, final
mirror-cost measurement and applicable source-policy approval. This supersedes
the56/59 classification count above, not the0/10 whole-chain readiness count.

## P62 coordinator delivery update

The [approved coordinator delivery](s14-p62-lease-owner.json) implements the
C coordinator/B resource split directly in the main worktree. B no longer
stores or invokes CCPU read/write callbacks. C preserves hydration before
publication, consume-on-failed-commit and captured-owner chunk copying. F
keeps admission/lifetime; its SoftPC unit owns the two session access wrappers.
The former candidate-creation gate and B callback description above are historical.

Non-error mirror change is13/78 to13/370 (zero additional points,292 additional
lines), approved by the owner. Cached full builds, coherent dual publication
and final-code targeted tests passed. Package8ec5f8b9-ea6f-43b8-9810-73d7592e7950
replaces P26. Existing DIR date-rendering diagnostics reproduce on P26 too;
they are recorded separately rather than counted as date-rendering acceptance.

The four chain-1 WOW consumer IDs remain open; this internal coordinator
relocation does not alone resolve their implementation ownership. The416-ID
partition and zero fully closed chains are unchanged. Continue with those
consumers and the coupled WOW machine-context chain; do not recreate B callbacks.

P63 removes C0075/C0328/C0329 by moving abort cleanup into the existing F
CallBack16 wrapper. [Delivery evidence](s14-p63-callback-owner.json) records
7/26 ->3/9 non-error mirror differences and final package/targeted tests.
Chain1 retains C0456; this does not close the whole chain.

## P65 direct completion delivery

[P65 evidence](s14-p65-wow-flush-owner.json) removes the F empty SIM32
completion and its link input. C owns the selected CCPU direct-view macro;
original paired SIM32 scratch-video writes remain unchanged. Cached full
builds, dual11 publication, argument/frame/abort, real CCPU/page-domain and
immutable Win16 dialog checks pass. Chain1 now has zero open direct IDs;
chain2 machine/context ownership remains the next implementation work.

P70: registry implementation/header remove 13 direct IDs, with zero mirror
delta. Final build, dual11 publication and targeted tests pass; enclosing P70
commit is the delivery receipt. The registry initialization race found by testing is repaired using
the existing commit mutex. See [evidence](s14-registry-binding-owner.json).

## P72 original query restoration

[Verified query restoration](s14-native-query-owner.json) eliminates C0530, C0531
and C0541 without a replacement B provider, callback or mirror edit. Original C
queries bind directly to modern USER. Remaining priority-batch roots:111. The
current ledger is173 eliminated/395 open; full-chain ownership remains incomplete.

## P73 native dialog and failure ownership

[Delivered implementation evidence](s14-native-dialog-owner.json) moves actual
installed USER export resolution, modal query/cursor and terminal diagnostic
bodies into B. C0375/C0376/C0377 are eliminated; total176 eliminated/392 open.
Ten bodies retain identity after one private trace-helper rename. No mirror
points or lines added. The existing original descriptor and HWND conversion
remain at their caller, pending the separate C coordination migration.
The failure provider is reached by109 of111 priority roots; moving it is a
shared prerequisite, not elimination of those111 roots. Cached full build,
dual11 publication and targeted lifecycle/real Win16 dialog tests pass.
