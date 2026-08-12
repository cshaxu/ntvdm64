# Lane P Bounded Trace Fixture Plan

Status: M47 fixture plan, 2026-08-08. This plan verifies M41-M46 contracts
without converting source scaffolding, owned-monitor experiments, or frozen
historical probes into a DOS runtime.

## Fixture Classes

| Class | Meaning | Runtime authority |
| --- | --- | --- |
| `P` | pure/profile fixture, no guest execution | adapter data validation only |
| `T` | transaction fixture against a future backend test double or real core | proves all-or-none mutation, never DOS success |
| `O` | historical-oracle observation | original source/object trace; may stop before unsupported dependency |
| `R` | integrated runtime fixture | only after a backend reaches the original chain and required contracts |

No class may substitute a CCPU lifecycle adapter, self-authored BIOS BOP/DEM
handler, scaffold IVT/BDA initializer, generic instruction decoder, or local
memory executor. Existing instrumentation remains default-disabled and may
record/assert/snapshot only; it may not change service outcomes, device
responses, BOP/SVC dispatch, or startup success conditions.

## Common Trace Record

Every fixture emits an ordered, bounded record with:

```text
fixture id, class, lane, source/contract revision,
profile identity prefix, historical owner or "none",
operation category, redacted role identity, byte-count bucket,
logical-address category, transaction/result category, terminal stop category
```

It never emits guest image bytes, guest path/command text, full hashes, host
paths, handles/tokens, environment strings, raw memory pointers, register
dumps beyond a declared finite result field, or copied protected binary data.
Logical addresses are reported only as named receiver categories (`cmdinfo`,
`ucombuf`, `execpath`, `config_receiver`, `dem_read_span`) unless a source
ledger has separately admitted a numeric address. A trace is evidence of its
listed claim only; it never claims historical-host restoration merely because
it bears an original function name.

## Fixture Matrix

| ID | Class | Subject | Required proof | Stop boundary |
| --- | --- | --- | --- | --- |
| P-01 | P | M41 set-info | complete SCS bindings are retained as copied addresses; a bad final range retains previous binding | before guest mutation |
| P-02 | P | M41 COMSPEC | bounded matching profile path yields only declared metadata/AL result | before external process or environment access |
| T-03 | T | M42 environment | capacity retry mutates BX only; success commits one double-NUL image atomically | immediately after transaction disposition |
| T-04 | T | M43 config/auto | exact 64-byte-bounded path result and no partial receiver write | before DOS `OPEN` unless an R fixture owns it |
| T-05 | T | M44 first command | one 36-byte CMDINFO, `EXECPATH`, `UCOMBUF`, and M41 bindings commit together | immediately after carry-clear disposition |
| T-06 | T | M45 directory | valid contained directory or invalid-drive result, never host environment mutation | immediately after result |
| T-07 | T | M45 completion | one terminal event captures zero-extended DX; reentry and raw redirection pointer are rejected | runner terminal callback |
| T-08 | T | M46 DEM read | open/read/seek/close token sequence, staging read and atomic guest commit | original DEM operation return |
| O-09 | O | historical service ownership | original dispatcher/function and consumer ordering only | first missing provider or operation boundary |
| R-10 | R | bootstrap | NTIO entry to original `SVC_DEMLOADDOS` request | first unprovided original dependency |
| R-11 | R | command init | original COMMAND reaches M44 once with valid record | after first-command transaction, before target execution |
| R-12 | R | smoke terminal | selected `.COM`/`.EXE` yields LP-07 or a separately observed terminal path | terminal state only |

`R-10` through `R-12` are not currently runnable. They require the M36/M37
backend adoption gate, M29 guest-write transaction, and a real original-chain
entry. Their test names and link targets must remain absent until those
preconditions are satisfied.

## Mutation Oracles

`T` fixtures use a caller-owned sentinel snapshot and compare only declared
metadata plus permitted byte ranges. They prove these rules:

1. Validation failure changes no guest range, register/flag result, session
   binding, terminal state, or namespace token state.
2. A successful multi-range operation publishes every declared write exactly
   once; no observer can see an intermediate record/buffer combination.
3. Resume, reset, fault, cancellation, duplicate commit, and stale epoch
   invalidate pending work and preserve the pre-call snapshot.
4. A token or resource may not survive close/reset/stop.
5. A completion trace is successful only if it names the declared terminal
   source (`RETURNEXITCODE` versus unqualified stop), never merely because the
   execution loop stopped.

The oracle records byte counts and equality outcomes, not the protected bytes
being compared. Fixtures that need synthetic bytes use owned non-Microsoft
sentinels whose contents carry no claim about guest artifacts.

## Containment Negatives

Each fixture family includes probes that fail closed when an implementation
attempts any of: host-root traversal, working-directory fallback, system
directory lookup, registry/environment lookup, temporary-file creation,
direct-device access, named pipe, process creation, thread creation for the
historical console loop, PIF processing, raw guest pointer export, raw native
handle import, or a second command/reentry request.

An implementation may report a typed `unsupported` or fault/stop result. It
must not record a passing operation merely because it avoided an immediate
crash.

## Build And Isolation Rules

Pure fixtures may link only owned profile/transaction test support. A future
`T` fixture links an explicitly selected backend test API, never the default
runner. An `O` fixture links only its explicitly named historical island and
remains default-disabled. An `R` fixture requires a separately approved
research target; it cannot be pulled into `ntdos64-run.exe` by a default CMake
dependency.

Every new target must demonstrate that it excludes frozen self-authored
startup/BOP/DEM substitutions and does not link `ntvdm64` sources directly.
The external backend boundary is versioned C ABI only.

## Admission Sequence

1. Implement and pass P-01/P-02 using only copied profile data.
2. After backend capability adoption, add T-03 through T-08 in dependency
   order: transaction, configuration/environment, command record, namespace,
   lifecycle.
3. Use O-09 to compare source-owned call ordering where the historical island
   is available; record a boundary rather than inventing a missing provider.
4. Admit R-10, then R-11, then R-12 one at a time. Any newly reached original
   dependency creates a new dossier before the next integrated run.

This order makes a green fixture meaningful: it identifies the exact contract
that became true, while refusing to use a local substitute to cross an
unproven historical boundary.
