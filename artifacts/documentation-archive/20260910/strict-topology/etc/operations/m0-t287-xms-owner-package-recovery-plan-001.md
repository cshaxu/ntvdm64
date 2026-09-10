# M0 T287 — original XMS owner-package recovery

## Objective

Recover the complete original `mvdm-host/xms.486` package through the smallest
same-shaped machine, session and host-interface bindings.  The package is
handled as one owner unit: its dispatcher, A20, block, UMB, IVT and allocator
paths are planned together.  BOP trace order cannot select individual
services or create a leaf workaround.

## Fixed boundaries

- Original XMS source remains under `mvdm-host/xms.486`; no `src.old` input is
  allowed.
- `adapter-mvdm-host-out/softpc` preserves the XMS-visible historical
  SoftPC/CCPU/SAS interface shapes and binds only typed mechanical operations.
- `adapter-bochs` supplies selector-blind CPU/RAM/A20/IVT/physical-span
  mechanics; it neither knows XMS nor implements UMB policy.
- `session` owns the existing guest-memory mapping-manager instance and its
  epoch/lease checks; XMS may not create a private guest-pointer mapper.
- Any direct guest-pointer source form must be retained only through a bounded
  same-shaped facade.  `xmsmem86.c` cannot cast a 32-bit guest address to a
  host pointer on x64.

## Sequential subtasks

| S | Scope | Exit criterion |
| --- | --- | --- |
| S1 | Rebaseline every current XMS source/file, BOP and dependency row against the merged OpenNT source identity and current component roots. Record each required external symbol, owner, interface shape and current enablement; distinguish old evidence from live product input. | One source/ABI/failure ledger covers all `xms.486` units and `52:00..0B`; no ownerless or `src.old` dependency remains. |
| S2 | Compose the complete original common XMS source set as an x86/x64 static package through declaration-only/same-shaped existing facades. | All directly composable translation units build with source identity evidence; unavailable historical machine paths remain named but not silently replaced. |
| S3 | Close the common A20 and bounded guest-memory lease binding, including original ordering and negative span/epoch tests. | `xmsa20.c` and reached dispatcher calls retain source order and use only the session manager plus typed machine A20 operations. |
| S4 | Close block allocation, capacity, move, free and realloc through one source-shaped extended-memory reservation/copy seam. | `xmsblock.c` and suballocator path have one consistent capacity/identity/failure contract, with no host-pointer conversion. |
| S5 | Close UMB and IVT/INT15 paths as complete source families: true unavailable failures remain original; any successful physical span must be supplied only by selector-blind mechanics. | `xmsumb.c` and `xmsmisc.c` have explicit success/failure dispositions, bounded IVT proof and no DOS/UMB semantics below their owner. |
| S6 | Compose the original dispatcher/ingress with the complete family matrix and run local x86/x64 regressions. | Every `52:00..0B` has one original source path or explicitly source-proven unavailable disposition; no per-trace service provider exists. |
| S7 | Perform source identity, interface, mapping-manager, exception-register and full-family closure audit; write T287 closure. | The complete XMS owner package is code-complete/local-regression closed, or every remaining external prerequisite is transferred by named later owner package. |

## Non-goals

No guest trace-driven repair, no private host-pointer XMS back end, no second
SoftPC executor, no UMB/DOS policy in a Bochs component, no new BOP family,
and no claim of full guest-continuity completion.
