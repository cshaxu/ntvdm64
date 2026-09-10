# M0 T337 S1 — Packaged system-file-root / DOS drive-root contract audit

## Result

The selected product already has one immutable, executable-relative host media
root.  It is not a missing guest loader:

```text
app executable path
  -> session.dos_media_root = <exe-dir>\dos
  -> mvdm_softpc_dos_find_file / mvdm_softpc_dos_copy_root
  -> original InitialiseDosEmulation (NTIO.SYS)
  -> original demInitialize / demLoadDos (NTDOS.SYS)
  -> original nt_pif/cmdconf configuration selection
```

The original later guest lookup is a distinct chain:

```text
NTIO SYSINIT
  -> SVC_DEMGETBOOTDRIVE
  -> original demGetBootDrive
  -> guest boot-drive-rooted \COMMAND.COM request
  -> original DEM demOpen
  -> CreateFileOem(guest-provided canonical path)
```

`demGetBootDrive` reads the historical NT setup `BootDir` registry value and
otherwise returns `C:`.  It has no reference to `session.dos_media_root`.
`demOpen` retains original slash canonicalization and passes the resulting
caller path directly to `CreateFileOem`; it has no selected system-file-root
translation.  Therefore loading a file from `dos/` earlier does not prove
that later guest `\COMMAND.COM` resolves to that directory.

## Source findings

- Original `sysinit1.asm` stores the `SVC_DEMGETBOOTDRIVE` `AL` result as the
  guest boot drive.  Its startup command path is `\COMMAND.COM`; T336 named
  the corresponding `comerr -> SVC_DEMEXITVDM` terminal branch.
- Original `demgset.c::demGetBootDrive` preserves the NT4 registry/default
  behavior.  It is not a media loader and must not be silently changed into a
  full virtual-drive policy.
- Original `demfile.c::demOpen` owns DOS filename canonicalization, sharing,
  named-pipe decisions, error mapping and handle publication.  Replacing it
  with an independent filesystem layer would violate its original ownership.
- The existing divergences that select executable-relative media roots are
  already narrowly confined to initial system-media/configuration locations:
  `MVDM-HOST-DIV-038`, `-154`, and `-157`.  They do not compose a later guest
  root path.

## Recovery-rung decision

1. **Direct original composition:** unavailable for the packaged layout.  The
   original later path uses the installed NT boot-drive contract, while this
   product distributes immutable media beside the executable.
2. **Same-shaped binding:** admissible candidate.  A narrow host-side binding
   must be evaluated at the original boot-drive/system-file boundary while
   retaining `demOpen` as the canonicalization/open/error owner.
3. **Mirror intrusion:** not selected.  No original source body needs a
   speculative edit in S1.
4. **New filesystem/virtual-C behavior:** rejected.  It would exceed the
   established system-media requirement and change unrelated DOS paths.

The attached [ledger](../operations/m0-t337-s1-packaged-system-file-root-contract-ledger.tsv)
records the individual source and binding rows.

## S2 decision boundary

S1 proves a real missing composition edge, but does not choose its behavior:
the remaining decision is whether the original guest system-file lookup is
bound through an existing original setup/current-directory contract, or needs
a narrowly scoped session/app host-path binding.  The latter must apply only
to the selected immutable system-media identity, preserve all ordinary host
drive paths and original DEM error ordering, and must not impersonate a whole
guest `C:` drive.

That policy choice changes the host namespace seen by a reached original
guest request.  Per the active packet stop condition, S2 requires owner
approval after this audit rather than being inferred from the previous
`NTIO.SYS` root binding.

## Procedure and verification

Read-only review used:

```text
rg -n -C 4 "mvdm_softpc_dos_copy_root|mvdm_softpc_dos_find_file|session_dos_media_root|dos_media_root" src
rg -n -C 4 "BOOTDRIVE_PATH|BOOTDRIVE_VALUE|SVC_DEMGETBOOTDRIVE|demOpen" src/mvdm-host
read original app/session, nt_msscs, nt_pif, dem, demmisc, demgset, demfile,
and guest sysinit source bodies
```

The review was static only.  It does not claim that a changed runtime reaches
COMMAND, nor does it select a root-projection implementation.
