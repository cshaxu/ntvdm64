# M0 T394 S3 — COMMAND redirection ownership migration

## Scope and result

Moved exactly the admitted COMMAND standard-stream redirection pair from
`adapter-mvdm-host-out/softpc` to the existing `redir` family:

| Final path | Role |
| --- | --- |
| `redir/include/mvdm_command_redirection.h` | COMMAND record/standard-handle identity ABI |
| `redir/mvdm_command_redirection.c` | session identity publish, resolve and retire bridge |

`cmdredir.c`, `cmdexec.c`, and `cmdmisc.c` now include the final `redir`
header.  No API signature, body, identity allocation rule, record lifetime,
zero/no-redirection handling, invalid-identity failure direction, pipe-worker
behavior, guest field, BOP, child-launch rule, or runtime behavior changed.
No forwarding file was needed.

## Consumer and product-graph audit

The complete direct consumer set is the three original COMMAND translation
units above.  `New-T290S3CmdRedirNinja.ps1` now compiles the final source path.
`New-T310OriginalSoftpcNinja.ps1` removes it from `softpc-bindings.lib`, adds
it to `redirector-bindings.lib`, and makes that library an explicit input to
the formal parent process and forced-closure links.  It is already consumed by
the VDMREDIR DLL graph.

Fresh generated formal graphs under `build/M0-T394/S3/formal-{x86,x64}` prove
one `adapter-redir/mvdm_command_redirection.obj`, no old SoftPC object, and
the required `redirector-bindings.lib` parent/closure edges on each width.

## Verification

The existing `t290_s3_cmdredir_fixture` was freshly compiled, linked and run
with the migrated object under generated MSVC `/MT` x86 and x64 environments:

| Check | x86 | x64 |
| --- | ---: | ---: |
| COMMAND redirection fixture | pass / exit 0 | pass / exit 0 |
| Formal graph final-owner/link inspection | pass | pass |
| Source/build/test stale old-path sweep | zero matches | zero matches |

The fixture verifies no-redirection state, redirection record construction,
published record identity, standard-input handle identity, identity release,
record retirement before original free, and session teardown.  Therefore it
covers the relevant ABI and failure direction without claiming a product
runtime behavior change.

## Disposition

S3 is closed.  S4 remains the separately admitted BaseVDM/COMMAND
record-state and native-child migration; it is not activated by this closure.
