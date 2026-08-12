# Adapter Observation Transaction ABI v1

## Purpose

This ABI is the one future bridge format for observing a bounded ordinary-RAM
range at a controlled generic `#UD` stop. It exists because the first
guest-startup descriptor needs to evaluate a selected Bochs firmware profile
without exposing a Bochs memory pointer or turning the adapter into a firmware
implementation.

## Record

`bx_ntvdm_observation_transaction_v1` is a fixed 144-byte C11 record:

| Field | Meaning |
| --- | --- |
| ABI header | magic, version, exact structure size, zero flags |
| `boundary` | copied generic exception event, required to be CPU vector `6` (`#UD`) |
| `cpu_before` | copied and validated x86 state at that same boundary |
| `guest_read` | one non-empty physical range entirely within the declared aperture |

The output bytes are explicitly absent from the record. They remain in an
adapter-owned same-process buffer whose lifetime is bounded by the eventual
pending observation operation. No guest, host or Bochs pointer is versioned or
retained.

## Preflight Rules

The ABI rejects a null record, bad ABI fields, non-zero flags, an invalid
exception/state snapshot, a vector other than `#UD`, zero length, an
out-of-aperture range, or output capacity other than the requested byte count.
It does not decide whether a particular physical range is ordinary RAM; that
final check remains solely in Bochs `BX-MEM-010` at commit time.

## Lifecycle (Not Yet Wired)

```text
profile-owned observer request
  -> adapter validates and queues one transaction for a future #UD boundary
  -> Bochs bridge verifies the same copied boundary and calls BX-MEM-010
  -> bytes enter adapter-owned output buffer
  -> adapter consumes or discards it before resume, reset, stop, or teardown
```

No current adapter runtime function queues, takes or completes this record.
No Bochs callback knows this ABI. This intentional non-connection means the
new record cannot currently read NTIO, IVT, BDA, or arbitrary guest memory.

Before wiring, the implementation must define one pending-operation owner;
same-boundary equality checks; output allocation/free rules; single-consume
semantics; reset/stop/fault cancellation; and a fixture whose requested ranges
come from a declared profile rather than Bochs source or ambient CLI input.

## Verification

`bx-ntvdm-observation-transaction-abi-test` is a standalone default-disabled
C11 target. It proves positive layout/range preflight and rejects zero length,
overflow, non-zero flags and a non-`#UD` boundary. It was built and run in
`artifacts/build/current/adapter-cli-r1` with MinGW GCC 16.1.0.

This ABI test is not runtime evidence and does not authorize an additional
Bochs patch beyond registered `BX-MEM-010`.
