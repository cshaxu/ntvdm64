# M0 T199 S15 Debugger Deferred Package Regression

## Question

Can the CLI safely route the complete Debugger BOP package without consuming a
nonexistent fourth service byte or emulating its host debugger protocol?

## Implementation

Ingress now identifies `C4 C4 56` as selector-only Debugger routing with
`has_service=0`; any observed fourth byte remains outside the BOP. The system
plane records one `DEBUG_DISPATCH` component. The package facade performs no
stack or guest-memory read and returns only typed controlled stop.

## Verification

1. MSVC x64 `/MT` built and ran
   `artifacts/build/t199-s15-debugger-package-r1/debugger-package.exe`.
   It checked both a three-byte window and a four-byte window ending in `90h`:
   both stop with no register or flag writes.
2. The bound `bx-vdm` composition rebuilt and ran at
   `artifacts/build/t199-s15-debugger-composition-r1`. Its generic-UD fixture
   checks the same two forms through common ingress and composition routing.
3. No guest trace, host debugger API, stack parser, CCPU/SAS access, Bochs
   change, or Debugger feature was added.

## Interpretation

The Debugger package is closed at the CLI profile's explicit deferred-stop
disposition. This is intentionally not a claim that the original no-debug
dispatcher has been reimplemented; it prevents a false-success continuation
until a separately designed debugger capability exists.

## Follow-up

Map the remaining non-family top-level BOP selectors by original owner and
failure contract before adding any package-wide disposition facade.
