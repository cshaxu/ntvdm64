# Td S2 P3 — Shared Interface-Conformance Audit

## Question

Before a service-family mirror claims source-first conformance, which shared
interfaces already have a valid OpenNT-shaped composition, which are approved
generic exceptions, and which are migration debt?

## Inputs

- `docs/etc/bop-list.md`, including the canonical BOP/dependency inventory.
- `src/bx-vdm/bop/opennt/{dem,xms}` and `src/bx-vdm/bop/shim` call sites for
  `GetVDMAddr` and `Sim32*`.
- OpenNT `softpc.new/host/src/nt_bop.c` and
  `softpc.new/base/keymouse/keybd_io.c`.
- `adapter-external-intrusion-exceptions.md`, especially `BX-VDM-001`,
  `BX-ABI-094` and `BX-MANTLE-095..097`.
- Td guest-pointer mapping governance.

## Procedure

1. Compared every tracked shared interface with its historical source call
   shape and its active bx-vdm/mantle form.
2. Required an existing exception record for every retained project-defined
   ABI; refused to infer approval from a same-spelled helper.
3. Checked whether guest-pointer calls were represented as an independent
   dependency.  They were not, although they are reachable from more than one
   owner family.
4. Added `BOP-DEPENDENCY-118` and recorded the five shared dispositions in
   the tracker overlay.

## Observations

- `nt_bop.c` is an historical host product dispatcher, not a reusable
  selector-neutral facade.  Its CCPU/SAS/CSR/DLL dependencies make it a
  deferred evidence source.
- The active `UpdateKbdInt15` fragment preserves the original reached
  operation through a named bounded seam.  It is a facade, not permission to
  import the keyboard product shell.
- The shared host-token manager has the explicit `BX-VDM-001` approval and
  preserves the original two-word guest layout without exposing `HANDLE`.
- The DPMI substrate records remain generic machine mechanics under their
  registered exceptions.  They cannot be advertised as DPMI behavior.
- Current DEM/COMMAND/XMS mirrors still use family-local pointer helpers.
  The required session/epoch mapping manager is a facade migration target,
  not an already-completed implementation.

## Interpretation and confidence

Confidence is high for the five dispositions because each names either its
exact OpenNT call site or its registered exception.  The inventory increases
from 320 to 321 rows solely to make the otherwise-untracked pointer interface
auditable.  This changes neither a BOP completion claim nor T257 S5's frozen
state.

## Follow-up

Td S2 P4/P5/P6 must classify their family rows against this shared baseline.
Any owner package that changes a pointer-using source route must first migrate
that route to the bounded mapping-manager facade and add direct/bounce lease
tests.  Td S2 P7 reconciles all 321 rows and decides whether T257 S5 may be
re-admitted.
