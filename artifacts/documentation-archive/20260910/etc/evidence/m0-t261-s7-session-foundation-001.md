# M0 T261 S7 Session Foundation Evidence 001

## Result

M0 T261 S7 is closed. `src/session` is the ninth formal host-side static
module. It contains only project-authored, fixed-width session lifecycle and
cancellation state; it imports no product component and exposes no guest,
machine or host-object ABI.

## Owner Map

- `session/ntdos64_session_v1.*` owns neutral instance identity, activation,
  completion and reverse-order teardown registration.
- `session/ntdos64_session_cancellation_v1.*` owns the formerly
  `adapter-softpc` cancellation state. The retained adapter-softpc header is
  spelling-only compatibility for mechanical callers; it owns no state.
- `app/bx_ntvdm_engine_run_v1.c` alone creates, activates, completes and resets
  the one supported instance. It binds the mapping registry before composing
  provider-owned startup state.
- `adapter-softpc/bx_ntvdm_guest_pointer_manager.*` remains the one mapping
  manager implementation, with its three existing typed instances. It binds
  them to the session's unique fixed-width identity and tears down through the
  session lifecycle; it neither creates a second manager nor exports machine
  state through `session`.

## Verification

- `tests/session/t261_s7_session_fixture.c` passes in formal root
  `build/t261/s7-r004`: it proves activation, duplicate teardown suppression,
  completion, reverse teardown, reset, and rejection of a second active
  session identity by the mapping registry.
- `ninja -C build/t261/s7-r004 -j 4 bin/ntdos64-native.exe` completes and
  links the formal native target with MSVC x64 `/MT`.
- A negative scan of `src/session` production sources finds no BOP, DOS, WOW,
  VDD, Redirector, Win32, Bochs, OpenNT or handle vocabulary. `src/app` no
  longer imports the legacy cancellation-controller spelling, and the old
  cancellation implementation is absent from live product inputs.
- The component manifest parses as JSON and names nine modules; `git diff
  --check` passes.

## Deliberate Limits

This closes neutral lifecycle ownership only. DEM, COMMAND, XMS and other
OpenNT provider runtime sessions retain their source-owned semantics. The
selector-aware observation and fixture-only residue in adapter production
roots remains S8 work and is not legitimized by this evidence.
