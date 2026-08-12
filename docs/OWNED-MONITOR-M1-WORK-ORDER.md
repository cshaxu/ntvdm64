# Owned Monitor M1 Work Order

Status: design work order, 2026-08-07. M1 is a generic transition mechanism
above M0. It is not a DOS API, a BIOS table, a BOP decoder, or a CPU-mode
override facility.

## Historical Evidence Boundary

The original-chain selector trace in
`R1-ORIGINAL-CHAIN-CLOSURE-LEDGER.md` establishes that a historical profile
uses an instruction-level transition. It is evidence for the need to intercept
some fetched byte patterns. It does not define M1's public names, selector
format, handler API, or execution semantics.

The only later M2 adapter may turn a profile-specific trace into a registered
M1 pattern. Until then, M1 tests use owned arbitrary bytes only.

## Exact M1 Contract

### Registry

- A registry holds at most 32 entries, each with a byte pattern of 1 through
  15 bytes, the architectural maximum x86 instruction length, and an opaque
  caller-owned handler ID.
- A pattern is compared against bytes fetched at a *physical* fetch address.
  M1 receives those bytes from M0's checked fetch route or a later backend
  using the same mapping contract. It never translates a linear address.
- Registration is permitted only in the `CONFIGURING` state. `freeze` changes
  the state to `FROZEN`; no reset reopens it.
- Empty/oversized patterns, a duplicate pattern, and any prefix overlap are
  rejected at registration. There is therefore no runtime priority or
  longest-match rule.
- Dispatch before `freeze`, without enough fetched bytes, or with no match is
  `UNHANDLED`; it does not alter CPU state or guest memory.

### Handler Input And Disposition

The handler receives an immutable copy of the visible M0 real-mode state,
physical fetch address, matched bytes, matched length, and opaque handler ID.
It produces one of exactly:

```text
UNHANDLED
HANDLED_RESUME
STOP
FAULT
```

Only `HANDLED_RESUME` carries a patch. The patch may replace selected values in
`AX`, `BX`, `CX`, `DX`, `SP`, `BP`, `SI`, `DI`, `DS`, and `ES`, and may replace
only FLAGS bits `CF`, `PF`, `AF`, `ZF`, `SF`, `TF`, `IF`, `DF`, and `OF`. It
may not write `CS`, `SS`, raw `IP`, CPU mode, control registers, descriptor
state, A20, mappings, device/IRQ state, or guest memory. `HANDLED_RESUME`
always advances IP by exactly the registered pattern length, with ordinary
16-bit wrap. This fixes IP-consumption semantics and prevents a handler from
becoming a generic control-transfer mechanism.

### Atomicity

M1 creates a proposed state from the immutable snapshot, validates all patch
bits and the exact IP advance, then commits the whole visible state in one
assignment. Any invalid output leaves state and RAM unchanged. `STOP`,
`FAULT`, and `UNHANDLED` also leave state and RAM unchanged. M1 has no memory
write output at all, so a later memory-affecting transition requires a new,
separately justified contract.

## Source Split

The planned sources are `owned_monitor_m1.[ch]` and
`owned_monitor_m1_test.c`. They may include `owned_monitor_m0.h`, but must not
include or link any frozen instrumentation or historical implementation.

## Required Tests

1. Reject zero/16-byte patterns, duplicates, and either-direction prefix
   overlap.
2. Reject registration after freeze; verify reset cannot unfreeze registry.
3. Verify physical fetch matching, no-match, and insufficient-fetch outcomes.
4. Verify `UNHANDLED`, `STOP`, and `FAULT` leave M0 state unchanged.
5. Verify an allowed general-register/FLAGS patch commits along with exactly
   the pattern-length IP advance.
6. Verify forbidden register, FLAGS, or IP output rejects atomically with no
   partial state change.
7. Verify the M1 test target and normal CLI remain separate link graphs.

## Implementation Gate

M1 implementation may start only after this work order is reviewed against
the M0 header and no source changes are needed in the frozen reference set.
It remains `EXCLUDE_FROM_ALL` until an execution backend reaches the M0 fetch
boundary. An M2 profile adapter is not part of M1 acceptance.

## Implementation Evidence

On 2026-08-07, M1 was implemented in `src/owned_monitor_m1.[ch]` and the
independent `owned-monitor-m1-test` target. The target links only M0 and M1
owned sources. It neither links nor names a historical byte encoding, selector,
or service.

The fixture verifies the complete required contract set: zero/oversize
patterns, duplicate and both-direction prefix conflict, freeze persistence
across M0 reset, physical checked fetch/no-match behavior, handled state and
FLAGS patch with exact IP wrap, stop/fault immutability, and invalid register,
FLAGS, and IP-consumption output rejection without a partial state change.

The recorded verification was:

```powershell
cmake --build artifacts/build/current/default-mingw-verify --target owned-monitor-m1-test --parallel 4
ctest --test-dir artifacts/build/current/default-mingw-verify --output-on-failure -R '^owned-monitor-m[01]$'
cmake --build artifacts/build/current/default-mingw-verify --target ntdos64-run --parallel 4
```

Both M0 and M1 tests passed. The resulting normal CLI response file still
contains only `CMakeFiles/ntdos64-run.dir/src/runner/ntdos64_run.c.obj`.
