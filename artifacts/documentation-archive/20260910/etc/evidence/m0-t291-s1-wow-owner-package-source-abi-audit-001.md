# M0 T291 S1 — WOW owner-package source/ABI audit

## Question

What is the complete, current source-first recovery boundary for the original
WOW32 and FAX host owner packages before any WOW provider or guest route is
enabled?

## Inputs

- Current `src/mvdm-host/{wow32,fax}` mirror files.
- The prior selected-file, active-subdomain and interface-family provenance
  ledgers, used only as audit inputs.
- Current `session` mapping-manager rules and outgoing adapter roots.
- The selected OpenNT-4.5 source tree for mirror comparison.

## Procedure

The reproducible exporter below validates that the live mirror and the two
input file ledgers have exactly the same 222 paths; rejects duplicate paths;
requires the 77 active WOW32 subdomain rows and 15 total interface families;
then emits current-component file and interface ledgers with SHA-256 values.

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools\governance\Export-T291WowOwnerPackageAudit.ps1 -RepositoryRoot O:\repos.hobby\ntvdm64
```

The audit also compared each live `wow32`/`fax` file against the selected
OpenNT-4.5 path. It found the single documented `wow32/wkfileio.c` divergence
and no other changed or missing selected path.

## Observations

- [File/subdomain ledger](../operations/m0-t291-s1-wow-file-subdomain-disposition-ledger.tsv):
  222 rows; 77 active WOW32 source bodies; 61 overlay-required and 16
  adapter-backed future subdomain bodies; 105 binding-only declaration/table
  forms; 32 not-host-runtime source/resource forms; and four FAX UI bodies.
- [Interface-family ledger](../operations/m0-t291-s1-wow-interface-family-ledger.tsv):
  nine WOW32 and six FAX original interface families. Every guest address is
  assigned only to `session.guest_memory`; persisted native identity is
  assigned only to `session.host_resource`; shared/cross-process records use
  copied broker IDs.
- The current named boundaries are
  `adapter-mvdm-host-out/{softpc,monitor,win32,wow}`, session and broker.
  The audit did not create a new generic compatibility layer.

## Interpretation

The smallest next implementation group is the original WOW32
thunk/frame/structure subdomain, but its code must not be admitted until its
complete CallBack16/VDMFRAME and bounded memory interface closure is designed.
The recovery order is in the companion [sequence record](../operations/m0-t291-s1-wow-recovery-sequence-001.md).

No S1 result is a WOWEXEC, GUI, Win16 application, BOP or guest-load claim.
Private USER/GDI, CSRSS, GDI-driver and monitor product-shell dependencies
remain explicit source facts, not invitations to rewrite them.
