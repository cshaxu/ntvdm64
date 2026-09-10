# M0 T290 — Original Redirector and brokered COMMAND extensions

## Purpose

Recover the complete original OpenNT `vdmredir` owner package, including its
`57:00..31` dispatch family and only the DEM/COMMAND brokered paths that the
original package actually consumes.  This is an original-provider recovery
task, not a generic file-redirection rewrite and not a trace-selected service
campaign.

## Source and component boundary

- `src/mvdm-host/vdmredir` remains the canonical original provider mirror.
- `src/adapter-mvdm-host-out/redir` supplies only same-shaped historical
  external interfaces; it cannot implement provider policy itself.
- `session` owns native handles, async records, lifecycle records and mapping
  managers.  Guest bytes are accessed only under checked synchronous leases.
- `adapter-bochs` exposes selector-blind mechanics only.  It cannot identify
  Redirector, BOP, DOS, pipe, network or async semantics.
- `adapter-mvdm-host-in` is the later BOP ingress bridge.  It is excluded
  until the whole provider package has passed its own local matrix.

## Sequential subtasks

| S | Scope | Exit evidence |
| --- | --- | --- |
| S1 | Audit all 50 original dispatch entries, fifteen provider bodies, six declarations, four build forms, `cmdredir.c` and reached DEM pipe dependencies. | A reproducible service/source/ABI/dependency ledger gives every entry one owner, source path, mapping/identity form, current disposition and next recovery group. |
| S2 | Freeze the source-first external-interface closure: current public Win32, historical unavailable, monitor/Bochs mechanical and session broker surfaces. | Four-rung recovery ledger rejects new provider logic where the original body can be composed through a same-shaped facade. |
| S3 | Compose original lifecycle, named-pipe and local broker owner groups through the accepted facades. | Original source bodies, focused x86/x64 local regressions and original failure directions for excluded operations. |
| S4 | Compose original mailslot and asynchronous-completion owner groups. | Checked guest lease/token, completion lifecycle and negative stale/cancel/disconnect regressions; no guest callback or raw host handle. |
| S5 | Compose the original NetAPI/NetBIOS/DLC/window/mode groups where public capability can retain the original contract; retain source-proven unavailable results where a private kernel/product dependency has no legal facade. | Every remaining original provider branch has a source-first result and whole-group x86/x64 regression. |
| S6 | Reconcile all `57:00..31`, brokered DEM/COMMAND dependencies, mirror divergences and adapter ownership; run the declared whole-family local matrix and one bounded integration observation. | No ownerless branch, no duplicate COMMAND broker, no ingress-only success claim and a formal T290 closure record. |

## Non-goals

No `src.old` input, new Redirector provider policy, raw host handle or pointer
in guest memory, second COMMAND broker, private CSR/BaseSrv reproduction,
Bochs semantic intrusion, or activation of an individual `57:xx` route before
the relevant owner group is code-complete.
