# M0 T394 S1 — adapter ownership audit

## Question

Does every current `adapter-mvdm-host-out` family describe the semantic
boundary it actually implements, and does every file below `softpc` have one
architecture-valid final owner before mechanical migration begins?

## Inputs

- `docs/rules/ARCHITECTURE.md`, `docs/design/ARCHITECTURE.md`, the T394
  proposal, and current family READMEs.
- Every current file below `src/adapter-mvdm-host-out` and direct production,
  test, and formal-generator references.
- The complete per-file [T394 S1 ledger](../operations/m0-t394-s1-adapter-ownership-ledger.tsv).

## Procedure

1. Count paths and source lines by adapter family.
2. Read each family contract and classify every `softpc` path by its caller,
   retained ABI, actual semantics, and dependency direction.
3. Cross-check the classifications against the T310 formal source lists and
   the existing specialist families. Reject a generic replacement component.

## Observations

| Current family | Files | C/C++ header/source lines | Audit result |
| --- | ---: | ---: | --- |
| `basesrv` | 8 | 1,279 | Named correctly: local BaseClient/BaseSrv VDM command protocol. |
| `debugger` | 6 | 275 | Named correctly: debugger product-interface boundary. |
| `monitor` | 10 | 234 | Named correctly: historical VDM monitor/TIB/DPMI-control boundary, not a general diagnostics owner. |
| `redir` | 11 | 402 | Named correctly: VDMREDIR/Redirector host bridge. |
| `softpc` | 38 | 4,149 | Mixed machine/resource-bridge, specialist-owned and as-yet-unsplit observation semantics; not currently name-accurate. |
| `vdd` | 1 | 0 | Correct but empty; it must receive the existing VDD boundary implementation. |
| `win32` | 29 | 2,456 | Named correctly: public/same-shaped modern Win32 and declaration boundary. |
| `wow` | 3 | 77 | Named correctly: WOW callback-frame boundary. |

The 38 `softpc` paths receive these final dispositions:

| Final owner | Paths | Meaning |
| --- | ---: | --- |
| retain `softpc` | 23 | CCPU/SoftPC synchronous machine mapping, A20, identity, descriptor, execution, event-worker and XMS mechanics; plus the selected-firmware-root and loaded-V7-VGA-ROM controlled read bridges. |
| `basesrv` | 4 | COMMAND/BaseVDM record state and native-child snapshots. |
| `redir` | 2 | COMMAND standard-stream redirection. |
| `vdd` | 5 | VDD configuration, explicit unavailability, and SFT/JFT shadow boundary. |
| split-required | 2 | `mvdm_softpc_termination` is a mixed session/app/machine/service/product observation surface; S5 must split it at function level before final ownership. |
| ABI audit-required | 1 | `error_abi.h` is an original SoftPC host ABI bridge; S5 must verify relocation to the existing `opennt-abi` carrier. |
| family policy | 1 | `softpc/README.md`, updated after the mechanical migration. |

No new `common`, `compat`, or generic family is needed. `session` is not a
destination: it remains dependency-neutral and cannot acquire COMMAND, VDD,
or observer vocabulary. Likewise, `wow` does not receive VDD state and
`basesrv` does not receive CCPU mapping mechanics.

## Interpretation and confidence

High confidence: the principal ownership drift is real and bounded. The
current `softpc` tree has twenty-three valid machine/resource-bridge/policy
paths, eleven already-final specialist move rows, two mixed-file split rows,
one original-host ABI candidate and one policy row. S2 can remain mechanical
because its five VDD rows are unaffected; S5 must not assume a final owner for
the mixed rows.

The prior whole-file `monitor` and `win32` placements are withdrawn. The
termination file contains a session termination conversion, app-only startup
environment capture, CCPU/SoftPC observations, COMMAND/DEM/BOP observations
and product-container probes. `monitor` remains reserved for its historical
VDM monitor/TIB/DPMI-control boundary and does not become a generic diagnostic
sink. `error_abi.h` is a source-compatible original SoftPC host ABI bridge;
its candidate owner is the existing `opennt-abi` carrier, pending S5 review.

## Follow-up

The four firmware/presentation paths are retained after owner correction:
`mvdm_softpc_firmware` adapts the app-selected resource root for original
SoftPC ROM initialization and DEM/COMMAND media queries, while
`mvdm_softpc_presentation_font` uses `c_sas_loads` to read the already-loaded
V7 VGA ROM for the app presentation consumer.  Neither path owns the app
resource policy or presentation policy; `app` remains their caller/configurer,
not their implementation owner.

S2 may move only its five VDD ledger rows, add forwarding-only includes for a
bounded transition where required, and update every consumer and formal source
list. It must not alter a signature, status/failure direction, mapping epoch,
or runtime result. The final reconciliation removes every forwarding path and
proves no non-machine implementation remains below `softpc` other than the
four approved SoftPC host-resource bridge paths.
