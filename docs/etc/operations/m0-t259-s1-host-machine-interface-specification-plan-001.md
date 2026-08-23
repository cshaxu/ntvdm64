# M0 T259 S1 — OpenNT Host and SoftPC/CCPU Interface Specification Plan

## Purpose

Recover one canonical specification for every active/reachable historical
OpenNT host interface and SoftPC/CCPU core-machine interface already
classified by Td S2. This converts the prior per-row audit into a source-first
implementation contract; it is not a substitute implementation of NT4.

## Interface Dispositions

| Disposition | Meaning |
| --- | --- |
| `direct-public-api` | Original owner calls a supported modern public API; only x86/x64 type or encoding adaptation is permitted. |
| `opennt-shaped-facade` | Imported original body retains the historical signature/call order; a bounded bx-vdm shim supplies the unavailable composition seam. |
| `bochs-native-mechanical-owner` | Native Bochs owns CPU/RAM/ROM/firmware/device mechanics; bx-vdm may only transport typed selector-blind facts. |
| `registered-exception` | A documented representation bridge is necessary, such as opaque host handles or the mapping manager. |
| `deferred-source-failure` | NT4-private product composition has no equivalent public interface. Preserve original failure/unavailability and assign the original owner package; do not recreate CCPU/SAS/CSR/BaseSrv. |

## Required Fields Per Interface Family

1. Original source tree/path, symbol/macro/structure and reached call site.
2. Calling convention, x86/x64 width, pointer/HANDLE/guest-memory lifetime.
3. Historical owner and modern owner (`OpenNT`, `bx-vdm`, `bx-mantle`, or
   `bx-core`), with the reason for any ownership boundary.
4. Disposition above, supported public API if applicable, or exact unavailable
   private dependency.
5. Source-order and observable failure/return contract.
6. Existing facade/exception/fixture and receiving owner package.

## S1 Work

1. Reconcile the Td S2 overlay and `BOP-DEPENDENCY-042..045`, `117`, and
   `118` into an authoritative interface-family inventory.
2. Separate OpenNT Win32/NTDLL/RTL public interfaces from NT4-private
   CCPU/SAS/CSR/BaseSrv/NtVdmControl/VDD interfaces.
3. Map SoftPC/CCPU source calls to either a selector-blind Bochs mechanical
   owner, an existing typed bx-vdm/mantle seam, or source-defined deferred
   failure. No BOP selector enters bx-core or bx-mantle.
4. Reconcile all current compatibility headers and registered exceptions to
   the canonical entries; identify obsolete or duplicate facade spellings.
5. Update the live tracker so receiving packages consume interface entries
   rather than rediscovering or redefining them.

## Subsequent T259 S Packages

- **S2:** compose the canonical Win32/NTDLL/RTL facade headers and x86/x64
  ABI rules for directly recoverable OpenNT owner calls.
- **S3:** compose the SoftPC/CCPU-to-Bochs/bx-mantle selector-blind machine
  contract specification and test forbidden-semantic boundaries.
- **S4:** reconcile each active source-shaped shim against the canonical
  spec, fix duplicate/obsolete facades, and run build/fixture conformance.
- **S5:** closure audit, follow-on dependency ordering and one bounded native
  observation only if a completed owner package requires it.

## Exit Rule

T259 closes only when the interface catalog is authoritative for all tracked
active/reachable families, current shims match it, unavailable paths have
explicit source failure/deferred receivers, and follow-on package proposals
refer to these catalog entries rather than defining private substitutes.
