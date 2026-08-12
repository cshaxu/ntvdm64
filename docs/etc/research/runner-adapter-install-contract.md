# Runner-to-Adapter Installation Contract v1

## Purpose

The CLI runner owns command-line admission. The adapter runtime is linked into
the selected engine process and owns the admitted session and payload lifetime.
This record defines the one process-lifecycle bridge between them without
placing guest bytes, DOS semantics, or Bochs configuration in the runner.

## Inputs and Admission

For a DOS target, `ntdos64-run` requires all three inputs:

```text
--engine <engine.exe>
--byob-profile <profile.json>
--byob-root <directory>
```

Before the engine starts, the runner validates the profile and every declared
NTIO, NTDOS and COMMAND identity against the supplied root. A missing pair or
failed validation returns exit status `3`; no engine process is created.

## Child Process Contract

After acceptance, the runner starts exactly one engine child. It hands the
already accepted strings over in both forms:

| Consumer | Contract |
| --- | --- |
| Engine command-line parser | `--byob-profile <profile> --byob-root <root> -- <target> [args...]` |
| Adapter runtime v1 in that engine | `NTDOS64_ADAPTER_PROFILE=<profile>` and `NTDOS64_ADAPTER_ROOT=<root>` in a Unicode child-only environment block |

The runner removes any inherited values with these two names before inserting
the accepted values. The parent environment is not changed. The environment
block is passed with `CREATE_UNICODE_ENVIRONMENT` and is released immediately
after successful process creation.

The adapter's `install_from_environment()` is the only current consumer of
these variables. It independently validates and loads the fixed identity set
into adapter-owned memory. An absent or invalid environment remains
fail-closed/pass-through at the Bochs exception seam.

## Ownership Limits

The runner does not read guest payload bytes, mount a DOS filesystem, parse a
BOP, select a DEM service, initialize a Bochs object, or construct a machine
profile. It owns only CLI syntax, identity admission, child lifetime and
diagnostics. The engine's command-line parser likewise remains free to reject
unsupported target execution; this transport contract is not a claim that the
engine boots NTDOS or runs COMMAND.

## Evidence

`tests/ntdos64-run-policy.cmake` starts the test-only
`runner-engine-probe.exe` through the production runner. The probe rejects a
missing/incorrect command-line handoff and also compares both child environment
variables byte-for-byte with the corresponding arguments. The isolated
`artifacts/build/current/adapter-cli-r1` CMake build reported:

```text
byob-profile ........ passed
ntdos64-run-policy .. passed
```

This proves CLI-to-engine installation transport only. The separate r12
fixture remains the evidence that an installed adapter session can commit the
first transaction in a real Bochs process.
