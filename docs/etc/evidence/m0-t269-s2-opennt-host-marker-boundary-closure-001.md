# M0 T269 S2 — OpenNT-host marker and private-boundary closure

## Question

Do the four non-exact OpenNT-host mirror bodies expose every retained-source
modification and every independent cropped product block at the source line
and in the component register, while keeping the private overlay private?

## Inputs and procedure

- Used the S1 selected OpenNT revision and per-file diff records.
- Re-read the changed hunks in `nt_error.c`, `vrinit.c`, `vrnmpipe.c` and
  `vrputil.c`; counted added lines against each retained current body.
- Added only two explanatory `DIVERGENCE:` crop boundaries: `HOST-DIV-027` at
  the retained `host_direct_access_error` block and `HOST-DIV-028` at the
  retained synchronous named-pipe block. Expanded `HOST-DIV-022` to name all
  omitted product-shell groups.
- Reconciled the added identifiers with `src/opennt-host/README.md`, then
  scanned private-overlay includes and direct VDM Redir consumers.

## Observations

| Mirror body | Added / retained lines | Source-shape result | Register result |
| --- | --- | --- | --- |
| `nt_error.c` | 18 / 81 (22.2%) | The retained `host_direct_access_error` control flow is original; product bindings and x64-safe transports are locally marked. | `HOST-DIV-012/013/014/026/027` are locally marked and registered. |
| `vrinit.c` | 4 / 77 (5.2%) | Only the original `VrInitialized` query remains; its static binding and all cropped product-shell groups are locally marked. | `HOST-DIV-017/022` are locally marked and registered. |
| `vrnmpipe.c` | 97 / 1,012 (9.6%) | Retained synchronous helper body is original-shaped; all source edits and the omitted async/service block are locally marked. | `HOST-DIV-015/016/020/023/024/025/028` are locally marked and registered. |
| `vrputil.c` | 12 / 190 (6.3%) | The original utility algorithm remains in order; only unavailable includes/diagnostics and defined x64 expressions differ. | `HOST-DIV-018/019` are locally marked and registered. |

The only current private-overlay implementation consumers remain the matching
`opennt-host` mirror bodies. The production-tree gate's root inspection is a
governance consumer, not a link/include consumer.

## Interpretation and follow-up

All modified retained bodies are below the 50-percent modified-retained-body
threshold. The two substantial deletions now name their complete unadmitted
logical blocks at the crop point; no function body or route behavior changed.
The remaining T269 issue is S3's `nt_pif.c` textual inclusion from
`opennt-bop`, which is an owner-edge repair and deliberately outside this S2
boundary.

## Verification

- The divergence/register scan found every `HOST-DIV-012` through
  `HOST-DIV-028` in both source and `opennt-host/README.md`; the private
  overlay had no implementation consumer outside its matching host mirror.
- A first concurrent-build attempt left a corrupt disposable `.ninja_log`; two
  exact task-local Ninja cache files were removed only after their two known
  build processes were stopped. A single fresh rebuild then completed all
  486/486 graph edges under `build/M0-T269-S2/r001`.
- `t263-s8-opennt-host-mirror-fixture.exe` exited zero and reported that its
  utility and synchronous-pipe paths pass. A following `ninja -n` reported
  `ninja: no work to do.` The final PowerShell wrapper returned nonzero only
  because its native-output capture variable was empty despite that explicit
  Ninja output; it is not a build or fixture failure.
