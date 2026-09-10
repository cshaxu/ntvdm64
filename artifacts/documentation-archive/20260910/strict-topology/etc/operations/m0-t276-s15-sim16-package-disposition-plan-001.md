# M0 T276 S15: W3 `sim16` package disposition plan

## Objective

Audit all eight original `sim16` paths as one historical 16-bit WOW simulator
package. Establish its build/profile role, serial/packet/guest-memory and
interrupt dependencies, and explicit adapter/guest/profile disposition before
enabling or translating a single 16-bit simulator routine.

## Constraint

`sim16` is not automatically a DOS guest image merely because it is 16-bit;
nor may its source be inserted into the modern host runtime. Its original DLL,
WOW library, serial-port and interrupt assumptions must first yield one
source-proven owner boundary or profile exclusion.

## Non-goals

No source move/edit, 16-bit build, WOW enablement, serial device bridge,
adapter implementation, Bochs device change, formal build edge or guest run.

## Acceptance

Every selected path has an exact non-enabled disposition; every serial,
packet, guest-memory, interrupt and WOW dependency has a named existing owner
or explicit profile exclusion.
