# M0 T199 S19 Machine-Owned BOP and BIOS Source/ABI/Owner Map

## Question

Which remaining BOP and historical SoftPC BIOS identities are machine-owned,
and how can the CLI profile route their known results without selecting a
historical `BIOS[]` branch or introducing BOP knowledge into Bochs?

## Inputs And Procedure

- The immutable `opennt-bop-inventory-005` source export and its pinned
  OpenNT inputs.
- `softpc.new/base/bios/bios.c`, `base/inc/bios.h`, `host/src/nt_bop.c`, and
  the retained DPMI/XMS source maps.
- Existing S11, S16 and T96 S7 evidence.
- Run `tests/bx-vdm/Test-SoftpcBiosOwnerMatrix.ps1` and
  `tests/bx-vdm/Test-BopInventoryProfileRegression.ps1`.

## Inventory And Owner Results

| Identity group | Count | Owner and present disposition |
| --- | ---: | --- |
| Named historical BIOS definitions | 41 | Inventory identity only; no definition independently enables a feature. |
| Conditional `BIOS[]` rows | 349 | 227 historical unavailable, 25 host-selector collisions, 97 Bochs-native candidates. No conditional branch is selected at runtime. |
| `12` BIOS memory size | 1 | Existing bounded `bx-vdm` machine-composition result: real-mode `AX=027Fh`, `RIP+3`; it is profile data, not a Bochs memory query. |
| `15` BIOS extended memory | 1 | Existing bounded `bx-vdm` result only for real-mode `AH=88h`: `AX=0C00h`, `RIP+3`; all other forms remain unhandled. |
| `5C`, `5D` | 2 | Product-variant conflict between BOP headers and NTVDM BIOS table; explicit deferred/controlled-stop boundary until one active historical product composition is source-identified. |
| `5F` | 1 | Existing source-defined machine handoff through checked action; bx-mantle remains selector-blind. |
| `FD` | 1 | DPMI/monitor real-mode transition, deferred behind C1--C6 machine capabilities. |
| XMS/DPMI machine prerequisites | 6 capability groups | C1 A20, C2 extended aperture, C3 UMB map, C4 protected mode, C5 interrupt/fault delivery, C6 transition/session; bx-core owns mechanics and bx-mantle owns typed lifecycle assembly. |

The two regressions passed with these exact counts:

```text
SoftPC BIOS owner matrix verified: definitions=41 rows=349 unavailable=227 collisions=25 native-candidates=97
OpenNT BOP inventory/profile regression verified: selectors=16 service-identities=195 BIOS-forms=349
```

## ABI Boundary

`bx-core` receives no selector, service, DOS, OpenNT, or SoftPC name. It owns
the native CPU, RAM/ROM, memory-map, mode, interrupt and device mechanics.
`bx-mantle` may expose only versioned fixed-width selector-blind machine
requests. `bx-vdm` owns copied BOP classification and the composition choice:
existing `12`/`15:88` profile results, `5F`'s checked request, or a typed
deferred/controlled-stop result. It cannot invoke or recreate `BIOS[]`.

## Implementation Admission

S20 may create one `bx-vdm` machine-BOP facade that moves the existing
`12`/`15:88` and `5F` paths behind common ingress and emits the selected
deferred/controlled-stop disposition for `5C`, `5D`, and `FD`. It may not add
a Bochs feature, infer a historical BIOS branch, or implement C1--C6. A later
machine-capability package must build each C capability selector-blind before
the XMS or DPMI provider package may produce results.
