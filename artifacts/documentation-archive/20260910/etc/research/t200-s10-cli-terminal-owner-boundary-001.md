# T200 S10 — CLI terminal-owner boundary

## Question

Does T200 S9's bounded native observation establish a user-facing CLI exit,
and which component must own the next repair?

## Inputs

- `src/bx-mantle/bx_ntvdm_finite_run.h` and `.cc`;
- `tests/bx-vdm/t198_s23_native_ntio_boundary_fixture.cc`;
- `src/cli/ntdos64_run.c`, `src/cli/ntdos64_bochs_engine.c`, and
  `src/cli/README.md`;
- T200 S9's retained native observation.

## Procedure

Read the bounded runner's result enumeration and cleanup path, then compare
the fixture's `main` return expression with both production CLI process
launchers and their child exit handling.  No guest execution, source change,
or Bochs configuration change was performed.

## Observations

1. `bx_ntvdm_run_finite_bare_bytes` is explicitly a private C++ fixture
   interface.  It initializes a disposable minimal machine, runs a fixed
   instruction budget, and returns a `bx_ntvdm_finite_run_status`; it has no
   product CLI ABI or DOS exit-code mapping.
2. The S9 program is `tests/bx-vdm/t198_s23_native_ntio_boundary_fixture.cc`.
   Its `main` returns `0` or `2` from assertions about observations such as
   instruction history and a generic-UD record.  Therefore the recorded
   process exit `2` is an assertion outcome, not guest or CLI status.
3. `ntdos64-run` owns user-facing child cancellation and calls
   `GetExitCodeProcess` for its selected engine.  The current
   `ntdos64-bochs-engine` in turn starts a separately bundled
   `ntdos64-bochs.exe` and returns that child exit code unchanged.
4. The current engine is consequently a retained external-product shell:
   it prepares a fixed `bochsrc` and does not instantiate the source-built
   `bx-mantle`/`bx-vdm` composition that S9 exercised.  It cannot translate a
   typed adapter stop into a documented CLI outcome.

## Interpretation and confidence

High confidence: S9 proves only that the one-slot `54:01` exhaustion reached
the composition's typed-stop boundary without a raw generic `#UD`.  It neither
proves an ordinary COMMAND return nor a product CLI exit.  The next owner is
the **CLI/engine composition package**, not COMMAND and not a further BOP
endpoint: it must replace the old `bochs.exe` product shell with a fixed-width
native-engine entry contract, while leaving Bochs lifecycle in `bx-mantle` and
BOP semantics in `bx-vdm`.

The intended direction is:

```text
ntdos64-run (CLI child/cancellation/diagnostics)
  -> native engine entry ABI (opaque fixed-width request/result)
     -> bx-mantle lifecycle -> bx-vdm composition -> bx-core mechanics
```

The CLI must not include Bochs headers or map typed stops itself.  A later
admitted engine entry may expose only a versioned result record whose terminal
kind is converted by the CLI into documented process status.

## Follow-up

T200 S11 is admitted only to map the native-engine entry/request/result ABI,
the profile/launch snapshot handoff, lifecycle ownership, and failure/status
taxonomy.  It must not implement the entrypoint, run the guest, add a BOP, or
alter Bochs.  Implementation needs a later S admission after that map.
