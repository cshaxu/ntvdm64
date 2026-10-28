# S10 superseded NT host source roles

The owner rejected this checkpoint's package-c restoration and required a
fresh build/publication of all ten latest-source components. The [correction](latest-source-release-review.md)
supersedes that operational choice; the source dispositions below remain
unchanged. The current delivery verifier checks the newly selected package,
not historical installed hashes as a reason for another rollback.

Base `de734bc9c`; exactly four unchanged physical predecessor files become
excluded. Their current NT capabilities retain independent partial owners.
This is not exclusion of DOS files, configuration, frontend or termination.

## Per-file original ownership

The [observer](retired-host-image-observer.py) joins every file to the frozen
DOS16 catalog and both WOW32 editions and verifies primary original byte
identity. The original NT `host/src/sources` recipe omits all four; none is a
current compile input or image provider. This is corroborated by the specific
superseding source chains below, not inferred solely from missing symbols.

| File | Physical-file disposition and original replacement | Capability still requiring its own evidence |
| --- | --- | --- |
| nt_term.c | Excluded predecessor. Original nt_reset supplies host_applClose, host_terminate and TerminateVDM. Its host_applClose retains GfxCloseDown, printer close, COM close, mouse-menu detach and temporary-config deletion in the old core order, and additionally stops input/sound/heartbeat. | Current nt_reset has registered lifecycle changes and remains partial. Normal/nested/error/service-loss exits, once-only teardown and original return codes remain S11/S13. The old hardware-only CloseHandle branch is not silently installed in the CPU40 profile. |
| nt_smenu.c | Excluded obsolete DosBox modal settings dialog. Original NT recipe omits it; original and selected resource sets have no DosBox template. Every selected host source is checked for nt_settings_menu/DosDlgProc/bKillFlag references; none uses them. Fullscreen/DGBOX cases are already empty in this old source. | Existing NTSRV/NTCON management, frontend selection and mouse/menu lifecycle remain separate. This dead application-local kill flag is not an original public frontend control contract to resurrect. |
| nt_hfx.c | Excluded unselected HFX host stub file. Its own title identifies stubs, and its file/directory operations are empty or return zero. Original NT chooses DEM file/handle/directory services instead. | DOS file semantics, errors, redirects, handle retirement and VDMREDIR/network behavior remain at selected partial owners. No successful no-op file operation is imported as a recovery. Generic HFX declarations or conditional source presence do not make these stubs the NT filesystem. |
| stf_conf.c | Excluded generic configuration-panel predecessor. It describes SoftPC.rez and the older resource-data API. Original NT instead selects config.c; the selected NT runtime query/set and config_inquire functions are present. Common runtime definitions occur in both files, but the original NT recipe resolves them to config.c. | NT configuration, PIF/environment/profile inputs, device changes, failure and retirement remain partial. This does not claim generic SoftPC.rez persistence or its unselected panel behavior has been restored/tested. |

All four retain 100% of primary original text; executable restoration
percentages are not applicable to these superseded physical files. Their
exclusion does not promote the selected replacements, whose mirror differences
and behavioral limits remain intact. No file or functionality is deleted by
the ledger update.

## Actual build/image evidence and control

[Image observations](retired-host-image-observations.json) bind **15 selected
function bodies** to actual production objects and held candidate code outside
relocation words: three nt_reset entries, three config entries and nine DEM
file/handle/directory entries. Cached/candidate executable sections match.
The original predecessor, original nt_reset and current nt_reset share the
five-operation core cleanup order; that is source correspondence, not proof
of every actual cleanup failure path.

The same observer binds all **73 apfnSVC pointers** to the exact selected
function targets. These include original unimplemented slots and the current
fast-I/O-unavailable provider; binding them does not prove those capabilities
complete or waive their remaining recovery work. The held candidate still
has the previously unexplained SOL Options failure.

The negative control changes only its expected DEM slot 3 from demCreate to
demCreateNew. Actual source/object/image inputs remain unchanged; it exits 1
at DEM_SLOT_EXPECTATION_MISMATCH. The normal observer passes. This establishes
that the pointer witness rejects the wrong selected owner, without modifying
an image or guest. No native application or guest integration test is newly
claimed for this source-only disposition.

## Verification and finite handoff

The [current gate](retired-host-delivery-verification.ps1) verifies these four
row changes and every unchanged shard row, both frozen catalogs and hashes,
source/actual provider correspondence, the negative control, retained receipts,
documentation, CURRENT <=16 KiB and unchanged accepted/candidate packages.

S11/S13 retain current selected reset/cleanup ownership and semantics; S12
retains real DEM file/handle/path and configuration consumption; S13 retains
device/profile/frontend propagation. Original NT-selected behavior remains
the recovery target; generic predecessor panel or Unix-HFX stub installation
would be a new, competing implementation rather than restoration.

The first full gate correctly failed: assets/release still matched accepted
package-c, but all ten installed system32 files matched the held candidate.
The cause/actor and time of that change are unknown; preserved file timestamps
do not establish them. An elevated read-only process inventory found no active
project worker or test. Under standing complete-package deployment authority,
the exact ten installed files were backed up and hash-verified, then all ten
were restored from accepted package-c and hash-verified. No unrelated files
or guest were touched. The exact operation, before/after hashes and terminal
receipt are retained in `retired-host-install-recovery/`; the initial failed
gate is retained in `retired-host-delivery-initial/`. The final gate is rerun
after recovery; a prior pass is not substituted for that failure.

No original source, accepted/candidate package or guest changed, and no mirror
exception was added. Installed bytes were restored to the existing accepted
baseline; this is not a new candidate release, S10/T473 closure or qualification.
