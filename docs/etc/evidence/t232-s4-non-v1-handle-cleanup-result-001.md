# M0 T232 S4 — Non-v1 Host-Handle Cleanup Result

> **Superseded ABI detail:** the cleanup result remains valid, but its
> 16-bit ID interpretation is replaced by M0 T234's
> [same-width 32-bit successor](t234-s1-32bit-opaque-host-handle-abi-result-001.md).

## Question

After DEM and COMMAND migrate, does any non-v1 product route or formal
fixture retain a private host-`HANDLE` to guest-token table?

## Inputs

- T232 S1--S3 manager and migration results.
- All `src/bx-vdm` product sources except `src/bx-vdm/bop-v1/`.
- Formal `tests/bx-vdm` sources named by
  `tools/build/t225-s7-full-module-manifest.json`.

## Procedure

1. Replaced the two DEM fixture-local `HANDLE[]` maps with owned entries in
   the common manager.
2. Replaced the two COMMAND fixture-local `handle_tokens` uses with manager
   publication and lookup, including explicit high-word rejection coverage.
3. Searched the non-v1 product and formal-fixture surface for private
   `HANDLE[]`, `handle_tokens`, and token-table patterns.
4. Ran the full formal Ninja graph and the seven focused manager/DEM/COMMAND
   fixtures from `build/M0-T232-S1/formal-r1`.

## Observations

- No non-v1 product route or formal fixture contains `handle_tokens` or a
  private `HANDLE[]` guest-token map.
- The only remaining `HANDLE roots[26]` declarations are namespace drive-root
  sets. They model projected drive roots, not external-handle translation,
  and are outside the manager's ownership.
- Retained v1 comparison material, including `src/bx-vdm/bop-v1/` and
  non-formal historical v1 tests, was deliberately not edited; it cannot be
  selected by the product or the formal module manifest.
- Full formal Ninja build completed 82 incremental actions successfully.
- All seven focused fixtures exited zero: manager foundation, DEM direct
  session, DEM `demhndl`, DEM `demfile`, DEM FCB/search, COMMAND console, and
  COMMAND lifecycle.

## Interpretation And Confidence

T232 has removed every active duplicate private host-handle mapping surface.
The common manager is the sole non-v1 representation mechanism; it is
portable across x86/x64 because only its host-private indexes store a native
`HANDLE`, while guest-visible IDs are always `uint16_t`.

The present runtime binds DEM and COMMAND independently, so this is not a
claim that an unimplemented combined native session already shares one live
manager instance. If such a session is admitted, it must own and inject one
manager; this follows from the already-closed ABI rather than reopening any
private table design.

## Follow-up

T232 is ready to close. XMS and later owner packages must consume this
manager rather than introduce another handle/token table.
