# T477 S4 VM frontend management projection

## Approved UI

Each frontend is a separate top-level group with its own worker chain.
Order: NTCON CONSOLE groups, NTVDM VM groups, WIN16/WOW groups, native
WIN32/WIN64 observation groups, then workers without an association.
Preserve the existing stable start-time order within each class. Nested VM
frontends start new groups; do not nest them visually under the hosting worker.

CONSOLE PID is NTCON PID. VM PID is hostPID-VMID; VM TASK stays empty.
Do not expose payload/incarnation or invent task descriptions. Existing TASK,
STACK, selection, scrolling and management behavior otherwise remain.
VM group rows are read-only. Worker actions remain their existing actions.

## Data and lifetime

NTSRV remains the snapshot authority. Select VM rows with service epoch,
owner generation and broker instance identity, never the displayed PID-VMID.
Append a copied 64-bit VM display identifier to the existing snapshot row;
advance the RPC protocol major for the layout change and rebuild all peers.
No separate transport or query is needed.

The existing reference-counted frontend association retains immutable display
identity (owner PID/generation and VM ID/type) from registration. This is a
controlled projection of immutable identity, not a new relationship registry.
It lets surviving workers retain a correctly identified MISSING group after
endpoint removal. Live state comes from the live instance and worker records.
No lost host is reopened by PID. Payload/incarnation remain internal only.

Use retained authenticated instance associations for grouping; multiple VMs
in one process must not merge. Reuse the native block sorting mechanism and
insert VM groups as another class. No general tree engine or recursive walk
is needed. Missing frontends stay in their own category; truly unassociated
workers go last. Reject VM close requests even if a client forges an action.

## Acceptance

Test two VM groups sharing a PID, full-width VM IDs, nested separate groups,
ID reuse, read-only selection, missing endpoints and retained siblings.
Exercise the real service projection, actual snapshot RPC and NTMON renderer.
Preserve NTCON close, worker trace and WOW task behavior. Cached x86/x64
builds, Full regression, coherent ten-image publication and reviewed commit
and push remain mandatory. Guest driver and guest multitasking are excluded.
