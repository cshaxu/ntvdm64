# T198 S23 P2 Native NTIO First-Boundary Witness 001

## Question

Does the exact source-built NTIO image reach the already admitted DEM `50:11`
provider when composed through the v2 pre-entry request?

## Inputs And Procedure

The retained source-built `NTIO.SYS` was read only after PowerShell verified
33,792 bytes and SHA-256
`CFC8BE16576BB6ACD16BB8FC9B2D9A080F544BBFDD2D2D2DF07ED908B3AB4937`.
Its bytes were compiled only into an ignored evidence build.  The complete
MSVC x64 `/MT` CPU5 closure linked the existing boot-namespace composition,
the S23 v2 request builder, and a test-only `bx-vdm` bridge.  That bridge
records `C4 C4 50 11`, delegates to the normal composition, then emits a typed
STOP only at a later unhandled `#UD`.

## Observations

The native fixture printed:

`t198-s23 status=0 observed-5011=1 observed-stop=1`

Bochs also logged the later real-mode prefetch segment-limit condition.  The
finite-run status `0` is its existing budget category; it does not encode an
external adapter STOP, while the test bridge’s separate stop marker proves
that STOP was requested.

## Interpretation And Follow-Up

The exact NTIO image crossed the generic machine boundary and reached the
existing source-derived `50:11` provider before the later missing boundary.
This proves first handoff, not DOS continuity.  The next task must make the
typed external STOP observable in the generic finite result before diagnosing
the reached later selector or machine condition.
