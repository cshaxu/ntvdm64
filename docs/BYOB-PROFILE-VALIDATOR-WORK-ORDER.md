# BYOB Profile Validator Work Order

Status: implementation work order, 2026-08-07. This is the first executable
entry gate for the owned monitor route. It is read-only and has no CCPU, DOS,
guest-memory, or historical-source dependency.

## Module Split

| Module | Responsibility | Must not do |
| --- | --- | --- |
| `byob_json` | bounded UTF-8 JSON tokenization and schema extraction; object field order is irrelevant | accept duplicate keys, unbounded nesting/input, paths, or file bytes as schema values |
| `byob_profile` | fixed-profile role/feature/name validation and typed result selection | open a guest image, allocate guest memory, or infer a component from directory contents |
| `byob_identity` | rooted regular-file open, reparse rejection, handle-based size/SHA-256 comparison | use `PATH`, current directory, system directories, registry, drive scan, cache copy, or a second path-based reopen |
| `byob_profile_test` | owned temporary-directory fixtures and redacted-result assertions | use Microsoft guest media or execute an engine |

The validator public result is one of the stable result codes defined in
`NT4-ENUS-BYOB-PROFILE-CONTRACT.md`. Its result record contains only profile
name, role, result code, byte count, and SHA-256 when accepted; it never
contains root paths, file content, guest addresses, or host-discovered paths.

## Validation Precedence

1. Bound and parse manifest bytes as UTF-8 JSON.
2. Validate fixed schema/profile/architecture/locale and top-level duplicates.
3. Validate component objects, role/name uniqueness, required roles, features,
   lowercase SHA-256 spelling, uint64 size, and nonempty compatibility group.
4. Validate every component name before opening any component.
5. Open and hash every selected file beneath the supplied root, retaining only
   identities until all components have been preflighted.
6. Return acceptance only after every identity matches. Any failure leaves no
   created output, guest allocation, guest memory write, or cached file.

## Required Owned Fixtures

| Fixture | Expected result |
| --- | --- |
| valid three-role profile, shuffled object fields | accepted |
| malformed JSON, duplicate JSON key, oversized nesting/string | `profile_format_invalid` |
| wrong schema/profile/x86/locale | `profile_target_mismatch` |
| duplicate/missing role or duplicate case-folded name | `role_missing_or_duplicate` |
| undeclared optional component or feature mismatch | `feature_declaration_invalid` |
| `..`, slash, backslash, colon, wildcard, dot segment, ADS spelling | `component_name_unsafe` |
| missing, zero-length, directory, or reparse component | `component_open_rejected` |
| correct name but altered byte count/hash | `component_identity_mismatch` |
| empty or inconsistent compatibility group | `compatibility_group_mismatch` |

Each failing fixture asserts that no monitor/machine constructor was called.

## Integration Gate

Initially expose the validator only through an `EXCLUDE_FROM_ALL` test/CLI
target. It may be linked into a future owned monitor session shell only after
the fixture matrix passes under the MinGW-w64 build and the default dispatcher
remains free of historical and frozen-instrumentation sources.

## Implementation Evidence

The first admission implementation is `src/runner/byob_profile.c`, exposed only by
the default-disabled `byob-profile-test` target. It accepts bounded UTF-8 JSON
with order-independent object fields, rejects unknown or duplicate schema
fields, requires the fixed NT4 EN-US profile identifiers, preflights every
selected name before opening a component, and performs handle-based identity
checks only after structural validation succeeds.

On 2026-08-07, MinGW-w64 GCC built and ran the owned temporary-directory
fixture. It proves acceptance of a shuffled three-role profile and deterministic
rejection for a changed component identity, wrong target profile, unsafe
component spelling, and duplicate JSON key. The remaining rows in the fixture
matrix (reparse/directory cases, optional-tier combinations, oversized input,
and all role permutations) remain required before this gate is connected to a
monitor session.

The accepted profile can now return a `byob_profile_selection` containing only
the three fixed-role names, byte counts, and hashes. `byob_image_load_exact`
then reopens one selected component with writes excluded, reads it through that
same handle, hashes the transient buffer, and returns it only on an exact
identity match. The owned fixture verifies this NTIO descriptor-to-buffer path.
It still performs no guest-memory operation.
