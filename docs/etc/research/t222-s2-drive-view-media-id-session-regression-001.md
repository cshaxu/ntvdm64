# T222 S2 Drive-View Media-ID Session Regression

`50:10 demGSetMediaID` now has Direct/Readonly normal-session evidence through
`dem_drive_view_provider_v1`.  The get form (`AL=0`, `BL=2`) preserves the two
caller-owned bytes at `DS:DX`, then performs the source-shaped checked writes
for volume serial, label, and file-system fields.  The fixture compares the
serial against the immutable admitted volume snapshot and proves the final
file-system range was written.

The set form (`AL!=0`) remains an unsupported volume mutation: it resumes with
CF set, changes no GPR, and leaves the caller buffer untouched in both
profiles.  It therefore does not introduce a Direct write path or a
Readonly-specific BOP branch.

The focused MSVC x64 `/MT` fixture compiled and linked from
`build/M0-T222-S2/028-drive-view-media-id`; its executable exited zero.  This
is one drive-view family entry only.  DPB, full-DPB, identity, clock, and the
remaining package matrix remain open.
