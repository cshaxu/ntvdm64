# Owner-requested Win95 program-loader paging research

Scope: standalone research requested on 2026-10-25, independent of S3 Clipboard
acceptance. Original media stays read-only; no original guest executable,
NTIO/NTDOS or host implementation is changed. No Computer Use or input injection.

## Reproducer and cause

The installed SYSTEM.INI disabled paging in `[386Enh]` with `Paging=off`.
Win95 reports about 9 MiB free physical memory, yet an ordinary
`VirtualAlloc(MEM_RESERVE | MEM_COMMIT, 1 MiB)` fails with error 8. Six-size
controls separate address reservation from commitment: 4 KiB through 2 MiB
reservations and initial single-page commitments succeed; full 1 MiB and
2 MiB commitments fail. Smaller tested commitments through 512 KiB succeed.
Reading files succeeds, but mapping OLE32 and loading it fail with error 8.
A small independently authored self-child launches/exits, whereas original
Notepad creation fails with error 8.

The same probe and worker with only `Paging=On` pass all six full commitments,
OLE32 mapping/loading and Notepad creation. Normal Win95 paging creates its
own WIN386.SWP. This establishes the configuration cause for these error-8
failures; it does not establish a host allocator defect or a general Win95
memory-manager algorithm defect.

Inputs: worker SHA-256
`9E883A9D5289EC73A5E442261E901B5C270871AC1D9152AA5AFD0ACBE341E59E`;
same on/off probe SHA-256
`3C91A12259263E963798A0168886E73DA82F59C40EEBBBE7CBD3B5677054C498`.
Read-only original KERNEL32 research input: 414720 bytes, SHA-256
`2A074C9B85F5B1BA3BE7D134DA93D8930BFF9ECB847176C25DF5E94DF5D18AB3`.
Its VirtualAlloc wrapper at RVA DD6D separates reservation and commitment and
maps failed provider results to error 8. Static disassembly corroborates that
the public error alone cannot distinguish the two operations; runtime matrix
evidence supplies that distinction. No disassembled implementation is imported.

## Verification and installation

Independent producer: `tests/observation/build-win95-loader-probe.ps1` and
`win95_loader_probe.c`. Runtime entry: `verify-win95-loader.ps1`, with
`-AfterMigration -Paging Off` versus `-AfterMigration -Paging On`.
`tests/component-integration/win95_paging_result_test.py --off <off-run>
--on <on-run> --output <result>` asserts same input hashes, all six sizes,
OLE32, Notepad and the self-child control.

The separate `verify-win95-menu.ps1 -AfterMigration -Paging On -AppsOnly`
verifies actual windows and normal closure. Notepad and Paint pass. WordPad
creation succeeds without error 8, but its main window/lifecycle does not
complete within the diagnostic deadline; it remains a non-pass of unknown
cause. Do not equate this with a resolved WordPad startup or an original-guest
limitation. A full-menu run also enumerated 18 links while another run saw 30;
this adjacent enumeration inconsistency is retained for separate investigation.
Do not claim a fresh complete-menu acceptance from these application runs.

Test markers now use DOS 8.3 names, INSPECT.TAG and APPS.TAG: four-character
extensions were not visible through this guest's short-name interface.
The corrected application-only run records APPS_ONLY=1 and MENU_LINKS=0.

`tools/win95-launch/Enable-Paging.ps1` changes only the existing Paging line,
requires a fresh external backup and lossless INI decoding, preserves file
attributes, and refuses restore after subsequent changes. Exact restore is
exercised before the installed change. Recoverable final backup:
`O:/WINNT/builds/pre-win95-paging-20261025`.
SYSTEM.INI before SHA-256:
`D10F953467E101BF65E9D18F37583BB4C24DF8C28E5A05EB865356584633721E`;
after SHA-256:
`D80C1EC4D4DEC13A93198E001EDF24DCE4EA63D0AC213030679DBAE6DD637AAD`.

Durable local evidence: `O:/WINNT/logs/win95-paging/evidence`, including
run-r3 (On), run-r4-off, application runs and run-final-default. Earlier
compile, shared-runtime-busy and lifecycle failures remain explicit diagnostic
records. Temporary owned stages/build copies are disposable after retention.
