# Td — Documentation repository declutter

## Status

Active: Td S1 P1. This standalone governance delivery does not allocate a
numeric T and does not admit product implementation work.

## Owner requests

- "现在doc怎么还有这么多文件？"
- "你觉得哪些可以清理出去？这docs不应该这么庞杂"
- "没错，批准。请你继续Td治理提交推送。"

## Baseline

At admission, `docs` contains 4,103 files. `docs/etc/operations` contains
315 TSV ledgers totaling about 299.6 MiB; `etc/evidence`, `etc/research`, and
fragmented task records contain historical supporting material. Current
authorities and state controls are small by comparison.

## Scope

1. Move generated ledgers and raw historical material to a manifest-backed
   archive outside `docs`.
2. Retain concise task conclusions and current authorities in their governed
   locations.
3. Repair active tool and document references, verify the result, and deliver
   only Td-owned changes in a commit and push.

## Non-goals

No product behavior, source selection, build configuration, ABI, runtime, or
guest-media change is authorized. Existing unrelated dirty-worktree changes
are not part of this delivery.

## Risks and stop conditions

Stop if an item is a live build/runtime input, if a preservation manifest
cannot be produced, or if staging would include unrelated user changes.

## Verification plan

Use file/hash manifests, direct-reference scans, the documentation-link and
governance checks, focused exporter regression, `git diff --check`, scoped
diff review, commit, and push.
