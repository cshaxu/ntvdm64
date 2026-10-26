# M0 T470 S4 — ToolHelp/debugger reconciliation

## Scope reconciliation

| Admitted row | Evidence | Final disposition |
| --- | --- | --- |
| Original ToolHelp query exports | S1 owner matrix; S2 independent NE probe | Current immutable ToolHelp/KERNEL pair rejects valid Task/Module queries. Guest rebuild/change and a host substitute are prohibited; retained explicit original compatibility limitation. |
| WOW `ClassFirst`/`ClassNext` | Original makefile, thunk and table | Deliberately `UNIMPLEMENTEDAPI` in original WOW. No matching correction exists; retained original limitation. |
| Non-WOW debugger event transport | Rebuilt cached native tests | `S43_DEBUGGER_EVENT_TRANSPORT_OK` and `S43_DEBUGGER_MODULE_EVENT_TRANSPORT_OK` pass. |
| WOW debugger producer/remote callback/task retirement | S3 original graph audit | Original `wow16`/`wkman.c`/`dbg.c` owner chain retained. Debugger remote TD is not repurposed as management control. |
| Historical real-worker debugger observer | Reconstructed legacy invocation | It fails before worker launch because its old flat-root `DEBUG_PROCESS` contract no longer matches the deployed layout; recorded non-pass, not accepted WOW behavior. |

## Delivery review

T470 changes only evidence and two independently authored test files. The
complete diff from the T469 baseline contains no `src/`, `assets/release/`,
guest-media, installed-image or package file. Therefore a new binary build,
release publication or deployment would be misleading; the current coherent
package remains T467 S3's hash-matched `build/t467-s3/package-r2` delivery.

`git diff --check` and documentation governance pass. The cached x86
`debugger-event-tests` target was rebuilt through the repository's MSVC
initialized serial runner with recorded exit zero. The S2 negative witness
also passes its explicit contract. No result above represents successful
ToolHelp queries or a complete modern WOW debugger session.

This candidate has no admissible project-owned implementation left: every
remaining non-pass requires a guest image change or a second provider/control
plane. Those are not deferred implementation debt. The next candidate is the
whole-provider acceptance package, which must consume these explicit non-passes
rather than relabel them as success.
