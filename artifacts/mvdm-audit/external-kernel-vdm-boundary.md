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
