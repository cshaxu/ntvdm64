# Owned Monitor M2 Oracle-Adapter Work Order

Status: design work order, 2026-08-08. M2 is the first profile-specific
adapter admission above the neutral M0/M1 substrate. It remains a bounded
oracle comparison, not a DOS runtime.

## Admitted Historical Evidence

The fixed OpenNT NT4 source identifies one narrow, side-effect-free behavior:

- `src/opennt/base/mvdm/dos/dem/demgset.c`, `demQueryDate`, obtains a local
  time snapshot and sets `AL` to weekday, `DH` to month, `DL` to day, and `CX`
  to year.
- `src/opennt/base/mvdm/dos/nt_bop.c`, `MS_bop_0`, obtains a service byte,
  calls the original dispatcher, and advances IP only after the dispatcher
  returns.
- `docs/R1-ORIGINAL-CHAIN-CLOSURE-LEDGER.md` records the bounded original
  CCPU -> BIOS -> `MS_bop_0` -> `DemDispatch` -> date trace, including its
  post-dispatch IP result and the lack of an idle call for this service.

This permits a single adapter conformance fixture. It does not authorize a
generic service table, a second dispatcher, DOS globals, image boot, file I/O,
or an original runtime claim.

## Required Split

```text
owned M1 generic transition
  -> profile-local NT4 trace descriptor
  -> pure date-result adapter
  -> injected clock snapshot
```

The date adapter receives a validated, immutable clock snapshot. It must not
call `GetLocalTime`, read a clock, retain a host handle, or access a guest
memory pointer. A later host-capability layer owns Win32 clock acquisition and
converts it to this neutral snapshot. M2 tests use a fixed owned snapshot, so
they do not depend on wall-clock time.

## Exact Adapter Limits

- The profile-local descriptor may contain historical trace bytes, but M1's
  public API remains byte-pattern neutral and does not expose product-specific
  names.
- The descriptor's consumed length must equal the original trace's complete
  dispatcher-plus-service-byte advance, not merely its prefix.
- The adapter may write only the historical `AL`, `CX`, `DH`, and `DL`
  portions of M1's admitted `AX`, `CX`, and `DX` patch fields, preserving the
  other bytes and all FLAGS.
- Month is 1..12, day is 1..31, weekday is 0..6, and year is 1980..65535.
  Invalid snapshot input returns `FAULT` without changing M0 state.
- The normal M1 `HANDLED_RESUME` contract performs the one atomic state
  commit. The adapter never writes guest memory or directly advances IP.

## New Source Boundary

| File | Responsibility | Must not do |
| --- | --- | --- |
| `src/archived/legacy-adapters/owned_clock_snapshot.h` | neutral validated civil-date snapshot | call Win32 or expose time-zone policy |
| `src/nt4_oracle_date_adapter.[ch]` | pure profile-local date mapping into an M1 handler output | implement a service table, fetch guest memory, host I/O |
| `src/archived/legacy-adapters/nt4_oracle_date_adapter_test.c` | fixed-snapshot conformance fixture | use an owner-supplied runtime or system clock |

The adapter target is `EXCLUDE_FROM_ALL` and must link only M0/M1 and these
new owned sources. It may not link the old self-authored trace files or a
historical object/library.

## Required Tests

1. A fixed valid snapshot sets exactly the documented low/high register bytes,
   preserves unrelated register bytes and FLAGS, and advances IP by the
   descriptor's full historical advance.
2. Each invalid month/day/weekday/year input returns `FAULT` with no changed
   state or RAM.
3. A neighboring nonmatching byte sequence is `UNHANDLED`.
4. The adapter has no Win32 import and no historical-object link dependency.
5. `ntdos64-run` remains a one-object CLI link target.

## Non-Claims

Passing M2 proves only that the owned constrained transition model can express
one source-derived, clock-snapshot result. It does not prove instruction
execution, an original BOP implementation, DEM execution, NTIO startup,
filesystem correctness, DPMI, V86, or DOS command execution.

## Implementation Evidence

On 2026-08-08, M2 was implemented as the default-disabled
`nt4-oracle-date-adapter-test` target. `nt4_oracle_date_adapter.c` has one
profile-local four-byte trace descriptor and a pure handler. It takes only an
`owned_clock_snapshot`; it imports no Win32 API and links no historical object
or frozen instrumentation source.

The fixture uses a fixed 1996-07-04 weekday-four snapshot. It verifies the
source-defined `AL`, `CX`, `DH`, and `DL` update, preserves unrelated register
bytes and FLAGS, and proves the complete four-byte IP advance. It also proves
that nonmatching bytes are unhandled and that invalid month, day, weekday, and
year values each produce an atomic fault with unchanged state and RAM.

The recorded verification was:

```powershell
cmake --build artifacts/build/current/default-mingw-verify --target nt4-oracle-date-adapter-test --parallel 4
ctest --test-dir artifacts/build/current/default-mingw-verify --output-on-failure -R '^(owned-monitor-m[01]|nt4-oracle-date-adapter)$'
cmake --build artifacts/build/current/default-mingw-verify --target ntdos64-run --parallel 4
```

All three isolated tests passed, and `ntdos64-run` again built unchanged.
