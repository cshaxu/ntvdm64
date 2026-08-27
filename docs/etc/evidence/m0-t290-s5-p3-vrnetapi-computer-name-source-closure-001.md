# M0 T290 S5 P3 — original NetAPI computer-name source closure

## Question

Can the exact original `VrGetComputerName` body use the public host computer
name API while retaining its original `LM20_CNLEN` gate, real-mode destination
contract and register results on both supported host widths?

## Inputs

- Exact original `src/mvdm-host/vdmredir/vrnetapi.c`.
- The declaration cohorts selected by S5 P2.
- The existing session-owned Redirector pointer scope, which supplies a
  bounded temporary target only for the source `ES:DI` destination.
- `tests/mvdm-host/vdmredir/t290_s5_vrnetapi_computer_name_fixture.c` and
  `tools/build/New-T290S5VrNetApiComputerNameNinja.ps1`.

## Procedure

1. Obtain the current host computer name through the same public
   `GetComputerNameA` API used by the original body.
2. If the name fits the original `LM20_CNLEN` condition, declare an exact
   `LM20_CNLEN + 1` writable `ES:DI` scope before invoking the original body.
3. Otherwise invoke the original body without a guest target and verify its
   source `NERR_BufTooSmall` result.
4. Generate, build and execute the fixture on MSVC x86 `/MT` and x64 `/MT`.
   The focused link uses `/FORCE:UNRESOLVED` only because uncalled sibling
   Lanman/SMB/SoftPC paths inhabit the same untouched original object.

## Observations

- Both graphs compile the complete exact `vrnetapi.c` translation unit and
  execute their fixture with exit code zero.
- A host name at or below the original 15-character limit is copied through
  the existing scoped lease, then the source body clears carry and sets
  `AX=0`.
- A longer name follows the original `NERR_BufTooSmall` plus carry-set branch;
  it does not create a larger project-defined result buffer.
- No raw host pointer, host handle, selector ingress, Lanman service or
  private SMB path is enabled. The fixture is not a product link claim.

## Interpretation

`VrGetComputerName` is locally source-composed on both host widths with only
the existing bounded real-mode pointer binding. The next still-admissible
candidate is the bounded `BX=1` form of `VrGetUserName`, conditional on a
source-shaped public NetWksta/character-conversion binding. The unbounded
`BX=0` form and all transaction, NetBIOS and DLC bodies retain their frozen
source-unavailable or other-owner dispositions.
