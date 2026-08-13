# T198 S8 Boot Namespace Provider Closure Map 001

## Result

The first provider slice should be the existing finite read-only boot
namespace, as one whole family rather than a service patch.  Its owner object
already combines COMMAND boot-file production, DEM normal file lifecycle and
pathname-search state.  It must be recomposed without `adapter_runtime`.

## Closure

| Layer | Required owned role | Explicit exclusion |
| --- | --- | --- |
| Global plane | generic record conversion, ingress, registry, DEM/COMMAND plane classification | byte recognizers outside the tables |
| Provider | `boot_namespace_provider_v1`, readonly namespace, DEM file and search contracts, COMMAND boot-file contracts | ambient host files, host paths and handles |
| Session | one copied pending-action state for read, multi-write and bulk-result transactions | environment discovery, process-global runtime |
| Mantle | preflighted ordinary-RAM read/write and typed resume/stop only | selector or provider knowledge |
| Core | copied event/outcome only | transaction or BOP meaning |

## Rejected Legacy Path

`bx_ntvdm_adapter_runtime_v1/v2/v3/v4` owns BYOB environment installation,
profile/session lifecycle, diagnostics and multiple unrelated provider
families.  It cannot be linked as the S8 dispatcher.  Its useful local code
is source evidence only: the new composition must receive an immutable
already-admitted boot namespace from CLI/host capability and carry one bounded
pending transaction through the fixed bridge.

## Required Transaction Expansion

The current generic outcome transports only pass/resume/stop and a narrow
register/CF delta.  The chosen family needs three typed mantle-owned mechanical
operations: checked guest read for DEM open/search, all-or-nothing multi-write
for COMMAND boot files, and checked bulk write for DEM read.  Provider code
decides only copied address/length records; mantle performs the memory access.
This is a new fixed outcome/action ABI and therefore must be designed before
provider code becomes reachable.

## Follow-up

S8 first defines the generic pending-action ABI and its core/mantle mechanics
without any boot namespace provider linked.  A later S will compose the whole
provider and a CLI-provided immutable namespace against that ABI.
