# Td S2 P7 — Full Interface-Conformance Closure

## Result

The live overlay now classifies all 321 canonical tracker rows exactly once.

| Disposition | Rows |
| --- | ---: |
| `opennt-shaped-facade` | 63 |
| `registered-exception` | 17 |
| `migration-debt` | 103 |
| `deferred` | 110 |
| `not-applicable-guest` | 28 |

The P7 checker verifies the exact ID universe, no duplicate overlay IDs,
registered exception references, migration conclusions and deferred receivers.

## T257 S5 disposition

T257 S5 remains frozen and is rejected for re-admission. DPMI startup,
descriptor and XMEM seams are migration debt, while atomic frame/IRET,
protected stack, mode transition, VCD and process-LDT paths remain deferred.
A new frame ABI would otherwise violate the source-first recovery order.

## Follow-up

Migrate pointer debts through dependency 118 under their owner packages.
Re-admit DPMI only through a whole source-first package covering pointer,
descriptor, frame/IRET and memory lifecycle together. Guest sources remain
guest-owned; mantle/core remain selector-blind.
