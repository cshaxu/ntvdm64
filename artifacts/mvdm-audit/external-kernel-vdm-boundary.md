# External NT kernel VDM: execution versus directory reset

The six exact original OpenNT source rows and hashes are in
`external-kernel-vdm-boundary.csv`. Their selected execution, initialization,
interrupt/APC, privileged transition and kernel VDM service mechanisms are
outside the approved CCPU40 product profile; this is a provider boundary, not
a blanket exclusion of guest-visible DOS functionality.

The shared `base/ntos/vdm/vdm.c` has a second, independently relevant
`VdmQueryDirectoryFile` service. It restarts a directory query using a previous
`FileIndex` and `FileName`, validates a user buffer and returns native status.
The original `demsrch.c::FileFindReset` requests that service when a directory
claims reset support and the saved name is not `.` or `..`. Current
`NtVdmControl` dispatches this request only through a session-bound typed
callback; a source search finds binding calls in the test fixture, not a
production registration. If unbound, it returns `STATUS_NOT_IMPLEMENTED`, and
the retained original caller takes its slow filename comparison path.

This source observation does not itself prove the selected product reaches
the fast branch or that the slow path fails. The historical
`t230_s5_demsrch_fcb_direct_import_fixture.c` explicitly models unavailable
VdmQueryDir; `vdm_control_fixture.c` tests callback dispatch, not a production
directory provider. DIV-362 adds an independently registered restart hint for
a missing anchor: inspect that contract before assuming equivalence when an
entry is deleted, renamed, duplicated by case, or directory order changes.
Determine actual production branch selection and verify real enumeration
with original index/name and fallback under reset, teardown and failure.
The OS kernel service is not a product import proposal and its absence does
not justify a success stub or a second filesystem authority.

## Current-source provider fixture

The existing `verify-dem-directory-reset.ps1` was run against current source
and cached x86 fixture libraries at `build/t473-s10/dem-reset-s10-r3`. It
completed 115 assertions with zero failures, including an unbound monitor
query, directory close/reopen, unrelated rename/create, deleted anchor and
successor, hint mismatch, FCB formatting and resource counts. The map binds
`FileFindReset` to the compiled test unit containing current `demsrch.c`, and
`NtVdmControl` to `monitor-bindings:vdm_control.obj`. Source SHA-256 was
`8AB053F632861DC37A748EDAB449D4E8D411145D27BD10D326EE7815E61F289D`;
test executable SHA-256 was
`94F96BA82B432DC9815DFD31AFFB63A34C455E46C6DDEBBE5FD3029FB151655A`.
The fixture uses selected source and real NT directory APIs with host seam
libraries and `/force:multiple`; it is neither the released executable nor
a guest INT 21 test, and does not establish indexed kernel service equivalence,
all filesystem ordering, concurrency or actual eight-minute reclamation.

Two prior attempts did not pass: r1 stopped before linking because its
fixture-library regex did not accept CRLF; r2 ran but terminated with
`STATUS_INVALID_HANDLE` during the closed-handle check. The script now accepts
either line ending and the check handles that specific invalid-handle
exception as well as the ordinary error return, while propagating other
exceptions. Neither failed attempt is product failure evidence. The runner
creates and removes its owned test directory; no guest or product image was
changed.
