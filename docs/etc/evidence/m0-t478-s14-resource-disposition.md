# T478 S14 — non-ROM resource applicability

Baseline: b047a1660. Finding 29 is dispositioned without production changes.
The original oversize-file descriptor leak exists in nt_rez.c, but no applicable
current non-ROM caller was found. This supersedes the old audit's tentative
CMOS applicability, not its controlled source-level leak witness.

## Original contract and actual selection

Both OpenNT references' softpc.new/base/system/cmosnt.c explicitly implement
cmos_pickup using static initialization plus POST, and cmos_update as a no-op.
The original NT system/sources selects cmosnt.c. Generic cmos.c file persistence
is not this NT contract. Current cmosnt.c preserves both bodies; the actual
ntvdm.exe.map resolves both symbols to original-softpc-system:cmosnt.obj.
Inherited comments in terminat.c/cmos.h are not evidence of active persistence.

A current C/H source scan finds one resource reader call in system/rom.c:364,
passing ROMS_REZ_ID. Current nt_rez.c routes that ID to the existing embedded
ROM provider before any file open. Scanning the COFF symbol tables of all 560
objects under the current worker/obj directory finds only rom.obj referring to
_host_read_resource; nt_rez.obj defines the reader and writer. There are no
external writer references in that bounded scan. This is current-build evidence,
not a universal claim about arbitrary dynamic callers or future configurations.

## Disposition and verification

[Receipt](../../../artifacts/mvdm-audit/shard-3/resource-s14-results.json).
Keep the inherited unselected non-ROM oversize-file limitation. Reopen only if
an applicable selected caller is added or demonstrated. Do not add CMOS file
persistence, new resource support, missing-ROM recovery, or change guest/media.
The existing embedded provider is necessary host integration, not duplicated
CMOS implementation to replace with the generic original branch.

All ten worktree release hashes match package
3e2e62b9-6d16-442a-9286-e169566a75e7. S13's 16/16 Full result applies to those
unchanged bytes; no new runtime test, product build or publication is claimed.
No O:/winnt write or push. S15/finding 30 follows separately.
