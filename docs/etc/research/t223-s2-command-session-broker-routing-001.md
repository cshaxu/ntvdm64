# T223 S2 COMMAND Session-Broker Routing Correction

## Question

Does the COMMAND route map preserve the original owner of `54:10
cmdGetStartInfo`, without expanding the admitted bootstrap/environment/config
component or inventing a session capability?

## Inputs

- `src/opennt/base/mvdm/dos/command/cmddisp.c`, slot 16.
- `src/opennt/base/mvdm/dos/command/cmdmisc.c`, `cmdGetStartInfo`, lines
  885--896: it returns `AL` from historical `DosSessionId`.
- Current plane, facade, provider-owner map, and all-17-slot COMMAND fixture
  below `src/bx-vdm/` and `tests/bx-vdm/`.

## Procedure

1. Compare the original routine's `DosSessionId` dependency with T223's
   explicitly admitted seven-service bootstrap component.
2. Remove service 16 from the bootstrap provider's ownership declaration.
3. Classify service 16 as the existing `SESSION` component and retain its
   deferred, controlled-stop disposition until a separately admitted session
   broker can preserve the original meaning.
4. Run focused bootstrap and all-17-slot route-map fixtures with MSVC x64
   `/MT` in disposable T223 roots.

## Observations

- Before this correction, service 16 was labelled `BOOTSTRAP` solely because
  the route facade needed a deferred owner; this contradicted its original
  `DosSessionId` contract and T223 scope.
- The route now carries `SESSION`, while the facade's existing deferred stop
  behavior remains selector- and result-compatible.
- The bootstrap ownership regression and the complete COMMAND route-map
  fixture both compile, link and run with exit code zero.

## Interpretation and confidence

This is a source-map correction, not an implementation of `cmdGetStartInfo`.
Its historical host session broker is neither independently composable nor
admitted into the contained CLI profile.  The corrected map prevents an
unrelated bootstrap provider from claiming that capability.  Confidence is
high for owner classification; the deferred controlled stop is retained as a
current limitation, not asserted to be the original success behavior.

## Follow-up

Continue only the seven T223 services as one bootstrap/environment/config
component.  A later separately admitted COMMAND session-broker package must
replace the deferred `54:10` result with a source-shaped capability decision.
