# T202 S4: admitted root-relative file open

## Question

Can the DEM provider access a real host file beneath an admitted drive without
building an ambient absolute path, exposing a root handle to guest state, or
depending on directory-wide 8.3 enumeration?

## Inputs

- T202 S2's adapter-private admitted root-handle namespace.
- T202 S3's direct-host file disposition and no-host-handle rule.
- New `bx_ntvdm_host_namespace_v1_open_file` capability and focused fixture.

## Procedure

1. Added a root-relative `NtCreateFile` helper which accepts only a bounded
   DOS-relative 8.3 component path and an already-admitted drive root.
2. Fixed its open disposition mapping to the ordinary `CREATE_*`/`OPEN_*`
   domain, forced non-directory synchronous opens, and rejects final reparse
   objects after opening.
3. Built and ran `Invoke-T202S4HostNamespaceOpenProbe.ps1` with MSVC x64
   `/MT`; it writes a temporary real-host file, reopens it through the root
   relative name and checks its byte, then checks that `..\\X.TXT` is rejected.

## Observations

`artifacts/build/t202-s4-root-relative-open-r7` compiled and exited zero.
The helper reached the actual admitted C: root and read the expected byte;
the parent-directory escape was rejected before a host handle was returned.

The earlier T202 S2 directory-enumeration fixture currently cannot walk from
C: to the process temporary directory: its conservative per-directory 8.3
projection detects a collision/rejection before the target component.  This
is an existing limitation of directory enumeration in this host environment,
not a failed root-handle installation and not a reason to synthesize C:.
The new direct file-open path avoids using that enumeration as an I/O lookup
mechanism.  Search still retains its separately stated 8.3 collision policy
and must not borrow this direct-open behavior without its own source review.

Revision r3 adds `bx_ntvdm_host_namespace_v1_open_file_ex`. It retains the
same root-relative, adapter-private boundary but maps a rejected native open
back to a Win32 error via the available `RtlNtStatusToDosError` API. The new
output is a copied scalar for later `demClientError`-shaped results; it does
not expose an `NTSTATUS`, root handle, or host path. The r3 focused probe
verifies both `ERROR_SUCCESS` for the actual temporary-file open and
`ERROR_INVALID_PARAMETER` for preflight rejection of `..`; r12 also confirms
the existing complete-provider fixture still passes.

Revision r4 extends the same file-only root-relative boundary with copied
attribute query/set and deletion operations. The focused temporary-file
fixture proves `GetFileInformationByHandle`, `SetFileInformationByHandle`
(`FileBasicInfo`), and `SetFileInformationByHandle` (`FileDispositionInfo`)
operate only after the admitted-root/open preflight and retain a Win32 error
result. The fixture changes attributes and deletes only its self-created
temporary file; r13 confirms the existing DEM provider closure still passes.
These are ordinary available user-mode APIs on the current MSVC/x64 target;
no missing Win32 API was encountered.

Revision r8 adds root-relative directory create/remove and file rename. The
fixture creates and removes a self-created directory, then creates, renames,
queries and deletes self-created files. `SetFileInformationByHandle` rejected
the required root-relative rename payload, so rename uses the available
user-mode `ntdll!NtSetInformationFile` with the stable local
`FileRenameInformation` class value and an already-private destination root
handle. It preserves the admitted-root boundary instead of falling back to
an ambient absolute path. Cross-drive rename is explicitly rejected as
`ERROR_NOT_SAME_DEVICE`. r8 and the r14 DEM provider regression pass under
MSVC x64 `/MT`; no missing host API remains for this primitive set.

## Interpretation and confidence

The adapter now has a contained direct-host primitive required by the later
DEM provider.  Its returned `HANDLE` is adapter-local and must immediately be
adopted by the S4 opaque file session; it is not a guest ABI.  The primitive
does not implement an OpenNT BOP, an overlay backend, a pathname parser for
guest memory, or any Redirector behavior.

## Follow-up

Use the root-relative primitive only behind the S4 whole-provider pathname
decode and file-token layer.  Retain the search enumeration limitation as an
explicit search-package test case; do not silently change it into long-name
or host-path behavior.

Revision r21 corrects a distinct directory-validation defect found while
testing `demCheckPath`: the old `directory_exists` helper walked each
directory by bounded enumeration, so a populated real root could fail before
the requested directory was examined.  It now uses the same root-relative
`NtCreateFile(FILE_DIRECTORY_FILE)` capability as directory mutation, with
`FILE_READ_ATTRIBUTES` and no create disposition.  The result remains a
boolean capability check with no host path or handle exposure and no NUL-file
side effect.  The r21 whole-provider fixture passes on the actual admitted
drive.
