# Adapter Startup Snapshot Evidence Export v0

## Need

r17 proves that the three startup ranges are atomically copied, but its
combined digest is deliberately non-invertible. NTIO preload must not rely on
unknown IVT or virtual-interrupt bytes. We therefore need a reproducible
evidence artifact containing the adapter-owned completed snapshot.

## Decision

An optional BYOB profile field declares one safe, relative evidence file name
only when it also declares a startup snapshot trigger. The adapter resolves it
under the already-selected BYOB root and, only after full snapshot completion,
atomically replaces that file with a fixed UTF-8 JSON record.

The record contains:

- schema and fixed record version;
- ordered opaque range IDs, physical ranges and copied bytes as hex;
- copied exception boundary and CPU-state snapshot needed to bind provenance;
- fixed adapter digest;
- no host path, Bochs object/pointer, BIOS/NTIO/DOS/BOP/DEM semantic name or
  inferred field value.

The evidence decoder, not the adapter, maps the selected range offsets to
source-identified IVT/BDA/virtual-interrupt fields. This keeps firmware
interpretation outside the engine and bridge.

## Failure Rules

- unsafe, absolute, separator-containing or empty output names reject profile
  installation;
- absent export declaration leaves normal snapshot behavior unchanged;
- a failure to create/write/replace the evidence file makes snapshot completion
  fail, so Bochs does not resume the intercepted instruction;
- the temporary file is in the same BYOB root and uses a fixed suffix; it is
  deleted on failure when possible;
- no evidence is written before all Bochs copies and adapter transaction
  validation have succeeded.

## Boundary

```text
Bochs generic preflight/copy
  -> adapter validates completed snapshot
  -> adapter fixed-record writer under selected BYOB root
  -> opaque success/failure back to Bochs
```

Bochs has no profile path, evidence path, output bytes or field interpretation.
The runner only selects profile/root; it does not read guest memory. The
adapter does not synthesize firmware, parse BOP, preload NTIO or implement a
host DOS capability.

## Admission Order

1. Extend strict BYOB profile selection and add safe-name negative tests.
   **Completed.**
2. Add adapter-only fixed record serializer plus atomic writer tests.
   **Completed.**
3. Bind writer invocation to the already one-time snapshot completion API.
   **Completed in adapter C11 runtime tests.**
4. Extend the r17 neutral-floppy fixture with the explicit profile export and
   verify the JSON against its log digest. **Completed by r18.**
5. Add a separate evidence decoder that maps only the declared source fields.
   **Completed:** `tools/probe/Read-StartupSnapshotEvidence.ps1` accepts only the
   fixed record/range layout, decodes the four source-ledger IVT locations and
   reports raw BDA/virtual-interrupt values.  It is an offline tool and has no
   engine, adapter-runtime or host-service dependency.
6. Update the startup descriptor; only then consider a generic NTIO preload
   transaction.
