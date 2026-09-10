# T95 S6 r28/r29 CLI namespace-root closure evidence

## r28 retained rejection

`artifacts/build/bochs-2.6-native-startup-transaction-msvc-r28-cli-current-dir`
is immutable compiler evidence, not a retry candidate.  The pre-existing full
`bx_ntvdm_host_namespace.c` failed before linking at its NT directory-enumeration
types (`FILE_ID_BOTH_DIR_INFORMATION` and its information-class declaration).
`54:04` does not enumerate directories, so importing that unverified graph to
answer a root query would be boundary expansion.

## r29 accepted narrow closure

The replacement is `bx_ntvdm_namespace_root_set_v1`: it captures no data of
its own, opens only a root already admitted by the immutable snapshot, retains
opaque handles, and exposes only `available_mask`.  It has no enumeration,
path traversal, search, host path or guest ABI.  r30 adds explicit
process-session release registration for those opaque handles.

Fresh root
`artifacts/build/bochs-2.6-native-startup-transaction-msvc-r29-cli-root-set`
linked the 34-object closure. Its executable SHA-256 is
`C6CE4112794A6EABA493A77711DDFABFD4B987FC200671E60D4ACA5DAFA2B69A`.
The immutable observation
`artifacts/analysis/t95-s6-r29-cli-root-set-022-20260811-001` retains the
unchanged source-built guest inputs and reaches CMD `54:04` without an
unmatched-UD record. The next observed BOP is `54:0c`,
`SVC_GETCONFIGSYS`, at `8DC8:0732`.

The r29 observer invokes the engine directly with no include/exclude values,
which represents the defined default policy but does **not** prove the public
CLI child handoff. A later CLI-owned observation must prove include/exclude
precedence. r29 also does not prove a DOS boot: `54:0c` remains listener-only
and the watchdog outcome is retained as downstream evidence.

The subsequent r30 rebuilt the same closure after adding its explicit
process-session release registration; its executable SHA-256 is
`1F1F0F4C42346AA29B093B177074E9DE29EA0055C95B45F1BBC2452C9C7ECC68` and its
immutable observation
`artifacts/analysis/t95-s6-r30-cli-root-release-023-20260811-001` again
consumes `54:04` and reaches `54:0c` with no broad interceptor.
