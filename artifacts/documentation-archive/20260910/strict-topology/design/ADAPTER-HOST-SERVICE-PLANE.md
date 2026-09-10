# Adapter NTVDM Host-Service Plane

## Purpose

The historical NTVDM host side contains BOP entry points, DEM dispatch, host
file/console services, and the CCPU/SAS state model. The guest side contains
NTIO, NTDOS, DOS utilities, and later WOW payloads. In the Bochs route, the
guest programs remain guest code; the unavailable historical *host
composition* is replaced as one coherent adapter-owned service plane.

This is not a DOS filesystem and not a Bochs extension. It is the host-facing
counterpart that guest DOS code historically calls through BOP services.

## Ownership

| Component | Owns |
| --- | --- |
| Bochs | CPU execution, physical RAM, firmware, devices, reset, exception mechanics. It never recognizes a service or host path. |
| Adapter mechanics | Typed CPU/memory transactions, bounded instruction observation, event dispatch, stop/result application. |
| Adapter NTVDM host-service plane | Source-derived selector/service catalogue, host compatibility services, resource admission, and the historical service-visible register/memory result contract. |
| Guest OpenNT/DOS | NTIO/NTDOS execution, DOS kernel behavior, DOS filesystem namespace and requests, utilities and WOW payloads. |
| CLI runner | Session options, BYOB resource root, diagnostics, lifecycle. |

The runner-to-engine installation contract is recorded in
`etc/research/runner-adapter-install-contract.md`. It passes the same accepted
resource identity to the adapter runtime as a child-only process environment;
it does not pass payload bytes or Bochs objects.

## One Plane, Not Per-Service Hacks

The plane is designed and tested as a single subsystem before runtime work:

1. **Event admission:** exact observed boundary, selector and service
   catalogue; unknown inputs fail closed.
2. **Session state:** one typed guest register/memory transaction model; no
   raw Bochs or host pointers leave their owner.
3. **Resource model:** immutable CLI-admitted resources and declared host
   capabilities, never ambient NT4 paths or TEB state.
4. **Service catalogue:** all services needed by the declared profile are
   named up front, each with source-derived ABI/failure evidence. A service is
   enabled only through the shared dispatcher, not a new special BOP hook.
5. **Result/stop model:** state commits only after fully preflighted memory
   actions; failure is an explicit bounded result.

The first vertical case, NTDOS loading, validates the common transaction and
resource machinery. It is not an architectural singleton and does not license
a direct `demLoadDos` substitute outside the plane.

## Compatibility Rule

Where original host-source composition is linkable without CCPU/SAS recovery,
the plane calls that original owner. Where it is not linkable, the plane may
rehost only the specific source-derived contract, with its ABI, order and
failure behavior recorded. It must never claim that the whole historical DEM
or BIOS composition has been recovered.

The plane may not implement CPU, paging, firmware, device emulation, DOS
kernel algorithms, a guest filesystem, or WOW application behavior. Requests
for any of those are routed to Bochs or guest code according to ownership.

## Initial Profile Envelope

The declared first profile is `NTIO -> NTDOS -> COMMAND /C` with an explicit
BYOB resource root. Its service catalogue and host capability matrix must be
complete before a runtime service is admitted. Later profiles extend the same
plane through a reviewed catalogue update, not a new adapter architecture.
