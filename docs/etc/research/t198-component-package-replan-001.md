# M0 T198 Component-Package Replan

## Governing correction

Native startup traces are integration and package-ordering evidence. They do
not create a next implementation task merely because they reach a new service.
A reached service is an endpoint of an already-defined component package.

The interrupted S133 native process is not evidence: it was terminated after
the owner correction and no result is retained or interpreted.

The owner then authorized one narrow prerequisite closure for the already
admitted pair `54:0E`/`54:04`. It is a bounded COMMAND console/launch endpoint
integration slice, not a complete COMMAND component package. Its single
native result may not create another BOP task; component-package sequencing
resumes immediately after it is recorded.

## Required package order

| Order | Component package | Required internal scope |
| --- | --- | --- |
| 1 | DEM | Namespace, search, session, device, state, and explicit failures. |
| 2 | COMMAND | Bootstrap, `54:05` registration, `54:01` launch/CMDINFO/return, current-directory, console/keyboard disposition, execution, environment, error and exit lifecycle. `54:0E` and `54:04` have only partial endpoint evidence. |
| 3 | XMS | A20, allocator, UMB, and INT15. |
| 4 | DPMI | PM, LDT, exception, and memory. |
| 5 | Redirector | Entire family is source-proven unavailable. |
| 6 | Debugger | Entire family is explicitly deferred. |
| 7 | Machine and BIOS | CPU, reset, RAM, ports, PIC, BIOS and mechanical BOP ownership in `bx-core`/`bx-mantle`. |

Every package follows this sequence: source/ABI/failure map; provider
disposition for every member; whole-package implementation plus family
regression; then one native integration trace. Trace output may reprioritize a
package but may not create a singleton endpoint task.

`54:0E` has evidence only for a fixed DX-zero four-byte resume and one native
continuation to `54:04`. `54:04` has adapter-only evidence for its restricted
root response and failure path; S133 was stopped before native integration.
Neither fact closes a COMMAND endpoint family or the COMMAND console/launch
component.

`bx-vdm` owns global BOP ingress, provider selection and host-service
composition. `bx-core` and `bx-mantle` remain selector-blind. `src/opennt`
remains the primary original-contract source.
