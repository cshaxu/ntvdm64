# M0 T306 S1 P8 — Physical external-frontier adoption disposition closure

## Question

Can every physical direct external call receive a safe T306 adoption result
without treating unresolved source identity as a reason to import an arbitrary
same-named OpenNT function or invent a generic adapter?

## Inputs

- The 7,367 physical `outside-zero-ledger1` rows in `one-ledger1.tsv`.
- Their P18 definition-resolution, P19 declaration-binding, P20 signature
  gate records, the generated T306 seed ledger, and P3--P7 cohort decisions.

## Procedure

The new deterministic exporter preserves each `one_ledger1_identity` and
joins only by that physical identity. Signature-confirmed rows receive their
already-audited cohort disposition. A row with a source candidate but no
caller-reachable declaration receives an owner-local declaration hold; a row
whose signature/condition remains unresolved receives an owner-local
source-form hold; and a row with no original C/C++ body receives an owner-local
source-absent/macro/import hold. No spelling-only promotion occurs.

## Result

The generated [external-boundary disposition ledger]
(../operations/m0-t306-s1-external-boundary-disposition-ledger.tsv) has all
7,367 physical direct external calls:

- 251 signature-confirmed seed rows, including the P5 correction that returns
  two false OLE `ReadByte` candidates to MVDM closure;
- 59 signature/condition-form holds;
- 3,952 source-candidate rows without a reachable declaration binding; and
- 3,105 rows with no original C/C++ definition candidate.

Each hold is a deliberate **non-adoption** result, not an unowned dependency:
the original caller package owns its eventual exact binding review. This keeps
T306 finite while preserving all physical evidence for later implementation.

## Decision

T306 selects exactly one future new non-MVDM mirror slice,
`opennt-host/netapi/netlib/ntstatus.c`; retains the current BaseVDM local seam;
and names the existing SoftPC, Win32, Redirector, monitor and WOW adapter
families/hard boundaries. It selects no full BaseClient, Kernel32, Workstation,
RPC, OLEAUT, NTDLL CSR or historical SoftPC product shell.

Later owner packages must consume the ledger at their immediate boundary; they
may turn a hold into a source-shaped binding only with file/declaration/ABI
evidence. They must not restart global discovery or generalize a hold into a
new compatibility layer.
