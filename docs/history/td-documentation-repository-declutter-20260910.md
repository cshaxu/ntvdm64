# Td — Documentation repository declutter

## Status

Closed: Td S1 P3. This standalone governance delivery does not allocate or
close a numeric T and does not admit product implementation work.

## Owner requests

- "现在doc怎么还有这么多文件？"
- "你觉得哪些可以清理出去？这docs不应该这么庞杂"
- "没错，批准。请你继续Td治理提交推送。"
- "接下来所有exe构建记录、中间体和结果，都应当放在 build目录下，但是按需要复制exe到o:\\ntvdm64去实地运行；所有观察日志放在o:\\ntvdm64\\logs里面。把这个写入治理规范。"
- "好的，请继续按照你的建议实施清理。"
- "目标是最终工作区干净，并提交推送当前S任务收口。T任务保持打开。"

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
3. Repair active tool and document references, enforce the product-run log
   root, preserve unrelated implementation work as local WIP, remove disposable
   workspace output, verify the result, and deliver only Td-owned changes in a
   commit and push.

## Non-goals

No product behavior, source selection, build configuration, ABI, runtime, or
guest-media change is authorized. Existing unrelated dirty-worktree changes
are preserved as WIP, but are not part of this delivery.

## Risks and stop conditions

Stop if an item is a live build/runtime input, if a preservation manifest
cannot be produced, or if staging would include unrelated user changes.

## Verification plan

Use file/hash manifests, direct-reference scans, the documentation-link and
governance checks, focused exporter regression, `git diff --check`, scoped
diff review, commit, and push.

## P2/P3 result

- Current governance now requires all locally built executables, intermediates,
  results, fixtures, and compiler/linker output under `build/`; only the formal
  `ntvdm32.exe` may be copied to `O:\ntvdm64\ntvdm32.exe` for a real-package
  run.
- Runtime observations, diagnostics, traces, reports, and stdout/stderr captures
  now belong below `O:\ntvdm64\logs\`. The startup observer rejects both its
  primary report and child diagnostic report paths when they are outside that
  root.
- 236 untracked observation outputs were moved to
  `O:\ntvdm64\logs\m0-t397-s4-archive-20260910`. 23 disposable root-level
  compiler/capture outputs were removed. The 108 tracked `artifacts/research`
  records accidentally included in the first broad move were restored exactly;
  they are not treated as disposable runtime logs.
- Existing product source, tests, and tools are preserved without push in local
  recovery branch `codex/product-wip-preserved-20260910`, commit `ef1c3cb28`.
  This leaves that product work open; it is excluded from Td.
- Verification: documentation governance and relative-link checks pass; the
  observer's path rejection/acceptance checks and scoped diff review pass. The
  Td delivery commit and push are recorded by the revision containing this
  closure.
