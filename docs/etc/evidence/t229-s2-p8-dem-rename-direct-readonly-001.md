# T229 S2 P8 — DEM demRename Direct/Readonly conformance

OpenNT `demRename` consumes source `DS:DX` and destination `ES:DI`, rejects
cross-drive and same-path cases before `MoveFileOem`, and otherwise routes host
failure through `demClientError`. The whole-provider regression supplies two
checked guest OEM path copies.

Direct renames only a fixture-owned source to fixture-owned target and clears
CF. Readonly returns `AX=5, CF=1` before `MoveFile`, retaining source and not
creating target. The current provider keeps Overlay as a routing seam only.

All formal Direct/Readonly fixture modes exited zero on 2026-08-18. Cross-drive,
same-path, and host-error variants remain separate matrix rows.