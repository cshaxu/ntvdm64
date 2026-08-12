# M54 Contained Profile Fixture Implementation Audit

Status: completed boundary audit, 2026-08-08. This examines M41-M47 against
the current default-disabled local code. It does not admit a new runtime
service.

## Result

There is no next contained-profile implementation that is both meaningful and
backend-independent. Every M41-M46 operation reaches a required execution
boundary before it can produce an honest result:

| Dossier family | First indispensable boundary |
| --- | --- |
| SetInfo / COMSPEC (M41) | copied logical guest read/translation at a transition epoch |
| Initial environment (M42) | paused multi-span ordinary-RAM transaction |
| CONFIG / AUTOEXEC (M43) | `DS:DX` transaction plus the same namespace later consumed by original DOS OPEN |
| First command (M44) | copied `CMDINFO` plus one atomic record/path/tail/binding publication |
| Directory / lifecycle (M45) | guest receiver transaction or a real session terminal callback |
| Namespace / DEM operations (M46) | original DEM reachability and M29 transactional guest-read publication |

Producing an isolated result byte string, path, CMDINFO record, or custom
file-operation answer would not test the required boundary. It would instead
reintroduce the prohibited self-authored DOS/BOP/DEM service route.

## Existing Local Code

`src/bootstrap_profile.[ch]`, `src/bop_rewrite.[ch]`,
`src/transition_registry.[ch]`, and the existing rooted namespace helpers are
historical/frozen instrumentation or earlier bounded research support. In
particular, a `C4 C4 50 11` rewrite plan cannot be promoted into the normal
path: it is precisely a synthetic substitution for an original transition.
None of these sources is evidence that M41-M46 can run, and none may acquire
new service behavior under this task.

## Decision

The next execution-relevant work is external to `ntdos64` implementation:
the governed core/mantle side must first establish a genuine second consumer
and implement the neutral configured decode-point and paused atomic-RAM
contracts. This project then has a ready, source-backed fixture plan (M47)
for validating that adoption without guest artifacts.

Until then, the only local work with positive ROI is maintenance of BYOB
validation evidence and recovery of a provenance-locked historical generated
ABI input. Neither substitutes for an executable NT4 chain.
