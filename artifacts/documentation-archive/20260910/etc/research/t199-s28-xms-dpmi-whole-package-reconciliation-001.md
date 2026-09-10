# M0 T199 S28 XMS/DPMI Whole-Package Reconciliation

## Question

Does the existing package plane give every XMS/DPMI BOP one truthful current
disposition after C1--C5 were audited, without claiming that C1/C2 close either
historical provider?

## Reconciled Disposition

| Package / identities | Component dependency | Current disposition |
| --- | --- | --- |
| XMS `52:00` | C1 A20 plus original XMS request/register semantics | Deferred: native prerequisite exists, provider does not. |
| XMS `52:01..05,0A,0B` | C2 plus XMS handles, copied structures and error mapping | Deferred: native prerequisite exists, provider does not. |
| XMS `52:06..08` | C3 UMB map | Deferred: C3 is not admissible in the no-device machine. |
| XMS `52:09` | C5 firmware/keyboard INT15 lifecycle | Deferred: C5 is not admissible in the no-device machine. |
| DPMI `53:00,01,03,04,05,0F,11` | C4 descriptor/mode state plus DPMI session | Deferred: C4 is not admissible as a standalone ABI. |
| DPMI `53:02,0A,14..18` | C5 interrupt/fault lifecycle | Deferred: C5 is not admissible. |
| DPMI `53:06` | C6 protected-to-real DOS bridge | Deferred: no DPMI session or DOS transition contract. |
| DPMI `53:07..09,0B,10,13` | C2 plus DPMI memory/session mapping | Deferred: C2 alone supplies no DPMI lifetime or flat mapping. |
| DPMI `53:0C,0D` | C6 session records | Deferred. |
| DPMI `53:0E` | C4 debug registers and exception lifecycle | Deferred. |
| DPMI `53:12` | VDD/VCD product integration | Explicitly unavailable. |

`52:00..0B` and `53:00..18` therefore retain exactly 37 callable identities:
36 deferred and one (`53:12`) explicit unavailable. `52:0C` and `53:19+` are
not callable and remain common-ingress unknown-service failures.

## Regression

MSVC x64 `/MT` compiled and ran the package-plane fixture at
`artifacts/build/t199-s28-xms-dpmi-facade-r4/t199-s28-xms-dpmi-plane.exe`.
It exercises all 12 XMS and all 25 DPMI identities through copied ingress and
provider selection, verifies the service/family record for each, preserves
deferred disposition for every member except `53:12`, and rejects `53:19`.

Earlier r2/r3 link attempts omitted existing ABI support objects and failed
with unresolved symbols; r4 names the complete fixture-local closure. They do
not indicate a machine or provider defect.

## Interpretation

The plane is a package identity/owner record, not an XMS or DPMI provider. It
does not issue a CPU result, perform a machine capability request, or return a
historical service failure. C1/C2 availability is a necessary prerequisite for
later whole-provider work, not permission to promote individual services.

## Follow-up

The blocked XMS/DPMI packages now have a stable whole-family disposition. The
next implementation admission must target a package whose full provider
contract is not blocked by C3--C6; it must not be selected from a trace hit.
