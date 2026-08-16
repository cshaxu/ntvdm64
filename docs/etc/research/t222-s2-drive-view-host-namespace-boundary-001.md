# T222 S2 Drive-View Host-Namespace Boundary

## Question

Can Direct/Readonly CWD receive its admitted host namespace without forcing
construction of the independent DEM file/search whole provider?

## Change

`dem_package_session_v1` now owns a validated borrowed
`drive_view_host_namespace` reference and exposes
`set_drive_view_host_namespace`. It binds the existing CWD context to the
boot namespace plane only. The existing `set_host_namespace` first uses that
binding, then retains its former responsibility of constructing the
whole-provider for file/search services.

## Evidence

The focused `dem-drive-view-provider` x64 `/MT` closure passed after the
change. The existing whole-provider session fixture was rebuilt from the same
source closure in `build/M0-T222-S2/013-whole-provider-compat` and its linked
executable exited zero.

## Interpretation

CWD host capability no longer has a hidden dependency on file/search provider
initialization. No selector behavior, Bochs code, Overlay behavior, or Virtual
behavior changed. The normal Direct/Readonly CWD package-session regression
and DTA migration remain required for T222 closure.