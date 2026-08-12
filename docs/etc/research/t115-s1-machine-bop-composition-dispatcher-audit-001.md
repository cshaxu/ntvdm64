# T115 S1 machine-BOP composition dispatcher audit 001

## Decision

Machine BOPs need a component distinct from both the Bochs backend and the
adapter NTVDM host-service plane. The admitted design direction is a small
`src/bx-ntvdm-machine-composition/` component. It owns only the source-backed
selection and synchronous composition of **machine** BOPs. It is not a new
CPU, BIOS, device, DOS, DEM, or host-capability implementation.

The first evidence-backed member is selector `02h`: OpenNT SoftPC binds it to
`unexpected_int`, which reads and acknowledges the PIC and records its cause
in the BIOS data area before the following guest `IRET`. T110--T114 prove
that this is neither XMS nor Redirector, and that its three platform calls
have finite Bochs-mechanical equivalents. It is the immediate reached
continuous-execution blocker, so it has higher ROI than new host-service
providers or optional devices.

## Complete-category rule

The component must consume a generated/pinned disposition catalogue for the
whole historical `BIOS[]` inventory, rather than accumulating recognizers in
the exception handler. T96 S7's 349 annotated forms retain the following
meaning:

| Category | Runtime owner/disposition |
| --- | --- |
| OpenNT host-service selectors and collisions | Existing adapter host-service plane; machine component never chooses an alternative table row from the three BOP bytes. |
| Bochs-native machine candidate | Machine-composition catalogue may select one separately admitted source-backed operation; otherwise it records/declines it. |
| Historical unavailable | Explicit unavailable/deferred; never fabricated as success. |

The catalogue is a routing and evidence record only. It must not install the
historical `BIOS[]` table, import a SoftPC execution backend, or enable an
unreached device.

## Placement comparison

| Candidate | Result | Reason |
| --- | --- | --- |
| Selector branch in `src/bochs/cpu/exception.cc` | Rejected | It would put selector/SoftPC semantics in Bochs; the existing #UD exception stays mechanical. |
| Adapter host-service plane | Rejected | It would make the adapter synchronously read/write the PIC and BIOS data area, violating both its host-service-only role and the fixed result ABI. |
| Historical SoftPC/CCPU BIOS dispatcher | Rejected | It restores the retired execution backend and brings SAS/CCPU/device composition into the product. |
| Separate machine-composition component | Selected | It preserves Bochs device ownership, leaves the adapter's host-service plane intact, and confines original handler selection to an auditable component. |

## Exact boundary

The future component receives a copied generic #UD/BOP request, a
catalogue-derived `machine` disposition, and an opaque, dynamically scoped
Bochs mechanics context. The context initially exposes exactly three
operations required by the unmodified `unexpected_int` object:

1. one-byte registered port read;
2. one-byte registered port write; and
3. checked ordinary-RAM byte write.

The context is not an adapter API, cannot escape the synchronous call, carries
no raw adapter/OpenNT pointer across the bridge, and must call Bochs's existing
registered port/RAM owners. It is intentionally not a generic device API. The
component returns only `declined`, `handled-and-resume`, or an explicitly
recorded mechanical stop/failure. CPU exception decoding, PIC behavior, memory
behavior and guest `IRET` remain owned by Bochs/guest code.

## Original-source rule

The historical `unexpected_int` object may be used only as a proven isolated
x86 compilation object with compatibility wrappers for `_inb`, `_outb`, and
`_c_sas_store`; `ios.c`, SAS, CCPU, and the historical BIOS dispatcher remain
excluded. This narrowly changes the prior evidence-only rule for one
individually admitted handler island. It is not authorization to link any
other historical SoftPC component. Each later handler needs its own source,
ABI, owner and disposition evidence.

## Required future controls

Before source integration, the exception register must receive a new entry
naming the Bochs mechanics-context seam, exact Bochs files, negative cases,
and removal/review condition. The first implementation packet must prove:

- host-service selectors cannot enter this component;
- selector `02h` cannot use a port/RAM operation outside the three recorded
  primitive forms;
- a declined machine selector leaves the unchanged Bochs #UD path intact; and
- the component neither initializes nor emulates a device.

This audit itself changes no code, exception register, build, link or runtime
behavior.
