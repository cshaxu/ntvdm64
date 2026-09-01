# M0 T345 — Host capability expansion

## Purpose

Recover bounded, package-owned MVDM host capabilities through public modern
Win32 interfaces while retaining the original MVDM caller shapes, ordering and
failure contracts. Capability recovery is source-first: a public API is a
binding option, never the authority for a new MVDM behavior.

## S plan

### S1 — Original capability / caller / failure inventory

Map the selected original drive, filesystem, console, registry and device
capability entrypoints; their direct callers; current source-shaped bindings;
public Win32 candidates; and their original success, permission and failure
results. Select one coherent lowest-dependency owner cohort only after this
map is complete.

### S2 — Selected owner-cohort recovery

Recover the selected original cohort with same-shaped adapter bindings. Keep
all unselected capability families unavailable with their source-defined
result. Do not create a generic capability dispatcher or virtual filesystem.

### S3 — Local contract and paired formal closure

Exercise original-call-shape success, access-denied and unavailable paths,
then verify the selected CPU40 x86 and x64 formal graphs. A native observation
may occur only after the owner cohort is complete and uses the fixed container.

## Non-goals

No host installation or system-file modification, registry mutation merely to
run the product, CSRSS/kernel VDM recreation, new guest/BOP behavior, virtual
drive replacement, multi-session enablement, CPU30, Bochs route or unrelated
machine/device repair.
