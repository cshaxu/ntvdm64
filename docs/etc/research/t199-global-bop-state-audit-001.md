# M0 T199 S1 Global BOP State Audit

## Inventory reconciliation

The authoritative inventory is `opennt-bop-inventory-005` with JSON SHA-256
`893EB8D98A476A4D8A7AE996635B2B2BD24706594DE3666C2E664A1FB97E5C28`.
It contains 16 top-level selectors and 194 service definitions. Two service
definitions are non-callable sentinels, leaving 192 callable host-family
services. The inventory also retains 41 BIOS constants and 349 conditional
historical BIOS table rows.

| Owner family | Defined | Callable | Current global status | Source/evidence basis |
| --- | ---: | ---: | --- | --- |
| DEM (`50`) | 74 | 73 | Source map complete; partial namespace/session/device/state rehosts exist, but no complete DEM package closure. | T96 S3 provider map and current composition manifest. |
| COMMAND (`54`) | 18 | 17 | Source map complete; partial bootstrap/launch/config/console candidates exist; no complete COMMAND package closure. | T96 S4 provider map; T198 endpoint evidence. |
| XMS (`52`) | 12 | 12 | Deferred as A20, allocator, UMB and INT15 package. | T96 S5 XMS map. |
| DPMI (`53`) | 25 | 25 | Deferred as PM/LDT/exception/memory package; VCD service 12 explicitly unavailable. | T96 S5 DPMI map. |
| Redirector (`57`) | 49 | 49 | Entire family explicitly unavailable for the CLI profile. | T96 S6 system map. |
| Debugger (`56`) | 16 | 16 | Entire family deferred to a future debugger owner. | T96 S6 system map. |
| Top-level non-family | 16 | n/a | Classified by ingress; WOW, VDD/UI, ISV and CPU lifecycle forms remain deferred, unavailable or machine-owned. | Inventory and T96 S6 selector map. |
| SoftPC BIOS rows | 349 | n/a | 227 historical-unavailable, 25 host-selector collisions, 97 Bochs-native candidates. | T96 S7 owner matrix. |

The ingress/profile regression already proves every one of the 192 callable
host-family services reaches its correct family plane, while both sentinels
reject. This is classification coverage only; it is not provider execution,
link closure, regression closure or native runtime closure.

## Current implementation evidence

The active composition manifest includes selected DEM namespace/session/state
providers and selected COMMAND bootstrap/launch/config/console candidates.
Membership does not imply routing. The following distinctions are binding:

| Surface | Current evidence | Missing before package closure |
| --- | --- | --- |
| DEM | Bounded namespace, open/seek/read/close, search/DTA, drive/DPB/GSET, IOCTL/error/state candidates. | One coherent disposition for all 73 callable services, whole-family regression and a package trace. |
| COMMAND | Bounded COMSPEC, registration, finite launch plan, config/autoexec, fixed keyboard no-install and root current-directory candidates. | Unified bootstrap/launch/console/execution/environment/lifecycle dispositions, full package regression and a package trace. |
| `54:0E` | Fixed DX=0/four-byte resume and one earlier continuation to `54:04`. | Complete console capability disposition and package regression. |
| `54:04` | Adapter-only restricted root response/direct failure. | Native integration evidence and complete current-directory disposition. |
| bx-core/bx-mantle | CPU5 finite mechanics, RAM, copied #UD bridge and finite lifecycle evidence. | Complete machine/BIOS feature disposition; no BOP semantic routing may enter these layers. |

## Ordered implementation plan

1. DEM: source/ABI/failure matrix, then all-service disposition, then one
   component implementation/regression and one trace.
2. COMMAND: apply the same process to bootstrap, launch, console, execution,
   environment and lifecycle together; existing endpoints are inputs only.
3. XMS: establish native A20/allocator/UMB/INT15 ownership before any result.
4. DPMI: establish PM/LDT/exception/memory ownership before any result.
5. Redirector: retain and regress whole-family unavailable contract.
6. Debugger: retain and regress whole-family deferred contract.
7. Machine/BIOS: complete bx-core/bx-mantle owner/disposition map and native
   regression without introducing BOP semantics into Bochs.

A package may run exactly one integration trace only after its source map,
disposition and whole-package regression have passed. Until every package has
a closure disposition, a trace may not create a repair or implementation task.
