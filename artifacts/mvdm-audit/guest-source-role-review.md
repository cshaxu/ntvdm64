# Historical guest source-role disposition

This S10 checkpoint resolves 187 previously pending file roles, using the
[per-path disposition table](guest-source-role-dispositions.csv), without
claiming 187 newly restored runtime capabilities. The files are original
16-bit guest build inputs; current policy retains immutable guests and does
not rebuild them. They are excluded only from the modern host compilation
and implementation-input obligation. The corresponding guest functionality,
guest/host ABI and behavior remain in the T acceptance scope.

## Evidence and decision

| Review | Accepted file roles | Specific original evidence |
| --- | --- | --- |
| [Shard 0](shard-0/guest-source-active-edges.md) | 67 | COMMAND and WRITE explicit object dependencies, actual link responses and include chains |
| [Shard 1 DOS](shard-1/guest-source-owner-review.md) | 13 | Twelve COMMAND source/message dependencies and APPEND's message skeleton through the original object/link target |
| [Shard 1 WRITE](shard-1/guest-write-owner-review.md) | 35 | Linked source objects and selected-source header includes; conditional alternatives retained explicitly |
| [Shard 2 WRITE](shard-2/write-guest-owner-review.md) | 32 | Actual object/link dependencies, source includes and RC resource inputs |
| [Shard 3 DOS](shard-3/guest-source-active-edges.md) | 14 | APPEND, DEBUG, EDLIN, EXE2BIN, FASTOPEN, MEM and SETVER source/include-to-object-to-guest-target chains |
| [Shard 3 WRITE](shard-3/write-guest-owner-review.md) | 26 | Original linked objects, included headers and RC resource inputs |

Every accepted row was independently checked against the prior frozen hash
and a byte-equal original source edition. No mirror-different file is
excluded by this batch. The [observation receipt](guest-source-role-observations.json)
records candidate tables and actual Ninja input/dependency snapshots. The
current worker graph (including VDMREDIR) and WOW32 graph have 632 and 229
VALID include-dependency blocks, respectively, with zero STALE blocks.
None of these 187 paths occurs in those exact observed host inputs or
dependencies. Historical recipes, true guest input relationships and current
profile observations jointly justify the role decision; absence from a
cached graph alone would not do so.

The original recipes can contain LANG/DBCS/OS2/debug alternatives. Proving a
file's guest build-input role does not require claiming every alternative is
the active shipped variant. Conversely, a recipe/source relationship does
not establish the derivation of a particular prebuilt guest image. That
question remains distinct. Local resource snapshots and checked-in link
responses are identified as observed inputs rather than universally generated
outputs.

Only the table's 187 exact paths change from pending to excluded. Other paths
in the [shard 1](shard-1/guest-source-owner-review.csv),
[shard 2](shard-2/guest-source-owner-review.csv) and
[shard 3](shard-3/guest-source-owner-review.csv) censuses remain unchanged.
Literal filename references, nearby targets and source directory membership
do not themselves justify exclusions. Actual executable/resource/library
bytes and unresolved source/include chains remain pending.

## Retained capability owners

- S12 retains DOS/COMMAND command execution, environment, search, native
  execution and exit semantics, plus each retained DOS utility's supported
  observable behavior. Original no-operation utilities are not reclassified
  as fully implemented services merely because their source role is known.
- S12/S13 retain the shared DOS ABI, memory, interrupt and device boundaries
  referenced by the included DOS declarations.
- S15/S16 retain WRITE's control, message, dialog, text, graphics, resource,
  file and printing interactions with the selected original host providers;
  the immutable guest image and its actual tests remain separate evidence.
- S14/S15 retain the guest/host thunk and declaration boundary. For example,
  a shared WOW16 include's historical role is not proof of the corresponding
  modern host conversion or UI behavior.

No source, resource or feature is removed from the repository or tests. Old
T225 guest-build scripts discovered during the current-generator search were
not run; they are not authority to rebuild a guest under current policy.
Any cleanup of displaced historical tooling remains in S18.

## Checkpoint result and limits

At checkpoint `3fd833983`, the merged 4,684-path ledger contained 52 full,
226 partial, 1,653 excluded file roles and 2,753 pending. The original frozen inventory and the full
source/declaration evidence are unchanged. Retained source estimates for the
partial rows are unchanged. This checkpoint changes no product or guest
bytes and runs no guest build or runtime acceptance. S10 remains open for
all unresolved applicable source roles and capability owner contracts.

The [independent review](guest-source-role-independent-review.md) caught and
resolved a missing condition on one include edge. The retained
[checkpoint result](guest-source-role-checkpoint-result.json) verifies the
bounded change and unchanged unrelated rows; documentation governance and
`git diff --check` also passed. The [background run record](guest-source-role-runs.json)
retains process IDs and exit results for this source-only delivery.

The [Kernel 3.1 successor](kernel31-source-role-review.md) records subsequent
classifications separately; the counts and results above describe this
checkpoint rather than overriding the current merged index.
