# M0 T277 S3 — first machine-facing family audit

The first 38 source-form work items are grouped by original call shape, not by
trace order. `malloc` stays an ordinary CRT binding. XMS A20 and overwrite
calls retain their original XMS control flow but require `adapter-softpc` to
use the session `guest_memory` lease and `adapter-bochs` mechanics; raw host
addresses are forbidden. DPMI register/segment/flag access is one atomic CPU
frame family, not a collection of register shims. `host_simulate` is one
bounded Bochs run request; IRET hooks belong to `adapter-vdm-monitor`; and the
real-mode switch is a typed `adapter-softpc -> adapter-bochs` request.

Every item remains implementation-disabled. The generated ledger is the exact
per-work-item proof; it records original location, owner, retained shape and
failure disposition.

## T276 file-treatment corrections found by S3

The former T276 final-disposition ledger remains historical evidence.  It
classified `xms.486/xmsa20.c`, `xms.486/xmsblock.c`, and
`xms.486/xmsumb.c` as `adapter-backed` with a `binding-only` change class.
S3's source-form review refines that future treatment: all three remain
`adapter-backed`, but require an `overlay-required` change class.

- `xmsblock.c` casts linear XMS addresses through `DWORD`/`PBYTE`; a native
  x64 pointer cannot stand in for an epoch-scoped `guest_memory` lease.
- `xmsumb.c` converts a host `PVOID` UMB allocation to `DWORD`; this cannot
  describe a modern native address without truncation.
- `xmsa20.c` stores a `GetVDMAddr` result in `pHimemA20State` across calls;
  the session mapping contract prohibits retaining such a lease.

The future overlay retains the original XMS allocation, list, `B0/B1/B2`, and
A20 control flow.  It replaces only those raw-pointer representations with
numerical guest address/span records and scoped mapping-manager leases.  No
XMS allocator, UMB policy, or DOS semantic is admitted into an adapter.  The
generated S3 ledger carries the correction per reached call; a separate
file-level correction ledger makes the reconciliation explicit for later T277
waves.

## Reproducibility

Regenerate the family ledger with:

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass -File tools/governance/Set-T277S3MachineFamilyDisposition.ps1 -RepositoryRoot O:\repos.hobby\ntvdm64
```

Verify the exact 38-work-item coverage, seven-family grouping, disabled
implementation state, and the three explicit XMS corrections with:

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass -File tools/governance/Verify-T277S3MachineFamilyDisposition.ps1 -RepositoryRoot O:\repos.hobby\ntvdm64
```
