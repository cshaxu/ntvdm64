# T223 S2 COMMAND Current-Directory ABI Repair

## Question

Does the recovered `54:04 cmdGetCurrentDir` provider preserve the original
COMMAND invalid-drive failure result while remaining inside the selected
bootstrap/environment/config component?

## Inputs

- OpenNT original owner:
  `src/opennt/base/mvdm/dos/command/cmdmisc.c`, `cmdGetCurrentDir`, lines
  517--527 and its implementation.
- Recovered provider:
  `src/bx-vdm/bx_ntvdm_cmd_current_dir_service.c`.
- Regressions:
  `tests/bx-vdm/bx_ntvdm_command_bootstrap_provider_v1_test.c`,
  `tests/bx-vdm/bx_ntvdm_cmd_current_dir_service_test.c`, and the adapter
  runtime invalid-drive route assertion.

## Procedure

1. Re-read the original routine's documented terminal results: `CF=1, AX=0`
   only for a directory longer than 64 bytes, and `CF=1, AX=1` for an invalid
   drive.
2. Compare both recovered invalid-drive exits: excluded/unavailable selected
   drive and missing or mismatched host context.
3. Change those two exits from `AX=0` to `AX=1`; do not alter the bounded
   `DS:SI` success write, continuation, profile selection, or host path
   lookup.
4. Compile the existing focused `command-bootstrap` source closure with MSVC
   x64 `/MT`, link it with `bcrypt.lib` and `ntdll.lib`, and run it from the
   disposable root `build/M0-T223-S2/001-command-bootstrap-current-dir-abi`.

## Observations

- Both recovered invalid-drive exits previously reported `CF=1, AX=0`.
- Both now report `CF=1, AX=1`, matching the original COMMAND contract.
- The focused source closure compiled, linked and ran with exit code zero.
  It includes the COMMAND bootstrap provider regression, which exercises the
  no-host-context `54:04` failure path.  The dedicated helper and adapter
  runtime assertions now encode the same `AX=1` result.
- No Bochs, mantle, CLI policy, Overlay, or Virtual behavior changed.

## Interpretation and confidence

This is a source-derived adapter correction at recovery rung 2: the original
translation unit depends on historical CCPU/SAS and session composition, while
the recovered provider retains its selected `AL`, `DS:SI`, carry and `AX`
contract through checked typed transactions.  Confidence is high for the
invalid-drive terminal result.  This record is a component progress checkpoint,
not T223 S2 closure: the other six services still require their package-wide
source/ABI reconciliation and Direct/Readonly family regression.

## Follow-up

Continue T223 S2 by auditing all seven selected service success and failure
paths as one COMMAND component.  Overlay and Virtual remain interface-only
deferred modes and must not acquire an implicit Direct fallback.
