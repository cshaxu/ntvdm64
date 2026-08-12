# M78 Runner BYOB Handoff Policy

Status: implemented and regression-tested modern CLI boundary, 2026-08-08.

## Implemented Behavior

`ntdos64-run.exe` is a MinGW-w64 research CLI with three deliberately separate
paths:

| Target kind | Behavior |
| --- | --- |
| PE32/PE64 | invokes the target with `CreateProcessW`, waits for it, forwards its exit code, and confines Ctrl-C cancellation to the child process group it created |
| DOS/NE with explicit external engine | first requires `--byob-profile profile.json --byob-root directory`; it validates the selected NTIO/NTDOS/COMMAND identity set, then launches the caller-supplied engine with `--byob-profile profile.json --byob-root directory -- target args...` |
| DOS/NE without both BYOB inputs | rejects with exit code `3`; no engine process is started |

The runner links the existing read-only `byob_profile` and `byob_identity`
components. Validation verifies local component names, sizes, SHA-256 values,
roles, target/feature declarations, and compatibility group. It neither loads
guest bytes into a machine nor retains them after validation.

The profile/root arguments are an explicit engine-handoff transport, not an
engine-compatibility claim. They ensure that the engine sees precisely the
same caller-selected inputs that the runner validated, without the runner
opening guest bytes after validation or inventing a system-directory search.

## Boundary Rules

- The CLI does not link OpenNT, NTVDMx64, CCPU, oracle fixtures, BOP/DEM
  handlers, `nvtdm.c`, or an owned monitor into its normal path.
- It does not identify an external engine as NTDOS-compatible; that remains
  the caller's explicit choice until the governed backend and profile adapter
  exist.
- PE handoff is host process policy, independent of any DOS engine.
- DOS/NE handoff is not a claim that the external engine executed the selected
  guest profile correctly. It only establishes BYOB identity before launch.

## Verification

`tests/ntdos64-run-policy.cmake` is registered as `ntdos64-run-policy` and
uses only a temporary two-byte `.com` file plus `ComSpec`; no guest binary is
used as test input. It verifies:

1. no-argument usage exits `2`;
2. a PE child exit of `37` is preserved; and
3. DOS target plus engine but no BYOB profile/root exits `3`.
4. With a valid synthetic three-file BYOB set, a test-only engine probe sees
   `--byob-profile <manifest> --byob-root <root> -- <target>` and its exit
   code `47` is forwarded by the runner.

The current MinGW build passed all 13 CTest cases, including BYOB identity,
profile validation, and engine-handoff transport. This is an outer-shell
result only. M70/M75 still block the first governed NTIO prefix execution.
