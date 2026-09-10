# M0 T251 — Redirector owner source/ABI/profile map

## Purpose

Establish the complete original OpenNT owner and compatibility inventory for
the `57:00..31` Redirector family before any service is implemented.  The
package is component-first: a trace hit cannot select a leaf implementation.

## Boundary

`VDMREDIR`/`VrDispatch` remain the authority.  This S does not implement a
network, named pipe, mailslot, NetBIOS, scheduler, window, DEM, COMMAND or
machine feature.  It must not create a second COMMAND child broker, expose a
host `HANDLE` in guest RAM, or put Redirector semantics into bx-core or
bx-mantle.

## S sequence

| S | Scope | Exit evidence |
| --- | --- | --- |
| S1 | Source/ABI/failure inventory of every `57:00..31` entry and every directly required non-BOP helper. | Live tracker rows identify the original translation unit, data ABI, lifecycle owner, direct composability, and external dependency for the whole package. |
| S2 | Freeze a bounded Direct CLI capability profile and select the first usable source-recovery rung for each included branch. | Four-rung source-recovery ledger distinguishes original reuse, smallest shim, registered intrusion, and unavailable/deferred behavior. |
| S3 | Implement the complete selected profile by owner group, not trace leaf, with explicit original-shaped rejection for excluded routes. | Original source or named shim bodies, local family regression, and no duplicate COMMAND broker. |
| S4 | Run the declared whole-family regression and one bounded source-built integration observation; reconcile the tracker and close. | Integration evidence validates the completed package without converting a next trace hit into a new leaf task. |

## Completion condition

T251 closes only after every `57:xx` entry has one auditable disposition and
the selected capability profile has complete owner-group regression.  It does
not claim DPMI, WOW16, VDD/debugger, full `cmdExec32`, or ordinary guest EXEC
parent return.
